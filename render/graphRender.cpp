#include <utils/utilGL.h>
#include "GraphRender.h"

#include <DataStructures/Geometry.h> 

void GraphRender::drawFunc(object* obj)
{
	
    // transformaciones
    beforeDraw(obj);

	Geometry* geom = (Geometry*)obj;

    glDisable(GL_LIGHTING);
    glColor3f(color[0], color[1], color[2]);
    
	glLineWidth(3.0);	
	glBegin(GL_LINES);
	for(int node = 0; node < geom->nodes.size(); node++)
	{
		for(int con = 0; con < geom->nodes[node]->connections.size(); con++)
		{
			glVertex3f(geom->nodes[node]->position.X(),geom->nodes[node]->position.Y(),geom->nodes[node]->position.Z());
			glVertex3f( geom->nodes[node]->connections[con]->position.X(),
						geom->nodes[node]->connections[con]->position.Y(),
						geom->nodes[node]->connections[con]->position.Z());
		}
	}
	glEnd();

	float pointSize = 5;
	glColor3f(0.5, 0, 0.5);
	glPointSize(pointSize*0.9);

	glBegin(GL_POINTS);
	// vertices normales
	//MyMesh::VertexIterator vi;
	//for(vi = geom->vert.begin(); vi!=geom->vert.end(); ++vi )
	for(int vi = 0; vi < geom->nodes.size(); vi++ )
	{
		int pIdx = geom->nodes[vi]->id;
		if(colors.size() > 0 && colors[pIdx].size()== 3)
			glColor4f(colors[pIdx][0], colors[pIdx][1], colors[pIdx][2], 1.0);

		glVertex3f(geom->nodes[vi]->position.X(),geom->nodes[vi]->position.Y(),geom->nodes[vi]->position.Z());
	}
	glEnd();
	glEnable(GL_LIGHTING);


    afterDraw(obj);
	
}