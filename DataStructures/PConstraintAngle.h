#ifndef P_CONSTR_ANGLE_H
#define P_CONSTR_ANGLE_H

#include <DataStructures/PConstraint.h>

enum angularMode{ANGLE = 0, PERPENDICULAR};

class pConstraintAngle : public PConstraint
{
public:
   pConstraintAngle() : PConstraint()
   {
   }

   pConstraintAngle(Particle* o1, Particle* p1, Particle* o2, Particle* p2, float _angle) : PConstraint()
   {
		init(o1, p1, o2, p2, _angle);
   }


   virtual void solve(vector< Vector3d >& incr, int nIterations);

   void init(Particle* o1, Particle* p1, Particle* o2, Particle* p2, float _angle);

   float angle;

   angularMode amode;

   virtual void drawFunc();
};

#endif // P_CONSTR_ANGLE_H