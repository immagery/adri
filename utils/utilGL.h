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

//void joint::update()
//{
    // Calcular matrix de transformacion
//    dirtyFlag = false;
//}

static void glVertex(Eigen::Vector3d v) { glVertex3d(v.x(), v.y(), v.z()); }

static void drawCircle(int res, double r)
{

    if (res < 4)
        res = 4;

    double passAngle = (2*M_PI )/ res;

    glBegin(GL_LINE_LOOP);

    for(int i = 0; i< res; i++)
        glVertex3d(cos(passAngle*i)*r,sin(passAngle*i)*r,0);

    // repetimos el punto inicial para cerrar el c�rculo.
    glVertex3d(r,0,0);

    glEnd();

}

static void drawTriCircle(int res, double r)
{
    glDisable(GL_LIGHTING);
    glPushMatrix();
    //glColor3f(1.0,0,0);
    drawCircle(res, r);

    glRotatef(90, 0,1,0); // rotar en y
    //glColor3f(0,1.0,0);
    drawCircle(res,r);
    glPopMatrix();

    glPushMatrix();
    //glColor3f(0,0,1.0); // rotar en x
    glRotatef(90, 1,0,0);
    drawCircle(res, r);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

static void drawPointLocator(Eigen::Vector3d pt, float size, bool spot)
{
    glDisable(GL_LIGHTING);
    if(spot)
    {
        glColor3f((GLfloat)0.5, (GLfloat)0.1, (GLfloat)0.1);
        size = size*2;
    }
    else
    {
        glColor3f((GLfloat)0.1, (GLfloat)0.4, (GLfloat)0.1);
    }

    glBegin(GL_LINES);
    // queda reconstruir el cubo y ver si se pinta bien y se ha calculado correctamente.
    glVertex3f(pt.x()+size, pt.y(), pt.z());
    glVertex3f(pt.x()-size, pt.y(), pt.z());

    glVertex3f(pt.x(), pt.y()+size, pt.z());
    glVertex3f(pt.x(), pt.y()-size, pt.z());

    glVertex3f(pt.x(), pt.y(), pt.z()+size);
    glVertex3f(pt.x(), pt.y(), pt.z()-size);

    glEnd();
    glEnable(GL_LIGHTING);
}

static void drawAxisHandle(double r)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    glColor3f(1.0,0,0);
    glVertex3d(0,0,0);
    glVertex3d(r,0,0);

    glColor3f(0,1.0,0);
    glVertex3d(0,0,0);
    glVertex3d(0,r,0);

    glColor3f(0,0,1.0);
    glVertex3d(0,0,0);
    glVertex3d(0,0,r);

    glEnd();
    glEnable(GL_LIGHTING);
}

static void drawExpansionBall(bool selected, float r)
{
    glDisable(GL_LIGHTING);
    //float blend = 0.1;
    //glEnable(GL_BLEND);

    glPushMatrix();

    if(!selected)
        glColor3f((GLfloat)0.9,(GLfloat)0.5,(GLfloat)0.5);
    else
        glColor3f((GLfloat)0.5,(GLfloat)0.9,(GLfloat)0.5);

    GLUquadricObj *quadric;
    quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_LINE );
    gluSphere( quadric , r , 15 , 10 );

    glPopMatrix();
    //glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

#endif // UTILGL_H
