#ifndef P_CONSTR_DIST_H
#define P_CONSTR_DIST_H

#include <DataStructures/PConstraint.h>

class pConstraintDistance : public PConstraint
{
public:
   pConstraintDistance() : PConstraint()
   {
   }

   pConstraintDistance(Particle* p1, Particle* p2) : PConstraint()
   {
		init(p1, p2);
   }

   virtual void solve(vector< Vector3d >& incr, int nIterations);

   void init(Particle* p1, Particle* p2);

   float distance;

   virtual void drawFunc();
};

#endif // P_CONSTR_DIST_H