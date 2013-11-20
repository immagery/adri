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

	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), transformMatrix(0,2), transformMatrix(0,3),
									  transformMatrix(1,0), transformMatrix(1,1), transformMatrix(1,2), transformMatrix(1,3),
									  transformMatrix(2,0), transformMatrix(2,1), transformMatrix(2,2), transformMatrix(2,3),
								      transformMatrix(3,0), transformMatrix(3,1), transformMatrix(3,2), transformMatrix(3,3)
									};

	glMultMatrixd(multiplyingMatrix);
	//glPopMatrix();
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

/*
void drawLine(double x, double y, double z)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    glVertex3d(0,0,0);
    glVertex3d(x,y,z);

    glEnd();
    glEnable(GL_LIGHTING);
}

void drawBone(double l, double r)
{

    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_LOOP);

    glVertex3d(r,r,r);
    glVertex3d(r,-r,r);
    glVertex3d(r,-r,-r);
    glVertex3d(r,r,-r);
    glVertex3d(r,r,r);  // repetimos el punto inicial para cerrar el círculo.

    glEnd();

    glBegin(GL_LINES);

    glVertex3d(r,r,r);
    glVertex3d(l,0,0);

    glVertex3d(r,-r,r);
    glVertex3d(l,0,0);

    glVertex3d(r,-r,-r);
    glVertex3d(l,0,0);

    glVertex3d(r,r,-r);
    glVertex3d(l,0,0);

    glEnd();
    glEnable(GL_LIGHTING);

}

void drawBoneStick(float radius, Eigen::Vector3d& pos)
{
	drawLine(0,0,0, pos.x(), pos.y(), pos.z());
	//drawLine(1,0,0, pos.X(), pos.Y(), pos.Z());
	//drawLine(1,0,0, pos.X(), pos.Y(), pos.Z());
}
*/

//JOINT
void DefGroupRender::drawFunc()
{   
	DefGroup* g = group;

    //if(jt->father)
    //{
     //   if(jt->father->shading->selected)
     //       glColor3f((GLfloat)SELR,(GLfloat)SELG,(GLfloat)SELB);
     //   else
     //       glColor3f(NORMALR,NORMALG,NORMALB);
	//}

	double maxRelation = 0;

	// Render the bone shape
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
	drawAxisHandle(maxRelation);
	for(int defIdx = 0; defIdx < group->deformers.size(); defIdx++)
		drawPointLocator(group->deformers[defIdx].relPos, maxRelation*0.25, false);

	glPopMatrix();


	/*
	Eigen::Matrix4f transformMatrix = jt->world;


	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), transformMatrix(0,2), transformMatrix(0,3),
										transformMatrix(1,0), transformMatrix(1,1), transformMatrix(1,2), transformMatrix(1,3),
										transformMatrix(2,0), transformMatrix(2,1), transformMatrix(2,2), transformMatrix(2,3),
										transformMatrix(3,0), transformMatrix(3,1), transformMatrix(3,2), transformMatrix(3,3)
									};

	glMultMatrixd(multiplyingMatrix);

	

    // Pintamos un tri-círculo
    if(selected)
        glColor3f((GLfloat)SELR,(GLfloat)SELG,(GLfloat)SELB);
    else
        glColor3f(NORMALR,NORMALG,NORMALB);

    // Pintamos 3 los círculos
    drawTriCircle(12, jointSize);

    // Pintamos los ejes del hueso
    drawAxisHandle(jointSize*25);

	// Draw twist vector
	if(jt->childs.size() == 1)
	{
		glDisable(GL_LIGHTING);
		
		glLineWidth(5);
		glBegin(GL_LINES);

		glColor3f(1.0,1.0,1.0);
		glVertex3d(0,0,0);
		glVertex3d(jt->childs[0]->twist.x()*10,jt->childs[0]->twist.y()*10,jt->childs[0]->twist.z()*10);

		glEnd();
		glLineWidth(1);
		glEnable(GL_LIGHTING);
	}

    // Pintamos la pelota de expansion
    //drawExpansionBall(selected, (float)(DEFAULT_SIZE*2*jt->expansion));
	//for(unsigned int i = 0; i< jt->nodes.size(); i++)
    //{
    //    drawDeformer(jt->childs[i]->drawFunc());
    //}
	
    glPopMatrix();*/
}

void DefGroupRender::computeRestPos(joint* jt) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	computeRestPosRec(jt);
	glPopMatrix();
}

void DefGroupRender::computeRestPosRec(joint* jt, joint* father)
{
	/*
	Eigen::Matrix3d rotationMatrix;
	if(!father)
	{
		jt->rTranslation = jt->pos;
		jt->rRotation = jt->qOrient * jt->qrot;

		rotationMatrix = jt->rRotation.toRotationMatrix();

		jt->rTwist = Quaterniond::Identity();
	}
	else
	{
		jt->rTranslation = father->rTranslation + 
						   father->rRotation._transformVector(jt->pos);
		
		jt->rRotation =  father->rRotation * jt->qOrient * jt->qrot;

		rotationMatrix = ( jt->qOrient * jt->qrot ).toRotationMatrix();
		//rotationMatrix = jt->rRotation.toRotationMatrix();

		// TwistComputation:
		Vector3d axis = jt->translation - father->translation;
		axis.normalize();

		Vector3d tempRestRot = father->rRotation._transformVector(axis);
		Quaterniond localRotationChild =  jt->qOrient * jt->qrot;
		Quaterniond localRotationChildRest =  jt->restRot;
		Vector3d referenceRest = localRotationChildRest._transformVector(tempRestRot);
		Vector3d referenceCurr = localRotationChild._transformVector(tempRestRot);

		Quaterniond nonRollrotation;
		nonRollrotation.setFromTwoVectors(referenceRest, referenceCurr);
		
		// Ejes Locales
		jt->rTwist = localRotationChild*localRotationChildRest.inverse()*nonRollrotation.inverse();
	}

	jt->restPos = jt->pos;
	jt->restRot = jt->qOrient * jt->qrot;


	//Eigen::Matrix3d rotationMatrix;
	//rotationMatrix = jt->rRotation.toRotationMatrix();

	Eigen::Matrix4f transformMatrix2;
	transformMatrix2 << rotationMatrix(0,0) , rotationMatrix(0,1) , rotationMatrix(0,2), jt->pos[0],
					   rotationMatrix(1,0) , rotationMatrix(1,1) , rotationMatrix(1,2), jt->pos[1],
					   rotationMatrix(2,0) , rotationMatrix(2,1) , rotationMatrix(2,2), jt->pos[2],
						0.0,				0.0,				0.0,			1.0;

	transformMatrix2.transposeInPlace();
	jt->iT = transformMatrix2.inverse().transpose();
	
	jt->worldPosition = jt->rTranslation;

	for(unsigned int i = 0; i< jt->childs.size(); i++)
    {
        computeRestPosRec(jt->childs[i], jt);
    }

	return;

    glPushMatrix();

    //glTranslated(jt->pos.X(),jt->pos.Y(),jt->pos.Z());

	//double orientAlpha, orientBeta, orientGamma;
	//jt->qOrient.ToEulerAngles(orientAlpha, orientBeta, orientGamma);

	Eigen::Matrix3d orientMatrix;
	Eigen::Matrix3d rotateMatrix;
	//jt->qOrient.ToMatrix(orientMatrix);
	//jt->qrot.ToMatrix(rotateMatrix);
	Eigen::Matrix3d oriRot =  (rotateMatrix * orientMatrix);

	Eigen::Matrix4d transformMatrix;
	transformMatrix << oriRot(0,0) , oriRot(0,1) , oriRot(0,2), jt->pos.x(),
						oriRot(1,0) , oriRot(1,1) , oriRot(1,2), jt->pos.y(),
						oriRot(2,0) , oriRot(2,1) , oriRot(2,2), jt->pos.z(),
						0,				0,				0,			1;


	//glRotatef((GLfloat)Rad2Deg(orientGamma),0,0,1);
    //glRotatef((GLfloat)Rad2Deg(orientBeta),0,1,0);
    //glRotatef((GLfloat)Rad2Deg(orientAlpha),1,0,0);
	
	//glRotatef((GLfloat)jt->orientJoint.Z(),0,0,1);
    //glRotatef((GLfloat)jt->orientJoint.Y(),0,1,0);
    //glRotatef((GLfloat)jt->orientJoint.X(),1,0,0);

	//double alpha, beta, gamma;
	//jt->qrot.ToEulerAngles(alpha, beta, gamma);

    //glRotatef((GLfloat)Rad2Deg(gamma),0,0,1);
    //glRotatef((GLfloat)Rad2Deg(beta),0,1,0);
    //glRotatef((GLfloat)Rad2Deg(alpha),1,0,0);

    //glRotatef((GLfloat)jt->rot.Z(),0,0,1);
    //glRotatef((GLfloat)jt->rot.Y(),0,1,0);
    //glRotatef((GLfloat)jt->rot.X(),1,0,0);

	transformMatrix.transposeInPlace();

	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), transformMatrix(0,2), transformMatrix(0,3),
										transformMatrix(1,0), transformMatrix(1,1), transformMatrix(1,2), transformMatrix(1,3),
										transformMatrix(2,0), transformMatrix(2,1), transformMatrix(2,2), transformMatrix(2,3),
										transformMatrix(3,0), transformMatrix(3,1), transformMatrix(3,2), transformMatrix(3,3)
									};

	glMultMatrixd(multiplyingMatrix);

    GLdouble modelview[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    jt->worldPosition = Eigen::Vector3d(modelview[12],modelview[13],modelview[14]);

	Eigen::MatrixXf m(4,4);
	m << modelview[0], modelview[1], modelview[2], modelview[3],
		modelview[4], modelview[5], modelview[6], modelview[7],
		modelview[8], modelview[9], modelview[10], modelview[11],
		modelview[12], modelview[13], modelview[14], modelview[15];
	jt->iT = m.inverse().transpose();

    for(unsigned int i = 0; i< jt->childs.size(); i++)
    {
        computeRestPosRec(jt->childs[i]);
    }

    glPopMatrix();
	*/
}

void DefGroupRender::computeWorldPos(joint* jt) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	computeWorldPosRec(jt, NULL);
	glPopMatrix();
}

void DefGroupRender::computeWorldPosRec(joint* jt, joint* father)
{

	/*
	Eigen::Matrix3d rotationMatrix;
	if(!father)
	{
		jt->translation = jt->pos;
		jt->rotation =  jt->qOrient * jt->qrot;

		rotationMatrix = jt->rotation.toRotationMatrix();

		jt->twist = Quaterniond::Identity();
		jt->parentRot = Eigen::Quaterniond::Identity();
	}
	else
	{
		jt->translation = father->translation + 
						  father->rotation._transformVector(jt->pos);
		
		jt->rotation =  father->rotation * jt->qOrient * jt->qrot;

		rotationMatrix = ( jt->qOrient * jt->qrot).toRotationMatrix();

		// TwistComputation:
		Vector3d axis = jt->rTranslation - father->rTranslation;
		axis.normalize();

		Quaterniond localRotationChild =  jt->qOrient * jt->qrot;
		Quaterniond localRotationChildRest =  jt->restRot;

		Vector3d tempRestRot = father->rRotation._transformVector(axis);

		Vector3d referenceRest = localRotationChildRest._transformVector(tempRestRot);
		Vector3d referenceCurr = localRotationChild._transformVector(tempRestRot);

		Quaterniond nonRollrotation;
		nonRollrotation.setFromTwoVectors(referenceRest, referenceCurr);
		
		// Ejes Locales
		jt->twist = localRotationChild*localRotationChildRest.inverse()*nonRollrotation.inverse();

		Vector3d quatAxis(jt->twist.x(),jt->twist.y(),jt->twist.z());
		quatAxis = (localRotationChild).inverse()._transformVector(quatAxis);
		//quatAxis = (father->qOrient * father->qrot).inverse()._transformVector(quatAxis);
	
		jt->twist.x() = quatAxis.x();
		jt->twist.y() = quatAxis.y();
		jt->twist.z() = quatAxis.z();

		Vector3d referencePoint = father->rotation.inverse()._transformVector(jt->translation - father->translation);
		Quaterniond redir;
		jt->parentRot = father->rotation * redir.setFromTwoVectors(Vector3d(1,0,0),referencePoint);	

	}
	
	//Eigen::Matrix3d rotationMatrix;
	//rotationMatrix = jt->rRotation.toRotationMatrix();

	Eigen::Matrix4f transformMatrix2;
	transformMatrix2 << rotationMatrix(0,0) , rotationMatrix(0,1) , rotationMatrix(0,2), jt->pos[0],
					    rotationMatrix(1,0) , rotationMatrix(1,1) , rotationMatrix(1,2), jt->pos[1],
					    rotationMatrix(2,0) , rotationMatrix(2,1) , rotationMatrix(2,2), jt->pos[2],
						0.0,				0.0,				0.0,			1.0;

	transformMatrix2.transposeInPlace();
	jt->world = transformMatrix2;

	jt->worldPosition = jt->translation;
	jt->W = transformMatrix2.transpose();
	

	for(unsigned int i = 0; i< jt->childs.size(); i++)
    {
        computeWorldPosRec(jt->childs[i], jt);
    }

	return;


    glPushMatrix();

	Eigen::Vector3d fatherTranslation(0,0,0);
	Eigen::Quaterniond fatherRotation(1,0,0,0);
	Eigen::Vector3d fatherWP(0,0,0);

	if (father != NULL) {
		fatherTranslation = father->translation;
		fatherRotation = father->rotation;
		fatherWP = father->getWorldPosition();
	}

	Eigen::Quaternion<double> q = jt->qrot;
	Eigen::Quaternion<double> qor = jt->qOrient;
	printf("Local rotation: %f %f %f %f\n", q.w(), q.x(), q.y(), q.z());
	printf("Local orient: %f %f %f %f\n", qor.w(), qor.x(), qor.y(), qor.z());
	q = q * qor;
	printf("Local rot*orient: %f %f %f %f\n", q.w(), q.x(), q.y(), q.z());
	Eigen::Vector3d p = jt->pos;

	if (father == NULL) {
		jt->translation = q.inverse()._transformVector(p);
		jt->rotation = q;
	} else {
		// Como quiero que la worldPos sea q*translation, una vez la tengo calculo la posición tal que q*trans = worldPos
		// usando la inversa de q*fatherRotation
		Eigen::Vector3d fatherPos = (fatherRotation)._transformVector(fatherTranslation);
		printf("Father position: %f %f %f\n", fatherWP.x(), fatherWP.y(), fatherWP.z());
		Eigen::Vector3d wwp = fatherWP + fatherRotation.inverse()._transformVector(p);		// lo que me dijiste tu
		printf("World pos: %f %f %f\n", wwp.x(), wwp.y(), wwp.z());
		//jt->translation = (q*fatherRotation).inverse()._transformVector( fatherPos + (fatherRotation)._transformVector(p));
		//jt->translation = (q*fatherRotation).inverse()._transformVector( fatherRotation._transformVector(fatherTranslation) 
		jt->translation = (q*fatherRotation).inverse()._transformVector(wwp);
		jt->rotation = q*fatherRotation;
	}

	printf("Translation: %f %f %f\n", p.x(), p.y(), p.z());
	printf("JT->Translation: %f %f %f\n", jt->translation.x(), jt->translation.y(), jt->translation.z());
	printf("JT->Rotation: %f %f %f %f\n", jt->rotation.w(), jt->rotation.x(), jt->rotation.y(), jt->rotation.z());

	Eigen::Vector3d myWorldPos = jt->rotation._transformVector(jt->translation);


    //glTranslated(jt->pos.X(),jt->pos.Y(),jt->pos.Z());

	//double orientAlpha, orientBeta, orientGamma;
	//jt->qOrient.ToEulerAngles(orientAlpha, orientBeta, orientGamma);

	Eigen::Matrix3d orientMatrix;
	Eigen::Matrix3d rotateMatrix;
	//jt->qOrient.ToMatrix(orientMatrix);
	//jt->qrot.ToMatrix(rotateMatrix);
	Eigen::Matrix3d oriRot =  (rotateMatrix * orientMatrix);

	Eigen::Matrix4f transformMatrix;
	transformMatrix << oriRot(0,0) , oriRot(0,1) , oriRot(0,2), jt->pos.x(),
						oriRot(1,0) , oriRot(1,1) , oriRot(1,2), jt->pos.y(),
						oriRot(2,0) , oriRot(2,1) , oriRot(2,2), jt->pos.z(),
						0,				0,				0,			1;

	//double alpha, beta, gamma;
	//jt->qrot.ToEulerAngles(alpha, beta, gamma);

	transformMatrix.transposeInPlace();

	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), transformMatrix(0,2), transformMatrix(0,3),
										transformMatrix(1,0), transformMatrix(1,1), transformMatrix(1,2), transformMatrix(1,3),
										transformMatrix(2,0), transformMatrix(2,1), transformMatrix(2,2), transformMatrix(2,3),
										transformMatrix(3,0), transformMatrix(3,1), transformMatrix(3,2), transformMatrix(3,3)
									};

	jt->world = transformMatrix;

	glMultMatrixd(multiplyingMatrix);

    GLdouble modelview[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    jt->worldPosition = Eigen::Vector3d(modelview[12],modelview[13],modelview[14]);


	printf("World position 1: %f %f %f\n", modelview[12], modelview[13], modelview[14]);
	printf("World position 2: %f %f %f\n\n", myWorldPos.x(), myWorldPos.y(), myWorldPos.z());

	Eigen::MatrixXf m(4,4);
	m << modelview[0], modelview[1], modelview[2], modelview[3],
		modelview[4], modelview[5], modelview[6], modelview[7],
		modelview[8], modelview[9], modelview[10], modelview[11],
		modelview[12], modelview[13], modelview[14], modelview[15];
	jt->W = m.transpose();

    for(unsigned int i = 0; i< jt->childs.size(); i++)
    {
        computeWorldPosRec(jt->childs[i], jt);
    }

    glPopMatrix();

	*/
}