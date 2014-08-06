#include <utils/utilGL.h>
#include "ModeloRender.h"
#include "ShadingNode.h"

#include <DataStructures/Modelo.h> 

void ModeloRender::drawAnalitics(object* obj)
{
	// transformaciones
	beforeDraw(obj);

	// Pintamos detalles de los que queramos marcar.
	Modelo* m = (Modelo*)obj;

	glDisable(GL_LIGHTING);

	if (m->grid)
	{
		float v = 0, r = 0, g = 0, b = 0;
		float numPieces = m->bind->surfaces.size();

		assert(numPieces > 0);

		Vector3d midPos(0.5, 0.5, 0.5);

		glPointSize(5);
		glBegin(GL_POINTS);
		Vector3d min = m->grid->bounding.min;
		for (int gridIdxX = 0; gridIdxX < m->grid->cells.size(); gridIdxX++)
		{
			for (int gridIdxY = 0; gridIdxY < m->grid->cells[gridIdxX].size(); gridIdxY++)
			{
				for (int gridIdxZ = 0; gridIdxZ < m->grid->cells[gridIdxX][gridIdxY].size(); gridIdxZ++)
				{
					Vector3d pt = m->grid->bounding.min + (Vector3d(gridIdxX, gridIdxY, gridIdxZ) + midPos) *  m->grid->cellSize;

					if (m->grid->cells[gridIdxX][gridIdxY][gridIdxZ].size() > 0)
					{

						v = (float)m->grid->cells[gridIdxX][gridIdxY][gridIdxZ][0]->pieceId / numPieces;
						GetColour(v, 0, 1, r, g, b);

						if (m->grid->cells[gridIdxX][gridIdxY][gridIdxZ].size() > 0 && m->grid->cells[gridIdxX][gridIdxY][gridIdxZ][0]->pieceId == auxValue)
						{
								glColor3d(r, g, b);
						}
						else
							continue;//glColor3d(1.0, 1.0, 1.0);

						glVertex3d(pt.x(), pt.y(), pt.z());
					}
				}
			}
		}

		glEnd();

		Vector3d boxMin = m->grid->bounding.min;
		Vector3d boxMax = m->grid->bounding.max;

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);

		glVertex3d(boxMin.x(), boxMin.y(), boxMin.z());
		glVertex3d(boxMin.x(), boxMax.y(), boxMin.z());

		glVertex3d(boxMin.x(), boxMin.y(), boxMin.z());
		glVertex3d(boxMin.x(), boxMin.y(), boxMax.z());

		glVertex3d(boxMin.x(), boxMax.y(), boxMax.z());
		glVertex3d(boxMin.x(), boxMax.y(), boxMin.z());

		glVertex3d(boxMin.x(), boxMax.y(), boxMax.z());
		glVertex3d(boxMin.x(), boxMin.y(), boxMax.z());


		glVertex3d(boxMax.x(), boxMin.y(), boxMin.z());
		glVertex3d(boxMax.x(), boxMax.y(), boxMin.z());

		glVertex3d(boxMax.x(), boxMin.y(), boxMin.z());
		glVertex3d(boxMax.x(), boxMin.y(), boxMax.z());

		glVertex3d(boxMax.x(), boxMax.y(), boxMax.z());
		glVertex3d(boxMax.x(), boxMax.y(), boxMin.z());

		glVertex3d(boxMax.x(), boxMax.y(), boxMax.z());
		glVertex3d(boxMax.x(), boxMin.y(), boxMax.z());


		glVertex3d(boxMin.x(), boxMin.y(), boxMin.z());
		glVertex3d(boxMax.x(), boxMin.y(), boxMin.z());

		glVertex3d(boxMin.x(), boxMin.y(), boxMax.z());
		glVertex3d(boxMax.x(), boxMin.y(), boxMax.z());

		glVertex3d(boxMin.x(), boxMax.y(), boxMin.z());
		glVertex3d(boxMax.x(), boxMax.y(), boxMin.z());

		glVertex3d(boxMin.x(), boxMax.y(), boxMax.z());
		glVertex3d(boxMax.x(), boxMax.y(), boxMax.z());


		glEnd();
	}

	glEnable(GL_LIGHTING);

	afterDraw(obj);
}


void ModeloRender::drawFunc(object* obj)
{
	GeometryRender::drawFunc( obj);

	if(shMode == SH_MODE_SELECTION) return;

}