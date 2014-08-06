#include <DataStructures\AirVoxelization.h>
#include <DataStructures\Modelo.h>
#include <utils\util.h>

#include <iostream>
#include <fstream>

#define TERMINATE_CRITERION 0.0001
#define DEBUG false

using namespace std;

/*
int findWeight(vector<weight>& weights, int label)
{
	for(unsigned int i = 0; i< weights.size(); i++)
	{
		if(weights[i].label == label)
			return i;
	}

	return -1;
}
*/

/////////////////////////////////////
////////////   VOXELIZATION   ///////////////

// constructor
voxGrid3d::voxGrid3d()
{

}

voxGrid3d::~voxGrid3d()
{
	for (int i = 0; i< cells.size(); i++)
	{
		for (int j = 0; j< cells[i].size(); j++)
		{
			for (int k = 0; k< cells[i][j].size(); k++)
			{
				cells[i][j][k].clear();
			}
			cells[i][j].clear();
		}
		cells[i].clear();
	}
	cells.clear();
}

void voxGrid3d::init(MyBox3 in_bounding, Vector3i divisions, float _cellSize)
{
	bounding = in_bounding;
	dimensions = divisions;
	cellSize = _cellSize;
	
	cells.resize(dimensions.x());

	#pragma omp parallel for
	for (int i = 0; i< dimensions.x(); i++)
	{
		cells[i].resize(dimensions.y());
		for (int j = 0; j< dimensions.y(); j++)
		{
			cells[i][j].resize(dimensions.z());
			for (int k = 0; k< dimensions.z(); k++)
			{
				cells[i][j][k].clear();
			}
		}
	}
}

void voxGrid3d::clearData()
{
	#pragma omp parallel for
	for(int i = 0; i< cells.size(); i++)
	{
		for(int j = 0; j< cells[i].size(); j++)
		{
			for(int k = 0; k< cells[i][j].size(); k++)
			{
				cells[i][j][k].clear();
			}
		}
	}
}

// merge grids of the same size
void voxGrid3d::mergeResults(voxGrid3d* grid, int idx)
{
	if(grid->dimensions.x() != dimensions.x() ||
	   grid->dimensions.y() != dimensions.y() ||
	   grid->dimensions.z() != dimensions.z() )
	   {
			printf("No se puede copiar!!, No coinciden los tamanos!!\n\n\n");
			return;
	   }

	//printf("Dimensiones de copia: (%d %d %d)\n", grid->dimensions.x(), grid->dimensions.y(), grid->dimensions.z());

	//#pragma omp parallel for
	for(int i = 0; i< grid->dimensions.x(); i++)
	{
		for(int j = 0; j< grid->dimensions.y(); j++)
		{
			for(int k = 0; k< grid->dimensions.z(); k++)
			{
				for (int typeIdx = 0; typeIdx < grid->cells[i][j][k].size(); typeIdx++)
				{
					T_vox type = grid->cells[i][j][k][typeIdx]->getType();
					if (type == VT_INTERIOR || type == VT_BOUNDARY)
					{
						// Podemos hacer una comparacion para quedarnos con lo que es interior sobre lo que es borde.
						cells[i][j][k].push_back(new vox3D(idx, (T_vox)type));
					}
				}				
			}
		}
	}
}

Vector3d voxGrid3d::getCenterOfCell(int i, int j, int k)
{
	return bounding.min + Eigen::Vector3d(((float)i+0.5)*cellSize, ((float)j+0.5)*cellSize,((float)k+0.5)*cellSize);
}

bool voxGrid3d::isOut(Eigen::Vector3i& pt)
{
	return (pt.x() < 0 || pt.y() < 0 || pt.z() < 0 ||
		    pt.x() >= dimensions.x() || pt.y() >=  dimensions.y()  || pt.z() >=  dimensions.z());

}

bool voxGrid3d::hasData(Eigen::Vector3i& pt)
{
	assert(false);
	return false;
}

int voxGrid3d::fillFromCorners( Vector3i minCell, Vector3i maxCell)
{
	vector<Vector3i> listForProcess;
	int cellsCount = 0;

	// Encolamos los 8 extremos.
	listForProcess.push_back( Vector3i( minCell.x(), minCell.y() , minCell.z() ));
	listForProcess.push_back( Vector3i( minCell.x(), maxCell.y() , minCell.z() ));
	listForProcess.push_back( Vector3i( minCell.x(), minCell.y() , maxCell.z() ));
	listForProcess.push_back( Vector3i( minCell.x(), maxCell.y() , maxCell.z() ));

	listForProcess.push_back( Vector3i(maxCell.x() , minCell.y() , minCell.z() ));
	listForProcess.push_back( Vector3i(maxCell.x() , maxCell.y() , minCell.z() ));
	listForProcess.push_back( Vector3i(maxCell.x() , minCell.y() , maxCell.z() ));
	listForProcess.push_back( Vector3i(maxCell.x() , maxCell.y() , maxCell.z() ));

	//Reset the visits
	Vector3i newDimensions; 
	newDimensions = maxCell - minCell + Vector3i(1,1,1);

	// Mark the out boundingBox
	#pragma omp parallel for
	for (int dimX = 0; dimX < dimensions.x(); dimX++)
	{
		for (int dimY = 0; dimY < dimensions.y(); dimY++)
		{
			for (int dimZ = 0; dimZ < dimensions.z(); dimZ++)
			{
				if (dimX < minCell.x() || dimY < minCell.y() || dimZ < minCell.z() ||
					dimX > maxCell.x() || dimY > maxCell.y() || dimZ > maxCell.z())
				{
					if (cells[dimX][dimY][dimZ].size() == 0)
					{
						cells[dimX][dimY][dimZ].push_back(new vox3D(VT_EXTERIOR));
					}
				}
			}
		}
	}

	// Expanding neighbours
	vector<Vector3i> neighbours;
	neighbours.push_back(Vector3i(0, 0, -1)); 
	neighbours.push_back(Vector3i(0, 0, 1));
	neighbours.push_back(Vector3i(0, -1, 0));
	neighbours.push_back(Vector3i(0, 1, 0));
	neighbours.push_back(Vector3i(-1, 0, 0));
	neighbours.push_back(Vector3i(1, 0, 0));

	// We try to empty the queue.
	while (!listForProcess.empty())
	{
		Vector3i pos = listForProcess.back(); listForProcess.pop_back();

		if (cells[pos.x()][pos.y()][pos.z()].size() == 0)
		{
			cells[pos.x()][pos.y()][pos.z()].push_back(new vox3D(VT_EXTERIOR));
			cellsCount++;
		}
		else continue;

		for (unsigned int l = 0; l< neighbours.size(); l++)
		{
			int i = neighbours[l].x();
			int j = neighbours[l].y();
			int k = neighbours[l].z();

			Vector3i neighbourPos(pos.x() + i, pos.y() + j, pos.z() + k);

			// We add cells if are inside the grid and are free
			if (neighbourPos.x() >= minCell.x() && neighbourPos.y() >= minCell.y() && neighbourPos.z() >= minCell.z() &&
				neighbourPos.x() <= maxCell.x() && neighbourPos.y() <= maxCell.y() && neighbourPos.z() <= maxCell.z())
			{
				Vector3i neighbourPosRelated = neighbourPos - minCell;
				if (cells[neighbourPos.x()][neighbourPos.y()][neighbourPos.z()].size() == 0)
				{
					listForProcess.push_back(neighbourPos);
				}
			}
		}
	}

	return cellsCount;
}

int voxGrid3d::fillFromCorners()
{
    vector<Vector3i> listForProcess;
    int cellsCount = 0;

    // Encolamos los 8 extremos.
    listForProcess.push_back(Vector3i(0,0,0));
    listForProcess.push_back(Vector3i(0,dimensions.y()-1,0));
    listForProcess.push_back(Vector3i(0,0,dimensions.z()-1));
    listForProcess.push_back(Vector3i(0,dimensions.y()-1,dimensions.z()-1));

    listForProcess.push_back(Vector3i(dimensions.x()-1,0,0));
    listForProcess.push_back(Vector3i(dimensions.x()-1,dimensions.y()-1,0));
    listForProcess.push_back(Vector3i(dimensions.x()-1,0,dimensions.z()-1));
    listForProcess.push_back(Vector3i(dimensions.x()-1,dimensions.y()-1,dimensions.z()-1));

    // Los posibles vecinos de expansion
    vector<Vector3i> neighbours;
    neighbours.push_back(Vector3i(0,0,-1));
    neighbours.push_back(Vector3i(0,0,1));
    neighbours.push_back(Vector3i(0,-1,0));
    neighbours.push_back(Vector3i(0,1,0));
    neighbours.push_back(Vector3i(-1,0,0));
    neighbours.push_back(Vector3i(1,0,0));

    // Hasta que se vacie la cola recorremos los cubos vecinos.
    while(!listForProcess.empty())
    {
        Vector3i pos = listForProcess.back(); listForProcess.pop_back();

        if(cells[pos.x()][pos.y()][pos.z()].size() == 0)
        {
            cells[pos.x()][pos.y()][pos.z()].push_back(new vox3D(VT_EXTERIOR));
            cellsCount++;
        }
        else continue;

        for(unsigned int l = 0; l< neighbours.size(); l++)
        {
            int i = neighbours[l].x();
            int j = neighbours[l].y();
            int k = neighbours[l].z();

			// We add cells if are inside the grid and are free
            if(pos.x() + i >= 0 && pos.y() + j >= 0 && pos.z() + k >= 0 &&
               pos.x() + i < dimensions.x() && pos.y() + j < dimensions.y() && pos.z() + k  < dimensions.z())
            {
                if(cells[pos.x() + i][pos.y() + j][pos.z() + k].size() == 0)
                    listForProcess.push_back(Vector3i(pos.x() + i,pos.y() + j,pos.z() + k));
            }
        }
    }

    return cellsCount;
}

int voxGrid3d::fillInside()
{
    int cellsCount = 0;
    for(int i = 0; i< dimensions.x(); i++)
    {
        for(int j = 0; j< dimensions.y(); j++)
        {
            for(int k = 0; k< dimensions.z(); k++)
            {
				if(cells[i][j][k].size() == 0)
					cells[i][j][k].push_back(new vox3D(VT_INTERIOR));
            }
        }
    }

    return cellsCount;
}

int voxGrid3d::typeCells(SurfaceGraph* mesh)
{
    int TypedCells = 0;

	int partValue = mesh->triangles.size() / 10;

	for(int i = 0; i< mesh->triangles.size(); i++ )
	{
        TypedCells += typeCells(mesh, i);
	}

	// Get BoundingBox
	MyBox3 SubSurfaceBounding;
	for (unsigned int i = 1; i< mesh->nodes.size(); i++)
	{
		if (i == 0) SubSurfaceBounding.min = SubSurfaceBounding.max = mesh->nodes[0]->position;

		SubSurfaceBounding.min = minPt(SubSurfaceBounding.min, mesh->nodes[i]->position);
		SubSurfaceBounding.max = maxPt(SubSurfaceBounding.max, mesh->nodes[i]->position);
	}

	// The max and min cell to explore.
	Vector3i cellMin = cellId(SubSurfaceBounding.min);
	Vector3i cellMax = cellId(SubSurfaceBounding.max);

	// Expand the box
	for (int comp = 0; comp < 3; comp++) if(cellMin[comp] > 0) cellMin[comp] -= 1;
	for (int comp = 0; comp < 3; comp++) if (cellMax[comp] < dimensions[comp]-1) cellMax[comp] += 1;

	// Marcamos lo que es externo.
	fillFromCorners(cellMin, cellMax);

	// Marcamos lo que es interno.
    fillInside();

    return TypedCells;
}

int voxGrid3d::typeCells(SurfaceGraph* mesh, int triIdx)
{

    // A. Anadimos los vertices
    Vector3i cell[3];
    int boundaryCells = 0;

	GraphNodePolygon* tri = mesh->triangles[triIdx];
	vector<Eigen::Vector3d> points;
	points.resize(tri->verts.size());
	for(int i = 0; i< tri->verts.size(); i++)
		points[i] = tri->verts[i]->position;

	assert(tri->verts.size() == 3);

    // B. VERTEXES -> boundary
	for(int i = 0; i<points.size(); i++)
    {
		cell[i] = cellId(points[i]); // Obtenemos la celda en la que cae el vértice

		int x = cell[i].x();
		int y = cell[i].y();
		int z = cell[i].z();

		if(cells[x][y][z].size() == 0)
			cells[x][y][z].push_back(new vox3D(VT_BOUNDARY));
		else
			cells[x][y][z][0]->setType(VT_BOUNDARY);
    }

    // Ahora recorremos celda por celda y vamos pintando el contorno.
    float processCellSize = cellSize/3;

    // 3D Rendering de arestas
    // cellSize -> lado de cada cubo.
    // ponemos el valor para las arestas tambi�n.
    for(int k = 0; k<tri->verts.size(); k++)
    {
        Vector3d v = ( points[(k+1)%3] - points[k] );
        Vector3d v1 = points[k];

        int divisions = (int)floor(v.norm()/processCellSize);
        Vector3d vDir = v/v.norm();
        for(int i = 0; i< divisions ; i++)
        {
            Vector3d intPoint = vDir*i*processCellSize + v1;
            Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
            {
				if(cells[cell.x()][cell.y()][cell.z()].size() == 0)
				{
					cells[cell.x()][cell.y()][cell.z()].push_back(new vox3D(VT_BOUNDARY));
				}
				else
				{
					cells[cell.x()][cell.y()][cell.z()][0]->setType(VT_BOUNDARY);
				}
            }
        }
    }

    // buscamos la aresta mas larga.
    int largeIdx = 0;
	Vector3d v = (points[(largeIdx+1)%3]-points[largeIdx]);
    float edgeLong = v.norm();
    for(int k = 1; k<3; k++)
    {
        v = (points[(k+1)%3]-points[k]);
        if(edgeLong < v.norm())
        {
            largeIdx = k;
            edgeLong = v.norm();
        }
    }

    Vector3d v1 = Vector3d(points[(largeIdx+1)%3] - points[largeIdx]);
    Vector3d v2 = Vector3d(points[(largeIdx+2)%3] - points[largeIdx]);
    Vector3d v3 = Vector3d(points[(largeIdx+2)%3] - points[(largeIdx+1)%3]);

    Vector3d aux = v1.cross(v2);
    Vector3d normal = aux.cross(v1);

    float v1Norm = (float)v1.norm();
    float v2Norm = (float)v2.norm();
    float v3Norm = (float)v3.norm();
    float normalNorm = (float)normal.norm();

    Vector3d v1Dir = v1/v1Norm;
    Vector3d v2Dir = v2/v2Norm;
    Vector3d v3Dir = v3/v3Norm;
    Vector3d normalDir = normal/normalNorm;

    // TOCHECK Eigen::Vector3d edgeCenter = v1Dir*(v1Dir*v2) + points[largeIdx];
	Vector3d edgeCenter = v1Dir * (v1Dir.dot(v2)) + points[largeIdx];
    int div1 = (int)ceil((points[largeIdx]-edgeCenter).norm()/processCellSize);
    int div2 = (int)ceil((points[(largeIdx+1)%3]-edgeCenter).norm()/processCellSize);

    for(int i = 1; i< div1 ; i++) // Saltamos el 0 porque es el mismo vertice.
    {
        Vector3d minPt = v1Dir*i*processCellSize + points[largeIdx];
		// Suponemos que al ser triangulo rectangulo mantiene proporciones.
        Vector3d maxPt = v2Dir*((float)i/(float)div1)*v2Norm + points[largeIdx]; 

        Vector3d line = maxPt-minPt;
        int Ydivs = (int)floor(line.norm()/processCellSize);

        for(int j = 1; j< Ydivs ; j++) // Saltamos el 0 porque es el mismo vertice.
        {
            Vector3d intPoint = normalDir*j*processCellSize + minPt;
            Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
            {
				if(cells[cell.x()][cell.y()][cell.z()].size() == 0)
				{
					cells[cell.x()][cell.y()][cell.z()].push_back(new vox3D(VT_BOUNDARY));
				}
				else
				{
					cells[cell.x()][cell.y()][cell.z()][0]->setType(VT_BOUNDARY);
				}
            }
        }
    }


    for(int i = 1; i< div2 ; i++) // Saltamos el 0 porque es el mismo vertice.
    {
        Vector3d minPt = -v1Dir*i*processCellSize + points[(largeIdx+1)%3];
		// Suponemos que al ser triangulo rectangulo mantiene proporciones.
        Vector3d maxPt = v3Dir*((float)i/(float)div2)*v3Norm + points[(largeIdx+1)%3]; 

        Vector3d line = maxPt-minPt;
        int Ydivs = (int)floor(line.norm()/processCellSize);

        for(int j = 1; j< Ydivs ; j++) // Saltamos el 0 porque es el mismo vértice.
        {
            Eigen::Vector3d intPoint = normalDir*j*processCellSize + minPt;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
			{
				if(cells[cell.x()][cell.y()][cell.z()].size() == 0)
				{
					cells[cell.x()][cell.y()][cell.z()].push_back(new vox3D(VT_BOUNDARY));
				}
				else
				{
					cells[cell.x()][cell.y()][cell.z()][0]->setType(VT_BOUNDARY);
				}
			}
        }
    }

     return boundaryCells;
}

Vector3i voxGrid3d::cellId(Eigen::Vector3d pt)
{
    Vector3d aux = pt - bounding.min;
    int x = (int)floor(aux.x()/cellSize);
    int y = (int)floor(aux.y()/cellSize);
    int z = (int)floor(aux.z()/cellSize);
    return Vector3i(x,y,z);
}


bool voxGrid3d::isContained(Vector3i pos, int surfaceId, bool alsoBoundary)
{
	for (int i = 0; i < cells[pos.x()][pos.y()][pos.z()].size(); i++)
	{
		int piece = cells[pos.x()][pos.y()][pos.z()][i]->pieceId;
		// If it is interior or boundary
		if (piece == surfaceId)
		{
			T_vox type = cells[pos.x()][pos.y()][pos.z()][i]->getType();

			if (type == VT_INTERIOR || type == VT_BOUNDARY)
			{
				if (!alsoBoundary && type == VT_BOUNDARY || alsoBoundary)
					return true;
			}
		}
	}

	return false;
}


Vector3d voxGrid3d::cellCenter(int i,int j,int k)
{
    Vector3d aux = bounding.min;
    aux += Vector3d(cellSize*(i+0.5),cellSize*(j+0.5),cellSize*(k+0.5));
    return aux;
}


// Guardamos en binario el grid entero, para no tener que recalcular cada vez
void voxGrid3d::LoadGridFromFile(string sFileName)
{
	// RE-CODE
	assert(false);

	/*
    ifstream myfile;
	const char* charFileName = sFileName.c_str();
    myfile.open(charFileName, ios::in |ios::binary);
    if (myfile.is_open())
    {

        // Datos generales
        myfile.read((char*)  &res, sizeof(int) );
        myfile.read( (char*) &dimensions[0], sizeof(int)*3 );
        myfile.read( (char*) &cellSize, sizeof(float) );
        myfile.read( (char*) &weightSize, sizeof(int));

        Eigen::Vector3d minPt,maxPt ;
        myfile.read( (char*) &minPt, sizeof(double)*3);
        myfile.read( (char*) &maxPt, sizeof(double)*3);
        bounding = MyBox3(minPt, maxPt);

        myfile.read( (char*) &valueRange, sizeof(float));

        cells.resize(dimensions[0]);

        //Datos de cada una de las celdas.
        for(unsigned int i = 0; i< cells.size(); i++)
        {
            cells[i].resize(dimensions[1]);
            for(unsigned int j = 0; j< cells[i].size(); j++)
            {
                cells[i][j].resize(dimensions[2]);
                for(unsigned int k = 0; k< cells[i][j].size(); k++)
                {
                    cells[i][j][k] = new cell3d(true);
                    cells[i][j][k]->LoadFromFile(myfile);
                }
            }
        }

    }
    myfile.close();

	*/

}

// Cargamos de disco el grid entero.
void voxGrid3d::SaveGridToFile(string sFileName)
{
	// RE-CODE
	assert(false);

	/*
    ofstream myfile;
    myfile.open(sFileName.c_str(), ios::binary);
    if (myfile.is_open())
    {

        // Datos generales
        myfile.write((const char*) &res, sizeof(int));
        myfile.write((const char*) &dimensions, sizeof(int)*3);
        myfile.write((const char*) &cellSize, sizeof(float));
        myfile.write((const char*) &weightSize, sizeof(int));
        myfile.write((const char*) &bounding.min, sizeof(double)*3);
        myfile.write((const char*) &bounding.max, sizeof(double)*3);
        myfile.write((const char*) &valueRange, sizeof(float));


        //Datos de cada una de las celdas.
        for(unsigned int i = 0; i< cells.size(); i++)
        {
            for(unsigned int j = 0; j< cells[i].size(); j++)
            {
                for(unsigned int k = 0; k< cells[i][j].size(); k++)
                {
                    cells[i][j][k]->SaveToFile(myfile);
                }
            }
        }

    }
    myfile.close();
	*/
}

void voxGrid3d::SaveToFile(ofstream& myfile)
{
	// RE-CODE
	assert(false);

	/*
    int auxType = getType();
    myfile.write((const char*) &auxType, sizeof(int));

	// Solo guardamos si es boundary
	if(getType() == BOUNDARY || getType() == INTERIOR)
		data->SaveToFile(myfile);
	*/
}

void voxGrid3d::LoadFromFile(ifstream& myfile)
{
	/*
    int tipoAux;
    myfile.read( (char*) &tipoAux, sizeof(int) );
    setType((T_cell)tipoAux);

	// Solo cargamos si es boundary
	if(getType() == BOUNDARY || getType() == INTERIOR)
	{
		if(data == NULL) 
			data = new cellData();

		data->LoadFromFile(myfile);
	}
	*/
}


///FUNCTIONS THAT DOES THE PROCESS
void VoxelizeModel(Modelo* m, bool onlyBorders)
{
	clock_t begin = clock();

	MyBox3 bounding_;
	m->getBoundingBox(bounding_.min, bounding_.max);

	float dimValue = pow(2.0, 7);
	Vector3i divisions(dimValue, dimValue, dimValue);

	Vector3d diagonal(	bounding_.max.x() - bounding_.min.x(),
						bounding_.max.y() - bounding_.min.y(),
						bounding_.max.z() - bounding_.min.z()  );

	bounding_.min -= diagonal * 0.01;
	bounding_.max += diagonal * 0.01;

	double cellSize = diagonal.norm() / dimValue;

	divisions.x() = ceil((bounding_.max.x() - bounding_.min.x()) / cellSize);
	bounding_.max.x() = divisions.x()*cellSize + bounding_.min.x();

	divisions.y() = ceil((bounding_.max.y() - bounding_.min.y()) / cellSize);
	bounding_.max.y() = divisions.y()*cellSize + bounding_.min.y();

	divisions.z() = ceil((bounding_.max.z() - bounding_.min.z()) / cellSize);
	bounding_.max.z() = divisions.z()*cellSize + bounding_.min.z();

	// Processing grid
	voxGrid3d* processGrid = new voxGrid3d();
	processGrid->init(bounding_, divisions, cellSize);

	if (!m->grid)  m->grid = new voxGrid3d();

	m->grid->init(bounding_, divisions, cellSize);

	// process each grid
	for (int surfIdx = 0; surfIdx < m->bind->surfaces.size(); surfIdx++)
	{
		if (VERBOSE) printf("Procesing surface %d de %d\n", surfIdx, m->bind->surfaces.size());

		// 1. clear grid
		processGrid->clearData();

		// 2. process surface
		processGrid->typeCells(&m->bind->surfaces[surfIdx]);

		// 3. copy result
		m->grid->mergeResults(processGrid, surfIdx);
	}

	delete processGrid;
	processGrid = NULL;

	if (VERBOSE)
	{
		clock_t end = clock();
		cout << ">> TOTAL (construccion del grid desde cero): " << double(timelapse(begin, end)) << " s" << endl;
	}
}