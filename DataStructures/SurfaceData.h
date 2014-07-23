#ifndef SURFACEDATA_H
#define SURFACEDATA_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "grid3D.h"
#include "skeleton.h"

#include <DataStructures\symMatrix.h>

using namespace std;

class GraphNode
{
public:
	GraphNode(int _id)
	{
		connections.clear();
		id = _id;
		visited = false;
		pieceId = -1;
	}

	GraphNode()
	{
		connections.clear();
		visited = false;
		pieceId= -1;
	}

	void copyFrom(GraphNode* gr)
	{
		id = gr->id;
		visited = gr->visited;
		connections.clear();
		pieceId = gr->pieceId;
	}

	~GraphNode()
	{
		connections.clear();
	}

	unsigned int id;
	unsigned int pieceId;
	vector<GraphNode* > connections;

	Vector3d position;

	bool visited;
};

/////////////////////////////////////////
/// \brief The Polygon (open or not) class over the graph
/////////////////////////////////////////
class GraphNodePolygon
{

public:
	GraphNodePolygon(int _id)
	{
		verts.clear();
		id = _id;
		visited = false;
	}

	GraphNodePolygon()
	{
		verts.clear();
		visited = false;
	}

	void copyFrom(GraphNodePolygon* gnp)
	{
		visited = gnp->visited;
		id = gnp->id;
		isClosed = gnp->isClosed;
	}

	bool visited;
	bool isClosed;
	int id;

	vector<GraphNode* > verts;
};

class SurfaceGraph
{
public:
	SurfaceGraph()
	{
		nodes.clear();
		triangles.clear();
	}

	~SurfaceGraph()
	{
		// De-allocate model memory
		for(int i = 0; i< nodes.size(); i++)
			delete nodes[i];

		for(int i = 0; i< triangles.size(); i++)
			delete triangles[i];

		nodes.clear();
		triangles.clear();
	}

	void copyFrom(SurfaceGraph* sg)
	{
		// Allocate model memory
		nodes.resize(sg->nodes.size());
		triangles.resize(sg->triangles.size());

		for(int i = 0; i< nodes.size(); i++)
		{
			nodes[i] = new GraphNode();
			nodes[i]->copyFrom(sg->nodes[i]);

			for(int ndIdx = 0; ndIdx < sg->nodes[i]->connections.size(); ndIdx++)
			{
				int idxNeigbour = sg->nodes[i]->connections[ndIdx]->id;
				nodes[i]->connections.push_back(nodes[idxNeigbour]);
			}
		}

		for(int i = 0; i< triangles.size(); i++)
		{
			triangles[i] = new GraphNodePolygon();
			triangles[i]->copyFrom(sg->triangles[i]);

			for(int ndIdx = 0; ndIdx < sg->triangles[i]->verts.size(); ndIdx++)
			{
				int idxVert = sg->triangles[i]->verts[ndIdx]->id;
				triangles[i]->verts.push_back(nodes[idxVert]);
			}
		}
	}

	vector<GraphNode*> nodes;
	vector<GraphNodePolygon* > triangles;
};

// Builds the Sruface Graph form an OFF file
// The file needs to be in a correct OFF format.
bool BuildSurfaceFromOFFFile(SurfaceGraph& graph, string& sFileName);

bool SaveOFFFromSurface(SurfaceGraph& graph, string& sFileName);

bool MergeMeshes(vector<string> sFileList, string result);

/////////////////////////////////////////
/// \brief The PointData class
/////////////////////////////////////////
class PointData
{
    public:
    PointData();
    PointData(int weightsSize);
	PointData(const PointData& pd);

    //void SaveToFile(ofstream& myfile);
    //void LoadFromFile(ifstream& myfile);

    void clear();
    void clearAll();

	float getDomain(int fatherId);

	void saveToFile(FILE* fout);
	void loadFromFile(ifstream& myfile);

    // Color para representar varias cosas.
    Eigen::Vector3f color;

	GraphNode* node;

	// Global owner node of the cell.
	int segmentId;

	// Distance to the owner cell
	float segmentDistance;

    // Segmentation labels
    vector<double> embedding;
    
    // Intermediate owner of the cell, and its weight
    int ownerLabel;
    float ownerWeight;
    float tempOwnerWeight;
    float confidenceLevel;

    // Influences temporary assigned to childs.
    vector<weight> auxInfluences;

    // Domain of influcence
    float domain;

	int domainId;

    // Iteration pass
    int itPass;

    // ConnexComponents
    bool validated;
    int component;
    bool assigned;

	bool isBorder;

    // Influences assigned to this cell
    vector<weight> influences;

	// Influences temporary assigned to childs.
    vector< vector<secWeight> >secondInfluences;
};

class EdgeData
{
	public:
    EdgeData()
    {
	}

	PointData* pt1;
	PointData* pt2;
};

class TriangleData
{
	public:
    TriangleData()
    {
		Idxs[0] = -1;
		Idxs[1] = -1;
		Idxs[2] = -1;

		pts[0] = NULL;
		pts[1] = NULL;
		pts[2] = NULL;
	}

	TriangleData(const TriangleData& tr)
    {
		Idxs[0] = tr.Idxs[0];
		Idxs[1] = tr.Idxs[1];
		Idxs[2] = tr.Idxs[2];

		// Ojo!-> estos punteros podrian fallar
		pts[0] = tr.pts[0];
		pts[1] = tr.pts[1];
		pts[2] = tr.pts[2];
	}

	int Idxs[3]; 
	PointData* pts[3];
};

////
/// \brief The binding class
///
class binding
{
public:

	void setDefaultValues()
	{
        // Parameters
        smoothPropagationRatio = 1;
        worldScale = 1;
        minConfidenceLevel = -1;
        minTriangleArea = -1;
		ntriangles = 0;
		smoothingPasses = 3;

		bindId = -1;

		weightsCutThreshold = -10;
	}

    binding()
    {
        // binded skeletons
        bindedSkeletons.clear();

        // Node definition data
        intPoints.clear();
        nodeIds.clear();
        traductionTable.clear();
        embeddedPoints.clear();
		pointData.clear();
		globalIndirection.clear();
 
		weightsSort.clear();
		weightsRepresentative.clear();

		cutThreshold.clear();
		weightsFiltered.clear();
    }

	void copyFrom(binding* bd)
	{
	    // Parameters
		smoothPropagationRatio = bd->smoothPropagationRatio;
		worldScale = bd->worldScale;
		minConfidenceLevel = bd->minConfidenceLevel;
		smoothingPasses = bd->smoothingPasses;
		minTriangleArea = bd->minTriangleArea;
		bindId = bd->bindId;
		weightsCutThreshold = bd->weightsCutThreshold;

		// TODO
		assert(false);

		// Hay que definir bien lo que se usa y no se usa de un binding

		/*
		// BINDED SKELETONS... get references

		// binded skeletons
		vector<skeleton*> bindedSkeletons;

		// Node definition data
		vector< DefNode > intPoints;
		map< int, DefNode*> nodeIds;
		vector< int > traductionTable;
		vector<vector<double> > embeddedPoints;
		vector<int> globalIndirection;

		vector< vector<weight> > weightsFiltered;

		// Weights sorted to be used as a calculus stoper.
		vector<vector<int> > weightsSort;
		vector<vector<bool> >weightsRepresentative;

		vector<double> cutThreshold;

		/// Info for each point of the geometry
		vector<PointData> pointData;
		vector<TriangleData> virtualTriangles;

		/// Data structure for process faster the geometry
		SurfaceGraph* mainSurface;
		vector<SurfaceGraph> surfaces;
		vector<bool> enabledSurfaces;

		//SurfaceGraph surface;
		unsigned int ntriangles;

		//Biharmonic distances
		//TODO: remove this structure for A 
		vector<symMatrixLight> BihDistances;

		// Biharmonic distances for computation
		vector<MatrixXf> A;		

		*/

	}

	~binding()
    {
		for(unsigned int i = 0; i< bindedSkeletons.size(); i++)
			delete bindedSkeletons[i];

        // Node definition data
        intPoints.clear();
        nodeIds.clear();
        traductionTable.clear();
        embeddedPoints.clear();
		pointData.clear();
		globalIndirection.clear();
 
		weightsSort.clear();
		weightsRepresentative.clear();

		cutThreshold.clear();
		weightsFiltered.clear();
	}

	binding(int points)
	{
        // Init default values
        setDefaultValues();

        // binded skeletons
        bindedSkeletons.clear();

        // Node definition data
        intPoints.clear();
        nodeIds.clear();
        traductionTable.clear();
        embeddedPoints.clear();

		pointData.resize(points);
		globalIndirection.resize(points);

		weightsSort.clear();
		weightsRepresentative.clear();

		cutThreshold.clear();
		weightsFiltered.clear();
	}

	void resize(int points)
	{
        // Init default values
        setDefaultValues();

        // binded skeletons
        bindedSkeletons.clear();

        // Node definition data
        intPoints.clear();
        nodeIds.clear();
        traductionTable.clear();
        embeddedPoints.clear();

		pointData.resize(points);
		globalIndirection.resize(points);

		weightsSort.clear();
		weightsRepresentative.clear();

		cutThreshold.clear();
		weightsFiltered.clear();
	}

	void saveCompactBinding(string fileName, map<int, string>& deformersRelation);

    // Parameters
    float smoothPropagationRatio;
    float worldScale;
    float minConfidenceLevel;

	int smoothingPasses;

    float minTriangleArea;

	int bindId;

	double weightsCutThreshold;

    // binded skeletons
    vector<skeleton*> bindedSkeletons;

    // Node definition data
    vector< DefNode > intPoints;
    map< int, DefNode*> nodeIds;
    vector< int > traductionTable;
    vector<vector<double> > embeddedPoints;
	vector<int> globalIndirection;

	vector< vector<weight> > weightsFiltered;

	// Weights sorted to be used as a calculus stoper.
	vector<vector<int> > weightsSort;
	vector<vector<bool> >weightsRepresentative;

	vector<double> cutThreshold;

    /// Info for each point of the geometry
    vector<PointData> pointData;
	vector<TriangleData> virtualTriangles;

	/// Data structure for process faster the geometry
	SurfaceGraph* mainSurface;
	vector<SurfaceGraph> surfaces;
	vector<bool> enabledSurfaces;

	//SurfaceGraph surface;
	unsigned int ntriangles;

	//Biharmonic distances
	//TODO: remove this structure for A 
	vector<symMatrixLight> BihDistances;

	// Biharmonic distances for computation
	vector<MatrixXf> A;
};


void normalizeWeightsByDomain(binding *bd, int surfIdx);
void cleanZeroInfluences(binding* bd);

void loadBinding(binding* bd, string fileName, vector<skeleton*> skts);

#endif // SURFACEDATA_H
