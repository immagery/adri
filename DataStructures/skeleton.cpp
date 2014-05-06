#include "skeleton.h"
#include "Scene.h"

#include <utils/util.h>
#include <render/skeletonRender.h>

// COW
#define subdivisionRatio_DEF 0.1
#define endChop_DEF 0.001

//TROLL
//#define subdivisionRatio_DEF 5.0
//#define endChop_DEF 0.01

#define ratioExpansion_DEF 0.7

using namespace Eigen;

bool joint::update()
{
	return true;
}

bool joint::propagateDirtyness()
{
	dirtyFlag = false;
	for(int i = 0; i< nodes.size(); i++)
		nodes[i]->propagateDirtyness();

	for(int i = 0; i< childs.size(); i++)
		childs[i]->propagateDirtyness();

	return true;
}

void joint::initDefaults()
{
	father = NULL;
    qOrient = Eigen::Quaternion<double>(1,0,0,0);
	restOrient = qOrient;
	rots = vector<Eigen::Quaterniond>(3, Eigen::Quaterniond::Identity());
	
	//orientJoint = Eigen::Vector3d(0,0,0);
    childs.clear();
    nodes.clear();
    
    iam = JOINT_NODE;

	embedding.clear();
	childVector.clear();

	deformerId = -1; // not initialized
	expansion = 1;
	smoothness = 1;

	shading = new JointRender(this);

	enableWeightsComputation = true;
}

joint::joint() : object()
{
	initDefaults();

	/*
    father = NULL;
    orientJoint = Eigen::Vector3d(0,0,0);
    childs.clear();
    nodes.clear();

    iam = JOINT_NODE;

	embedding.clear();
	childVector.clear();

	deformerId = -1; // not initialized
	expansion = 1;
	smoothness = 1;

	shading = new JointRender(this);
	*/
}

joint::joint(unsigned int nodeId) : object(nodeId)
{
	initDefaults();

	/*
	iam = JOINT_NODE;
	
	father = NULL;
    orientJoint = Eigen::Vector3d(0,0,0);
    childs.clear();
    nodes.clear();

    expansion = 1;
	smoothness = 1;

	deformerId = -1; // not initialized
	
	embedding.clear();
	childVector.clear();

	shading = new JointRender(this);
	*/
}

joint::joint(joint* _father) : object()
{
	initDefaults();

    father = _father;
    
	/*
	orientJoint = Eigen::Vector3d(0,0,0);
    //JointSkeletonId = 0;
    childs.clear();
    nodes.clear();

    expansion = 1;
	smoothness = 1;

	deformerId = -1; // not initialized

    iam = JOINT_NODE;

	embedding.clear();
	childVector.clear();

	shading = new JointRender(this);
	*/
}

joint::joint(joint* _father, unsigned int nodeId) : object(nodeId)
{
    father = _father;
    //orientJoint = Eigen::Vector3d(0,0,0);
	qOrient = Eigen::Quaternion<double>(1,0,0,0);
	restOrient = qOrient;
	rots = vector<Eigen::Quaterniond>(3, Eigen::Quaterniond::Identity());
    //JointSkeletonId = 0;
    childs.clear();
    nodes.clear();
    expansion = 1;
	smoothness = 1;
    iam = JOINT_NODE;

	embedding.clear();
	childVector.clear();

	shading = new JointRender(this);
}

joint::~joint()
{
    for(unsigned int i = 0; i< childs.size(); i++)
    {
        delete childs[i];
    }

    childs.clear();
    nodes.clear();

    father = NULL;

    //orientJoint = Eigen::Vector3d(0,0,0);
	qOrient = Eigen::Quaternion<double>(1,0,0,0);
	restOrient = qOrient;
}

void joint::setFather(joint* f)
{
    father = f;
    //printf("SetFather\n"); fflush(0);
}

joint* joint::getFather()
{
    return father;
}

void joint::pushChild(joint* child)
{
    childs.push_back(child);
   // printf("PushChild\n"); fflush(0);
}

int joint::getChildCount()
{
    //printf("GetChildCount\n"); fflush(0);
    return childs.size();
}

joint* joint::getChild(int id)
{
    if(id < 0 || id >= (int)childs.size())
    {
        printf("Error!. Acceso a array de joints fuera de rango.");
        return NULL;
    }

    //printf("Get Child\n"); fflush(0);

    return childs[id];
}

void joint::setWorldPosition(Eigen::Vector3d pos)
{
    //printf("setWorldPosition\n"); fflush(0);
    worldPosition = pos;
}

Eigen::Vector3d joint::getWorldPosition()
{
    return worldPosition;
}


bool joint::getBoundingBox(Eigen::Vector3d& minAuxPt,Eigen::Vector3d& maxAuxPt)
{
    // Devuelve su longitud más es máximo de todo lo que cuelga
    maxAuxPt.x() = 0;

    double maxLongTemp = 0;
    for(unsigned int i = 0; i< childs.size(); i++)
    {
        childs[i]->getBoundingBox(minAuxPt, maxAuxPt);
        if(maxLongTemp < maxAuxPt.x())
            maxLongTemp = maxAuxPt.x();
    }

    maxAuxPt.x() = pos.norm() + maxLongTemp;
    return true;
}

void joint::computeWorldPos()
{
    if(dirtyFlag) {
		update();
		//dirtyFlag = false;
		((JointRender*)shading)->computeWorldPos(this);
	}
	/*
    glPushMatrix();

    glTranslated(pos.X(),pos.Y(),pos.Z());

    glRotatef((GLfloat)orientJoint.Z(),0,0,1);
    glRotatef((GLfloat)orientJoint.Y(),0,1,0);
    glRotatef((GLfloat)orientJoint.X(),1,0,0);

    glRotatef((GLfloat)rot.Z(),0,0,1);
    glRotatef((GLfloat)rot.Y(),0,1,0);
    glRotatef((GLfloat)rot.X(),1,0,0);

    GLdouble modelview[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    worldPosition = Eigen::Vector3d(modelview[12],modelview[13],modelview[14]);

    for(unsigned int i = 0; i< childs.size(); i++)
    {
        childs[i]->computeWorldPos();
    }

    glPopMatrix();
	*/
}

void joint::computeRestPos() {
	//if(dirtyFlag) update();
	((JointRender*)shading)->computeRestPos(this);
}

void joint::setJointOrientation(double ojX,double  ojY,double  ojZ, bool radians)
{

	Eigen::Quaterniond q[3];
	double angles[3];

	angles[0] = ojX;
	angles[1] = ojY;
	angles[2] = ojZ;

	if(radians)
	{
		// Convert to degrees
		for(int i = 0; i< 3; i++)
			angles[i] = angles[i]*360/(M_PI*2);
	}

	// Rotation over each axis
	for(int i = 0; i< 3; i++) {
		getAxisRotationQuaternion(q[i], i, angles[i]);
		rots[i] = q[i];
	}

	// Concatenate all the values in X-Y-Z order
	Eigen::Quaterniond qrotAux =  q[2] * q[1] * q[0];


	qOrient = qrotAux; 
	restOrient = qOrient;
}

void joint::setRotation(Quaterniond _q) 
{
	qrot = _q;
}

void joint::setRotation(double rx, double ry, double rz, bool radians) {
	Eigen::Quaterniond q[3];
	double angles[3];

	angles[0] = rx;
	angles[1] = ry;
	angles[2] = rz;

	if(radians)
	{
		// Convert to degrees
		for(int i = 0; i< 3; i++)
			angles[i] = angles[i]*360/(M_PI*2);
	}

	// Rotation over each axis
	for(int i = 0; i< 3; i++) {
		getAxisRotationQuaternion(q[i], i, angles[i]);
		rots[i] = q[i];
	}

	// Concatenate all the values in X-Y-Z order
	Eigen::Quaterniond qrotAux =  q[2] * q[1] * q[0];

	qrot = qrotAux; // Quaternion<double>(qrotAux.w(), qrotAux.x(),qrotAux.y(),qrotAux.z());
}

void joint::getRelatives(vector<joint*>& joints)
{
    joints.push_back(this);

    for(unsigned int i = 0; i< childs.size(); i++)
        childs[i]->getRelatives(joints);
}

//JOINT
void joint::drawFunc()
{    
	
    if(dirtyFlag) update();

	((JointRender*) shading)->drawFunc(this);

	/*
    //double l = pos.Norm();

    glColor3f(1.0,1.0,1.0);

    glPushMatrix();

    //double alpha = 0;
    //if(pos.X() > 0)
    //    alpha = atan(pos.Z()/pos.X())*(360/(2*M_PI));

    //double beta = 0;
    //if(l > 0)
    //    beta = asin(pos.Y()/l)*(360/(2*M_PI));

    //glRotatef(-alpha,0,1,0);
    //glRotatef(-beta,0,0,1);

    if(father)
    {
        if(father->shading->selected)
            glColor3f((GLfloat)SELR,(GLfloat)SELG,(GLfloat)SELB);
        else
            glColor3f(NORMALR,NORMALG,NORMALB);

        drawLine(pos.X(), pos.Y(),pos.Z());
        //drawBone(l,DEFAULT_SIZE);
    }

    glTranslated(pos.X(),pos.Y(),pos.Z());

    glRotatef((GLfloat)orientJoint.Z(),0,0,1);
    glRotatef((GLfloat)orientJoint.Y(),0,1,0);
    glRotatef((GLfloat)orientJoint.X(),1,0,0);

    glRotatef((GLfloat)rot.Z(),0,0,1);
    glRotatef((GLfloat)rot.Y(),0,1,0);
    glRotatef((GLfloat)rot.X(),1,0,0);

    // Pintamos un tri-círculo
    if(shading->selected)
        glColor3f((GLfloat)SELR,(GLfloat)SELG,(GLfloat)SELB);
    else
        glColor3f(NORMALR,NORMALG,NORMALB);

    // Pintamos 3 los círculos
    drawTriCircle(12, DEFAULT_SIZE);

    // Pintamos los ejes del hueso
    drawAxisHandle(DEFAULT_SIZE*0.7);

    // Pintamos la pelota de expansion
    drawExpansionBall(shading->selected, (float)(DEFAULT_SIZE*2*expansion));

    for(unsigned int i = 0; i< childs.size(); i++)
    {
        childs[i]->drawFunc();
    }

    glPopMatrix();
	*/
}

void joint::select(bool bToogle, int id)
{
    bToogle |= ((unsigned int)id == nodeId);

    shading->selected = bToogle;
    for(unsigned int i = 0; i< childs.size(); i++)
        childs[i]->select(bToogle, id);
}

 void readBone(FILE* fin, skeleton* skt, joint* root)
 {
     int num1 = 0; char str[1000];
     float posX, posY, posZ;
     float rotX, rotY, rotZ;
     float ojX, ojY, ojZ;
     float wpX, wpY, wpZ;

     fscanf(fin, "%s", &str[0]);
     fscanf(fin, "%f", &posX);
     fscanf(fin, "%f", &posY);
     fscanf(fin, "%f", &posZ);

     fscanf(fin, "%f", &rotX);
     fscanf(fin, "%f", &rotY);
     fscanf(fin, "%f", &rotZ);

     fscanf(fin, "%f", &ojX);
     fscanf(fin, "%f", &ojY);
     fscanf(fin, "%f", &ojZ);

     fscanf(fin, "%f", &wpX);
     fscanf(fin, "%f", &wpY);
     fscanf(fin, "%f", &wpZ);

     fscanf(fin, "%d", &num1);

     root->sName = str;
     root->resetTransformation();
     //root->addTranslation(posX, posY, posZ);
     
	 root->setTranslation(posX, posY, posZ);
	 root->setRotation(rotX, rotY, rotZ, false);
     root->setJointOrientation(ojX,ojY,ojZ);

     //root->setJointId(scene::getNewId());
     //root->setWorldPosition(Eigen::Vector3d(wpX, wpY, wpZ));
     skt->joints.push_back(root);

     skt->jointRef[root->nodeId] = root;

     for(int i = 0; i< num1; i++)
     {
         joint* jt = new joint(root, scene::getNewId(T_BONE));
         root->pushChild(jt);

         //printf("anadimos el joint: %s %f %f %f con %d hijos\n",str, posX, posY-pos.Y(), posZ-pos.Z(),num1);
         readBone(fin, skt, jt);
     }
 }

skeleton::skeleton() : object()
{
    joints.clear();
    root = NULL;
    iam = SKELETON_NODE;

	shading = new SkeletonRender(this);
}

skeleton::skeleton(unsigned int nodeId) : object(nodeId)
{
    joints.clear();
    root = NULL;
    iam = SKELETON_NODE;

	shading = new SkeletonRender(this);
}

skeleton::~skeleton()
{
    delete root;
    joints.clear();
}

void skeleton::drawFunc()
{
    if(dirtyFlag) 
		update();

	if(root)
		root->drawFunc();
    // Aplicamos las rotaciones y traslaciones pertinentes
}

bool skeleton::getBoundingBox(Eigen::Vector3d& minAuxPt,Eigen::Vector3d& maxAuxPt)
{
    // Calculamos la mayor caja que podría darse con la posición
    // inicial del equeleto. Esto es si todos los huesos pueden estirarse.
    maxAuxPt = Eigen::Vector3d(0,0,0);
    for(int i = 0; i< root->getChildCount(); i++)
    {
        root->getChild(i)->getBoundingBox(minAuxPt, maxAuxPt);
    }

    double l = maxAuxPt.x() * cos(45*2*M_PI/360);
    if(l <= 0)
        l = 1;

    minAuxPt = root->pos - Eigen::Vector3d(l,l,l);
    maxAuxPt = root->pos + Eigen::Vector3d(l,l,l);

    return true;
}

void skeleton::select(bool bToogle, int id)
{
    root->select( bToogle, id);
}

bool skeleton::update()
{
	if(shading)
		((SkeletonRender*)shading)->updateSkeleton(this);

	if(dirtyFlag)
	{
		// Skeleton elements update
		// proposeNodes(bb->bindedSkeletons, bb->intPoints);

		return true;
	}

	return false;
}

void readSkeletons(string fileName, vector<skeleton*>& skts)
{
     FILE* fin;
     fin = fopen(fileName.c_str(), "r");

     if(!fin) 
	 {
		 printf("No hay fichero de esqueleto.\n"); 
		 fflush(0);
		 return;
	 }

     int num1 = 0;

     fscanf(fin, "%d", &num1);
     for(int i = 0; i< num1; i++)
     {
         // Skeleton creation
         skeleton* skt = new skeleton();
         skt->sName = "skeleton" + i;

         //Root joint Creation
         skt->root = new joint(scene::getNewId(T_BONE));

         readBone(fin, skt, skt->root);

		 // Actualizar transformacion de reposo
		 skt->root->computeRestPos();

		 // Actualizar valores en 
		 skt->root->dirtyFlag = true;
         skt->update();

         skts.push_back(skt);
     }

     fclose(fin);
}

int subdivideBone(joint* parent, joint* child,
				  //Eigen::Vector3d origen, Eigen::Vector3d fin,
				  vector< DefNode >& nodePoints,
				  vector<int>& ids,
				  float subdivisionRatio)
{
	//float subdivisionRatio = subdivisionRatio_DEF;
	Eigen::Vector3d origen =  parent->getWorldPosition();
	Eigen::Vector3d fin = child->getWorldPosition();
	int boneId = parent->nodeId;

	double longitud= (float)((fin-origen).norm());
	double endChop = longitud*endChop_DEF;

	if(longitud == 0)
		return 0;

	Eigen::Vector3d dir = (fin-origen)/longitud;
	longitud = longitud - endChop;
	int numDivisions = (int)floor(longitud/subdivisionRatio);

	double newSubdivLength = longitud;
	if(numDivisions > 0)
		newSubdivLength = longitud/ double(numDivisions);

	Eigen::Vector3d newOrigen = origen;
	Eigen::Vector3d newFin = fin-dir*endChop;

	// Añadimos los nodos
	// Saltamos el primer nodo que corresponde a la raíz del joint
	for(int i = 1; i<= numDivisions; i++)
	{
		//int globalNodeId = nodePoints.size();
		ids.push_back(nodePoints.size());	
		Vector3d newPos = newOrigen+(dir*newSubdivLength*i);
		nodePoints.push_back(DefNode(newPos,boneId));
		nodePoints.back().relPos = newPos-origen;
		nodePoints.back().nodeId = scene::getNewId(T_DEFNODE);
		nodePoints.back().ratio = (float)i/(float)numDivisions;
		nodePoints.back().childBoneId = child->nodeId;

		// Expansion propagation
		float expValue = parent->expansion;
		float expValue2 = child->expansion;

		// Queda por decidir si queremos continuidad entre las expansiones o no.
		float ratio2 = nodePoints.back().ratio/ratioExpansion_DEF;
		if(ratio2 > 1) ratio2 = 1;
		if(ratio2 < 0) ratio2 = 0;

        float dif = 1-expValue;
        float newValue =  expValue + dif*ratio2;

		nodePoints.back().expansion = newValue;
	}

	// Comprobamos que el último punto sea lo que toca...
	//assert(newfin == nodePoints.back());

	if(VERBOSE)
	{
		float error = (float)(newFin - nodePoints.back().pos).norm();
		if(longitud>subdivisionRatio && fabs(error) > pow(10.0, -5))
		{
			// TODEBUG
			printf("numDivisions: %d, subdivisionRatio: %f, newSubdivLength: %f\n", numDivisions, subdivisionRatio, newSubdivLength);
			printf("Tenemos un posible error de calculo de nodos en huesos: %f\n", error);
			printf("y la longitud es: %f\n", longitud);
			fflush(0);
		}
	}

	return numDivisions+1;
}

void addNodes(joint* jt, vector< DefNode >& nodePoints, vector<int>& idx,
			  float subdivisionRatio)
{
	// We add a node in the joint pos.
	idx.push_back(nodePoints.size());
	nodePoints.push_back(DefNode(jt->getWorldPosition(), jt->nodeId));
	nodePoints.back().nodeId = scene::getNewId(T_DEFNODE);
	nodePoints.back().ratio = 0.0;
	nodePoints.back().expansion = jt->expansion;

	// We link all the child nodes to this node.
	for(int i = 0; i < jt->getChildCount(); i++)
	{
		int numDivisions;
		numDivisions = subdivideBone(jt, jt->getChild(i), 
			nodePoints, idx, subdivisionRatio);

		addNodes(jt->getChild(i), nodePoints, idx, subdivisionRatio);
	}
}

// A Partir del esqueleto define unos nodos para la segmentacion
// IN: skt
// OUT: nodePoints
int proposeNodes(vector<skeleton*>& skts, vector< DefNode >& nodePoints)
{
	// Initialization
	nodePoints.clear();

	vector< int > nodePointsIdx;
	for(unsigned int sk = 0; sk < skts.size(); sk++)
	{
		vector<int> skNodePointsIdx;
		addNodes(skts[sk]->root, nodePoints, skNodePointsIdx, skts[sk]->minSegmentLength);
		for(unsigned int skNPidx = 0; skNPidx<skNodePointsIdx.size(); skNPidx++)
			nodePointsIdx.push_back(sk);
	}

	// The nodes are linked to the bones.
	for(unsigned int j = 0; j < nodePoints.size(); j++)
	{
		joint* jt = skts[nodePointsIdx[j]]->getJoint(nodePoints[j].boneId);
		if(jt == NULL)
		{
			if(VERBOSE)
				printf("There is a problem.\n"); fflush(0);
		}
		else
		{
			jt->nodes.push_back(&nodePoints[j]);
			int prie = 0;
		}
	}

	// Devolvemos el numero de nodos añadidos.
	return nodePoints.size();
}

 float GetMinSegmentLenght(float minLength, float cellSize)
 {
	if(minLength > cellSize*3)
		return minLength-(float)0.01;

	else
		return cellSize;
 }

 float getMinSkeletonSize(skeleton* skt)
 {
	float minlength = -1;
	for(unsigned int i = 0; i< skt->joints.size(); i++)
	{
		if(skt->joints[i]->father)
		{
			if(minlength < 0)
				minlength = (skt->joints[i]->father->getWorldPosition()-skt->joints[i]->getWorldPosition()).norm();
			else
			{
				float length = (skt->joints[i]->father->getWorldPosition()-skt->joints[i]->getWorldPosition()).norm();
				if(length < minlength && length != 0)
					minlength = length;
			}
		}
	}

	return minlength;
 }