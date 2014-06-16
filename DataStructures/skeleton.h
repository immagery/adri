#ifndef SKELETON_H
#define SKELETON_H

#include "Object.h"
#include "DefNode.h"
#include <Eigen/Dense>
#include <Eigen/Core>

//#include <vcg\math\quaternion.h>
#include <map>


// SKELETON
class joint : public object
{
    public:

        joint* father;
        vector< joint* > childs;
        vector< DefNode* > nodes;

		Quaterniond qOrient;
		Quaterniond restOrient;

		int deformerId;

		vector< double > embedding;
		vector< vector< double > > childVector;

		// Modelling
		Vector3d translation;
		Quaterniond rotation;

		Quaterniond AuxRotation;

		// Rest modelling
		Vector3d rTranslation;
		Quaterniond rRotation;

		// Rest modelling
		Vector3d restPos;
		Quaterniond restRot;

		Quaterniond parentRot;

		Quaterniond twist;
		Quaterniond rTwist;

		Quaterniond nonRollRot;
		Quaterniond boneAxisRot;

		Matrix4f iT;
		Matrix4f W;
		Matrix4f world;

		vector<Quaterniond> rots;

        float expansion;
		float smoothness;

        // For precomputation
        Vector3d worldPosition;

		bool enableWeightsComputation;

    public:

        joint();
        joint(unsigned int nodeId);
        joint(joint* _father);
        joint(joint* _father, unsigned int nodeId);
        ~joint();

		void initDefaults();

        virtual void drawFunc();
        virtual bool getBoundingBox(Eigen::Vector3d& minAuxPt,
                                    Eigen::Vector3d& maxAuxPt);

        virtual void select(bool bToogle, int id);

        void computeWorldPos();
		void computeRestPos();

        // GETTERS & SETTERS
        void setJointOrientation(double ojX,
                                 double  ojY,
                                 double  ojZ,
								 bool radians = false);

		virtual void setRotation(double rx, double ry, double rz, bool radians = true);
		virtual void setRotation(Quaterniond _q) ;

        void setWorldPosition(Eigen::Vector3d pos);
        Eigen::Vector3d getJointOrientation();
        Eigen::Vector3d getWorldPosition();

		// Hierarchy relations.
		joint* getFather();
        joint* getChild(int id);
        int getChildCount();        
        void getRelatives(vector<joint*>& joints);

		void setFather(joint* f);
		void pushChild(joint* child);

		virtual bool update();
		virtual bool propagateDirtyness();

};

class skeleton : public object
{
    public:

    skeleton();
    skeleton(unsigned int nodeId);
    ~skeleton();

	void clear()
	{
		joints.clear();
		jointRef.clear();
		root = NULL;
	}

    // Variables
    joint* root;
    vector< joint* > joints; // all the joints of the skeleton
    map<int, joint*> jointRef; // a reference to get joints faster, the id is scene global
	map<int, joint*> deformerRef; // a reference to get joints faster, the id is deformers set

	float minSegmentLength;

    virtual void drawFunc();
    virtual bool getBoundingBox(Eigen::Vector3d& minAuxPt,Eigen::Vector3d& maxAuxPt);
    virtual void select(bool bToogle, int id);

    virtual bool update();

    int getBoneCount()
    {
        return (int)joints.size();
    }

    void GetJointNames(vector<string>& names)
    {
        names.resize(joints.size());
        for(unsigned int i = 0; i< joints.size(); i++)
        {
            names[i] = ((joint*)joints[i])->sName;
        }
    }

    joint* getJoint(int jointId)
    {
        return jointRef[jointId];
    }

};

void readSkeletons(string fileName, vector<skeleton*>& skts);
void readBone(FILE* fin, skeleton* skt, joint* root);

class Modelo;
class sceneUpdatingFlags;

int proposeNodes(vector<skeleton*>& skts, vector< DefNode >& nodePoints);

void addNodes(joint* jt, vector< DefNode >& nodePoints, float subdivisionRatio);

int subdivideBone( joint* parent, joint* child,/* Eigen::Vector3d origen, Eigen::Vector3d fin,*/
				  vector< DefNode >& nodePoints,
				  float subdivisionRatio);

float GetMinSegmentLenght(float minLength, float cellSize);

float getMinSkeletonSize(skeleton* skt);

#endif // SKELETON_H
