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

    virtual void Clear();

    Cage* currentCage;

	// No se que es, pero debe ser para visulizar deformaciones
    //Geometry* currentRender;

	Geometry* originalModel;

    // Cajas para hacer la deformacion
    Cage* modelCage; // Caja envolvente original
    Cage* dynCage; // Caja que puede variar interactivamente
    vector< Cage* > stillCages; // Cajas est‡ticas de posiciones concretas (tests)

    /// Biharmonic Distances matrix (nxn)
    vector< vector< double> > embedding;

	// Bind
	binding* bind;

	// OldBindings
    //vector<binding*> bindings;

	vector<TriangleData> virtualTriangles;

	//Harmonic coords grid
	grid3d* HCgrid;

	bool computedBindings;

	//vector<int> modelVertexDataPoint;
	//vector<int> modelVertexBind;
	//vector<int> globalIndirection;

    //grid3d* grid;

    string sModelPath; // Ruta donde est‡ el modelo
    string sModelPrefix; // Prefijo de los ficheros del modelo

    // Plugs de salida
    vector< deformer* > deformers; // Applied deformers

    virtual void drawFunc();

    virtual bool select(bool bToogle, unsigned int id);

	void initGrid();

	void setSpotVertex(int i); 
	void setSpotVertexes(vector<int>& indices);
	void addSpotVertex(int i);
	void cleanSpotVertexes();

	int vn() {return (int)nodes.size();}
	int fn() {return (int)triangles.size();}
};

void BuildSurfaceGraphs(Modelo* m);
void propagateIdFromNode(int id, vector<int>& frontIds,vector<int>& harvestIds,vector<bool>& visIds,vector<int>& connIds,vector<GraphNode*> nodes);

#endif // MODEL_H
