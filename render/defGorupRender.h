
#ifndef DEFGROUP_RENDER_H
#define DEFGROUP_RENDER_H

#define DEBUG false

#include <render\shadingNode.h>
#include <DataStructures\AirRig.h>

// render size
#define DEFAULT_SIZE 0.05

using namespace std;

class DefGroupRender : public shadingNode
{    
    public:
		DefGroupRender() : shadingNode()
        {
			group = NULL;
        }

        DefGroupRender(DefGroup* g) : shadingNode()
        {
			group = g;
        }

		DefGroup* group;

        void drawFunc();
		void drawWithNames();

		//void computeWorldPos(joint* jt);
		//void computeWorldPosRec(joint* jt, joint* father);
		//void computeRestPos(joint* jt);
		//void computeRestPosRec(joint* jt, joint* father = NULL);

		static float jointSize;
};

void drawLine(double x, double y, double z);
void drawBone(double l, double r);

#endif // SKELETON_RENDER_H
