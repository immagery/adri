#ifndef UTILGL_H
#define UTILGL_H

#ifdef WIN32
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef WIN64
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef MACOSX
#include <gl.h>
#include <glu.h>
#endif

#include <utils/util.h>
#include <Eigen\Core>

void glVertex(Vector3d v);
void drawCircle(int res, double r);
void drawSphere(int res, double r);
void drawTriCircle(int res, double r);

void drawPointLocator(Vector3d pt, float size, int type);
void drawPointLocator(Vector3d pt, float size, bool spot);

void drawAxisHandle(double r);
void drawAxisHandleWithExtremes(double r);

void drawExpansionBall(bool selected, float r);

#endif // UTILGL_H
