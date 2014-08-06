#ifndef PCONSTRAINT_H 
#define PCONSTRAINT_H
 
#include <DataStructures/Particle.h>

enum contraint_type{EQUALITY = 0, INEQUALITY};

class PConstraint 
{
public:
    PConstraint()
	{
		stiffness = 1.0;
		elems.clear();
		type = EQUALITY;
		cardinality = 0;
	}

    ~PConstraint(){}

    int cardinality;
    vector <Particle*> elems;
    float stiffness;
    contraint_type type;

    virtual void solve(vector< Vector3d >& incr, int numIterations);

    virtual void drawFunc() {}

};

#endif // PCONSTRAINT_H