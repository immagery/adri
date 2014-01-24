#include <utils/utilGL.h>
#include "ModeloRender.h"
#include "ShadingNode.h"

#include <DataStructures/Modelo.h> 

void ModeloRender::drawFunc(object* obj)
{
	GeometryRender::drawFunc( obj);

	if(shMode == SH_MODE_SELECTION) return;

	// transformaciones
	beforeDraw(obj);

	// Pintamos detalles de los que queramos marcar.
    glBegin(GL_TRIANGLES);
	
	Modelo* m = (Modelo*)obj;

	glDisable(GL_LIGHTING);
	glColor3f(1.8, 0.2, 0.3);

	if(m->bind)
	{
		for(unsigned int trs = 0; trs < m->bind->virtualTriangles.size(); trs++ )
		{
			glVertex( m->bind->virtualTriangles[trs].pts[0]->node->position);
			glVertex( m->bind->virtualTriangles[trs].pts[1]->node->position);
			glVertex( m->bind->virtualTriangles[trs].pts[2]->node->position);
		}
	}

    glEnable(GL_LIGHTING);
	glEnd();

    afterDraw(obj);
}