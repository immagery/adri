
#ifndef SKELETON_RENDER_H
#define SKELETON_RENDER_H

#define DEBUG false

#include "..\render\shadingNode.h"
#include "..\DataStructures\skeleton.h"

// render size
#define DEFAULT_SIZE 0.1

using namespace std;

class JointRender : public shadingNode
{    
    public:
		JointRender() : shadingNode()
        {
			skt = NULL;
        }

        JointRender(joint* g) : shadingNode()
        {
			skt = g;
        }

		joint* skt;

        void drawFunc(joint* jt);
		void computeWorldPos(joint* jt);
		void computeWorldPosRec(joint* jt, joint* father);
		void computeRestPos(joint* jt);
		void computeRestPosRec(joint* jt);

		static float jointSize;
};

void drawLine(double x, double y, double z);
void drawBone(double l, double r);

// GEOMETRY
class skeleton;
class joint;
class SkeletonRender : public shadingNode
{    
    public:
		SkeletonRender() : shadingNode()
        {
			skt = NULL;
        }

        SkeletonRender(skeleton* g) : shadingNode()
        {
			skt = g;
        }

		skeleton* skt;

        void drawFunc(skeleton* skt);
		bool updateSkeleton(skeleton* skt);

};

#endif // SKELETON_RENDER_H
