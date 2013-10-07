#ifndef GRAPH_RENDER_H
#define GRAPH_RENDER_H

#define DEBUG false

#include "ShadingNode.h"

using namespace std;

// GRAPH
class Geometry;
class object;
class GraphRender : public shadingNode
{    
    public:
		GraphRender() : shadingNode()
        {
			geom = NULL;
			spotVertex = 0;
        }

        GraphRender(Geometry* g) : shadingNode()
        {
			geom = g;
			spotVertex = 0;
        }

		Geometry* geom;

		int spotVertex;
		vector<int> spotVertexes;

        virtual void drawFunc(object* obj);
};

#endif // GEOMETRY_RENDER_H
