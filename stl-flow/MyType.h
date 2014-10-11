#ifndef MYTYPE
#define MYTYPE
#include <list>
#include <set>
#include <string>
using namespace std;

#define ACSIZE  8	//8 for default
#define PI_ 3.1415926

//static float colorR[5][4] = {{0.00, 0.146, 1.00, 0.5},{ 0.463, 0.146, 0.00, 0.5},{0.146, 0.315, 0.00, 0.5},{1.00, 0.00, 0.146, 0.5},{0.00, 0.00, 0.146, 0.5}};
static float colorR[5][4] = {{0.85, 0.20, 0.146, 0.5},{0.00, 0.146, 1.00, 0.5},{0.263, 0.046, 0.00, 0.5},{0.00, 1.00, 0.146, 0.5},{0.146, 0.615, 0.30, 0.5}};
static float colorNode[6][4] = {{0.10, 0.20, 0.87, 1.0},{0.90, 0.10, 0.11, 1.0},{0.10, 0.80, 0.0, 1.0},{0.10, 0.90, 0.97, 1.0},{0.80, 0.20, 0.91, 1.0},{0.10, 0.10, 0.10,1.0}};

static float RRcolor[4] = {0.0, 0.0, 1.0, 1.0};
static float SScolor[4] = {0.8, 0.20, 0.20, 1.0};
static float RScolor[4] = {0.0, 0.0, 0.0, 0.8};

static float red[4] = {1.0, 0.0, 0.0, 1.0};
static float green[4] = {0.0, 1.0, 0.0, 1.0};
static float blue[4] = {0.0, 0.0, 1.0, 1.0};
static float gray[4] = {0.5, 0.5, 0.5, 1.0};
static float black[4] = {0.2, 0.2, 0.2, 1.0};
static float pink[4] = {1.0, 0.75, 0.80, 1.0};
static float chocolate[4] = {0.82,0.41,0.25, 1.0};
static float purple[4] = {0.63, 0.125,0.94};
static float cyan[4] = {0.0, 1.0, 1.0, 1.0};
struct vector3d{
    float x, y, z;
};

struct vector2d{
    float x,y;
};

struct vector4d{
    float x;
    float y;
    float z;
    float w;
};

class vector3d2{
public:
    vector3d vect;
    float entropy;
    float r;		//used when display tube
    float mag;
};

struct myseed{
    vector3d pos;
    float entropy;
};

struct streamLine{
    list<vector3d2> pointslist;
    list<vector3d> dirslist;
    float *vertexarray;
    float *colorarray;
    float *normalarray;
    int *indices;
    int totalsegments;
    float entropy;
    int id;
    float maxmag;
    int clusterID;
    int offset;
    bool rep;
};
#endif
