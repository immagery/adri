#ifndef PARTICLE_POOL_H
#define PARTICLE_POOL_H

#include "DataStructures/node.h"
#include "DataStructures/Particle.h"
#include "DataStructures/PConstraint.h"
#include "DataStructures/PPreConstraint.h"

class ParticlePool : public node 
{
    public:
    ParticlePool();
    ParticlePool(unsigned int id);
    void InitDefaultValues();

    int addParticle(Vector3d pos, bool resize = false);

    void resizePool(int newSize)
    {
        pool.resize(newSize);
    }

    void drawFunc();

    // Simulation functions
    void simulate(float timeStep);

    Vector3d computeExtforces(int partIdx); // Get external forces applied to this particle
    void dampVelocities();
    void GenerateCollisions(int partId, vector< Vector3d >& incr);
    void GenerateCollisionFromPreContraints();
	void velocityUpdate();

    //Pool of particles and constraints
    int poolSize;
    vector<Particle> pool; // All the particles 

    vector<PConstraint*> modelConstraints; // Particle constraints
    vector<PConstraint*> colisionConstraints; // Particle constraints

	vector<PPreConstraint*> preColisionConstraints; // Particle constraints predefined
    
    // scene conditions
    double gravity;
	double damping;

	double collision_damping;

    int numIterations;
    double timeStep;

	int simStep;

	Vector3d worldPos;
	Vector3d worldY;
};

#endif // PARTICLE_POOL_H