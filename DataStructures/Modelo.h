#ifndef MODEL_H
#define MODEL_H

#include "Geometry.h" // Data Structures based un vcg library
#include "Cage.h"
#include "Deformer.h"
#include "grid3D.h"
#include "SurfaceData.h"

class Modelo : public Geometry
{
public:

    Modelo();
    Modelo(unsigned int id);
    ~Modelo();

	void copyFrom(Modelo& orig_model);

    virtual void Clear();

	// Original geometry for compute deformations
	Geometry* originalModel;
	bool originalModelLoaded;

	// Bind
	binding* bind;
	bool computedBindings;

	bool rigBinded;

	// Temp triangles
	vector<TriangleData> virtualTriangles;

    string sModelPath; // Ruta donde est‡ el modelo
    string sModelPrefix; // Prefijo de los ficheros del modelo

    vector< deformer* > deformers; // Applied deformers

    //Cage* currentCage;

	// No se que es, pero debe ser para visulizar deformaciones
    //Geometry* currentRender;

	//TODELETE
    // Cajas para hacer la deformacion
    Cage* modelCage; // Caja envolvente original
    Cage* dynCage; // Caja que puede variar interactivamente
    vector< Cage* > stillCages; // Cajas est‡ticas de posiciones concretas (tests)
	Cage* currentCage;

    /// Biharmonic Distances matrix (nxn)
    //vector< vector< double> > embedding;

	// OldBindings
    //vector<binding*> bindings;

	//Harmonic coords grid
	//grid3d* HCgrid;

	//vector<int> modelVertexDataPoint;
	//vector<int> modelVertexBind;
	//vector<int> globalIndirection;

    //grid3d* grid;

    virtual void drawFunc();

	// vertex and triangle counters
	int vn() {return (int)nodes.size();}
	int fn() {return (int)triangles.size();}

	//void initGrid();

	// For visualization and analisis purposes
	void setSpotVertex(int i); 
	void setSpotVertexes(vector<int>& indices);
	void addSpotVertex(int i);
	void cleanSpotVertexes();
	virtual bool select(bool bToogle, unsigned int id);

};

void BuildSurfaceGraphs(Modelo* m);
void propagateIdFromNode(int id, vector<int>& frontIds,vector<int>& harvestIds,vector<bool>& visIds,vector<int>& connIds,vector<GraphNode*> nodes);

#endif // MODEL_H
