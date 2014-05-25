#include "PPreConstraintVertebrae.h" 
#include <utils/utilGL.h>

#include <DataStructures/PConstraintAtachment.h>

bool PPreConstraintVertebrae::evaluate()
{
	Vector3d z   = moving_z->position  - plane_pos->position;
	Vector3d dir = plane_dir->position - plane_pos->position;
	dir.normalize();

	bool buildConstraint = false; 

	if(frontBackMode)
		buildConstraint = (z.normalized().dot(dir) > 0) && (moving_z->v.normalized().dot(dir) > 0);
	else
		buildConstraint = (z.normalized().dot(dir) < 0) && (moving_z->v.normalized().dot(dir) < 0);

	return buildConstraint;
}

PConstraint* PPreConstraintVertebrae::buildConstraint()
{
	Vector3d z   = (moving_z->position  - plane_pos->position).normalized();
	Vector3d dir;
	
	if(frontBackMode)
		dir = (plane_dir->position - plane_pos->position).normalized();
	else
		dir = -(plane_dir->position - plane_pos->position).normalized();

	Vector3d normalizedV = -moving_z->v.normalized();

	Vector3d pc_prima = -dir.dot(z)*dir;
	Vector3d pc = normalizedV.dot(pc_prima)*normalizedV + moving_z->position;

	pConstraintAtachment* d = new pConstraintAtachment(moving_z, pc);

	return d;
}

void PPreConstraintVertebrae::drawFunc()
{
    glColor3f(0.7, 0.7, 0);
	glBegin(GL_LINES);
		glVertex3d(moving_z->position.x(), moving_z->position.y(), moving_z->position.z());
        glVertex3d(plane_pos->position.x(), plane_pos->position.y(), plane_pos->position.z());
	glEnd();
}