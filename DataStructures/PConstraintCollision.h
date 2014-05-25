#ifndef P_CONSTR_COLLISION_H
#define P_CONSTR_COLLISION_H

#include <DataStructures/PConstraint.h>

class pConstraintCollision : public PConstraint
{
public:
   pConstraintCollision() : PConstraint()
   {
   }

   virtual void solve(vector< Vector3d >& incr);

   void init(Particle* p1, Particle* p2);

   float distance;

   virtual void drawFunc();
};

#endif // P_CONSTR_COLLISION_H