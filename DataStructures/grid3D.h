#ifndef GRID3D_H
#define GRID3D_H

#include "DataStructures.h"
#include "DefNode.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "Box3.h"

using namespace std;
//using namespace vcg;

class skeleton;

enum T_cell { /*00*/UNTYPED = -1, /*01*/BOUNDARY = -2, /*10*/EXTERIOR = -3, /*11*/INTERIOR = -4 };

class voronoiGraphData
{
public:
    voronoiGraphData()
    {
        voronoiGraph.clear();
        intPoints.clear();
        traductionTable.clear();
    }

    voronoiGraphData(const voronoiGraphData& v)
    {
        intPoints.resize(v.intPoints.size());
        for(unsigned int i = 0; i< v.intPoints.size(); i++)
            intPoints[i] = v.intPoints[i];

        voronoiGraph.resize(v.voronoiGraph.size());
        for(unsigned int i = 0; i< v.voronoiGraph.size(); i++)
        {
            voronoiGraph[i].resize(v.voronoiGraph[i].size());
            for(unsigned int j = 0; j< v.voronoiGraph[i].size(); j++)
            {
                voronoiGraph[i][j] = v.voronoiGraph[i][j];
            }
        }
    }

    vector< vector<bool> > voronoiGraph;

    vector< DefNode > intPoints;
    map< int, DefNode*> nodeIds;

    vector< vector<double> > nodeDistances;
    vector< double > meanDistances;
    vector< vector<double> > embeddedPoints;

    vector< int > traductionTable;

};

class cellData
{
    public:
    cellData();
    cellData(int weightsSize);

    void SaveToFile(ofstream& myfile);
    void LoadFromFile(ifstream& myfile);

    void clear();
    void clearAll();

    // Flags for better computation
    Eigen::Vector3i pos;

    // Distance for optimized front propagation
    double distanceToPos;

    // The cell contains a vertex or not
    bool vertexContainer;

    // Weights corresponfin to labels
    //vector<float> weights;

    // Segmentation labels
    //vector<int> labels;

	// Color para representar varias cosas.
	Eigen::Vector3f color;

    // Segmentation labels
    vector<double> embedding;

	// Global owner node of the cell.
    int segmentId;

    // Intermediate owner of the cell, and its weight
    int ownerLabel;
    float ownerWeight;
	int tempOwnerLabel;
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

	// Influences assigned to this cell
	vector<weight> influences;

    // Aux Weights for optimize computation
    //vector<float> auxWeights;

	float getDomain(int fatherId)
	{
		for(unsigned int i = 0; i< influences.size(); i++)
		{
			if(influences[i].label == fatherId)
				return influences[i].weightValue;
		}

		return 0;
	}
};


class cell3d
{
public:

    //constructors
    cell3d();

    cell3d(bool noData);

    cell3d(int weightsSize);

    cell3d(T_cell tipo, int weightsSize);

    ~cell3d();

    // With these two functions the type is codified as a couple of booleans
    T_cell getType()
    {
        if(tipo[0])
        {
            if(tipo[1]) return INTERIOR;
            else        return EXTERIOR;
        }
        else
        {
            if(tipo[1])  return BOUNDARY;
            else         return UNTYPED;
        }
    }

    void setType(T_cell t)
    {
        if(t == INTERIOR)
        {
            tipo[0] = true; tipo[1] = true;
        }
        else if(t == EXTERIOR)
        {
            tipo[0] = true; tipo[1] = false;
        }
        else if(t == BOUNDARY)
        {
            tipo[0] = false; tipo[1] = true;
        }
        else if(t == UNTYPED)
        {
            tipo[0] = false; tipo[1] = false;
        }
    }


    ///  VARIABLES ///
    // tipo de la celda
    //T_cell tipo;
    bool tipo[2];

    // Has been changed -> inicialized to false (TO REMOVE)
    bool changed;

    bool frontier;

    cellData* data;

    void SaveToFile(ofstream& myfile);
    void LoadFromFile(ifstream& myfile);
};

class Modelo;


// Uniform grid
class grid3d
{
public:

    // constructor
    grid3d();

    grid3d(MyBox3 bounding, Vector3i divisions, int weightsSize);

    void initwithNoData(MyBox3 bounding_, Vector3i divisions);
    void init(MyBox3 bounding, Vector3i divisions, int weightsSize);

	void clearData();

	void getCoordsFromPoint(Vector3d& pt, vector<weight>& weights);
	void getCoordsFromPointSimple(Vector3d& pt, vector<weight>& weights);

	bool hasData(Vector3i& pt);
	bool isOut(Vector3i& pt);
	void copyValues(vector<weight>& weights, vector<weight>& weights_out);

	Vector3d getCenterOfCell(int i, int j, int k);

	void initBasicData();

    void clear();

    void updateStatistics();

    // Dimensiones del grid en los tres ejes (vcg)
    Vector3i dimensions;

    // Resolution
    int res;

    // Bounding box del grid (vcg)
    //Box3d bounding;
	MyBox3 bounding;

    // Numero de vertices representados en el grid
    int weightSize;

    // tamano del lado de la celda
    float cellSize;

    // Valores que pueden tomar los pesos del grid.
    float valueRange;

    float kValue;
    float alphaValue;
    bool metricUsed;

	// Vars to track
	float smoothPropagationRatio;
	float worldScale;
	float minConfidenceLevel;

	// binded skeletons
	vector<skeleton*> bindedSkeletons;

	// Node definition data
	voronoiGraphData v;

	// Grid processing temp data
	vector<int> tradIds;

    unsigned int totalCellsCounts;
    unsigned int borderCellsCounts;
    unsigned int inCellsCounts;
    unsigned int outCellsCounts;

    //voronoiGraphData voronoi;

    // Celdas del grid
    vector< vector< vector< cell3d* > > > cells;

    ////  FUNCTIONS ///

    // Marca las celdas con los diferentes tipos segun la geometria de entrada.
    //int typeCells(MyMesh& mesh);

	// Marca las celdas con los diferentes tipos segun la geometria de entrada.
	// Con la nueva estructura de grafo
	int typeCells(Modelo* mesh);
	int typeCells(Modelo* m, int i);

    // Extiende por interior los pesos asignados en las celdas de los v√©rtices.
    void expandWeights();

	// Extiende por interior los pesos asignados en las celdas de los vertices, 
	// optimizado para no morir en peso de memoria.
	void expandWeightsOptimized(Modelo* m);

    // Normaliza los valores para que estén entre 0 y 1, 
	// como todas las funciones son continuas, esta también debería serlo.
    void normalizeWeights();

	void normalizeWeightsOptimized();

	// Normaliza los valores en los vectores auxiliares limitados por el dominio.
	// Solo normaliza las celdas que tienen como propierarios sameLevelIds.
	void normalizeWeightsByDomain();

	// Elimina los pesos igual a cero
	void cleanZeroInfluences();

    // Marca las celdas con los diferentes tipos seg√∫n la geometr√≠a de entrada.
    //int typeCells(MyFace& face);

    // Devuelve la direccion de la celda a la que pertenece el punto.
    Vector3i cellId(Eigen::Vector3d pt);

	// Devuelve el centro de la celda en coordenadas de mundo.
    Vector3d cellCenter(int i,int j,int k);

    // Rellena las celdas que quedan dentro de los contornos.
    // Se llama cuando el contorno ya est√° inicializado.
    int fillFromCorners();

    // Rellena las celdas que quedan dentro de los contornos.
    // Se llama cuando el resto de celdas ya est√°n inicializadas.
    int fillInside();

    // Guardamos en binario el grid entero, para no tener que recalcular cada vez
    void SaveGridToFile(string sFileName);

    // Cargamos de disco el grid entero.
    void LoadGridFromFile(string sFileName);


	// Report functions
	void ( *fPrintText)(char*);
	void ( *fPrintProcessStatus)(int);

	bool loadedFunctions;

	void SafefPrintText(char* text)
	{
		if(fPrintText) fPrintText(text);
	}
	void SafefPrintProcessStatus(int value)
	{
		if(fPrintProcessStatus) fPrintProcessStatus(value);
	}

};

void interpolateLinear( vector<weight>& result, 
						vector<weight>& ptminWeights, 
						vector<weight>& ptmaxWeights, 
						float interpolationValue);

void interpolateBiLinear(vector<weight>& result, 
						 vector<weight>& pt01, 
						 vector<weight>& pt02,
						 vector<weight>& pt03, 
						 vector<weight>& pt04,
						 float interpolationValue1,
						 float interpolationValue2);

void interpolateTriLinear(vector<weight>& result,
						  vector<vector<weight>* >& pts,
						 float interpolationValue1,
						 float interpolationValue2,
						 float interpolationValue3);
#endif // GRID3D_H
