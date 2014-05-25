#include "particlePool.h"

#define VELOCITY_CUT 0.001

void ParticlePool::InitDefaultValues()
{
    poolSize = 0;
    pool.size();    
    gravity = -9.8;
    numIterations = 5;
    timeStep = 0.1;
	damping = 0.99;

	collision_damping = 0.65;

	simStep = 0;

    colisionConstraints.clear();
    modelConstraints.clear();
}

ParticlePool::ParticlePool() : node()
{
    InitDefaultValues();
}

ParticlePool::ParticlePool(unsigned int id) : node(id)
{
    InitDefaultValues();
}

int ParticlePool::addParticle(Vector3d pos, bool resize)
{
    pool[poolSize].init(poolSize, pos);
    poolSize++;
    return poolSize-1;
}

void ParticlePool::drawFunc()
{
    for(int i = 0; i< poolSize; i++)
        pool[i].drawFunc();

    for(int i = 0; i< modelConstraints.size(); i++)
        modelConstraints[i]->drawFunc();

    for(int i = 0; i< colisionConstraints.size(); i++)
        colisionConstraints[i]->drawFunc();

    for(int i = 0; i< preColisionConstraints.size(); i++)
        preColisionConstraints[i]->drawFunc();
}


void ParticlePool::simulate(float timeStep)
{

	simStep++;
    // 1. Compute new velocity
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
        pool[partIdx].v += timeStep*pool[partIdx].weight*computeExtforces(partIdx)*pool[partIdx].onlyRestrictionForces;
    }

    // 2. DampVelocities
    dampVelocities();

    //3. Compute estimations and assign as a init solution
     vector< Vector3d > dP(poolSize);
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
        Particle& pt = pool[partIdx];
        dP[partIdx] = pt.position + timeStep*pt.v;
	}

	colisionConstraints.clear();

    //4. COLLISIONS EVALUATION
	// a. Generate collision constraints
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
        GenerateCollisions(partIdx, dP);
    }

	// b. Evaluate each preconstraint
	GenerateCollisionFromPreContraints();
	
    //5. Solve the constraints
    for(int iter = 0; iter < numIterations; iter++)
    {
        // a. collision constraints
        for(int colRep = 0; colRep< 1; colRep++)
		{
			for(int collIdx = 0; collIdx < colisionConstraints.size(); collIdx++)
			{
				colisionConstraints[collIdx]->solve(dP, numIterations);
			}
		}

        // b. model constraints
        for(int constrIdx = 0; constrIdx < modelConstraints.size(); constrIdx++)
        {	
            modelConstraints[constrIdx]->solve(dP, numIterations);

			/*for(int partIdx = 0; partIdx < poolSize; partIdx++)
			{
				if(dP[partIdx].x() != dP[partIdx].x())
					int stop = 0;
			}*/
        }

    }

    //6. Update with new velocity and positions
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
        Particle& pt = pool[partIdx];
        pt.v = (dP[partIdx] - pt.position)/timeStep;
        pt.position = dP[partIdx];
    }

    // 2. DampVelocities
    velocityUpdate();
}

Vector3d ParticlePool::computeExtforces(int partIdx)
{
    //TODEBUG
    // Now just gravity
    return Vector3d(0,gravity,0);
}

void ParticlePool::dampVelocities()
{
	
	// TO_DEBUG
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
        Particle& pt = pool[partIdx];
		pt.v *= damping;
		
		if(pt.v.norm() < VELOCITY_CUT)
			pt.v = Vector3d(0,0,0);
	}

	return;
	


	// PROPOSED FOR PBD
	// 1-2. center of masses and its velocity
	Vector3d cm(0,0,0);
	Vector3d vm(0,0,0);
	float massSum = 0;
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
		cm += pool[partIdx].position*pool[partIdx].mass;
		vm += pool[partIdx].v*pool[partIdx].mass;
		massSum += pool[partIdx].mass;
	}
	cm /= massSum;
	vm /= massSum;

	
	//3. Angular momentum
	Vector3d L(0,0,0);
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
		L += (pool[partIdx].position-cm).cross(pool[partIdx].v*pool[partIdx].mass);
	}

	
	//4. Linear momentum
	Matrix3d I;
	I.fill(0);
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
		Matrix3d r;
		r.fill(0);
		Vector3d dist = pool[partIdx].position-cm;
		r(0,1) = -dist.z();
		r(0,2) =  dist.y();
		r(1,0) =  dist.z();
		r(1,2) = -dist.x();
		r(2,0) = -dist.y();
		r(2,1) =  dist.x();
		I += r*r.transpose()*pool[partIdx].mass;
	}

	
	//5. Angular velocity
	Vector3d omega = I.inverse()*L;
    for(int partIdx = 0; partIdx < poolSize; partIdx++)
    {
		Vector3d dist = pool[partIdx].position-cm;
		Vector3d incrVelocity = vm + omega.cross(dist) - pool[partIdx].v;
		pool[partIdx].v += incrVelocity * damping * pool[partIdx].weight;
	}

}

void ParticlePool::GenerateCollisions(int partId, vector< Vector3d >& incr)
{
	//if(pool[partId].onlyRestrictionForces < 1.0) return;
	Vector3d desplPart = incr[partId] - worldPos;

	// Colision con el suelo
	if(desplPart.normalized().dot(worldY) < 0)
	{
		Vector3d crashVector = desplPart.dot(worldY) * worldY;

		incr[partId] -= crashVector;

		// Colision con el suelo-> corregimos la trayectoria
		Vector3d dir = incr[partId] - pool[partId].position;
		//dir.y() *= -1;

		float neg = pool[partId].v.dot(worldY);

		if(neg < 0)
		{
			pool[partId].v -= neg*worldY;
		}

		/*
		// TODEBUG
		dir.y() *= 0;

		//if(fabs(pool[partId].position.y()) > VELOCITY_CUT)
		incr[partId] = pool[partId].position + dir;

		// Cambiamos la velocidad
		pool[partId].v *= -collision_damping;
		*/

		//if(fabs(pool[partId].v.norm()) < VELOCITY_CUT)
		//	pool[partId].v.fill(0);
	}
}

void ParticlePool::GenerateCollisionFromPreContraints()
{
	for(int pc = 0; pc < preColisionConstraints.size(); pc++)
	{
		if(preColisionConstraints[pc]->evaluate())
		{
			colisionConstraints.push_back(preColisionConstraints[pc]->buildConstraint());
		}
	}
}

void ParticlePool::velocityUpdate()
{
	/*
	for(int partIdx = 0; partIdx < (poolSize/5)-5; partIdx++)
    {
		int idxPt = partIdx*5;
		int idxDir = (partIdx+1)*5;

		Vector3d dir = pool[idxDir].position-pool[idxPt].position;
		Vector3d n = dir.normalized();

		// He hecho una proyección un poco bestia, quizas
		// se puede ponderar de alguna manera.
		Vector3d vProj = pool[idxPt].v.dot(n)*n;

		//pool[idxPt].v = vProj;
	}
	*/
}