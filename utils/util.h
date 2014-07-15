#ifndef UTIL_OPS_H
#define UTIL_OPS_H

// Eigen
#include <Eigen\Core>
#include <Eigen\Geometry>

// Std lib
#include <cmath>
#include <string>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <map>

using namespace std;
using namespace Eigen;

// FLAGS FOR FASTER PROBLEM SEARCH
#define ONLY_ONE_A_MATRIX true

#define CROP_NODE_ID 100000

// Flag for verbose data in debig mode
#define VERBOSE false

string   getCurrentTime();

void GetColour(double v,double vmin,double vmax, float& r, float& g, float& b);
void GetColourGlobal(double v,double vmin,double vmax, float& r, float& g, float& b);

float Orientation(float ox, float oy, float oz, float nx, float ny, float nz );

double timelapse(clock_t clock1,clock_t clock2);

int indexOf(vector<int>& labels, int label);

double roundUtil(double x);

float sign(float v);

float det(Eigen::Vector3f u1, Eigen::Vector3f u2, Eigen::Vector3f u3);

double Deg2Rad(double deg);

double Rad2Deg(double rad);

Eigen::Quaternion<double> fromEulerAngles(double alpha, double beta, double gamma);
void toEulerAngles (const Eigen::Quaterniond& q, double& alpha, double& beta, double& gamma);
void getAxisRotationQuaternion(Eigen::Quaterniond& q, int axis, double angle);

vector<string> &split(const string &s, char delim, vector<string> &elems);

#define SMALL_NUM   0.00000001 // anything that avoids division overflow

struct Ray{
	Vector3d P0;
	Vector3d P1;
};

struct TriangleAux{
	Vector3d V0;
	Vector3d V1;
	Vector3d V2;
};

int intersect3D_RayTriangle( Ray R, TriangleAux T, Vector3d& I );
int intersect3D_RayPlane( Ray R, Vector3d& origin, Vector3d& u, Vector3d& v, Vector3d& I );
int intersect3D_RayPlane( Vector3d& rayOrigin, Vector3d& rayDir, Vector3d& planeOrigin, Vector3d& u, Vector3d& v, Vector3d& I );

double fRand(double fMin, double fMax);

float kernelFunction(float partDistance,
                     float baseDistance,
                     bool invert,
                     double K = 100,
                     double alpha = 10);

// Returns the min of each the component in a vector3d way
Vector3d minPt(Vector3d pt1, Vector3d pt2);

// Returns the max of each the component in a vector3d way
Vector3d maxPt(Vector3d pt1, Vector3d pt2);

#endif // UTIL_H
