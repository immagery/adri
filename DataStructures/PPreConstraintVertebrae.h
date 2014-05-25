#ifndef PPRE_CONSTRAINT_VERTEBRAE_H 
#define PPRE_CONSTRAINT_VERTEBRAE_H

#include <PPreConstraint.h>
#include <PConstraint.h>

class PPreConstraintVertebrae : PPreConstraint
{
public:
    PPreConstraintVertebrae(Particle* z, Particle* center, Particle* dir, bool front_back) : PPreConstraint()
	{
		moving_z = z;
		plane_dir = dir;
		plane_pos = center;

		frontBackMode = front_back;
	}

	Particle* moving_z;
	Particle* plane_pos;
	Particle* plane_dir;

	bool frontBackMode;

    ~PPreConstraintVertebrae(){}

	virtual bool evaluate();
	virtual PConstraint* buildConstraint();
	virtual void drawFunc();

};

#endif // PPRE_CONSTRAINT_VERTEBRAE_H