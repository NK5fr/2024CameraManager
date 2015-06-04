#include "imagedetect.h"

using namespace std;
using namespace std::chrono;

ImageDetect::ImageDetect(int imageWidth, int imageHeight, int imlimit, int subwinsiz, int minPix, int maxPix) {
    this->imageWidth = imageWidth;
    this->imageHeight = imageHeight;
    this->subwinsiz = subwinsiz;
    this->imlimit = imlimit;
    this->minpix = minPix;
    this->maxpix = maxPix;
    newarray = new unsigned char[imageWidth * imageHeight];
    newarray2 = new unsigned char[imageWidth * imageHeight];
    //imarray = new unsigned char[imageWidth * imageHeight];
    imarray = nullptr;
    initPoints = new ImPoint[MAX_POINTS];
    finalPoints = new ImPoint[MAX_POINTS];
    swtab = new SubWin(imageWidth, imageHeight);
    pointDef = new PointDef(imageHeight, MAX_POINTS);
    points = new ImPointsOneCam(MAX_POINTS);
    duplRemoved = new VectorOneCam(MAX_POINTS);
}

ImageDetect::~ImageDetect() {
    delete[] newarray;
    delete[] imarray;
    delete[] newarray2;
    delete[] initPoints;
    delete[] finalPoints;
    delete swtab;
    delete pointDef;
    delete points;
    delete duplRemoved;
}

void ImageDetect::setSubwinSize(int subwinsiz) {
    if (this->subwinsiz != subwinsiz) {
        this->subwinsiz = subwinsiz;
    }
}

void ImageDetect::start() {
    running = true;
    int runStep = 0;
    const int checkTimeStep = 50;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    while (running) {
        if (imarray != nullptr) {
            if (removeBackgroundFirst) {
                imageRemoveBackground();
                memcpy(newarray2, newarray, imageWidth * imageHeight);
            }
            if (runStep % checkTimeStep == 0) {
                t1 = high_resolution_clock::now();
            }
            imageDetectPoints();
            removeDuplicatePoints();
            if (runStep % checkTimeStep == 0) {
                t2 = high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
                cout << "ImageDetect used: " << duration << " microseconds\n";
                //printf("ImageDetect used: %.4f ms\n", elapsed_secs * 1000);
            }
            writingToPoints = true;
            int size = sizeof(ImPoint) * this->points->numpoints;
            int size2 = sizeof(ImPoint) * this->duplRemoved->numpointsnew;
            memcpy(initPoints, this->points->table, sizeof(ImPoint) * this->points->numpoints);
            memcpy(finalPoints, this->pointDef->newpt, sizeof(ImPoint) * this->duplRemoved->numpointsnew);
            numPoints = this->points->numpoints;
            numFinalPoints = this->duplRemoved->numpointsnew;
            writingToPoints = false;
            delete[] imarray;
            imarray = nullptr;
            runStep++;
        } else {
            QThread::msleep(threadSleepMs);
        }
    }
}

void ImageDetect::stop() {
    running = false;
}

bool ImageDetect::isBusy() {
    return (imarray != nullptr);
}

void ImageDetect::imageDetectPoints() {  // imno for logging
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To detect bright spots (points/particles) in an image
    //          This routine replaces the previous program:  SPOT with subroutines
    //

    points->numpoints = 0;

    // ML 2013-08-19 For å kunne variere disse parametrene via en løkke
    int numOfPointsFound = points->numpoints;

    const int MAXLOOP = imageWidth * imageHeight; //22.04.2010Set a limit to ensure that the loop finishes

    // not referenced  int subwinsiz2 =   subwin * subwin; //this->imgParam->subwinsiz * this->imgParam->subwinsiz;
    int subwincent = subwinsiz / 2; //(this->imgParam->subwinsiz/2);

    // --- Remove image background in image from all cameras
    imageRemoveBackground();
    //memcpy(filteredImage, newarray, imageWidth * imageHeight);
    // --- Loop over images from one camera
    int ic, jc;
    // --- Search for points
    pointDef->xlast = 0;
    pointDef->ylast = 0;
    int iloop = 0;

    
    while (iloop < MAXLOOP) {
        iloop++;
        if (!imageFindPoint()) break;
        imageMaxPixelPosition();
        if (min(pointDef->maxx, pointDef->maxy) >= minpix && max(pointDef->maxx, pointDef->maxy) <= 2 * maxpix) { //Allow possible dipols to be treated further also
            pointDef->newpnt.x = pointDef->pnt.x - (subwinsiz / 2); // + 1 ???
            pointDef->newpnt.y = pointDef->pnt.y - (subwinsiz / 2); // + 1 ???
            int ii = (int) (pointDef->newpnt.x + 0.5); // 23.04.2010
            int jj = (int) (pointDef->newpnt.y + 0.5); // 23.04.2010
            subwinRead(ii, jj, ic, jc); // Read pixel values from imarray1 into sub-window
            //printf("(%i,%i)", ii, jj);
            subwinRemoveBackground(); // Remove background in sub-window
            subwinFindPoint(ic, jc);
            if (imagePointPosition()) {
                //Check minimum size of point
                if (min(pointDef->maxx, pointDef->maxy) >= minpix && max(pointDef->maxx, pointDef->maxy) <= 2 * maxpix) { //Allow possible dipols to be treated further also
                    if (points->numpoints < MAX_POINTS - 1) {
                        points->table[points->numpoints] = pointDef->pnt;
                        points->numpoints++;
                    }
                }
            }
        }
        imageDeletePoint();
    } //While iloop
    
    if (points->numpoints > 0) {  // unødvendig test?
        for (int i = numOfPointsFound; i < points->numpoints; i++) {
            // Korreksjon slik at punkt-verdiene starter i 1, ikke 0
            points->table[i].x++;
            //points->table[i].y = imageHeight - points->table[i].y;
        }
    }
} // ImageDetectPoints()

///////////// Image::imagePointPosition() /////////////////////////////////////////////////////
//
//

bool ImageDetect::imagePointPosition() {
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To find coordinates of a point in an image
    //          This routine replaces the previous program:
    //             a SPOT subroutine (bilde_beregnposisjon)

    // --- Initialization 
    double sumi = 0.0;
    double sumx = 0.0;
    double sumy = 0.0;
    int iweight = 0;
    //
    // --- Calculate center of a point using the square of the point pixel values as weight
    int i1, i2;
    for (int jj = 0; jj < pointDef->maxy; jj++) {
        int j = pointDef->stripes[jj].y1;
        i1 = pointDef->stripes[jj].x1;
        i2 = pointDef->stripes[jj].x2;
        for (int i = i1; i < i2; i++) {
            int ipix = imarray[(j * imageWidth) + i];
            double rpix = (double) (ipix) / 10.0;
            double ipix2 = rpix*rpix*rpix; //Use power of three to reduce effect of shadow intensity
            sumi += ipix2;
            sumx += i*ipix2;
            sumy += j*ipix2;
            iweight++;
        } //END DO
    } //END DO
    //
    //if (iweight == 0 || (int)sumi == 0) {
    if (iweight == 0 || fabs(sumi) < 0.0001) {  // 22.04.2010
        return false;
    }// ENDIF
    //
    pointDef->pnt.x = sumx / sumi;
    pointDef->pnt.y = sumy / sumi;
    pointDef->pnt.weight = iweight; //Number of pixels in the 'point'
    return true;
} // ImagePointPosition

///////////// Image::imageDeletePoint() /////////////////////////////////////////////////////
//
//
void ImageDetect::imageDeletePoint() {
    //void ImageDeletePoint(int imy, int inarray[ImageDef::IMDIMX][ImageDef::IMDIMY]){  2010-02-25
    //ELvoid ImageDeletePoint(int imy, int inarray[ImageDef::IMDIMX][ImageDef::IMDIMY], ofstream *unlog){
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To delete a point in an image
    //          This routine replaces the previous program:
    //             a SPOT subroutine (bilde_slettpunkt)
    //
    // Input:
    //        imy: vertical dimension of image
    //        inarray: Input image
    // Input via module PointDef
    //         data on the extension of the detected point
    //
    // --- Erase the pixels associated with the point	
    for (int jj = 0; jj < pointDef->maxy; jj++) {
        int  j = pointDef->stripes[jj].y1;
        int  i1 = pointDef->stripes[jj].x1;
        int  i2 = pointDef->stripes[jj].x2;
        for (int i = i1; i < i2; i++) {
            imarray[(j * imageWidth) + i] = 0;
            newarray[(j * imageWidth) + i] = 0;
        }
    } //    END DO
} // ImageDeletePoint

///////////// Image::imageMaxPixelPosition() /////////////////////////////////////////////////////
//
//
void ImageDetect::imageMaxPixelPosition() {  // EL 2010-02-24
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    //
    // Purpose: To find the location of the maximum pixel value in a cluster of possible particle pixels
    //          This routine replaces the previous program:
    //             a SPOT subroutine (bilde_beregnposisjon)


    // Lars Aksel 2014-06-13 - 'jmin' was never used in the following code.

    //	DO jj = 1, maxy
    int maxValue = 0;
    int maxValuePosX = -1;
    int maxValuePosY = -1;
    for (int i = 0; i < pointDef->maxy; i++) {
        int yLine = pointDef->stripes[i].y1;
        int xMin = pointDef->stripes[i].x1;
        int xMax = pointDef->stripes[i].x2;
        for (int x = xMin; x < xMax; ++x) {
            if (newarray[(yLine * imageWidth) + x] > maxValue) {
                maxValue = newarray[(yLine * imageWidth) + x];
                maxValuePosX = x;
                maxValuePosY = yLine;
            }
        }
    } //      END DO
    //
    // --- Find the location of the maximum pixle value
    pointDef->pnt.x = (double) maxValuePosX; // ==jlocmax[0][totmaxj]
    pointDef->pnt.y = (double) maxValuePosY;
    pointDef->pnt.weight = 1; //Number of pixels in the 'point'
} // ImageMaxPixelPosition


///////////// Image::imageRemoveBackground() /////////////////////////////////////////////////////
//
//
void ImageDetect::imageRemoveBackground() {
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To remove the background in an image
    //          This routine replaces the previous program:
    //           a SPOT subroutine (dobbelloop + bilde_fjernbakgrunn)
    //

    // Lars Aksel - 15.01.2015 - Legger inn igjen kode, med fiks på tidligere problem 
    // *Utkommenterer Lars sin endring, setter inn det gamle. Vil kanskje ikke funke med GPU* (Gjelder ikke lenger - 15.01.2014)

    // not referenced  int	subwinsiz2 = subwinsiz * subwinsiz; //this->imgParam->subwinsiz * this->imgParam->subwinsiz;

    int imthresh;
    int subwinavg;
    int subwinsiz2 = subwinsiz * subwinsiz;

#ifdef SIMD_TESTING_PHASE_ONE
    const __m128i vk0 = _mm_set1_epi8(0);       // constant vector of all 0s for use with _mm_unpacklo_epi8/_mm_unpackhi_epi8
#endif

    for (int yStart = 0; yStart < imageHeight; yStart += subwinsiz) {
        //for (int yStart = 0; yStart < imy - subwinsiz; yStart += subwinsiz) { // Lars Aksel - 27.01.2015
        int yEnd = yStart + subwinsiz;
        if (yEnd > imageHeight) {
            yEnd = imageHeight;
            yStart = imageHeight - subwinsiz;
        }
        for (int xStart = 0; xStart < imageWidth; xStart += subwinsiz) {
            //for (int xStart = 0; xStart < imx - subwinsiz; xStart += subwinsiz) { // Lars Aksel - 27.01.2015
            int xEnd = xStart + subwinsiz;
            if (xEnd > imageWidth) {
                xEnd = imageWidth;
                xStart = imageWidth - subwinsiz;
            }
            subwinavg = 0;
            if (imlimit > 120) {
                imthresh = imlimit;//assume fixed threshold
            } else {
                //use dynamic threshold as offset from average pixel value
                //Remove subwin background based on average pixel value
                // Calculate average pixel value in sub-window	

                //long* iterator = (long*) &imarray[yStart][xStart];

                for (int y = yStart; y < yEnd; ++y) {
#ifdef SIMD_TESTING_PHASE_ONE
                    __m128i vsum = _mm_set1_epi32(0);
                    for (int i = xStart; i < xEnd; i += 16) {
                        __m128i v = _mm_load_si128((__m128i*) &imarray[(y * imageWidth) + i]);      // load vector of 8 bit values
                        __m128i vl = _mm_unpacklo_epi8(v, vk0); // unpack to two vectors of 16 bit values
                        __m128i vh = _mm_unpackhi_epi8(v, vk0);
                        __m128i wordPacked = _mm_add_epi16(vl, vh);
                        __m128i wordL = _mm_unpacklo_epi16(wordPacked, vk0);
                        __m128i wordH = _mm_unpackhi_epi16(wordPacked, vk0);
                        vsum = _mm_add_epi32(vsum, _mm_add_epi32(wordL, wordH));
                    }
                    unsigned int mySum = vsum.m128i_u32[0];
                    mySum += vsum.m128i_u32[1];
                    mySum += vsum.m128i_u32[2];
                    mySum += vsum.m128i_u32[3];
                    subwinavg += mySum;
#else
                    for (int x = xStart; x < xEnd; ++x) {
                        subwinavg += imarray[(y * imageWidth) + x];
                    }
#endif                    
                }
                subwinavg /= subwinsiz2;

                // Calculate threshold value (average pixel value in sub-window + an offset)
                // The particles will have a max pixel value of 220 - 230
                if ((subwinavg + imlimit) < 210) {
                    imthresh = subwinavg + imlimit;
                } else {
                    // Set offset treshold value (use smaller offset when background is bright -> less contrast)
                    imthresh = min(imlimit, (int) (0.3 * (255. - subwinavg)));
                    //imthresh = max(min(imlimit,INT(210.-subwinavg)),10)
                    imthresh = subwinavg + imthresh;
                }
            }

#ifdef SIMD_TESTING_PHASE_ONE
            __m128i imThreshNum = _mm_set1_epi8((imthresh - 1));
            for (int y = yStart; y < yEnd; ++y) {
                for (int i = xStart; i < xEnd; i += 16) {
                    __m128i v = _mm_load_si128((__m128i*) &imarray[(y * imageWidth) + i]); // Load 16 * 8-bit elements (128-bit)
                    __m128i mask = _mm_andnot_si128(_mm_cmpeq_epi8(v, imThreshNum), _mm_cmpeq_epi8(_mm_max_epu8(v, imThreshNum), v)); // unsigned 'greater-than' comparison
                    //__m128i mask = _mm_cmpgt_epi8(v, imThreshNum); // signed 'greater-than' comparison
                    v = _mm_blendv_epi8(vk0, v, mask);
                    _mm_store_si128((__m128i*) &newarray[(y * imageWidth) + i], v); // Store 16 * 8-bit elements (128-bit)
                }
            }
#else
            for (int y = yStart; y < yEnd; ++y) {
                for (int x = xStart; x < xEnd; ++x) {
                    if (imarray[(y * imageWidth) + x] >= imthresh) {
                        newarray[(y * imageWidth) + x] = imarray[(y * imageWidth) + x];
                    } else newarray[(y * imageWidth) + x] = 0; // Lars Aksel - 23.01.2015 - Bug fix
                }
            }
#endif
        }
    }
} // ImageRemoveBacground

///////////// Image::subwinRemoveBackground() /////////////////////////////////////////////////////
//
//
void ImageDetect::subwinRemoveBackground() {
    //void SubwinRemoveBackground(int icam, int imlimit, int subwinsiz2) { 2010-02-25
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To remove the background in a part of an image
    //          This routine replaces the previous program:
    //             a SPOT subroutine (vin_fjernbakgrunn)
#if defined(SIMD_TESTING_PHASE_ONE) || defined(SIMD_TESTING_PHASE_TWO)
    const __m128i vk0 = _mm_set1_epi8(0);       // constant vector of all 0s for use with _mm_unpacklo_epi8/_mm_unpackhi_epi8
#endif
    int imthresh;
    long subwinavg = 0L;
    int subwinsiz2 = subwinsiz * subwinsiz;
    if (imlimit > 120) {
        imthresh = imlimit;//assume fixed threshold
    } else {
        //use dynamic threshold as offset from average pixel value
        //Remove subwin background based on average pixel value
        // Calculate average pixel value in sub-window	
        for (int j = 0; j < subwinsiz; j++) {
#ifdef SIMD_TESTING_PHASE_ONE
            __m128i vsum = _mm_set1_epi32(0);
            for (int i = 0; i < subwinsiz; i += 16) {
                __m128i v = _mm_load_si128((__m128i*) &swtab->sw[(j * imageWidth) + i]);      // load vector of 8 bit values
                __m128i vl = _mm_unpacklo_epi8(v, vk0); // unpack to two vectors of 16 bit values
                __m128i vh = _mm_unpackhi_epi8(v, vk0);
                __m128i wordPacked = _mm_add_epi16(vl, vh);
                __m128i wordL = _mm_unpacklo_epi16(wordPacked, vk0);
                __m128i wordH = _mm_unpackhi_epi16(wordPacked, vk0);
                vsum = _mm_add_epi32(vsum, _mm_add_epi32(wordL, wordH));
            }
            unsigned int mySum = vsum.m128i_u32[0];
            mySum += vsum.m128i_u32[1];
            mySum += vsum.m128i_u32[2];
            mySum += vsum.m128i_u32[3];
            subwinavg += mySum;
#else
            for (int i = 0; i < subwinsiz; i++) {
                subwinavg += swtab->sw[(j * imageWidth) + i];
            }
#endif
        }
        subwinavg /= subwinsiz2;

        // Calculate threshold value (average pixel value in sub-window + an offset)
        // The particles will have a max pixel value of 220 - 230
        if ((subwinavg + imlimit) < 210) {
            imthresh = subwinavg + imlimit;
        } else {
            // Set offset treshold value (use smaller offset when background is bright -> less contrast)
            imthresh = min(imlimit, (int) (0.3*(255. - subwinavg)));
            //imthresh = max(min(imlimit,INT(210.-subwinavg)),10)
            imthresh = subwinavg + imthresh;
        }
    }


#ifdef SIMD_TESTING_PHASE_ONE
    // --- Set subwin background to black
    __m128i imThreshNum = _mm_set1_epi8(imthresh);
    for (int j = 0; j < subwinsiz; j++) {
        for (int i = 0; i < subwinsiz; i += 16) {
            __m128i v = _mm_load_si128((__m128i*) &swtab->sw[(j * imageWidth) + i]);
            __m128i mask = _mm_cmplt_epu8(v, imThreshNum); // unsigned 'greater-than' comparison
            //__m128i mask = _mm_cmpgt_epi8(imThreshNum, v); // signed 'greater-than' comparison
            v = _mm_blendv_epi8(v, vk0, mask);
            _mm_store_si128((__m128i*) &swtab->sw[(j * imageWidth) + i], v);
        }
    }
#else
    for (int j = 0; j < subwinsiz; j++) {
        for (int i = 0; i < subwinsiz; i++){
            if (swtab->sw[(j * imageWidth) + i] < imthresh) {
                swtab->sw[(j * imageWidth) + i] = 0;
            }
        }
    }
#endif
} // SubwinRemoveBackground()

///////////// Image::subwinRead() /////////////////////////////////////////////////////
//
//
void ImageDetect::subwinRead(int ix, int iy, int &ic, int &jc) {
    // SUBROUTINE SubwinRead(ix, iy, imin, imax, imy, inarray,ic,jc)
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To read part of an image into a subwindow
    //          This routine replaces the previous program:
    //             a SPOT subroutine (vin_les)
    swtab->x1 = max(min(ix, imageWidth - subwinsiz), 0);
    //make sure y-location is within 1:imy - subwinsiz + 1
    swtab->y1 = max(min(iy, imageHeight - subwinsiz), 0);
    int i1 = swtab->x1;
    int j1 = swtab->y1;
    //Set the location of the maximum pixel to the location in the subwindow
    ic = ix - i1; // + 1;
    jc = iy - j1; // + 1;

    // --- Extract subwindow from image
    for (int j = 0; j < subwinsiz; j++) {
#ifdef SIMD_TESTING_PHASE_ONE
        for (int i = 0; i < subwinsiz; i += 16) { // Copies 16 bytes/elements for every loop...
            __m128i data = _mm_loadu_si128((__m128i*) &imarray[((j1 + j) * imageWidth) + (i1 + i)]);
            _mm_storeu_si128((__m128i*) &swtab->sw[(j * imageWidth) + i], data);
        }
#else 
        for (int i = 0; i < subwinsiz; i++) {
            swtab->sw[(j * imageWidth) + i] = imarray[((j1 + j) * imageWidth) + (i1 + i)];
        }
#endif
    }
} // SubwinRead()

///////////// Image::imageFindPoint() /////////////////////////////////////////////////////
//
//
bool ImageDetect::imageFindPoint() {
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Date: 27 April 2000
    //
    // Purpose: To find a point in an image
    //          This routine replaces the previous program:
    //             a SPOT subroutine (bilde_finnpunkt)
    //

    // --- Initialization 
    pointDef->maxx = 0;
    pointDef->maxy = 0;
    int imin = 0; // icam *  ImageDef::newdim[2]+ ImageDef::searchwin[0];
    int jmin = 0;
    int imax = imageWidth; // icam *  ImageDef::newdim[2]+ ImageDef::searchwin[2];

    // --- Search for location of first pixel value > 0 
    bool found = false;
    int jstart = pointDef->ylast;
    int jslutt = imageHeight;
    int j = jstart;
    int i = pointDef->xlast;


    // Lars Aksel - 2014-06-25 - Optimized single-loop algorithm to find point (CPU-run)
    const int maxLoop = jslutt * imax;
    unsigned char* iter;

#ifdef SIMD_TESTING_PHASE_TWO
    if ((&newarray[(jstart * imageWidth) + i] - (newarray + maxLoop)) % 16 == 0) {
        const __m128i vk0 = _mm_set1_epi8(0);
        for (iter = &newarray[(jstart * imageWidth) + i]; iter < (newarray + maxLoop); iter += 16) {
            __m128i v = _mm_load_si128((__m128i*) iter);
            __m128i mask = _mm_andnot_si128(_mm_cmpeq_epi8(v, vk0), _mm_cmpeq_epi8(_mm_max_epu8(v, vk0), v)); // unsigned 'greater-than' comparison
            //__m128i mask = _mm_cmpgt_epi8(v, vk0); // signed 'greater-than' comparison
            if (mask.m128i_u64[0] > 0) {
                for (int i = 0; i < 8; i++) {
                    if (mask.m128i_u8[i] > 0) {
                        iter = (iter - 16) + i;
                        break;
                    }
                }
                break;

            }
            if (mask.m128i_u64[1] > 0) {
                for (int i = 8; i < 16; i++) {
                    if (mask.m128i_u8[i] > 0) {
                        iter = (iter - 16) + i;
                        break;
                    }
                }
                break;
            }
        }
    } else {
        for (iter = &newarray[(jstart * imageWidth) + i]; iter < newarray + maxLoop; ++iter) {
            if (*iter > 0) {
                break;
            }
        }
    }    
#else
    for (iter = &newarray[(jstart * imageWidth) + i]; iter < newarray + maxLoop; ++iter) {
        if (*iter > 0) {
            break;
        }
    }
#endif
    if (iter < newarray + maxLoop) {
        int diff = iter - newarray;
        i = (diff % imax);
        j = diff / imax;
        found = true;
    }

    // --- A pixel value > 0 was found
    // --- Update start point location
    pointDef->ylast = j;
    pointDef->xlast = i + 1;

    // --- Return if this was the last pixel and it is also zero
    if (!found) {
        return false;
    }

    // --- Update point table
    int jj = 0;
    pointDef->stripes[jj].x1 = i; //leftmost extension for first stripe
    //	pointDef->stripes[jj].x2 = i; //rightmost extension for first stripe
    pointDef->stripes[jj].y1 = j;
    int ileft = pointDef->stripes[jj].x1;
    // --- Search to the right to find the extension of the point
    while (newarray[(j * imageWidth) + i] != 0 && i < imax) {
        i++; //DO i = ii, imax
    }
    pointDef->stripes[jj].x2 = i; //rightmost extension for first stripe
    int iright = pointDef->stripes[jj].x2; //rightmost extension for this point
    //pointDef->stripes[jj].numpoints = iright - ileft; //stripes(jj)%x2 - stripes(jj)%x1 + 1
    bool done = false;
    j++;
    while (j < imageHeight && !done) {
        i = pointDef->stripes[jj].x1; //next stripe
        while (newarray[(j * imageWidth) + i] == 0 && i < pointDef->stripes[jj].x2) i++;
        //The pixel below was black, search to the right to see if there pixels connected to the row above   

        if (i == pointDef->stripes[jj].x2) done = true; //The point does not continue further downwards			
        else { //The point does continue further downwards
            jj++;
            pointDef->stripes[jj].x1 = i;
            //     stripes(jj)%x2 = i
            pointDef->stripes[jj].y1 = j;
            while (i < imax && newarray[(j * imageWidth) + i] > 0) {
                i++; //search to the right at the end
            }
            pointDef->stripes[jj].x2 = i; //rightmost extension for this stripe
            iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension for this point
            i = pointDef->stripes[jj].x1 - 1;
            while (i >= imin && newarray[(j * imageWidth) + i] > 0) {
                i--; //search to the left at the end
            }
            pointDef->stripes[jj].x1 = i + 1; //leftmost extension for this stripe
            ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension for this point
            //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
            //  Update extension of longest stripe in x-direction?
        } // ENDIF
        j++;
    }//	END WHILE
    //     Update extension in x-direction
    pointDef->ileft = ileft;
    pointDef->iright = iright;
    pointDef->maxx = iright - ileft; // + 1
    // Update extension in y-direction
    pointDef->maxy = jj + 1; // Sto opprinnelig jj, menmå være +1 i c++
    return true;
} // ImageFindPoint()

///////////// Image::setNull() /////////////////////////////////////////////////////
//
//
void ImageDetect::setNull(int frax, int tilx, int fray, int tily) {
    for (int j = fray; j < tily; j++) {
        for (int i = frax; i < tilx; i++) {
            swtab->sw[(j * imageWidth) + i] = 0;
        }
    }
}

///////////// Image::maxval() /////////////////////////////////////////////////////
//
//
int ImageDetect::maxval() {
    int max = 0;
    for (int j = 0; j < subwinsiz; j++) {
        for (int i = 0; i < subwinsiz; i++) {
            if (swtab->sw[(j * imageWidth) + i] > max) {
                max = swtab->sw[(j * imageWidth) + i];
            }
        }
    }
    return max;
}

///////////// Image::subwinFindPoint() /////////////////////////////////////////////////////
//
//
void ImageDetect::subwinFindPoint(int &ii, int &jj) {
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To find a point in a sub-window
    //          This routine replaces the previous program:
    //             a SPOT subroutine (vin_finnpunkt)

    // --- Initialization 
    pointDef->maxx = 0;
    pointDef->maxy = 0;
    int imin = 0; //icam*  ImageDef::newdim[2]+ ImageDef::searchwin[0];
    int imax = imageWidth; //icam*  ImageDef::newdim[2]+ ImageDef::searchwin[2];

    // --- Erase any part of the subwindow that is outside the search window 
    setNull(0, imin - swtab->x1, 0, subwinsiz);
    setNull(imax - swtab->x1, subwinsiz, 0, subwinsiz);
    setNull(0, subwinsiz, 0, 0 - swtab->y1);
    setNull(0, subwinsiz, imageHeight - swtab->y1, subwinsiz);
    //
    // --- As the window is centered around the max pixel value, a search is not necessary any more
    int ic = ii + (subwinsiz / 2); // - 1
    int	jc = jj + (subwinsiz / 2); // - 1
    int	ileft = ic;
    int	iright = ic;  // CORRECTED from = jc, ML & EL 2009-09-25
    //	int ipix =  swtab->sw[ic][jc];  not used 2010-02-25
    int ipixlastx; // = maxval();
    int ipixlasty;
    int i = ic;
    int j = jc;
    while (j > 0 && swtab->sw[((j - 1) * imageWidth) + i] > 0 && swtab->sw[((j - 1) * imageWidth) + i] <= swtab->sw[(j * imageWidth) + i]) {
        j--;
    }
    int j1 = j;
    j = jc + 1;
    while (j < /*this->imgParam->*/subwinsiz && swtab->sw[(j * imageWidth) + i] > 0 && swtab->sw[(j * imageWidth) + i] <= swtab->sw[((j - 1) * imageWidth) + i]) {
        j++;
    }
    int j2 = j;
    pointDef->jhigh = j1;
    pointDef->jlow = j2;
    jj = jc - j1; // + 1
    // --- Search to the left to find the extension of the stripe
    ii = ic;
    i = ic;
    j = jc;
    while (i > 0 && swtab->sw[(j * imageWidth) + (i - 1)] != 0 && swtab->sw[(j * imageWidth) + (i - 1)] <= swtab->sw[(j * imageWidth) + i]) {
        i--;
    }
    pointDef->stripes[jj].x1 = i;
    pointDef->stripes[jj].y1 = j; // Sto ikke opprinnelig, lagt til
    // --- Search to the right to find the extension of this stripe
    i = ic + 1;
    while (i < subwinsiz && swtab->sw[(j * imageWidth) + i] != 0 && swtab->sw[(j * imageWidth) + i] <= swtab->sw[(j * imageWidth) + (i - 1)]) {
        i++;
    }
    pointDef->stripes[jj].x2 = i;
    //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
    ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension of the stripe
    iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension of the stripe
    i = ic;
    if (jc > j1) {
        for (j = jc - 1; j >= j1; j--) {
            jj = j - j1; // + 1
            pointDef->stripes[jj].x1 = ic; // Endret fra i; //leftmost extension of stripe
            pointDef->stripes[jj].x2 = ic + 1; // endret fra i+1; //rightmost extension of stripe
            pointDef->stripes[jj].y1 = j;
            // --- Search to the left to find the extension of the stripe
            ii = ic;
            ipixlastx = swtab->sw[(j * imageWidth) + ii];
            ipixlasty = swtab->sw[((j + 1) * imageWidth) + ii];
            i = ic;
            if (i < pointDef->stripes[jj + 1].x1) ipixlasty = ipixlastx;
            while (i >= 0 && swtab->sw[(j * imageWidth) + i] > 0 && swtab->sw[(j * imageWidth) + i] <= max(ipixlastx, ipixlasty)) {
                ipixlastx = swtab->sw[(j * imageWidth) + i];
                if (i > 0 && i - 1 >= pointDef->stripes[jj + 1].x1) {
                    ipixlasty = swtab->sw[((j + 1) * imageWidth) + (i - 1)];
                } else {
                    ipixlasty = ipixlastx;
                }
                i--;
            }
            pointDef->stripes[jj].x1 = i + 1;

            // --- Search to the right to find the extension of this stripe
            ii = ic;
            ipixlastx = swtab->sw[(j * imageWidth) + ii];
            ipixlasty = swtab->sw[((j + 1) * imageWidth) + ii];
            i = ii;
            if (i > pointDef->stripes[jj + 1].x2) ipixlasty = ipixlastx;
            while (i < subwinsiz && swtab->sw[(j * imageWidth) + i] > 0 && swtab->sw[(j * imageWidth) + i] <= max(ipixlastx, ipixlasty)) {
                ipixlastx = swtab->sw[(j * imageWidth) + i];
                if (i < subwinsiz - 1 && i + 1 <= pointDef->stripes[jj + 1].x2)	ipixlasty = swtab->sw[((j + 1) * imageWidth) + (i + 1)];
                else ipixlasty = ipixlastx;
                i++;
            }
            //????			pointDef->stripes[jj].x2 = j-1;
            pointDef->stripes[jj].x2 = i;
            //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
            ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension of the stripe
            iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension of the stripe
        } //	END DO
    } //endif

    i = ic;
    if (jc < j2) {
        for (j = jc + 1; j < j2; j++) {
            jj = j - j1; // + 1
            pointDef->stripes[jj].x1 = ic;// Endret fra i; //leftmost extension of stripe
            pointDef->stripes[jj].x2 = ic + 1;//Endret fra i+1; //rightmost extension of stripe
            pointDef->stripes[jj].y1 = j;
            // --- Search to the left to find the extension of the stripe
            ii = ic;
            ipixlastx = swtab->sw[(j * imageWidth) + ii];
            ipixlasty = swtab->sw[((j - 1) * imageWidth) + (ii - 1)];
            for (i = ii - 1; i >= 0; i--) {
                if (i < pointDef->stripes[jj - 1].x1) ipixlasty = ipixlastx;
                int ipixnext = swtab->sw[(j * imageWidth) + i];
                if (ipixnext == 0 || ipixnext > max(ipixlastx, ipixlasty)) {
                    break;
                }
                pointDef->stripes[jj].x1 = i; //leftmost extension of the stripe
                ipixlastx = swtab->sw[(j * imageWidth) + i];
                ipixlasty = swtab->sw[((j - 1) * imageWidth) + (i - 1)];
            } //	END DO
            // --- Search to the right to find the extension of this stripe
            ii = ic;
            ipixlastx = swtab->sw[(j * imageWidth) + ii];
            ipixlasty = swtab->sw[((j - 1) * imageWidth) + (ii + 1)];
            for (i = ii; i < subwinsiz; i++) {
                if (i >= pointDef->stripes[jj - 1].x2) {
                    ipixlasty = ipixlastx;
                }
                int ipixnext = swtab->sw[(j * imageWidth) + i];
                if (ipixnext == 0 || ipixnext > max(ipixlastx, ipixlasty)) {
                    break;
                }
                pointDef->stripes[jj].x2 = i + 1; //rightmost extension of the stripe
                ipixlastx = swtab->sw[(j * imageWidth) + i];
                ipixlasty = swtab->sw[((j - 1) * imageWidth) + (i + 1)];
            }	//END DO
            //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
            ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension of the stripe
            iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension of the stripe
        } //	END DO
        jj = j2 - j1 - 1; // Lagt til
    } else {
        jj = jc - j1; // + 1 //To account for cases where jc = j2
    } //endif
    bool done = false;
    if (pointDef->stripes[0].x2 - 1 - ic > ic - pointDef->stripes[0].x1) {
        //
        // --- Search up to to the right to find the extension of the point
        j = j1;
        while (j > 0 && !done) {
            j--;
            i = ic + 1;
            //Endret				while ( swtab->sw[(j * imageWidth) + i] == 0 &&  swtab->sw[i][j+1] > 0 && i < pointDef->stripes[j - j1 + 1].x2) i++;
            //				while ( swtab->sw[(j * imageWidth) + i] == 0 &&  swtab->sw[i][j+1] > 0 && i < pointDef->stripes[j1 - j].x2) i++;
            while (swtab->sw[(j * imageWidth) + i] == 0 && swtab->sw[((j + 1) * imageWidth) + i] > 0 && i < pointDef->stripes[j - j1].x2) i++;
            if (swtab->sw[(j * imageWidth) + i] == 0) {//The point does not continue further downwards
                done = true;
            } else { //The point does continue further up
                ii = i;
                ipixlastx = swtab->sw[((j + 1) * imageWidth) + (ii - 1)];
                ipixlasty = swtab->sw[((j + 1) * imageWidth) + ii];
                int ipixnext = swtab->sw[(j * imageWidth) + ii];
                if (ipixnext < max(ipixlastx, ipixlasty)) {
                    jj++;
                    //pointDef->stripes[jj].numpoints = 1;
                    pointDef->stripes[jj].x1 = i;
                    pointDef->stripes[jj].x2 = i + 1;
                    pointDef->stripes[jj].y1 = j;
                    pointDef->jhigh = jj;
                    // --- Search to the right to find the extension of this stripe
                    for (i = ii; i < /*this->imgParam->*/subwinsiz; i++) {
                        ipixlasty = swtab->sw[((j + 1) * imageWidth) + i];
                        ipixnext = swtab->sw[(j * imageWidth) + i];
                        if (ipixnext == 0 || ipixnext > max(ipixlastx, ipixlasty)) break;
                        pointDef->stripes[jj].x2 = i + 1; //rightmost extension for this stripe
                        ipixlastx = ipixlasty;
                    } //END DO
                    //
                    //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
                    ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension for this point
                    iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension for this point
                } else {
                    done = true;
                }
            } //ENDIF
        } //	END DO
    } else if (pointDef->stripes[0].x2 - 1 - ic < ic - pointDef->stripes[0].x1) { // endret fra <
        //
        // --- Search up to to the left to find the extension of the point
        done = false;
        j = j1;
        while (j > 0 && !done) {
            j = j - 1;
            i = ic - 1;
            //Endret			while ( swtab->sw[(j * imageWidth) + i] == 0 &&  swtab->sw[i][j+1] > 0 && i > pointDef->stripes[j - j1 /*+ 1*/].x1) {
            //			while ( swtab->sw[(j * imageWidth) + i] == 0 &&  swtab->sw[i][j+1] > 0 && i > pointDef->stripes[j1 - j /*+ 1*/].x1) {
            while (swtab->sw[(j * imageWidth) + i] == 0 && swtab->sw[((j + 1) * imageWidth) + i] > 0 && i > pointDef->stripes[j - j1].x1) {
                i--;
            }
            if (swtab->sw[(j * imageWidth) + i] == 0) { //The point does not continue further downwards
                done = true;
            } else {//The point does continue further up
                ii = i;
                ipixlastx = swtab->sw[((j + 1) * imageWidth) + (ii + 1)];
                ipixlasty = swtab->sw[((j + 1) * imageWidth) + ii];
                int ipixnext = swtab->sw[(j * imageWidth) + ii];
                if (ipixnext < max(ipixlastx, ipixlasty)) {
                    jj++;
                    //pointDef->stripes[jj].numpoints = 1;
                    pointDef->stripes[jj].x1 = i;
                    pointDef->stripes[jj].x2 = i + 1;
                    pointDef->stripes[jj].y1 = j;
                    pointDef->jhigh = jj;
                    // --- Search to the left to find the extension of this stripe
                    for (i = ii - 1; i >= 0; i--) {
                        ipixlasty = swtab->sw[((j + 1) * imageWidth) + i];
                        ipixnext = swtab->sw[(j * imageWidth) + i];
                        if (ipixnext == 0 || ipixnext > max(ipixlastx, ipixlasty)) break;
                        pointDef->stripes[jj].x1 = i; //rightmost extension for this stripe
                        ipixlastx = ipixlasty;
                    } //END DO
                    //
                    //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
                    ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension for this point
                    iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension for this point
                } else {
                    done = true;
                }
            } //ENDIF
        } //END DO
    } //endif
    //
    // --- Search down to find the extension of the point
    //
    if (pointDef->stripes[j2 - j1 - 1].x2 - 1 - ic > ic - pointDef->stripes[j2 - j1 - 1].x1) { //Endret fra >
        //
        // --- Search down to to the right to find the extension of the point
        done = false;
        j = j2 - 1;  // Endret fra j=j2
        while (j < /*this->imgParam->*/subwinsiz && !done) {
            j++;
            i = ic + 1;
            while (swtab->sw[(j * imageWidth) + i] == 0 && swtab->sw[((j + 1) * imageWidth) + i] > 0
                   && i < pointDef->stripes[j - j1 /*+ 1*/].x2 - 1)
                   i++;
            //END DO
            if (swtab->sw[(j * imageWidth) + i] == 0) {  //The point does not continue further downwards
                done = true;
            } else { //The point does continue further down
                ii = i;
                ipixlastx = swtab->sw[((j - 1) * imageWidth) + (ii - 1)];
                ipixlasty = swtab->sw[((j - 1) * imageWidth) + ii];
                int ipixnext = swtab->sw[(j * imageWidth) + ii];
                if (ipixnext < max(ipixlastx, ipixlasty)) {
                    jj++;
                    //pointDef->stripes[jj].numpoints = 1;
                    pointDef->stripes[jj].x1 = i;
                    pointDef->stripes[jj].x2 = i + 1;
                    pointDef->stripes[jj].y1 = j;
                    pointDef->jhigh = jj;
                    // --- Search to the right to find the extension of this stripe
                    for (i = ii; i < subwinsiz; i++) {
                        ipixlasty = swtab->sw[((j - 1) * imageWidth) + i];
                        ipixnext = swtab->sw[(j * imageWidth) + i];
                        if (ipixnext == 0 || ipixnext > max(ipixlastx, ipixlasty)) {
                            break;
                        }
                        pointDef->stripes[jj].x2 = i + 1; //rightmost extension for this stripe
                        ipixlastx = ipixlasty;
                    } //END DO
                    //
                    //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
                    ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension for this point
                    iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension for this point
                } else {
                    done = 1;
                }
            } //ENDIF
        } //END DO
    } else if (pointDef->stripes[j2 - j1 - 1].x2 - 1 - ic < ic - pointDef->stripes[j2 - j1 - 1].x1) {//Endret fra <
        //
        // --- Search down to to the left to find the extension of the point
        done = false;
        j = j2 - 1; // Endret fra j=j2
        while (j < /*this->imgParam->*/subwinsiz - 1 && !done){
            j++;
            i = ic - 1;
            while (swtab->sw[(j * imageWidth) + i] == 0 && swtab->sw[((j - 1) * imageWidth) + i] > 0 &&
                   i > pointDef->stripes[j - j1 /*+ 1*/].x1)
                   i--;
            //END DO
            if (swtab->sw[(j * imageWidth) + i] == 0)  { //The point does not continue further downwards
                done = true;
            } else { //The point may continue further down
                ii = i;
                ipixlastx = swtab->sw[((j - 1) * imageWidth) + (ii + 1)];
                ipixlasty = swtab->sw[((j - 1) * imageWidth) + ii];
                int ipixnext = swtab->sw[(j * imageWidth) + ii];
                if (ipixnext < max(ipixlastx, ipixlasty)) {
                    jj++;
                    //pointDef->stripes[jj].numpoints = 0;
                    pointDef->stripes[jj].x1 = i;
                    pointDef->stripes[jj].x2 = i + 1;
                    pointDef->stripes[jj].y1 = j;
                    pointDef->jhigh = jj;
                    // --- Search to the left to find the extension of this stripe
                    for (i = ii; i >= 0; i--) {
                        ipixlasty = swtab->sw[((j - 1) * imageWidth) + i];
                        ipixnext = swtab->sw[(j * imageWidth) + i];
                        if (ipixnext == 0 || ipixnext > max(ipixlastx, ipixlasty)) break;
                        pointDef->stripes[jj].x1 = i; //rightmost extension for this stripe
                        ipixlastx = ipixlasty;
                    } //END DO
                    //
                    //pointDef->stripes[jj].numpoints = pointDef->stripes[jj].x2 - pointDef->stripes[jj].x1; // + 1
                    ileft = min(ileft, pointDef->stripes[jj].x1); //leftmost extension for this point
                    iright = max(iright, pointDef->stripes[jj].x2); //rightmost extension for this point
                } else {
                    done = true;
                }
            } //ENDIF
        } //END DO
    } //endif
    //     Update extension in x-direction
    pointDef->ileft = ileft;
    pointDef->iright = iright;
    pointDef->maxx = iright - ileft; // + 1
    //     Update extension in x-direction
    pointDef->maxy = jj + 1; // Sto opprinnelig jj, men må være +1 i C++
    for (j = 0; j < pointDef->maxy; j++) {
        pointDef->stripes[j].x1 = pointDef->stripes[j].x1 + swtab->x1; // - 1
        pointDef->stripes[j].x2 = pointDef->stripes[j].x2 + swtab->x1; // - 1
        pointDef->stripes[j].y1 = pointDef->stripes[j].y1 + swtab->y1; // - 1
    } //  END DO
    pointDef->ileft = pointDef->ileft + swtab->x1; // - 1
    pointDef->iright = pointDef->iright + swtab->x1; // - 1
    return;
} // SubwinFindPoint

///////////// Image::duplicatePoints() /////////////////////////////////////////////////////
//
//
void ImageDetect::removeDuplicatePoints() {  // imno for logging
    // ---
    // Written by: Stig Magnar Løvås, SINTEF 2000
    // Adapted for R3DCV by Mildrid Ljosland and Else Lervik, HiST, 2009
    //
    // Purpose: To remove duplicate points
    //          This routine replaces the previous program: FJERN
    // Input:
    //        minsep: Minimum distance [pixels] to the next separate point

    int minsep2 = minsep * minsep;
    //ProdRunImage *image = (ProdRunImage *)this;

    // --- Get points from this image
    // ---    Get and check number of points in image from each camera
    int numpoints = points->numpoints;

    if (numpoints > 0) { //THEN
        /*
        if (numpoints > MAX_POINTS) {
            if (calibrationRun) {  // not possible to use polymorhism        
                return numpoints;  // too large value. Continue with next.
            } else {
                //errorMsgAndExit("Overflow, number of points (" + createStrOneNo(numpoints) + ") > maxpoints (" + createStrOneNo(ArrayLimits::MAX_POINTS) + ").", "Serious error in DuplicatePoints(). Exits.");
            }
        }
        */
        // ---       Get all points in image
        for (int i = 0; i < numpoints; i++) {
            pointDef->table[i].x = points->table[i].x;
            pointDef->table[i].y = points->table[i].y;
            pointDef->table[i].weight = points->table[i].weight;
        } //  END DO

        int numpointsnew = 0;
        int nummatch = 0;
        // ---    Reset array for control of used points to zero
        for (int i = 0; i < numpoints; i++) {
            pointDef->usedpt[i] = 0;
        }

        // ---    Check for duplicate points
        for (int i = 0; i < numpoints /*- 1*/; i++) { // Tar også med siste punkt
            if (pointDef->usedpt[i] == 0) { //THEN
                nummatch = 1;
                pointDef->match[nummatch - 1] = pointDef->table[i];
                pointDef->usedpt[i] = 1;
                for (int j = i + 1; j < numpoints; j++) {
                    double dist = pow((pointDef->table[j].x - pointDef->match[0].x), 2) + pow(pointDef->table[j].y - pointDef->match[0].y, 2);

                    if (dist < minsep2 && pointDef->usedpt[j] == 0) { //THEN
                        pointDef->match[nummatch] = pointDef->table[j];
                        nummatch++;
                        pointDef->usedpt[j] = 1;
                    } //ENDIF
                } //END DO
                pointDef->newpt[numpointsnew].x = 0.0;
                pointDef->newpt[numpointsnew].y = 0.0;
                pointDef->newpt[numpointsnew].weight = 0.0;
                for (int j = 0; j < nummatch; j++) {
                    pointDef->newpt[numpointsnew].x += pointDef->match[j].x * pointDef->match[j].weight;
                    pointDef->newpt[numpointsnew].y += pointDef->match[j].y * pointDef->match[j].weight;
                    pointDef->newpt[numpointsnew].weight += pointDef->match[j].weight;
                } //END DO
                pointDef->newpt[numpointsnew].x /= pointDef->newpt[numpointsnew].weight;
                pointDef->newpt[numpointsnew].y /= pointDef->newpt[numpointsnew].weight;
                pointDef->newpt[numpointsnew].weight = nummatch;
                numpointsnew++; // = numpointsnew + 1
            } //ENDIF
        } //END DO

        duplRemoved->numpointsnew = numpointsnew;

        for (int i = 0; i < numpointsnew; i++) {
            // original format
            // for002.dat format
            double xb1, yb1;
            if (pointDef->newpt[i].x < imageWidth + 1) { //then
                xb1 = pointDef->newpt[i].x - 0.5;
                yb1 = pointDef->newpt[i].y - 0.5;
            } else if (pointDef->newpt[i].x < 2 * imageWidth + 1) { //then
                xb1 = pointDef->newpt[i].x - (imageWidth + 0.5);
                yb1 = pointDef->newpt[i].y - 0.5;
            } else {
                xb1 = pointDef->newpt[i].x - (2 * imageWidth + 0.5);
                yb1 = pointDef->newpt[i].y - 0.5;
            } //endif
            double xb = xb1 - imageWidth / 2.0;
            double yb = -yb1 + imageHeight / 2.0;
            duplRemoved->vectors[i].setVector(xb1, yb1, xb, yb);
            duplRemoved->numOfMatches[i] = pointDef->newpt[i].weight;  // ML 2013-08-23
        } //END DO
    } else {
        duplRemoved->numpointsnew = 0;
    }
    //return numpoints;
} // DuplicatePoints()