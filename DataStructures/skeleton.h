#ifndef SKELETON_H
#define SKELETON_H

#include "Object.h"
#include "DefNode.h"
#include <Eigen/Dense>

#include <vcg\math\quaternion.h>
#include <map>

class DefNode;


// SKELETON
class joint : public object
{
    public:

        joint* father;
        vector< joint* > childs;
        vector< DefNode* > nodes;

		vcg::Quaternion<double> qOrient;

		int deformerId;

		vector< double > embedding;
		vector< vector< double > > childVector;

		// Modelling
		Eigen::Vector3d translation;
		Eigen::Quaternion<double> rotation;

		// Rest modelling
		Eigen::Vector3d rTranslation;
		Eigen::Quaternion<double> rRotation;

		Eigen::Matrix4f iT;
		Eigen::Matrix4f W;
		Eigen::Matrix4f world;

        float expansion;
		float smoothness;

        // For precomputation
        Point3d worldPosition;

		bool enableWeightsComputation;

    public:

        joint();
        joint(unsigned int nodeId);
        joint(joint* _father);
        joint(joint* _father, unsigned int nodeId);
        ~joint();

		void initDefaults();

        virtual void drawFunc();
        virtual bool getBoundingBox(Point3d& minAuxPt,
                                    Point3d& maxAuxPt);

        virtual void select(bool bToogle, int id);

        void computeWorldPos();
		void computeRestPos();

        // GETTERS & SETTERS
        void setJointOrientation(double ojX,
                                 double  ojY,
                                 double  ojZ);

        void setWorldPosition(Point3d pos);
        Point3d getJointOrientation();
        Point3d getWorldPosition();

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

    // Variables
    joint* root;
    vector< joint* > joints; // all the joints of the skeleton
    map<int, joint*> jointRef; // a reference to get joints faster, the id is scene global
	map<int, joint*> deformerRef; // a reference to get joints faster, the id is deformers set

	float minSegmentLength;

    virtual void drawFunc();
    virtual bool getBoundingBox(Point3d& minAuxPt,Point3d& maxAuxPt);
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

int subdivideBone( joint* parent, joint* child,/* Point3d origen, Point3d fin,*/
				  vector< DefNode >& nodePoints,
				  float subdivisionRatio);

float GetMinSegmentLenght(float minLength, float cellSize);

float getMinSkeletonSize(skeleton* skt);

#endif // SKELETON_H
