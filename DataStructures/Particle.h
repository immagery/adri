#ifndef PARTICLE_H
#define PARTICLE_H

#include "DataStructures/node.h"
#include "DataStructures/axis.h"

enum renderMode {RENDER_POINT = 0, RENDER_SPHERE, RENDER_SPRITE};


// de momento tenemos aquí una particula generica, pero quizás conviene adaptarlo
// segun las necesidades de cada momento.

class Particle : public node {

public:
    Particle();
    Particle(unsigned int id);
	~Particle();

    void initDefaults();
    void init(int id, Vector3d& pos);

	// Render functions
    void drawFunc();
	void setColor(float r, float g, float b) {color[0] = r; color[1] = g; color[2] = b;}

    // Basic atributes.
    Vector3d position;
    Vector3d v;
    float mass;
    float weight;

	float onlyRestrictionForces;

    // Atributes
    axis orientation;


    // rendering info
    renderMode mode;
	Vector3f color;

};

#endif // PARTICLE_H