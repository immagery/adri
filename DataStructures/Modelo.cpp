#include <DataStructures/Modelo.h>
#include <render/GeometryRender.h>
#include <render/ModeloRender.h>
#include <DataStructures/grid3D.h>

//#include "GreenCoords.h"
//#include "HarmonicCoords.h"
//#include "MeanValueCoords.h"

#include <iostream>
#include <fstream>

void Modelo::cleanSpotVertexes()
{
	((GeometryRender*)shading)->spotVertexes.clear();
}

void Modelo::addSpotVertex(int i)
{
	//((GeometryRender*)shading)->spotVertex = i;
	for(unsigned int b = 0; b < globalIndirection.size(); b++)
		if(globalIndirection[b]== i)
		{
			((GeometryRender*)shading)->spotVertexes.push_back(globalIndirection[b]);
			return;
		}
}

void Modelo::setSpotVertexes(vector<int>& indices)
{
	((GeometryRender*)shading)->spotVertexes.clear();
	//((GeometryRender*)shading)->spotVertex = i;
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
}

void Modelo::setSpotVertex(int i)
{
	//((GeometryRender*)shading)->spotVertex = i;
	for(unsigned int b = 0; b < globalIndirection.size(); b++)
		if(globalIndirection[b]== i)
		{
			((GeometryRender*)shading)->spotVertex = globalIndirection[b];
			return;
		}
}

Modelo::Modelo() : Geometry()
{
    // Modelo original que cargamos en memoria.
    sModelPath = "";
    sModelPrefix = "";

    modelCage = NULL;
    dynCage = NULL;

    currentCage = NULL;
    //currentRender = this;

    embedding.clear();
	modelVertexDataPoint.clear();
	modelVertexBind.clear();

	bindings.clear();
	computedBindings = false;

	delete shading;
	shading = new ModeloRender(this);
}

Modelo::Modelo(unsigned int nodeId) : Geometry(nodeId)
{
    // Modelo original que cargamos en memoria.
    sModelPath = "";
    sModelPrefix = "";

    modelCage = NULL;
    dynCage = NULL;

    currentCage = NULL;
    //currentRender = this;

    embedding.clear();
	modelVertexDataPoint.clear();
	modelVertexBind.clear();

	bindings.clear();
	computedBindings = false;

	delete shading;
	shading = new ModeloRender(this);
}

Modelo::~Modelo()
{
    delete modelCage;
    delete dynCage;

    for(unsigned int i = 0; i< stillCages.size(); i++)
        delete stillCages[i];

    stillCages.clear();

    modelCage = NULL;
    dynCage = NULL;
    currentCage = NULL;
    //currentRender = NULL;

	modelVertexDataPoint.clear();
	modelVertexBind.clear();
	globalIndirection.clear();
	embedding.clear();

	for(unsigned int i = 0; i< bindings.size(); i++)
		delete bindings[i];
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
	//Geometry::drawFunc();
	((ModeloRender*)shading)->drawFunc(this);

    // Pintamos la caja que toque.
    if(!currentCage)
    {
        if(modelCage)
             currentCage = modelCage;
    }

    shading->beforeDraw(this); // Las cajas y demás se moverán solidarias.

    if(currentCage)
        currentCage->drawFunc();

    shading->afterDraw(this);
}

bool Modelo::select(bool bToogle, unsigned int id)
{
    //bool lo_tengo = false;
	bToogle = bToogle;

	bool selected = false;

    if(shading) shading->selected = ( id == nodeId);
	selected = ( id == nodeId);

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

	return selected;
}

void BuildSurfaceGraphs(Modelo& m, vector<binding*>& bindings)
{
	//m->bindings.push_back(new binding(m->vn()));

	vector<GraphNode*>& nodes = m.nodes;
	vector<GraphNodePolygon*>& triangles = m.triangles;

	/*
	vector<GraphNode*> nodes;
	nodes.resize(m.vn);
	for(int i = 0; i< nodes.size(); i++)
		nodes[i] = new GraphNode(i);

	MyMesh::FaceIterator fi;
    int idx = 0;
    for(fi = m.face.begin(); fi!=m.face.end(); ++fi ) 
	{
		int pts[3];
		for(int i = 0; i< 3; i++)
			pts[i] = (*fi).V(i)->IMark();

		for(int vert = 0; vert < 3; vert++)
		{
			bool found = false;

			for(int con = 0; con < nodes[pts[vert]]->connections.size(); con++)
			{
				found |= nodes[pts[vert]]->connections[con]->id == pts[(vert+1)%3];
			}
			if(!found)
			{
				nodes[pts[vert]]->connections.push_back(nodes[pts[(vert+1)%3]]);
				nodes[pts[(vert+1)%3]]->connections.push_back(nodes[pts[vert]]);
			}
		}
    }
	*/

	// Buscamos componentes conexas a la vez que nos quedamos
	// con los datos para luego crear los grafos.
	int idDispatcher = -1;
	vector<int> connIds;
	vector<bool> visIds;
	visIds.resize(m.nodes.size(),false);
	connIds.resize(m.nodes.size(), -1);

	for(unsigned int n = 0; n < nodes.size(); n++)
	{
		// todavia no se ha trabajado, debe ser nuevo
		if(connIds[n] < 0)
		{
			// Asignamos ID nuevo
			idDispatcher++;
			connIds[n] = idDispatcher;

			// Iniciamos variables de recorrido
			for(unsigned int v = 0; v < visIds.size(); v++)
				visIds[v] = false;

			visIds[n] = true;

			vector<int> frontIds; 
			
			for(int neigh = 0; neigh < nodes[n]->connections.size(); neigh++)
				frontIds.push_back(nodes[n]->connections[neigh]->id);

			vector<int> harvestIds; harvestIds.push_back(idDispatcher);
			propagateIdFromNode(idDispatcher, frontIds, harvestIds, visIds, connIds, nodes);
		}
	}

	printf("Count of graphs: %d\n", idDispatcher+1);

	vector<bool> founded;
	founded.resize(idDispatcher+1, false);
	for(int conId = 0; conId < connIds.size(); conId++)
	{
		bool foundAll = true;
		for(int i = 0; i<= idDispatcher; i++)
		{
			foundAll &= founded[i];
			if(!founded[i]) 
			{ 
				founded[i] = connIds[conId] == i;
			}
		}
		if(foundAll) 
			break;
	}

	int relateId = 0;
	map<int, int> relateGraphId;
	for(int f = 0; f < founded.size(); f++)
	{
		if(founded[f])
		{
			relateGraphId[f] = relateId;
			relateId++;
		}
	}

	vector<int> graphNodesCounter;
	graphNodesCounter.resize(relateId);

	for(int i = 0; i< connIds.size(); i++)
	{
		connIds[i] = relateGraphId[connIds[i]];
		graphNodesCounter[connIds[i]]++;
	}

	for(int i = 0; i < graphNodesCounter.size(); i++) 
	{
		printf("[Connected part %d]-> %d# nodes\n", i, graphNodesCounter[i]);
	}

	bindings.resize(graphNodesCounter.size());
	m.modelVertexDataPoint.resize(nodes.size());
	m.modelVertexBind.resize(nodes.size());

	for(int bbIdx = 0; bbIdx < bindings.size(); bbIdx++)
	{
		bindings[bbIdx] = new binding(graphNodesCounter[bbIdx]);
		bindings[bbIdx]->bindId = bbIdx;

		bindings[bbIdx]->surface.nodes.resize(graphNodesCounter[bbIdx]);

		//for(int i = 0; i< bindings[bbIdx]->surface.nodes.size(); i++)
		//	bindings[bbIdx]->surface.nodes[i] = new GraphNode(i);

		//vector<GraphNode*> subGraph;
		//subGraph.resize(nodes.size(), NULL);
		int count = 0;
		for(int i = 0; i< nodes.size(); i++)
		{
			if(connIds[i] == bbIdx)
			{
				bindings[bbIdx]->surface.nodes[count] = nodes[i];
				bindings[bbIdx]->pointData[count].node = nodes[i];

				m.modelVertexBind[nodes[i]->id] = bbIdx;
				m.modelVertexDataPoint[nodes[i]->id] = count;
				count++;
			}

		}

		assert(count == graphNodesCounter[bbIdx]);

		bindings[bbIdx]->surface.triangles.resize(triangles.size());
		count = 0;
		for(int i = 0; i< triangles.size(); i++)
		{
			bool found = true;
			for(int trTemp = 0; trTemp < triangles[i]->verts.size(); trTemp++)
			{
				int vertId = triangles[i]->verts[trTemp]->id;
				found &= (m.modelVertexBind[vertId] == bbIdx);
			}

			if(found)
			{
				bindings[bbIdx]->surface.triangles[count] = triangles[i];
				count++;
			}
		}

		bindings[bbIdx]->surface.triangles.resize(count);
		
	}

	/*
	// Construimos estos dos vectores de referencias para ir más rápido en otras
	// computaciones.
	vector<int>& modelVertexDataPoint = m.modelVertexDataPoint;
	vector<int>& modelVertexBind = m.modelVertexBind;

	modelVertexDataPoint.resize(nodes.size());
	modelVertexBind.resize(nodes.size());

	int count = 0; 
	for(int i = 0; i< bindings.size(); i++)
	{
		for(int j = 0; j< bindings[i]->pointData.size(); j++)
		{
			modelVertexBind[bindings[i]->pointData[j].modelVert] = i;
			modelVertexDataPoint[bindings[i]->pointData[j].modelVert] = j;
			count++;
		}
	}

	assert(count == nodes.size());
	*/

	// Recorremos los vertices y acumulamos el area de sus triangulos ponderado por 1/3 que le corresponde.
    //MyMesh::VertexIterator vi;  idx = 0;
    //for(vi = m.vert.begin(); vi!=m.vert.end(); ++vi ) 
	/*for(int vi = 0; vi < m.nodes.size(); vi++)
	{
		int idBind = m.modelVertexBind[m.nodes[vi]->id];
		int idVertexInBind = m.modelVertexDataPoint[m.nodes[vi]->id];
		m.bindings[idBind]->pointData[idVertexInBind].position = m.nodes[vi]->position;
    }
	*/

	// Guardamos una indirección para tener ordenados los pesos... esto podría variar
	// para optimizar los cálculos.
	int counter = 0;
	m.globalIndirection.resize(m.vn());
	for(int i = 0; i< bindings.size(); i++)
	{
		for(int j = 0; j< bindings[i]->pointData.size(); j++)
		{
			bindings[i]->globalIndirection[j] = counter;
			m.globalIndirection[bindings[i]->pointData[j].node->id] = counter;
			counter++;
		}
	}

	// Construimos una matriz de adyacencia que tambien
	// recoge si una arista es borde(1) o no (2)
	vector< vector<short> > edges; 
	edges.resize(m.vn());
	for(int i = 0; i< edges.size(); i++)
		edges[i].resize(m.vn(), 0);
	
	//MyMesh::FaceIterator fj;
    //for(fj = m.face.begin(); fj!=m.face.end(); ++fj )
	for(int fj = 0; fj < m.triangles.size(); fj++ )
	{
        Eigen::Vector3d O, c, s;
        Eigen::Vector3i idVerts;
        for(int i = 0; i<3; i++) // Obtenemos los indices de los vertices de t
			idVerts[i] = m.triangles[fj]->verts[i]->id;

		for(int i = 0; i<3; i++)
			edges[idVerts[i]][idVerts[(i+1)%3]]++;
	}

	for(int i = 0; i< bindings.size(); i++)
	{
		for(int pt = 0; pt < bindings[i]->pointData.size(); pt++)
		{
			int vert = bindings[i]->pointData[pt].node->id;
			for(int con = 0; con< bindings[i]->surface.nodes[pt]->connections.size(); con++)
			{
				int conected = bindings[i]->surface.nodes[pt]->connections[con]->id;
				int modelVertConected = bindings[i]->pointData[conected].node->id;
				int count = edges[vert][modelVertConected]+edges[modelVertConected][vert];

				// Comprobamos si es un borde -> si hay conexion deberia tener valor de mas de uno.
				bindings[i]->pointData[pt].isBorder |= count < 2;
				bindings[i]->pointData[conected].isBorder |= count < 2;
			}
		}
	}
}

void propagateIdFromNode(int id, vector<int>& frontIds,vector<int>& harvestIds,vector<bool>& visIds,vector<int>& connIds,vector<GraphNode*> nodes)
{
	while(frontIds.size() > 0)
	{
		int newId = frontIds.back();
		frontIds.pop_back();

		// Si ya ha sido visitado en este pase seguimos.
		if(visIds[newId]) continue;

		int connId = connIds[newId];

		if(connId < 0)
		{
			connIds[newId] = id;
			visIds[newId] = true;
			for(unsigned int neigh = 0; neigh < nodes[newId]->connections.size(); neigh++)
					frontIds.push_back(nodes[newId]->connections[neigh]->id);
		}
		else if(connId != id)
		{
			for(unsigned int i = 0; i< connIds.size(); i++)
				if(connIds[i] == connId) 
				{
					connIds[i] = id;
					visIds[newId] = true;
				}

			harvestIds.push_back(newId);
		}
	}
}

/*
bool Modelo::processGreenCoordinates()
{

    newModeloGC.Clear();
    vcg::tri::Append<MyMesh, MyMesh>::Mesh(newModeloGC,modeloOriginal, false);

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
    vcg::tri::Append<MyMesh, MyMesh>::Mesh(newModeloHC,modeloOriginal, false);

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
    vcg::tri::Append<MyMesh, MyMesh>::Mesh(newModeloMVC,modeloOriginal, false);

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
    // Ojo!-> si no se han computado las coordenadas antes -> ÁÁERROR!!

    // Primero escogemos la caja con la que queremos hacer la deformaci—n
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
