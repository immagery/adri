#ifndef SURFACEDATA_H
#define SURFACEDATA_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "grid3D.h"
#include "skeleton.h"

using namespace std;

class symMatrix
{
public:
	symMatrix()
	{
		size = 0;
		withDiagonal = true;
		elements.clear();
	}

	~symMatrix()
	{
		size = 0;
		withDiagonal = true;
		for(unsigned int i = 0; i< elements.size(); i++)
			elements[i].clear();
		elements.clear();
	}

	void resize(int _size, bool _withDiagonal = true)
	{
		size = _size;
		withDiagonal = _withDiagonal;
		elements.resize(size);

		for(int i = 0; i< size; i++)
		{
			if(withDiagonal)
				elements[i].resize(size-i, 0);
			else
				elements[i].resize(size-i-1, 0);
		}
	}

	double getSafe(int i, int j)
	{
		if(i < size && i > 0 && j < size && j > 0)
		{
			if(j <= i)
				return elements[j][i-j];
			else /*if(j > i)*/
				return elements[i][j-i];
		}

		return 0;
	}

	double get(int i, int j)
	{
		if(j <= i)	
			return elements[j][i-j];
		else /*if(j > i)*/
				return elements[i][j-i];
	}
	
	void setSafe(int i, int j, double value)
	{
		if(i < size && i > 0 && j < size && j > 0)
		{
			if(j <= i)	
				elements[j][i-j] = value;
			else if(j > i)
				elements[i][j-i] = value;
		}
	}

	void set(int i, int j, double value)
	{
		if(j <= i)	
			elements[j][i-j] = value;
		else if(j > i)
			elements[i][j-i] = value;
	}

	vector< vector<double> > elements;
	int size;
	bool withDiagonal;
};

class GraphNode
{
public:
	GraphNode(int _id)
	{
		connections.clear();
		id = _id;
		visited = false;
	}

	GraphNode()
	{
		connections.clear();
		visited = false;
	}

	~GraphNode()
	{
		connections.clear();
	}

	unsigned int id;
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

	vector<GraphNode*> nodes;
	vector<GraphNodePolygon* > triangles;
};

// Builds the Sruface Graph form an OFF file
// The file needs to be in a correct OFF format.
bool BuildSurfaceFromOFFFile(SurfaceGraph& graph, string& sFileName);

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

    // The cell contains a vertex or not
    bool vertexContainer;

    // Color para representar varias cosas.
    Eigen::Vector3f color;

	GraphNode* node;

    // Segmentation labels
    vector<double> embedding;

    // Global owner node of the cell.
    int segmentId;

	// Distance to the owner cell
	float segmentDistance;

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

	// Influences assigned to this cell
    //vector< vector<float> > secondInfluences;

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
	vector<symMatrix> BihDistances;
};


void normalizeWeightsByDomain(binding *bd);
void cleanZeroInfluences(binding* bd);

void loadBinding(binding* bd, string fileName, vector<skeleton*> skts);

#endif // SURFACEDATA_H
