#include <utils/utilGL.h>
#include "GeometryRender.h"

#include <DataStructures/Geometry.h> 

void GeometryRender::drawNamesFunc(object* obj)
{
    // transformaciones
    beforeDraw(obj);

	Geometry* geom = (Geometry*)obj;
        
    int size = colors.size();
    if(size<=0) glColor4f(color[0], color[1], color[2],1);

	for(int tr = 0; tr< geom->triangles.size(); tr++ )
	{
    	glPushName((GLuint)tr);
		glBegin(GL_TRIANGLES);

        for(int i = 0; i<3; i++)
        {
			int pIdx = geom->triangles[tr]->verts[i]->id;
			glNormal3d(geom->faceNormals[tr].x(), geom->faceNormals[tr].y(), geom->faceNormals[tr].z());

            if(size > 0 && pIdx < size)
            {
                if(colors[pIdx].size()== 3)
				{
				float r = colors[pIdx][0];
				float g = colors[pIdx][1];
				float b = colors[pIdx][2];

                glColor4f(colors[pIdx][0], colors[pIdx][1], colors[pIdx][2],1);
				}
                else
                glColor4f(color[0], color[1], color[2], 1.0);
            }

            glVertex(geom->nodes[pIdx]->position);
        }

		glEnd();
		glPopName();
	}

    afterDraw(obj);
}

void GeometryRender::drawFunc(object* obj)
{
	// Selection disabled
	/*
	if(shMode == SH_MODE_SELECTION)
	{
		drawNamesFunc(obj);
		return;
	}
	*/

    // transformaciones
    beforeDraw(obj);

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

		for(int tr = 0; tr< geom->triangles.size(); tr++ )
		{
             for(int i = 0; i<3; i++)
             {
				 int pIdx = geom->triangles[tr]->verts[i]->id;
					
				 if(shMode == SH_MODE_FLAT || shMode == SH_MODE_SELECTION)
				 {
					glNormal3d(geom->faceNormals[tr].x(), 
							   geom->faceNormals[tr].y(), 
							   geom->faceNormals[tr].z());
				 }

                 if(size > 0 && pIdx < size)
                 {
                     if(colors[pIdx].size()== 3)
					 {
						float r = colors[pIdx][0]*blend;
						float g = colors[pIdx][1]*blend;
						float b = colors[pIdx][2]*blend;

                        glColor4f(colors[pIdx][0]*blend, colors[pIdx][1]*blend, colors[pIdx][2]*blend,blend);
					 }
                     else
                        glColor4f(color[0]*blend, color[1]*blend, color[2]*blend,blend);
                 }
			
				 if(shMode != SH_MODE_FLAT)
				 {
					glNormal3d(geom->vertNormals[pIdx].x(), geom->vertNormals[pIdx].y(), geom->vertNormals[pIdx].z());
				 }

                 glVertex(geom->nodes[pIdx]->position);
             }

			 //if(shMode == SH_MODE_SELECTION) glPopName();
         }
        glEnd();

		if(spotVertexes.size() > 0)
		{
			glDisable(GL_LIGHTING);
			float pointSize = 8;
			glColor3f(0.9, 0.1, 0.1);
			glPointSize(pointSize*0.9);

			glBegin(GL_POINTS);
			int countSV= 0;

			// vertices normales
			for(int vi = 0; vi < geom->nodes.size(); vi++ )
			{
				int pIdx = geom->nodes[vi]->id;

				//if(colors.size() > 0 && colors[pIdx].size()== 3)
				//	glColor4f(colors[pIdx][0], colors[pIdx][1], colors[pIdx][2], 1.0);
				for(int sv = 0; sv< spotVertexes.size(); sv++)
				{
					if(pIdx == spotVertexes[sv])
					{
						glVertex(geom->nodes[vi]->position);
						countSV++;
						break;
					}
				}
				if(countSV == spotVertexes.size()) break;
			}
			glEnd();
			glEnable(GL_LIGHTING);
		}

		if(spotVertex >= 0)
		{
			glDisable(GL_LIGHTING);
			float pointSize = 8;
			glColor3f(0.1, 0.9, 0.1);
			glPointSize(pointSize*0.9);

			glBegin(GL_POINTS);

			// vertices normales
			for(int vi = 0; vi < geom->nodes.size(); vi++ )
			{
				int pIdx = geom->nodes[vi]->id;

				//if(colors.size() > 0 && colors[pIdx].size()== 3)
				//	glColor4f(colors[pIdx][0], colors[pIdx][1], colors[pIdx][2], 1.0);
				if(pIdx == spotVertex)
					glVertex(geom->nodes[vi]->position);
			}
			glEnd();
			glEnable(GL_LIGHTING);
		}

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

        for(int tr = 0; tr< geom->triangles.size(); tr++ )
		{
             glBegin(GL_LINE_LOOP);
             for(int i = 0; i<=3; i++) glVertex(geom->triangles[tr]->verts[i%3]->position);
             glEnd();
        }

		float pointSize = 2;
		glColor3f(0.5, 0, 0.5);
		glPointSize(pointSize);

		glBegin(GL_POINTS);
		// vertices normales
		//MyMesh::VertexIterator vi;
		//for(vi = geom->vert.begin(); vi!=geom->vert.end(); ++vi )
		for(int vi = 0; vi < geom->nodes.size(); vi++ )
		{
			int pIdx = geom->nodes[vi]->id;
			if(colors.size() > 0 && colors[pIdx].size()== 3)
				glColor4f(colors[pIdx][0], colors[pIdx][1], colors[pIdx][2], 1.0);

			glVertex(geom->nodes[vi]->position);
		}
		glEnd();

		glEnable(GL_LIGHTING);

    }


    if(selected)
    {
        glDisable(GL_LIGHTING);

        if(subObjectmode)
            glColor3f(0.2, 0.2, 1.0);
        else
            glColor3f(0.2, 1.0, 0.2);

        for(int tr = 0; tr< geom->triangles.size(); tr++ )
		{
             glBegin(GL_LINE_LOOP);
             for(int i = 0; i<=3; i++) 
			 {
				glVertex(geom->triangles[tr]->verts[i%3]->position);
			 }
             glEnd();
        }

        if(subObjectmode)
        {
            float pointSize = 5;

            glColor3f(0.5, 0, 0.5);
            glPointSize(pointSize*0.9);

            glBegin(GL_POINTS);
            // vertices normales
            for(int vi = 0; vi < geom->nodes.size(); vi++ )
			{
                glVertex(geom->nodes[vi]->position);
            }
            glEnd();

            glColor3f(1.0, 1.0, 0.0);
            glPointSize(pointSize);
            glBegin(GL_POINTS);
            // vertices seleccionados
            for(unsigned int sel = 0; sel < selectedIds.size(); sel++ )
            {
                glVertex(geom->nodes[selectedIds[sel]]->position);
            }
            glEnd();

        }
        glEnable(GL_LIGHTING);
    }

    afterDraw(obj);
	
}