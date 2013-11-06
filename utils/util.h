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

#define VERBOSE false

string   getCurrentTime();

void GetColour(double v,double vmin,double vmax, float& r, float& g, float& b);
void GetColourGlobal(double v,double vmin,double vmax, float& r, float& g, float& b);

float Orientation(float ox, float oy, float oz, float nx, float ny, float nz );

double timelapse(clock_t clock1,clock_t clock2);

int indexOf(vector<int>& labels, int label);

double round(double x);

double sign(double v);

double det(Eigen::Vector3d u1, Eigen::Vector3d u2, Eigen::Vector3d u3);

double Deg2Rad(double deg);

double Rad2Deg(double rad);

Eigen::Quaternion<double> fromEulerAngles(double alpha, double beta, double gamma);
void toEulerAngles (const Eigen::Quaterniond& q, double& alpha, double& beta, double& gamma);
void getAxisRotationQuaternion(Eigen::Quaterniond& q, int axis, double angle);

#endif // UTIL_H
