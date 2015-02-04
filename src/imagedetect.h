#ifndef DETECTPARTICLES
#define DETECTPARTICLES

#include <algorithm>

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
    PointDef(int height) : xlast(0), ylast(0), maxx(0), maxy(0), jlow(0), jhigh(0), ileft(0), iright(0) {
        stripes = new Stripe[height];
    }
    ~PointDef() {
        delete[] stripes;
    }
    Stripe* stripes;
    ImPoint pnt;
    ImPoint newpnt;
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

class ImageDetect {
public:
    ImageDetect(int imageWidth, int imageHeight, int imlimit = 0, int subwinsiz = 0);
    ~ImageDetect();

    void imageDetectPoints();

    void setImage(unsigned char* imarray) { this->imarray = imarray; }
    unsigned char* getNewImage() { return this->newarray; }
    void setThreshold(int imlimit) { this->imlimit = imlimit; }
    int getNumPoints() { return this->points->numpoints; }
    ImPoint* getPoints() { return this->points->table; }
    void setSubwinSize(int subwinsiz);

private:
    PointDef* pointDef;
    ImPointsOneCam* points;
    SubWin* swtab;
    unsigned char* newarray;
    unsigned char* imarray;

    int minpix = 5;
    int maxpix = 20;
    int imlimit;
    int subwinsiz;
    const int MAX_POINTS = 5000;
    
    int imageWidth;
    int imageHeight;

    void imageRemoveBackground();
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