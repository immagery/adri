#include "SkeletonRender.h"
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

#include <DataStructures/Skeleton.h>

float JointRender::jointSize = DEFAULT_SIZE;

void SkeletonRender::drawFunc(skeleton* obj)
{
    // transformaciones
    beforeDraw(obj);

	/*
	Geometry* geom = (Geometry*)obj;

    // Pintamos en modo directo el modelo
    if(shtype == T_POLY || shtype == T_XRAY)
    {

        float blend = 1;
        if(shtype == T_XRAY)
        {
            blend = 0.3;
            glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }

        glBegin(GL_TRIANGLES);

        int size = colors.size();
        if(size<=0)
            glColor4f(color[0]*blend, color[1]*blend, color[2]*blend,blend);

        MyMesh::FaceIterator fi;
        for(fi = geom->face.begin(); fi!= geom->face.end(); ++fi )
         {
             //glNormal3dv(&(*fi).N()[0]);
             for(int i = 0; i<3; i++)
             {
                 int pIdx = fi->V(i)->IMark();

                 if(size > 0 && pIdx < size)
                 {
                     if(colors[pIdx].size()== 3)
                        glColor4f(colors[pIdx][0]*blend, colors[pIdx][1]*blend, colors[pIdx][2]*blend,blend);
                     else
                        glColor4f(color[0]*blend, color[1]*blend, color[2]*blend,blend);
                 }

                 glNormal(fi->V(i)->N());
                 glVertex(fi->V(i)->P());
             }
         }
        glEnd();


        if(shtype == T_XRAY)
        {
            glDisable(GL_BLEND);
            //glBlendFunc(GL_ONE, GL_ONE);
        }
    }
    else if(shtype == T_LINES && !selected)
    {

        glDisable(GL_LIGHTING);
        glColor3f(color[0], color[1], color[2]);
        MyMesh::FaceIterator fi;
        for(fi = geom->face.begin(); fi!=geom->face.end(); ++fi )
        {
             glBegin(GL_LINE_LOOP);
             glLineWidth(10.0);
             for(int i = 0; i<=3; i++) glVertex((*fi).V(i%3)->P());
             glEnd();
        }
        glEnable(GL_LIGHTING);

    }


    if(selected)
    {
        glDisable(GL_LIGHTING);

        if(subObjectmode)
            glColor3f(0.2, 0.2, 1.0);
        else
            glColor3f(0.2, 1.0, 0.2);

        MyMesh::FaceIterator fi;
        for(fi = geom->face.begin(); fi!=geom->face.end(); ++fi )
        {
             glBegin(GL_LINE_LOOP);
             glLineWidth(10.0);
             for(int i = 0; i<=3; i++) glVertex((*fi).V(i%3)->P());
             glEnd();
        }

        if(subObjectmode)
        {
            float pointSize = 5;

            glColor3f(0.5, 0, 0.5);
            glPointSize(pointSize*0.9);

            glBegin(GL_POINTS);
            // vertices normales
            MyMesh::VertexIterator vi;
            for(vi = geom->vert.begin(); vi!=geom->vert.end(); ++vi )
            {
                 glVertex((*vi).P());
            }
            glEnd();

            glColor3f(1.0, 1.0, 0.0);
            glPointSize(pointSize);
            glBegin(GL_POINTS);
            // vertices seleccionados
            for(unsigned int sel = 0; sel < selectedIds.size(); sel++ )
            {
                glVertex(geom->vert[selectedIds[sel]].P());
            }
            glEnd();

        }
        glEnable(GL_LIGHTING);
    }

	*/

    afterDraw(obj);
}

bool SkeletonRender::updateSkeleton(skeleton* skt)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    skt->root->computeWorldPos();
    glPopMatrix();

    return true;
}


void drawLine(double x1, double y1, double z1, double x2, double y2, double z2)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);

    glVertex3d(x1,y1,z1);
    glVertex3d(x2,y2,z2);

    glEnd();
    glEnable(GL_LIGHTING);
}

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
    glVertex3d(r,r,r);  // repetimos el punto inicial para cerrar el c�rculo.

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

void drawBoneStick(float radius, Point3d& pos)
{
	drawLine(0,0,0, pos.X(), pos.Y(), pos.Z());
	//drawLine(1,0,0, pos.X(), pos.Y(), pos.Z());
	//drawLine(1,0,0, pos.X(), pos.Y(), pos.Z());
}

//JOINT
void JointRender::drawFunc(joint* jt)
{    
    glColor3f(1.0,1.0,1.0);
    glPushMatrix();

    if(jt->father)
    {
        if(jt->father->shading->selected)
            glColor3f((GLfloat)SELR,(GLfloat)SELG,(GLfloat)SELB);
        else
            glColor3f(NORMALR,NORMALG,NORMALB);

		//
		drawBoneStick(jointSize, jt->pos);
        //drawLine(jt->pos.X(), jt->pos.Y(),jt->pos.Z());
    }

    //glTranslated(jt->pos.X(),jt->pos.Y(),jt->pos.Z());

	Matrix33d orientMatrix;
	Matrix33d rotateMatrix;
	jt->qOrient.ToMatrix(orientMatrix);
	jt->qrot.ToMatrix(rotateMatrix);
	Matrix33d oriRot =  (orientMatrix * rotateMatrix);

	Eigen::Matrix4d transformMatrix;
	transformMatrix << oriRot[0][0] , oriRot[0][1] , oriRot[0][2], jt->pos.X(),
					   oriRot[1][0] , oriRot[1][1] , oriRot[1][2], jt->pos.Y(),
					   oriRot[2][0] , oriRot[2][1] , oriRot[2][2], jt->pos.Z(),
					   0,				0,				0,			1;

    //glRotatef((GLfloat)jt->orientJoint.Z(),0,0,1);
    //glRotatef((GLfloat)jt->orientJoint.Y(),0,1,0);
    //glRotatef((GLfloat)jt->orientJoint.X(),1,0,0);

	//double orientAlpha, orientBeta, orientGamma;
	//jt->qOrient.ToEulerAngles(orientAlpha, orientBeta, orientGamma);

	//glRotatef((GLfloat)Rad2Deg(orientGamma),0,0,1);
    //glRotatef((GLfloat)Rad2Deg(orientBeta),0,1,0);
    //glRotatef((GLfloat)Rad2Deg(orientAlpha),1,0,0);

	//double alpha, beta, gamma;
	//jt->qrot.ToEulerAngles(alpha, beta, gamma);

	transformMatrix.transposeInPlace();

	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), transformMatrix(0,2), transformMatrix(0,3),
										transformMatrix(1,0), transformMatrix(1,1), transformMatrix(1,2), transformMatrix(1,3),
										transformMatrix(2,0), transformMatrix(2,1), transformMatrix(2,2), transformMatrix(2,3),
										transformMatrix(3,0), transformMatrix(3,1), transformMatrix(3,2), transformMatrix(3,3)
									};

	glMultMatrixd(multiplyingMatrix);

    //glRotatef((GLfloat)Rad2Deg(gamma),0,0,1);
    //glRotatef((GLfloat)Rad2Deg(beta),0,1,0);
    //glRotatef((GLfloat)Rad2Deg(alpha),1,0,0);

    // Pintamos un tri-c�rculo
    if(selected)
        glColor3f((GLfloat)SELR,(GLfloat)SELG,(GLfloat)SELB);
    else
        glColor3f(NORMALR,NORMALG,NORMALB);

    // Pintamos 3 los c�rculos
    drawTriCircle(12, jointSize);

    // Pintamos los ejes del hueso
    drawAxisHandle(jointSize*25);

    // Pintamos la pelota de expansion
    //drawExpansionBall(selected, (float)(DEFAULT_SIZE*2*jt->expansion));
	//for(unsigned int i = 0; i< jt->nodes.size(); i++)
    //{
    //    drawDeformer(jt->childs[i]->drawFunc());
    //}

    for(unsigned int i = 0; i< jt->childs.size(); i++)
    {
        jt->childs[i]->drawFunc();
    }

    glPopMatrix();
}

void JointRender::computeRestPos(joint* jt) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	computeRestPosRec(jt);
	glPopMatrix();
}

void JointRender::computeRestPosRec(joint* jt)
{
    glPushMatrix();

    //glTranslated(jt->pos.X(),jt->pos.Y(),jt->pos.Z());

	double orientAlpha, orientBeta, orientGamma;
	jt->qOrient.ToEulerAngles(orientAlpha, orientBeta, orientGamma);

	Matrix33d orientMatrix;
	Matrix33d rotateMatrix;
	jt->qOrient.ToMatrix(orientMatrix);
	jt->qrot.ToMatrix(rotateMatrix);
	Matrix33d oriRot =  (orientMatrix * rotateMatrix);

	Eigen::Matrix4d transformMatrix;
	transformMatrix << oriRot[0][0] , oriRot[0][1] , oriRot[0][2], jt->pos.X(),
						oriRot[1][0] , oriRot[1][1] , oriRot[1][2], jt->pos.Y(),
						oriRot[2][0] , oriRot[2][1] , oriRot[2][2], jt->pos.Z(),
						0,				0,				0,			1;


	//glRotatef((GLfloat)Rad2Deg(orientGamma),0,0,1);
    //glRotatef((GLfloat)Rad2Deg(orientBeta),0,1,0);
    //glRotatef((GLfloat)Rad2Deg(orientAlpha),1,0,0);
	
	//glRotatef((GLfloat)jt->orientJoint.Z(),0,0,1);
    //glRotatef((GLfloat)jt->orientJoint.Y(),0,1,0);
    //glRotatef((GLfloat)jt->orientJoint.X(),1,0,0);

	double alpha, beta, gamma;
	jt->qrot.ToEulerAngles(alpha, beta, gamma);

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
    jt->worldPosition = Point3d(modelview[12],modelview[13],modelview[14]);

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
}

void JointRender::computeWorldPos(joint* jt) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	computeWorldPosRec(jt);
	glPopMatrix();
}

void JointRender::computeWorldPosRec(joint* jt)
{
    glPushMatrix();

    //glTranslated(jt->pos.X(),jt->pos.Y(),jt->pos.Z());

	double orientAlpha, orientBeta, orientGamma;
	jt->qOrient.ToEulerAngles(orientAlpha, orientBeta, orientGamma);

	Matrix33d orientMatrix;
	Matrix33d rotateMatrix;
	jt->qOrient.ToMatrix(orientMatrix);
	jt->qrot.ToMatrix(rotateMatrix);
	Matrix33d oriRot =  (orientMatrix * rotateMatrix);

	Eigen::Matrix4d transformMatrix;
	transformMatrix << oriRot[0][0] , oriRot[0][1] , oriRot[0][2], jt->pos.X(),
						oriRot[1][0] , oriRot[1][1] , oriRot[1][2], jt->pos.Y(),
						oriRot[2][0] , oriRot[2][1] , oriRot[2][2], jt->pos.Z(),
						0,				0,				0,			1;


	/*glRotatef((GLfloat)Rad2Deg(orientGamma),0,0,1);
    glRotatef((GLfloat)Rad2Deg(orientBeta),0,1,0);
    glRotatef((GLfloat)Rad2Deg(orientAlpha),1,0,0);*/

	double alpha, beta, gamma;
	jt->qrot.ToEulerAngles(alpha, beta, gamma);

    /*glRotatef((GLfloat)Rad2Deg(gamma),0,0,1);
    glRotatef((GLfloat)Rad2Deg(beta),0,1,0);
    glRotatef((GLfloat)Rad2Deg(alpha),1,0,0);*/

	transformMatrix.transposeInPlace();

	GLdouble multiplyingMatrix[16] = {transformMatrix(0,0), transformMatrix(0,1), transformMatrix(0,2), transformMatrix(0,3),
										transformMatrix(1,0), transformMatrix(1,1), transformMatrix(1,2), transformMatrix(1,3),
										transformMatrix(2,0), transformMatrix(2,1), transformMatrix(2,2), transformMatrix(2,3),
										transformMatrix(3,0), transformMatrix(3,1), transformMatrix(3,2), transformMatrix(3,3)
									};

	glMultMatrixd(multiplyingMatrix);

    GLdouble modelview[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    jt->worldPosition = Point3d(modelview[12],modelview[13],modelview[14]);

	Eigen::MatrixXf m(4,4);
	m << modelview[0], modelview[1], modelview[2], modelview[3],
		modelview[4], modelview[5], modelview[6], modelview[7],
		modelview[8], modelview[9], modelview[10], modelview[11],
		modelview[12], modelview[13], modelview[14], modelview[15];
	jt->W = m.transpose();

    for(unsigned int i = 0; i< jt->childs.size(); i++)
    {
        computeWorldPosRec(jt->childs[i]);
    }

    glPopMatrix();
}