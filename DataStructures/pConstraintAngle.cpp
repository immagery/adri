#include "PConstraintAngle.h"

#include <utils/utilGL.h>

#define ANGLE_EPSILON 0.0000001
#define ERROR_EPSILON 0.0000001

void pConstraintAngle::solve(vector< Vector3d >& estimation, int nIterations)
{
    if(elems.size() != 4) return;

    int idx1 = elems[0]->nodeId;
    int idx2 = elems[1]->nodeId;
	int idx3 = elems[2]->nodeId;
    int idx4 = elems[3]->nodeId;

	float sumWeights = 0;
	for(int i = 0; i< elems.size(); i++)
		sumWeights += elems[0]->weight;

	if(sumWeights == 0) return; // Todos los elementos están bloqueados

	Vector3d pt00 = estimation[idx1];
	Vector3d pt01 = estimation[idx2];
	Vector3d pt10 = estimation[idx3];
	Vector3d pt11 = estimation[idx4];

	Vector3d n1 = (pt01-pt00).normalized();
	Vector3d n2 = (pt11-pt10).normalized();
	double dotProd = n1.dot(n2);

	Vector3d q0,q1,q2,q3;
	double constraint;

	if(amode == PERPENDICULAR)
	{
		q0 = -n2; 
		q1 =  n2;
		q2 = -n1;
		q3 =  n1;
		constraint = dotProd;

		if(fabs(constraint)< ERROR_EPSILON) return;
	}
	else
	{
		double acosComp = 0;
		if(fabs(dotProd - 1) < ANGLE_EPSILON)
			acosComp = 0;
		else if(fabs(dotProd + 1) < ANGLE_EPSILON) 
			acosComp = M_PI;
		else
			acosComp = acos(dotProd);
			
		constraint = acosComp-angle;

		if(fabs(constraint)< ERROR_EPSILON) return;

		q0 = -n2 *  (-1/sqrt((1-(dotProd*dotProd))));
		q1 =  n2 *  (-1/sqrt((1-(dotProd*dotProd))));
		q2 = -n1 *  (-1/sqrt((1-(dotProd*dotProd))));
		q3 =  n1 *  (-1/sqrt((1-(dotProd*dotProd))));
	}
	
	double jac_sum = elems[0]->weight*pow(q0.norm(),2) + 
					 elems[1]->weight*pow(q1.norm(),2) + 
					 elems[2]->weight*pow(q2.norm(),2) + 
					 elems[3]->weight*pow(q3.norm(),2) ;


	float s = constraint / (jac_sum);

	Vector3d incr0 = -s*elems[0]->weight * q0;
	Vector3d incr1 = -s*elems[1]->weight * q1;
	Vector3d incr2 = -s*elems[2]->weight * q2;
	Vector3d incr3 = -s*elems[3]->weight * q3;

	float RealStiffness = 1-pow((1-stiffness),1.0/nIterations);
	
	estimation[idx1] += incr0*RealStiffness;
	estimation[idx2] += incr1*RealStiffness;
	estimation[idx3] += incr2*RealStiffness;
	estimation[idx4] += incr3*RealStiffness;

	/*
    float weightSum = elems[0]->weight + elems[1]->weight;
    float norm = (estimation[idx1]-estimation[idx2]).norm();
    float sw = ( norm - distance)/(weightSum);
    Vector3d incr = sw*(estimation[idx1]-estimation[idx2])/norm;

    // dos fuerzas en la misma direccion
    estimation[idx1] -= elems[0]->weight*incr;
    estimation[idx2] += elems[1]->weight*incr;
	*/
}

void pConstraintAngle::init(Particle* o1, Particle* p1, Particle* o2, Particle* p2, float _angle)
{
    elems.resize(4);
    elems[0] = o1;
    elems[1] = p1;
	elems[2] = o2;
	elems[3] = p2;

	if(_angle == 90)
		amode = PERPENDICULAR;
	else
	{
		amode = ANGLE;
		angle = _angle/360*2*M_PI;
	}

}

void pConstraintAngle::drawFunc()
{
    if(elems.size() == 0) return;

    glColor3f(0.0, 1.0, 0);
	glBegin(GL_LINES);
		glVertex3d(elems[0]->position.x(), elems[0]->position.y(), elems[0]->position.z());
        glVertex3d(elems[1]->position.x(), elems[1]->position.y(), elems[1]->position.z());
		glVertex3d(elems[2]->position.x(), elems[2]->position.y(), elems[2]->position.z());
        glVertex3d(elems[3]->position.x(), elems[3]->position.y(), elems[3]->position.z());
	glEnd();
}
