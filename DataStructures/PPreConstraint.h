#ifndef PPRECONSTRAINT_H 
#define PPRECONSTRAINT_H

#include <DataStructures/PConstraint.h>

class PPreConstraint : public PConstraint
{
public:
    PPreConstraint(): PConstraint() {} 
    ~PPreConstraint(){}

	virtual bool evaluate();
	virtual PConstraint* buildConstraint();
};

#endif // PPRECONSTRAINT_H