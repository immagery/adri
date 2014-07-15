#include <DataStructures/Modelo.h>
#include <render/GeometryRender.h>
#include <render/ModeloRender.h>
#include <DataStructures/grid3D.h>
#include <DataStructures/scene.h>

//#include "GreenCoords.h"
//#include "HarmonicCoords.h"
//#include "MeanValueCoords.h"

#define DEBUG_OPS true

#include <iostream>
#include <fstream>

void Modelo::cleanSpotVertexes()
{
	((GeometryRender*)shading)->spotVertexes.clear();
}

void Modelo::addSpotVertex(int i)
{
	((GeometryRender*)shading)->spotVertex = i;
	//for(unsigned int b = 0; b < globalIndirection.size(); b++)
	//	if(globalIndirection[b]== i)
	//	{
			((GeometryRender*)shading)->spotVertexes.push_back(i);//globalIndirection[b]);
			return;
	//	}
}

void Modelo::setSpotVertexes(vector<int>& indices)
{
	assert(false);
	/*
	((GeometryRender*)shading)->spotVertexes.clear();
	
	for(unsigned int b = 0; b < globalIndirection.size(); b++)
	{
		for(unsigned int ind = 0; ind < indices.size(); ind++)
		{
			if(globalIndirection[b]== indices[ind])
			{
				((GeometryRender*)shading)->spotVertexes.push_back(globalIndirection[b]);
			}	
		}
	}
	*/
}

void Modelo::setSpotVertex(int i)
{
	assert(false);
	// tofix [13/11/13]

	//((GeometryRender*)shading)->spotVertex = i;
	//for(unsigned int b = 0; b < globalIndirection.size(); b++)
	//	if(globalIndirection[b]== i)
	//	{
	//		((GeometryRender*)shading)->spotVertex = globalIndirection[b];
	//		return;
	//	}
}

void Modelo::copyFrom(Modelo& orig_model)
{
	// TODEBUG
	assert(false);

	if(orig_model.originalModelLoaded)
	{
		originalModel = new Geometry();
		originalModel->copyFrom(orig_model.originalModel);
		originalModelLoaded = orig_model.originalModel;
	}

	
	// Bind
	if(bind)
	{
		bind->copyFrom(orig_model.bind);
		computedBindings = orig_model.bind;
	}
	
	rigBinded = orig_model.rigBinded;
	sModelPath = orig_model.sModelPath;
	sModelPrefix = orig_model.sModelPrefix;
}

Modelo::Modelo() : Geometry()
{
    // Modelo original que cargamos en memoria.
    sModelPath = "";
    sModelPrefix = "";

	bind = new binding();
	computedBindings = false;
	rigBinded = false;

	grid = NULL;

	delete shading;
	shading = new ModeloRender(this);
}

Modelo::Modelo(unsigned int nodeId) : Geometry(nodeId)
{
    // Modelo original que cargamos en memoria.
    sModelPath = "";
    sModelPrefix = "";

	originalModel = new Geometry(scene::getNewId(T_OBJECT));
	originalModelLoaded = false;

	bind = new binding();
	computedBindings = false;
	rigBinded = false;

	grid = NULL;

	delete shading;
	shading = new ModeloRender(this);
}

Modelo::~Modelo()
{
    //currentRender = NULL;

	//modelVertexDataPoint.clear();
	//modelVertexBind.clear();
	//globalIndirection.clear();
	//embedding.clear();
	delete bind;
	delete originalModel;

	//for(unsigned int i = 0; i< bindings.size(); i++)
	//	delete bindings[i];
}

void initgrid()
{
	//TODO
}

void Modelo::Clear()
{
	// Hay que revisarlo.
	assert(false);

	/*
    // Cajas para hacer la deformacion
    if(modelCage)
        modelCage->Clear(); // Caja envolvente original

    if(dynCage)
        dynCage->Clear(); // Caja que puede variar interactivamente
    for(unsigned int i = 0; i< stillCages.size(); i++)
    {
        stillCages[i]->Clear();
        delete stillCages[i];
    }

    stillCages.clear();
    sModelPath = "";
    sModelPrefix = "";

    for(unsigned int i = 0; i< embedding.size(); i++)
        embedding[i].clear();

    embedding.clear();

    //PerVertGC.clear();
    //PerFaceGC.clear();
    //PerVertHC.clear();
    //PerVertMVC.clear();

    //HCgrid.clear();

    //newModeloGC.Clear();
    //newModeloHC.Clear();
    //newModeloMVC.Clear();

    currentCage = NULL;
	*/
}

void Modelo::drawFunc()
{
    // Pintamos este modelo
	((ModeloRender*)shading)->drawFunc(this);

    shading->beforeDraw(this); // Las cajas y dem·s se mover·n solidarias.

	// Objects associated
    //if(currentCage)
    //    currentCage->drawFunc();

    shading->afterDraw(this);
}

bool Modelo::select(bool bToogle, unsigned int id)
{
    //bool lo_tengo = false;
	bToogle = bToogle;

	bool selected = false;

    if(shading) shading->selected = ( id == nodeId) & bToogle;
	selected = ( id == nodeId) & bToogle;

	/*
    if(modelCage && modelCage->shading != NULL)
    {
        bool sel = id == modelCage->nodeId;
        modelCage->shading->selected = sel;
        modelCage->shading->visible = sel;

        if(sel)
            currentCage = modelCage;
    }
    if(dynCage && dynCage->shading != NULL)
    {
        bool sel = id == dynCage->nodeId;
        dynCage->shading->selected = sel;
        dynCage->shading->visible  = sel;

        if(sel)
            currentCage = dynCage;
    }

    for(unsigned int i = 0; i< stillCages.size(); i++)
    {
        if(stillCages[i]->shading)
        {
            bool sel = id == stillCages[i]->nodeId;
            stillCages[i]->shading->selected = sel;
            ((Cage*)stillCages[i])->shading->visible = sel;

            if(sel)
                currentCage = stillCages[i];
        }
    }
	*/

	return selected;
}

void BuildSurfaceGraphs(Modelo* m)
{
	//vector<binding*> bindings;

	vector<GraphNode*>& nodes = m->nodes;
	vector<GraphNodePolygon*>& triangles = m->triangles;

	bool initMinBBox = false;
	m->minBBox = Vector3d(0,0,0);
	m->maxBBox = Vector3d(0,0,0);

	// Buscamos componentes conexas a la vez que nos quedamos
	// con los datos para luego crear los grafos.
	int idDispatcher = -1;
	vector<int> connIds;
	vector<bool> visIds;
	visIds.resize(m->nodes.size(),false);
	connIds.resize(m->nodes.size(), -1);
	vector<int> harvestIds;

	for(unsigned int n = 0; n < nodes.size(); n++)
	{
		// A. BoundingBox computation
		if(initMinBBox)
		{
			m->minBBox = Vector3d(min(m->minBBox.x(), nodes[n]->position.x()), 
								  min(m->minBBox.y(), nodes[n]->position.y()), 
								  min(m->minBBox.z(), nodes[n]->position.z()));
		}
		else
		{
			m->minBBox = nodes[n]->position;
			m->maxBBox = nodes[n]->position;
			initMinBBox = true;
		}

		m->maxBBox = Vector3d(max(m->maxBBox.x(), nodes[n]->position.x()), 
							  max(m->maxBBox.y(), nodes[n]->position.y()), 
							  max(m->maxBBox.z(), nodes[n]->position.z()));


		// B. Connectivity segmentation of the model
		if(connIds[n] < 0) // Not processed
		{
			// We assign a new ID
			idDispatcher++;
			connIds[n] = idDispatcher;

			// Initialization of variables for running
			for(unsigned int v = 0; v < visIds.size(); v++) visIds[v] = false;

			// Visited
			visIds[n] = true;

			// Add the neighbours
			vector<int> frontIds; 			
			for(int neigh = 0; neigh < nodes[n]->connections.size(); neigh++)
				frontIds.push_back(nodes[n]->connections[neigh]->id);

			harvestIds.push_back(idDispatcher);
			propagateIdFromNode(idDispatcher, frontIds, visIds, connIds, nodes);
		}
	}

	printf("Count of connex components: %d\n", idDispatcher+1);

	if(DEBUG_OPS)
	{
		// Check if all the vertices are assigned
		printf("Checking connexions...\n");
		for(unsigned int n = 0; n < nodes.size(); n++)
		{
			if(connIds[n] < 0)
			{
				printf("Aqui hay un problema con las conexiones, nodo %d\n", n);
			}		
		}

		printf("Posibles ids:\n");
		for(int r = 0; r < harvestIds.size(); r++)
		{
			printf("%d ", harvestIds[r]);
		}
		printf("\n");
	}

	map<int, int> idsDispatched;

	// Recopilation of dispatched ID's and reassignement
	vector<bool> founded;
	founded.resize(idDispatcher+1, false);
	int newIdCount = 0;
	for(int conId = 0; conId < connIds.size(); conId++)
	{
		map<int, int>::iterator it = idsDispatched.find(connIds[conId]);
		if(it == idsDispatched.end())
		{
			idsDispatched[connIds[conId]] = newIdCount;
			newIdCount++;
		}
	}

	// Apply the new ID and count the concidences
	vector<int> graphNodesCounter;
	graphNodesCounter.resize(idsDispatched.size());
	for(int i = 0; i< connIds.size(); i++)
	{
		connIds[i] = idsDispatched[connIds[i]];
		graphNodesCounter[connIds[i]]++;
	}

	if(DEBUG_OPS)
	{
		map<int, int>::iterator it = idsDispatched.begin();
		for(; it != idsDispatched.end(); it++) 
		{
			printf("[Connected part %d, %d]-> %d# nodes\n", it->first, 
														    it->second, 
															graphNodesCounter[it->second]);

			if(graphNodesCounter[it->second] < 2 )
				printf("   >>>   Hay un grupo degenerado   <<<   \n");

		}
	}

	m->bind->resize(nodes.size());
	binding* bd = m->bind;
	bd->surfaces.resize(graphNodesCounter.size());

	// Quiero probar que explota para que poder corregirlo.
	if(graphNodesCounter.size() > 0)
		bd->mainSurface = NULL;//&bd->surfaces[0];

	int globalCount = 0;
	for(int bbIdx = 0; bbIdx < bd->surfaces.size(); bbIdx++)
	{
		SurfaceGraph* subGraph = &bd->surfaces[bbIdx];
		subGraph->nodes.resize(graphNodesCounter[bbIdx]);

		int count = 0;
		for(int i = 0; i< nodes.size(); i++)
		{
			if(connIds[i] == bbIdx)
			{
				nodes[i] -> pieceId = count;

				bd->surfaces[bbIdx].nodes[count] = nodes[i];
				bd->pointData[nodes[i]->id].node = nodes[i];
				
				bd->pointData[nodes[i]->id].component = bbIdx;

				count++;
				globalCount++;
			}
		}

		assert(count == graphNodesCounter[bbIdx]);
	}

	assert(globalCount == nodes.size());

	for(int bbIdx = 0; bbIdx < bd->surfaces.size(); bbIdx++)
	{
		SurfaceGraph* subGraph = &bd->surfaces[bbIdx];

		// Count triangle coincidences
		vector<int> triangleIds;
		for(int i = 0; i< triangles.size(); i++)
		{
			for(int trTemp = 0; trTemp < triangles[i]->verts.size(); trTemp++)
			{
				int vertId =  triangles[i]->verts[trTemp]->id;
				int pieceId = triangles[i]->verts[trTemp]->pieceId;
				// If any of the vertices are from this piece
				if(bd->pointData[vertId].component == bbIdx)
				{
					triangleIds.push_back(i);
					break;
				}
			}
		}

		// Copy the pointers
		subGraph->triangles.resize(triangleIds.size());
		for(int i = 0; i< triangleIds.size(); i++)
		{
			subGraph->triangles[i] = triangles[triangleIds[i]];
		}
	}

	// Construimos una matriz de adyacencia que tambien
	// recoge si una arista es borde(1) o no (2)
	vector< vector<short> > edges; 
	edges.resize(m->vn());
	for(int i = 0; i< edges.size(); i++)
		edges[i].resize(m->vn(), 0);
	
	for(int fj = 0; fj < m->triangles.size(); fj++ )
	{
        Eigen::Vector3d O, c, s;
        Eigen::Vector3i idVerts;
        for(int i = 0; i<3; i++) // Obtenemos los indices de los vertices de t
			idVerts[i] = m->triangles[fj]->verts[i]->id;

		for(int i = 0; i<3; i++)
			edges[idVerts[i]][idVerts[(i+1)%3]]++;
	}

	for(int pt = 0; pt < m->nodes.size(); pt++)
	{
		int vert = bd->pointData[pt].node->id;
		for(int con = 0; con< m->nodes[pt]->connections.size(); con++)
		{
			int conected = m->nodes[pt]->connections[con]->id;
			int count = edges[vert][conected]+edges[conected][vert];

			// Comprobamos si es un borde -> si hay conexion deberia tener valor de mas de uno.
			bd->pointData[pt].isBorder |= count < 2;
			bd->pointData[conected].isBorder |= count < 2;
		}
	}

}

void propagateIdFromNode(int id, 
						 vector<int>& frontIds,
						 vector<bool>& visIds,
						 vector<int>& connIds,
						 vector<GraphNode*> nodes)
{
	while(frontIds.size() > 0)
	{
		int newId = frontIds.back();
		frontIds.pop_back();

		// Visited
		if(visIds[newId]) continue;

		int connId = connIds[newId];
		if(connId < 0) // Not assigned
		{
			connIds[newId] = id;
			visIds[newId] = true;
			for(unsigned int neigh = 0; neigh < nodes[newId]->connections.size(); neigh++)
			{
				if(!visIds[nodes[newId]->connections[neigh]->id])
					frontIds.push_back(nodes[newId]->connections[neigh]->id);
			}
		}
		else if(connId != id)
		{
			// Change the id for all the piece found
			for(unsigned int i = 0; i< connIds.size(); i++)
			{
				if(connIds[i] == connId) 
				{
					connIds[i] = id;
				}
			}

			//harvestIds.push_back(newId);
		}

	
	}
}

/*
bool Modelo::processGreenCoordinates()
{

    newModeloGC.Clear();
     tri::Append<MyMesh, MyMesh>::Mesh(newModeloGC,modeloOriginal, false);

    string sGCSavedCoords = sModelPath;
    sGCSavedCoords.append("/").append(name).append("_").append(GC_COMP_FILE_NAME);

    ifstream myfile;
    myfile.open (sGCSavedCoords.c_str(), ios::in |ios::binary);
    if (myfile.is_open())
    {
        myfile.close();
        LoadGCCoordsFromFile(PerVertGC, PerFaceGC, sGCSavedCoords);
        return false;
    }
    else
    {
        myfile.close();
        gpCalculateGreenCoordinates( modeloOriginal, cage, PerVertGC, PerFaceGC, sGCSavedCoords);
    }

    return true;

}
*/
/*
bool Modelo::processHarmonicCoordinates()
{
    newModeloHC.Clear();
     tri::Append<MyMesh, MyMesh>::Mesh(newModeloHC,modeloOriginal, false);

    //unsigned int resolution = pow(2,7);
    unsigned int resolution = pow(2,7);
    string sHCSavedGrid = sModelPath;
    sHCSavedGrid.append("/").append(name).append("_").append(HC_GRID_FILE_NAME);

    ifstream myfile;
    myfile.open (sHCSavedGrid.c_str(), ios::in |ios::binary);
    if (myfile.is_open())
    {
        myfile.close();
        loadHarmonicCoordinates(modeloOriginal, HCgrid, PerVertHC, sHCSavedGrid);
                return false;
    }
    else
    {
        myfile.close();
        getHarmonicCoordinates(modeloOriginal, cage, HCgrid, PerVertHC, resolution, sHCSavedGrid);
    }

    return true;
}
*/

/*
bool Modelo::processMeanValueCoordinates()
{
    newModeloMVC.Clear();
     tri::Append<MyMesh, MyMesh>::Mesh(newModeloMVC,modeloOriginal, false);

    string sMVCSavedGrid = sModelPath;
    sMVCSavedGrid.append("/").append(name).append("_").append(MVC_COMP_FILE_NAME);
    ifstream myfile;
    myfile.open (sMVCSavedGrid.c_str(), ios::in |ios::binary);
    if (myfile.is_open())
    {
        myfile.close();
        LoadMVCoordsFromFile(PerVertMVC, sMVCSavedGrid);
                return false;
    }
    else
    {
        myfile.close();
        CalculateMeanValueCoords( modeloOriginal, cage, PerVertMVC, sMVCSavedGrid);
    }

    return true;
}
*/

/*
bool Modelo::deformMesh(int cageId, coordsType coords)
{
    // Ojo!-> si no se han computado las coordenadas antes -> ¡¡ERROR!!

    // Primero escogemos la caja con la que queremos hacer la deformación
    MyMesh* newCage;
    if(cageId < 0 || cageId >= (int)stillCages.size()) // Caso normal
        newCage = &dynCage;
    else
    {
        newCage = stillCages[cageId];
    }

    if(coords == HARMONIC_COORDS)
    {
        deformMeshWithHC(modeloOriginal, cage, newModeloHC, *newCage, PerVertHC);
    }
    else if(coords == GREEN_COORDS)
    {
        deformMeshWithGC(modeloOriginal, cage, newModeloGC, *newCage, PerVertGC, PerFaceGC);
    }
    else if(coords == MEANVALUE_COORDS)
    {
        deformMeshWithMVC(modeloOriginal, cage, newModeloMVC, *newCage, PerVertMVC);
    }
    else if(coords == ALL_COORDS)
    {
        // Green Coordinates
        deformMeshWithGC(modeloOriginal, cage, newModeloGC, *newCage, PerVertGC, PerFaceGC);

        // Harmonic Coordinates
        deformMeshWithHC(modeloOriginal, cage, newModeloHC, *newCage, PerVertHC);

        //Mean Value Coordinates
        deformMeshWithMVC(modeloOriginal, cage, newModeloMVC, *newCage, PerVertMVC);
    }

    return true;
}
*/
