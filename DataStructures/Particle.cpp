#include "Particle.h"

#include <utils/utilGL.h>
#include <DataStructures/Geometry.h> 

void Particle::initDefaults()
{
    init(0, Vector3d(0,0,0));
    mode = RENDER_POINT;
	color = Vector3f(1,1,1);
}

void Particle::init(int id, Vector3d& pos)
{
    nodeId = id;
    position = pos;
    v = Vector3d(0,0,0);
    mass = 1.0;
    weight = 1.0;
	onlyRestrictionForces = 1.0;

}

Particle::Particle() : node() 
{
    initDefaults();
}

Particle::Particle(unsigned int id) : node(id)
{
    initDefaults();
}


Particle::~Particle(void)
{
}


void Particle::drawFunc() 
{
	glColor3f(color.x(), color.y(), color.z());
    if(mode == RENDER_POINT)
    {
        glPointSize(8);
	    glBegin(GL_POINTS);
		    glVertex3d(position.x(), position.y(), position.z());
	    glEnd();
    }
}