#ifndef P_CONSTR_ATACHMENT_H
#define P_CONSTR_ATACHMENT_H

#include <DataStructures/PConstraint.h>

class pConstraintAtachment : public PConstraint
{
public:
   pConstraintAtachment() : PConstraint()
   {
   }

   pConstraintAtachment(Particle* p1, Vector3d atachment) : PConstraint()
   {
		init(p1, atachment);
   }

   virtual void solve(vector< Vector3d >& incr, int nIterations);

   void init(Particle* p1, Vector3d atachment);

   Vector3d atachmentPoint;

   virtual void drawFunc();
};

#endif // P_CONSTR_ATACHMENT_H