#ifndef DETECTPARTICLES
#define DETECTPARTICLES

#include <chrono>
#include <iostream>
#include <qthread.h>
#include <algorithm>

// Testing av SIMD-instruksjoner - Lars Aksel - 04.06.2015
#define SIMD_TESTING_128_BIT // Requires SSE3-support
//#define SIMD_TESTING_256_BIT // Requires AVX2-support, not finished

// Unsigned greater-than comparison 8-bit
static inline __m128i _mm_cmpgt_epu8(__m128i x, __m128i y) {
  // Returns 0xFF where x > y:
  return _mm_andnot_si128(_mm_cmpeq_epi8(x, y), _mm_cmpeq_epi8(_mm_max_epu8(x, y), x));
}

// Unsigned less-than comparison 8-bit
static inline __m128i _mm_cmplt_epu8(__m128i x, __m128i y) {
  // Returns 0xFF where x < y:
  return _mm_cmpgt_epu8(y, x);
}

struct ImPoint {
    ImPoint() : x(0.0), y(0.0), weight(0.0) {}
    double x;
    double y;
    double weight;
};

struct ImPointsOneCam {
    int numpoints;
    ImPoint* table;
    ImPointsOneCam(int maxPoints) : numpoints(0) {
        table = new ImPoint[maxPoints];
    }
    ~ImPointsOneCam() {
        delete[] table;
    }
};

struct Stripe {
    int y1;
    int x1;
    int x2;
};

struct PointDef {
    PointDef(int height, int max_points) : xlast(0), ylast(0), maxx(0), maxy(0), jlow(0), jhigh(0), ileft(0), iright(0) {
        stripes = new Stripe[height];
        table = new ImPoint[max_points];
        match = new ImPoint[max_points];
        newpt = new ImPoint[max_points];
        usedpt = new int[max_points];
    }
    ~PointDef() {
        delete[] stripes;
        delete[] table;
        delete[] match;
        delete[] newpt;
    }
    Stripe* stripes;
    ImPoint pnt;
    ImPoint newpnt;
    ImPoint* table;
    ImPoint* match;
    ImPoint* newpt;
    int* usedpt;
    int xlast;
    int ylast;
    int maxx;
    int maxy;
    int jlow;
    int jhigh;
    int ileft;
    int iright;
};

struct SubWin {
    int x1, y1;
    unsigned char* sw;
    SubWin(int imageWidth, int imageHeight) {
        x1 = y1 = 0;
        sw = new unsigned char[imageWidth * imageHeight];
        memset(sw, 0, sizeof(unsigned char) * imageWidth * imageHeight);
    }
    ~SubWin() {
        delete[] sw;
    }
};

/////////// Datatransfer out of duplicatePoints() and in to pointCoordinates() /////////////
struct Vector {
    double vec[4];
    Vector() {
        for (int i = 0; i < 4; i++) {
            vec[i] = 0.0;
        }
    }
    Vector(const double *vector) {
        for (int i = 0; i < 4; i++) {
            vec[i] = vector[i];
        }
    }
    inline void setVector(double xb1, double yb1, double xb, double yb) {
        vec[0] = xb1;
        vec[1] = yb1;
        vec[2] = xb;
        vec[3] = yb;
    }
    inline double findDistance(Vector p2) {
        double x = vec[0] - p2.vec[0];
        double y = vec[1] - p2.vec[1];
        return sqrt(x * x + y * y);
    }
};

struct VectorOneCam {
    int numpointsnew;
    int* numOfMatches; // ML 2013-08-23
    Vector* vectors;  // std constructor Vector() nullifies these
    VectorOneCam(int max_points) {
        numpointsnew = 0;
        numOfMatches = new int[max_points];
        vectors = new Vector[max_points];
        for (int i = 0; i < max_points; i++) {
            numOfMatches[i] = 0;
        }
    }
    VectorOneCam(const VectorOneCam &original) : numpointsnew(original.numpointsnew) { // OBS: public copy constructor      
        for (int i = 0; i < numpointsnew; i++) {
            this->vectors[i] = original.vectors[i];
            this->numOfMatches[i] = original.numOfMatches[i];   // ML 2013-08-23
        }
    }
    VectorOneCam &operator=(const VectorOneCam &original) {  // OBS: public assignment operator      
        if (this != &original) {
            this->numpointsnew = original.numpointsnew;
            for (int i = 0; i < numpointsnew; i++) {
                this->vectors[i] = original.vectors[i];
                this->numOfMatches[i] = original.numOfMatches[i];
            }
        }
        return *this;
    }
};


class ImageDetect {
public:
    ImageDetect(int imageWidth, int imageHeight, int imlimit = 0, int subwinsiz = 0, int minPix = 0, int maxPix = 0);
    ~ImageDetect();

    void imageDetectPoints();
    void imageRemoveBackground();
    void removeDuplicatePoints();

    void setImage(unsigned char* imarray) { this->imarray = imarray; }
    void setThreshold(int imlimit) { this->imlimit = imlimit; }
    void setSubwinSize(int subwinsiz);
    void setMaxPix(int maxpix) { this->maxpix = maxpix; }
    void setMinPix(int minpix) { this->minpix = minpix; }
    void setMinSep(int minSep) { this->minsep = minSep; }
    ImPoint* getInitPoints() { return this->points->table; }
    ImPoint* getFinalPoints() { return this->pointDef->newpt; }
    unsigned char* getFilteredImage() { return this->newarray; }
    unsigned char* getImage() { return this->imarray; }
    int getInitNumPoints() { return this->points->numpoints; }
    int getFinalNumPoints() { return this->duplRemoved->numpointsnew; }
    int getMaxPix() { return this->maxpix; }
    int getMinPix() { return this->minpix; }
    int getImageWidth() { return this->imageWidth; }
    int getImageHeight() { return this->imageHeight; }
    const int getMaxPoints() { return this->MAX_POINTS; }

private:
    PointDef* pointDef;
    ImPointsOneCam* points;
    SubWin* swtab;
    VectorOneCam* duplRemoved;
    unsigned char* newarray;
    unsigned char* imarray;

    int minpix = 5;
    int maxpix = 20;
    int minsep = 10;
    int imlimit;
    int subwinsiz;
    const int MAX_POINTS = 5000;

    int imageWidth;
    int imageHeight;

    bool imageFindPoint();
    void imageDeletePoint();
    void subwinRead(int ix, int iy, int &ic, int &jc);
    void subwinRemoveBackground();
    void subwinFindPoint(int &ii, int &jj);
    bool imagePointPosition();
    void imageMaxPixelPosition();
    void setNull(int frax, int tilx, int fray, int tily);
    int maxval();
};

#endif