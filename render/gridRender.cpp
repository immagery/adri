#include <utils/utilGL.h>
#include <render/gridRender.h>
#include <utils/util.h>

Eigen::Vector3f greenCube(0.5,1.0,0.5);
Eigen::Vector3f redCube(1.0,0.5,0.5);
Eigen::Vector3f blueCube(0.5,0.5,1.0);
Eigen::Vector3f whiteCube(0.8,0.8,0.8);
Eigen::Vector3f brightWhiteCube(1,1,1);

void drawQuad(Eigen::Vector3f orig, Eigen::Vector3f direction1, Eigen::Vector3f direction2, Eigen::Vector3f normal, Eigen::Vector3f color, bool blend)
{
        if(blend)
        {
            glColor4f((GLfloat)color[0], (GLfloat)color[1], (GLfloat)color[2], 0.3);
            glEnable(GL_BLEND);
        }
        else
            glColor3fv(&color[0]);

        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glBegin(GL_QUADS);
        // queda reconstruir el cubo y ver si se pinta bien y se ha calculado correctamente.
        glNormal3fv(&normal[0]);
        Eigen::Vector3f pt2 = orig+direction1;
        Eigen::Vector3f pt3 = orig+direction1+direction2;
        Eigen::Vector3f pt4 = orig+direction2;

        glVertex3fv(&orig[0]); glVertex3fv(&pt2[0]); glVertex3fv(&pt3[0]); glVertex3fv(&pt4[0]);

        glEnd();

        if(blend)
        {
            glDisable(GL_BLEND);
        }
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);

}


void drawCube(Eigen::Vector3d o, double cellSize, Eigen::Vector3f color,vector<bool>& renderSide, bool blend = false, bool lighting = false)
{
    Eigen::Vector3f v[8];

    v[0] = Eigen::Vector3f(o.x(), o.y()+cellSize, o.z());
    v[1] = Eigen::Vector3f(o.x(), o.y()+cellSize, o.z()+cellSize);
    v[2] = Eigen::Vector3f(o.x(), o.y(), o.z()+cellSize);
    v[3] = Eigen::Vector3f(o.x(), o.y(), o.z());
    v[4] = Eigen::Vector3f(o.x()+cellSize, o.y()+cellSize, o.z());
    v[5] = Eigen::Vector3f(o.x()+cellSize, o.y()+cellSize, o.z()+cellSize);
    v[6] = Eigen::Vector3f(o.x()+cellSize, o.y(), o.z()+cellSize);
    v[7] = Eigen::Vector3f(o.x()+cellSize, o.y(), o.z());

    if(!lighting)
        glDisable(GL_LIGHTING);

    if(blend)
        glColor4f((GLfloat)color[0], (GLfloat)color[1], (GLfloat)color[2], 0.3);
    else
        glColor3f((GLfloat)color[0], (GLfloat)color[1], (GLfloat)color[2]);
	
    glBegin(GL_QUADS);
    // queda reconstruir el cubo y ver si se pinta bien y se ha calculado correctamente.
		//if(renderSide[0])
		{
		   glNormal3f(-1,0,0);
		   glVertex3fv(&v[0][0]); glVertex3fv(&v[1][0]); glVertex3fv(&v[2][0]); glVertex3fv(&v[3][0]);
		}

		//if(renderSide[1])
		{
		glNormal3f(1,0,0);
		glVertex3fv(&v[7][0]); glVertex3fv(&v[6][0]); glVertex3fv(&v[5][0]); glVertex3fv(&v[4][0]);
		}
	   //if(renderSide[2])
	   {
		glNormal3f(0,0,1);
		glVertex3fv(&v[1][0]); glVertex3fv(&v[5][0]); glVertex3fv(&v[6][0]); glVertex3fv(&v[2][0]);
	   }
	   //if(renderSide[3])
	   {
		glNormal3f(0,0,-1);
		glVertex3fv(&v[0][0]); glVertex3fv(&v[3][0]); glVertex3fv(&v[7][0]); glVertex3fv(&v[4][0]);
	   }
	   //if(renderSide[4])
	   {
		glNormal3f(0,-1,0);
		glVertex3fv(&v[6][0]); glVertex3fv(&v[7][0]); glVertex3fv(&v[3][0]); glVertex3fv(&v[2][0]);
	   }
	   //if(renderSide[5])
	   {
		glNormal3f(0,1,0);
		glVertex3fv(&v[0][0]); glVertex3fv(&v[4][0]); glVertex3fv(&v[5][0]); glVertex3fv(&v[1][0]);
	   }
	glEnd();

    if(!lighting) glEnable(GL_LIGHTING);
}

void gridRenderer::drawFunc(object* obj)
{
    // transformaciones
    //beforeDraw(obj);

    if(!dirtyFlag || !dirtyXZ || !dirtyXY)
    {
        updateSlices();
        dirtyFlag = false;
    }

	vector<bool> renderSide;
	//renderSide.resize(6,false);

    // drawing
    if(visible)
    {
        if(m_bShowAllGrid)
        {
			//printf("intentamos pintar %d celdas/n", grid.borderCellsCounts);fflush(0);
			//vector<GLfloat> pointsToRender;
			//vector<GLfloat> colorsToRender;
			//pointsToRender.resize(grid.borderCellsCounts*3);
			//colorsToRender.resize(grid.borderCellsCounts*3);

			glDisable(GL_LIGHTING);

			glPointSize(1);
			
            // Lo ideal es crear un grid booleano para pintar o no, tener resuelto el test.
            for(int i = 0; i< grid->dimensions.x(); i++)
            {
                for(int j = 0; j< grid->dimensions.y(); j++)
                {
                    for(int k = 0; k< grid->dimensions.z(); k++)
                    {
                        cell3d* cell = grid->cells[i][j][k];
                        Eigen::Vector3d o(grid->bounding.min + Eigen::Vector3d(i,j,k)*grid->cellSize + Eigen::Vector3d(0.5,0.5,0.5)*grid->cellSize);

						if(cell->getType() != EXTERIOR)
                        {
							Eigen::Vector3f col;
							col[0] = cell->data->color[0];
							col[1] = cell->data->color[1];
							col[2] = cell->data->color[2];

							if(cell->data->ownerWeight > 0)
							{
								glPointSize(10*cell->data->ownerWeight);

								glBegin(GL_POINTS);
								glColor3f((GLfloat)col[0], (GLfloat)col[1], (GLfloat)col[2]);
								glVertex3f((GLfloat)o[0], (GLfloat)o[1], (GLfloat)o[2]);
								glEnd();
							}
                        }
                    }
                }
			}
			
			glEnable(GL_LIGHTING);

			//glDisable(GL_LIGHTING);
			//glPointSize(5);
			//glBegin(GL_POINTS);

			/*
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);

			printf("ipara pintar\n");fflush(0);
			glVertexPointer(3, GL_FLOAT, 0, &pointsToRender[0]);
			glColorPointer(3, GL_FLOAT, 0, &colorsToRender[0]);
			printf("pintado\n");fflush(0);

			glPushMatrix();
			glDrawArrays(GL_POINTS, 0, 3*grid.borderCellsCounts);
			glPopMatrix();

			// deactivate vertex arrays after drawing
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			*/

			}
        else
        {
            if(sliceValuesXY.size() != 0 && sliceValuesXZ.size() != 0)
            {
                float incremento = (grid->bounding.max.z()-grid->bounding.min.z())/grid->dimensions.z();
                float posZ = incremento*XYValue/2;
                Eigen::Vector3f orig(grid->bounding.min.x(), grid->bounding.min.y(), grid->bounding.min.z());
                orig = orig + Eigen::Vector3f(0,0, posZ);

                Eigen::Vector3f dir1(incremento,0,0);
                Eigen::Vector3f dir2(0,incremento,0);
                Eigen::Vector3f n(0,0,incremento);


                for( int i = 0; i< grid->dimensions.x(); i++)
                {
                    for( int j = 0; j< grid->dimensions.y(); j++)
                    {
                        Eigen::Vector3f o(orig + Eigen::Vector3f(i*incremento,j*incremento,posZ));
                        drawQuad(o,dir1,dir2,n, sliceValuesXY[i][j], false);
                    }
                }

                incremento = (grid->bounding.max.y()-grid->bounding.min.y())/grid->dimensions.y();
                float posY = incremento*XZValue/2;
                orig = Eigen::Vector3f(grid->bounding.min.x(), grid->bounding.min.y(), grid->bounding.min.z());
                orig = orig + Eigen::Vector3f(0,posY,0);

                dir1 = Eigen::Vector3f(incremento,0,0);
                dir2 = Eigen::Vector3f(0,0,incremento);
                n = Eigen::Vector3f(0,incremento,0);


                for( int i = 0; i< grid->dimensions.x(); i++)
                {
                    for( int k = 0; k< grid->dimensions.z(); k++)
                    {
                        Eigen::Vector3f o(orig + Eigen::Vector3f(i*incremento,posY,k*incremento));
                        drawQuad(o,dir1,dir2,n, sliceValuesXZ[i][k], false);
                    }
                }
            }
            else
            {
                Initialized = false;
                propagateDirtyness();
                //printf("Los planos no estan bien inicializados\n");
            }
        }

        glDisable(GL_LIGHTING);
        glColor3f((GLfloat)0.1, (GLfloat)0.9, (GLfloat)0.1);
        glBegin(GL_LINES);

        for(unsigned int i = 0; i< grid->v.voronoiGraph.size(); i++)
        {
            Eigen::Vector3d orig = ((DefNode)grid->v.intPoints[i]).pos;

            for(unsigned int j = 0; j< grid->v.voronoiGraph[i].size(); j++)
            {
                if(grid->v.voronoiGraph[i][j])
                {
                    // queda reconstruir el cubo y ver si se pinta bien y se ha calculado correctamente.
                    glVertex3f(orig.x(), orig.y(), orig.z());
                    glVertex3f(grid->v.intPoints[j].pos.x(), grid->v.intPoints[j].pos.y(), grid->v.intPoints[j].pos.z());
                }
            }
        }

        // pintamos el grafo de voronoi
        for(unsigned int i = 0; i< grid->v.intPoints.size(); i++)
        {
            drawPointLocator(grid->v.intPoints[i].pos, 0.025, (int)i == desiredVertex);
        }

        glEnd();
        glEnable(GL_LIGHTING);
    }

    // postDrawing
    //afterDraw(obj);
}


void gridRenderer::beforeDraw(object* obj)
{
    if(dirtyFlag) 
		update(obj);
}

bool gridRenderer::update(object* obj)
{
    if(!dirtyFlag && !dirtyXZ && !dirtyXY)
        return true;
    else
    {
        updateSlices();
        dirtyFlag = false;
    }

    return true;
}

void gridRenderer::updateGridColorsAndValuesRGB()
{
	// Actualizamos los colores para no tener que buscar y calcular
	for(int i = 0; i< grid->dimensions.x(); i++)
    {
        for(int j = 0; j< grid->dimensions.y(); j++)
        {
            for(int k = 0; k< grid->dimensions.z(); k++)
            {
                if(grid->cells[i][j][k]->getType() != EXTERIOR )
                {
					cell3d* cell = grid->cells[i][j][k];

					if(cell->data->influences.size() != 3) continue;

					cell->data->ownerWeight = 1;
					cell->data->color = Eigen::Vector3f( cell->data->influences[0].weightValue,  cell->data->influences[1].weightValue,  cell->data->influences[2].weightValue);
				}
			}
		}
	}
}

void gridRenderer::updateGridColorsAndValues()
{
	float minValue = 99;
	float maxValue = -99;

		// Actualizamos los colores para no tener que buscar y calcular
	for(int i = 0; i< grid->dimensions.x(); i++)
    {
        for(int j = 0; j< grid->dimensions.y(); j++)
        {
            for(int k = 0; k< grid->dimensions.z(); k++)
            {
                if(grid->cells[i][j][k]->getType() != EXTERIOR )
                {
					cell3d* cell = grid->cells[i][j][k];
					for(int infl = 0; infl < cell->data->influences.size(); infl++)
					{
						if(cell->data->influences[infl].label == m_iWeightsId)
						{
							minValue = min(cell->data->influences[infl].weightValue, minValue);
							maxValue = max(cell->data->influences[infl].weightValue, maxValue);
						}
					}
				}
			}
		}
	}

	// Actualizamos los colores para no tener que buscar y calcular
	for(int i = 0; i< grid->dimensions.x(); i++)
    {
        for(int j = 0; j< grid->dimensions.y(); j++)
        {
            for(int k = 0; k< grid->dimensions.z(); k++)
            {
                if(grid->cells[i][j][k]->getType() != EXTERIOR )
                {
					cell3d* cell = grid->cells[i][j][k];

					int ce = -1;
					/*if(cell->data->influences.size() > 0)
					{
						float r,g,b;
						float value = 1.0;
						cell->data->ownerWeight =  0.3;
						GetColourGlobal(value,minValue,maxValue, r, g, b);
						cell->data->color = Eigen::Vector3f(r,g,b);
					}
					else
					{
						float r,g,b;
						float value = 0.0;
						cell->data->ownerWeight = 0.0;
						GetColourGlobal(value,minValue,maxValue, r, g, b);
						cell->data->color = Eigen::Vector3f(r,g,b);
					}
					continue;*/
					
					for(int infl = 0; infl < cell->data->influences.size(); infl++)
					{
						if(cell->data->influences[infl].label == m_iWeightsId)
						{
							ce = infl;
							cell->data->ownerWeight = cell->data->influences[infl].weightValue;
							
							if(cell->data->ownerWeight>1.0)
								cell->data->ownerWeight = 1.0;

							float r, g, b;
							float value = cell->data->ownerWeight;
							GetColourGlobal( value, minValue, maxValue, r, g, b);
							cell->data->ownerWeight = (value-minValue)/(maxValue-minValue) * 2;
							cell->data->color = Eigen::Vector3f( r, g, b);
							break;
						}
					}
					if(ce == -1)
					{
						cell->data->ownerWeight = 0.000;
						float r,g,b;
						GetColourGlobal(-1.0,0.0,1.0, r, g, b);
						cell->data->color = Eigen::Vector3f(r,g,b);
					}
				}
			}
		}
	}

	// Actualizamos el valor de influencia para pintarlo de alguna manera particular
}


void gridRenderer::init(int numLabels)
{
    int boneCount = numLabels;
    grid->valueRange = boneCount;
    grid->tradIds.resize(boneCount);
    for(int i = 0; i< boneCount; i++)
    {
        grid->tradIds[i] = rand()%boneCount;

        if(i > 0)
        {
            while(grid->tradIds[i] == grid->tradIds[i-1])
                grid->tradIds[i] = rand()%boneCount;
        }
    }
}

void gridRenderer::updateSlicesForSegmentation(int maxRange)
{
	XYValue = floor(((float)m_iCurrentSliceXY/1000.0)*((float)grid->dimensions.z()));
	XZValue = floor(((double)(m_iCurrentSliceXZ)/1000.0)*((float)grid->dimensions.y()));

	if(dirtyXY)
	{
		for(unsigned int i = 0; i< sliceValuesXY.size(); i++)
		{
			for(unsigned int j = 0; j< sliceValuesXY[i].size(); j++)
			{
				if(grid->cells[i][j][XYValue]->getType() == EXTERIOR)
				{
					sliceValuesXY[i][j].x() = 1.0;
					sliceValuesXY[i][j].y() = 1.0;
					sliceValuesXY[i][j].z() = 1.0;
				}
				else
				{
					if(grid->cells[i][j][XYValue]->data->segmentId < 0)
					{
						sliceValuesXY[i][j].x() = 0.0;
						sliceValuesXY[i][j].y() = 0.0;
						sliceValuesXY[i][j].z() = 0.0;
					}
					else
					{
						//int idOwnerTraduced = tradIds[grid.cells[i][j][XYValue]->data->segmentId];
						float idOwnerTraduced = (grid->cells[i][j][XYValue]->data->segmentId * 13) % maxRange;
						GetColour(idOwnerTraduced/maxRange, 0.0,1.0,
							  sliceValuesXY[i][j].x(), sliceValuesXY[i][j].y(), sliceValuesXY[i][j].z());
					}
				}
			}
		}
		dirtyXY = false;
	}

	if(dirtyXZ)
	{
		for(unsigned int i = 0; i< sliceValuesXZ.size(); i++)
		{
			for(unsigned int j = 0; j< sliceValuesXZ[i].size(); j++)
			{
				if(grid->cells[i][XZValue][j]->getType() == EXTERIOR)
				{
					sliceValuesXZ[i][j].x() = 1.0;
					sliceValuesXZ[i][j].y() = 1.0;
					sliceValuesXZ[i][j].z() = 1.0;
				}
				else
				{
					if(grid->cells[i][XZValue][j]->data->ownerLabel<0)
					{
						sliceValuesXZ[i][j].x() = 0.0;
						sliceValuesXZ[i][j].y() = 0.0;
						sliceValuesXZ[i][j].z() = 0.0;
					}
					else
					{
						//int idOwnerTraduced = tradIds[grid.cells[i][XZValue][j]->data->ownerLabel];
						float idOwnerTraduced = (grid->cells[i][XZValue][j]->data->segmentId * 13) % maxRange;
						GetColour(idOwnerTraduced/maxRange, 0.0,1.0,
								  sliceValuesXZ[i][j].x(), sliceValuesXZ[i][j].y(), sliceValuesXZ[i][j].z());
					}
				}
			}
		}

		dirtyXZ = false;	
	}
}

void gridRenderer::updateSlicesForVolumeLabels(int maxRange)
{
	XYValue = floor(((float)m_iCurrentSliceXY/1000.0)*((float)grid->dimensions.z()));
	XZValue = floor(((double)(m_iCurrentSliceXZ)/1000.0)*((float)grid->dimensions.y()));

	assert(maxRange != 0);

	//printf("vis_labels.\n"); fflush(0);
	if(dirtyXY)
	{
	//if(sliceValuesXY.size() > 0)
		//printf("se va a actualizar\n"); fflush(0);

	for(unsigned int i = 0; i< sliceValuesXY.size(); i++)
	{
		for(unsigned int j = 0; j< sliceValuesXY[i].size(); j++)
		{
			if(grid->cells[i][j][XYValue]->getType() == BOUNDARY)
				sliceValuesXY[i][j] = Eigen::Vector3f(0.8,0.1,0.1);
			else if(grid->cells[i][j][XYValue]->getType() == INTERIOR)
				sliceValuesXY[i][j] = Eigen::Vector3f(0.1,0.8,0.1);
			else if(grid->cells[i][j][XYValue]->getType() == EXTERIOR)
				sliceValuesXY[i][j] = Eigen::Vector3f(0.1,0.1,0.8);
		}
	}
	dirtyXY = false;
	//printf("actualizado XY\n"); fflush(0);
	}

	if(dirtyXZ)
	{
	//if(sliceValuesXZ.size() > 0)
	   // printf("se va a actualizar\n"); fflush(0);

	for(unsigned int i = 0; i< sliceValuesXZ.size(); i++)
	{
		for(unsigned int j = 0; j< sliceValuesXZ[i].size(); j++)
		{
			if(grid->cells[i][XZValue][j]->getType() == BOUNDARY)
				sliceValuesXZ[i][j] = Eigen::Vector3f(0.8,0.1,0.1);
			else if(grid->cells[i][XZValue][j]->getType() == INTERIOR)
				sliceValuesXZ[i][j] = Eigen::Vector3f(0.1,0.8,0.1);
			else if(grid->cells[i][XZValue][j]->getType() == EXTERIOR)
				sliceValuesXZ[i][j] = Eigen::Vector3f(0.1,0.1,0.8);
		}
	}

	dirtyXZ = false;
	//printf("actualizado XZ\n"); fflush(0);
	}
}

void gridRenderer::updateSlicesForWeights(float maxRange, int desiredIndex)
{
	int searchedIndex = -1;

	XYValue = floor(((float)m_iCurrentSliceXY/1000.0)*((float)grid->dimensions.z()));
	XZValue = floor(((double)(m_iCurrentSliceXZ)/1000.0)*((float)grid->dimensions.y()));

	if(dirtyXY)
	{
		for(unsigned int i = 0; i< sliceValuesXY.size(); i++)
		{
			for(unsigned int j = 0; j< sliceValuesXY[i].size(); j++)
			{
				if(grid->cells[i][j][XYValue]->getType() == EXTERIOR)
				{
					sliceValuesXY[i][j].x() = 1.0;
					sliceValuesXY[i][j].y() = 1.0;
					sliceValuesXY[i][j].z() = 1.0;
				}

				else
				{
					//vector<int>* labvector = &(grid.cells[i][j][XYValue]->data->labels);
					//vector<float>* weightvector = &(grid.cells[i][j][XYValue]->data->weights);
					searchedIndex = -1;
					for(unsigned int ce = 0; ce < grid->cells[i][j][XYValue]->data->influences.size(); ce++)
					{
						if(grid->cells[i][j][XYValue]->data->influences[ce].label == desiredIndex)
						{
							searchedIndex = ce;
							break;
						}
					}

					if(searchedIndex >= 0)
						GetColour(grid->cells[i][j][XYValue]->data->influences[searchedIndex].weightValue, 0.0,1.0,
						  sliceValuesXY[i][j].x(), sliceValuesXY[i][j].y(), sliceValuesXY[i][j].z());
					else
					{
						GetColour(0.0,0.0,1.0, sliceValuesXY[i][j].x(), sliceValuesXY[i][j].y(), sliceValuesXY[i][j].z());
					}

				}
			}
		}
		dirtyXY = false;
	}

	if(dirtyXZ)
	{
		for(unsigned int i = 0; i< sliceValuesXZ.size(); i++)
		{
			for(unsigned int j = 0; j< sliceValuesXZ[i].size(); j++)
			{
				if(grid->cells[i][XZValue][j]->getType() == EXTERIOR)
				{
					sliceValuesXZ[i][j].x() = 1.0;
					sliceValuesXZ[i][j].y() = 1.0;
					sliceValuesXZ[i][j].z() = 1.0;
				}
				else
				{
					searchedIndex = -1;
					//vector<int>* labvector = &(grid.cells[i][XZValue][j]->data->labels);
					//vector<float>* weightvector = &(grid.cells[i][XZValue][j]->data->weights);
					for(unsigned int ce = 0; ce< grid->cells[i][XZValue][j]->data->influences.size(); ce++)
					{
						if(grid->cells[i][XZValue][j]->data->influences[ce].label == desiredIndex)
						{
							searchedIndex = ce;
							break;
						}
					}

					if(searchedIndex >= 0)
					{
						GetColour((float)grid->cells[i][XZValue][j]->data->influences[searchedIndex].weightValue, 0.0,1.0,
						  sliceValuesXZ[i][j].x(), sliceValuesXZ[i][j].y(), sliceValuesXZ[i][j].z());
					}
					else
					{
						GetColour(0, 0.0,1.0, sliceValuesXZ[i][j].x(), sliceValuesXZ[i][j].y(), sliceValuesXZ[i][j].z());
					}
				}
			}
		}

		dirtyXZ = false;
	}
}

void gridRenderer::updateSlices()
{

    if(grid->valueRange == 0)
    {
        printf("tenemos un problema con valueRange.\n"); fflush(0);
        return;
    }

    if(!Initialized)
    {
        int dimX = grid->dimensions.x();
        sliceValuesXY.clear(); sliceValuesXY.resize(dimX);
        sliceValuesXZ.resize(grid->dimensions.x());

        for(unsigned int i = 0; i< sliceValuesXY.size(); i++)
            sliceValuesXY[i].resize(grid->dimensions.y());

        for(unsigned int i = 0; i< sliceValuesXZ.size(); i++)
            sliceValuesXZ[i].resize(grid->dimensions.z());

        Initialized = true;
    }

    int maxRange = grid->valueRange;
    assert(maxRange != 0);

    int desiredIndex = 0;
    if(desiredVertex >= 0)  desiredIndex = desiredVertex;

    switch(iVisMode)
    {
    case VIS_LABELS:
    default:
		updateSlicesForVolumeLabels(maxRange);
    break;
    
	case VIS_WEIGHTS:
        updateSlicesForWeights(1.0, desiredIndex);
        break;

    case VIS_SEGMENTATION:
		updateSlicesForSegmentation(maxRange);
        break;
    }
}
