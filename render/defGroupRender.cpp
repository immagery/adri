#include <render/defGorupRender.h>
#include <utils/utilGL.h>

#define NORMALR 0.5
#define NORMALG 0
#define NORMALB 0.5

#define SELR 0.1
#define SELG 0.8
#define SELB 0.1

#define SUBSELR 1.0
#define SUBSELG 1.0
#define SUBSELB 1.0

// render size
#define DEFAULT_SIZE 0.05

#include <DataStructures/Skeleton.h>
#include <DataStructures/Scene.h>

using namespace Eigen;


void useModelMatrix(Quaterniond rot, Vector3d pos)
{
    //glPushMatrix();
	Eigen::Matrix4d transformMatrix;
	Eigen::Matrix3d rotateMatrix;
	rotateMatrix = rot.toRotationMatrix();
	transformMatrix << rotateMatrix(0,0) , rotateMatrix(0,1) , rotateMatrix(0,2), pos.x(),
						rotateMatrix(1,0) , rotateMatrix(1,1) , rotateMatrix(1,2), pos.y(),
						rotateMatrix(2,0) , rotateMatrix(2,1) , rotateMatrix(2,2), pos.z(),
						0,				0,				0,			1;

	transformMatrix.transposeInPlace();

	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), 
									  transformMatrix(0,2), transformMatrix(0,3),
									  transformMatrix(1,0), transformMatrix(1,1), 
									  transformMatrix(1,2), transformMatrix(1,3),
									  transformMatrix(2,0), transformMatrix(2,1), 
									  transformMatrix(2,2), transformMatrix(2,3),
								      transformMatrix(3,0), transformMatrix(3,1), 
									  transformMatrix(3,2), transformMatrix(3,3)
									};

	glMultMatrixd(multiplyingMatrix);
	//glPopMatrix();
}

void drawOpaqueStickDefGroup(float length)
{
	if(length == 0)
		return;

	float relation = length*0.2;
	float midRelation = relation/2.0;
	Vector3d pts[4];
	pts[0] = Vector3d( midRelation,  -midRelation,  -midRelation);
	pts[1] = Vector3d( midRelation,  midRelation,  -midRelation);
	pts[2] = Vector3d( midRelation,  midRelation,  midRelation);
	pts[3] = Vector3d( midRelation,  -midRelation,  midRelation);
	
    glBegin(GL_TRIANGLES);
	for(int pt = 0; pt < 4; pt++)
	{
		glVertex3d(0,0,0);
		glVertex3d(pts[(pt+1)%4].x(),pts[(pt+1)%4].y(),pts[(pt+1)%4].z());
		glVertex3d(pts[pt].x(),pts[pt].y(),pts[pt].z());
		
		glVertex3d(length,0,0);
		glVertex3d(pts[pt].x(),pts[pt].y(),pts[pt].z());
		glVertex3d(pts[(pt+1)%4].x(),pts[(pt+1)%4].y(),pts[(pt+1)%4].z());
	}

    glEnd();
}

void drawStickDefGroup(float length)
{
	float relation = length*0.2;
	float midRelation = relation/2.0;
	Vector3d pts[4];
	pts[0] = Vector3d( midRelation,  -midRelation,  -midRelation);
	pts[1] = Vector3d( midRelation,  midRelation,  -midRelation);
	pts[2] = Vector3d( midRelation,  midRelation,  midRelation);
	pts[3] = Vector3d( midRelation,  -midRelation,  midRelation);
	
	glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

	for(int pt = 0; pt < 4; pt++)
	{
		glVertex3d(pts[pt].x(),pts[pt].y(),pts[pt].z());
		glVertex3d(pts[(pt+1)%4].x(),pts[(pt+1)%4].y(),pts[(pt+1)%4].z());

		glVertex3d(0,0,0);
		glVertex3d(pts[pt].x(),pts[pt].y(),pts[pt].z());

		glVertex3d(length,0,0);
		glVertex3d(pts[pt].x(),pts[pt].y(),pts[pt].z());
	}

    glEnd();
    glEnable(GL_LIGHTING);
}

void DefGroupRender::drawWithNames()
{
	DefGroup* g = group;

	double maxRelation = 0;

	// Render the bone shape
	glColor3f(1.0,1.0,1.0);
	for(int groupIdx = 0; groupIdx < g->relatedGroups.size(); groupIdx++)
	{
		DefGroup* child = g->relatedGroups[groupIdx];
		glPushMatrix();
		useModelMatrix(child->transformation->parentRot, g->transformation->translation);
		Vector3d line = child->transformation->translation - g->transformation->translation;

		drawOpaqueStickDefGroup(line.norm());

		maxRelation = max(line.norm()*0.2, maxRelation);
		glPopMatrix();
	}

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	useModelMatrix(g->transformation->rotation, g->transformation->translation);
	
	if(g->relatedGroups.size() == 0)
	{
		drawSphere(10, 1.0);
		maxRelation = 1.0;
	}

	glPopMatrix();
}

//DEFGROUP_RENDERING
void DefGroupRender::drawFunc()
{   
	DefGroup* g = group;

	double maxRelation = 0;

	glLineWidth(1);

	// Render the bone shape
	if(selected)
		glColor3f(0.5,0.25,0.5);
	else if(highlight)
	{
		//printf("highlight bone: %d - %s\n", g->nodeId, g->sName.c_str());
		glColor3f(0.0,1.0,1.0);
	}
	else
		glColor3f(1.0,1.0,1.0);

	for(int groupIdx = 0; groupIdx < g->relatedGroups.size(); groupIdx++)
	{
		DefGroup* child = g->relatedGroups[groupIdx];
		glPushMatrix();
		useModelMatrix(child->transformation->parentRot, g->transformation->translation);
		Vector3d line = child->transformation->translation - g->transformation->translation;
		drawStickDefGroup(line.norm());

		maxRelation = max(line.norm()*0.2, maxRelation);
		glPopMatrix();
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	useModelMatrix(g->transformation->rotation, g->transformation->translation);
	
	if(g->relatedGroups.size() == 0)
	{
		drawTriCircle(20, scene::drawingNodeStdSize*0.5);
		maxRelation = 1.0;
	}

	drawAxisHandle(maxRelation*scene::drawingNodeStdSize);

	if(scene::drawDefNodes)
	{
		for(int defIdx = 0; defIdx < group->deformers.size(); defIdx++)
		{
			if(group->deformers[defIdx].freeNode) continue;

			int type = 0;
			if(group->deformers[defIdx].dirtyFlag) 
			{	type = 1;
				if(group->deformers[defIdx].segmentationDirtyFlag) 
					type = 2;
			}

			drawPointLocator(group->deformers[defIdx].relPos, maxRelation*0.25,type);
		}
	}

	glPopMatrix();
}