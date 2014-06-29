#ifndef MODEL_H
#define MODEL_H

#include "Geometry.h" // Data Structures based un vcg library
#include "Cage.h"
#include "Deformer.h"
#include "grid3D.h"
#include "SurfaceData.h"

#include <DataStructures\AirVoxelization.h>

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

	// Bind data
	binding* bind;
	bool computedBindings;
	bool rigBinded;

	// Grid for analize volume
	voxGrid3d* grid;

    string sModelPath; // Ruta donde est‡ el modelo
    string sModelPrefix; // Prefijo de los ficheros del modelo

    virtual void drawFunc();

	// vertex and triangle counters
	int vn() {return (int)nodes.size();}
	int fn() {return (int)triangles.size();}

	// For visualization and analisis purposes
	void setSpotVertex(int i); 
	void setSpotVertexes(vector<int>& indices);
	void addSpotVertex(int i);
	void cleanSpotVertexes();
	virtual bool select(bool bToogle, unsigned int id);

};

// Builds a surface from the model loaded
// The mesh must be non-manifold.
// Detects borders, holes, and degenerated meshes.
void BuildSurfaceGraphs(Modelo* m);

// Aux function to build conected meshes in the model.
void propagateIdFromNode(int id, vector<int>& frontIds,
								 vector<bool>& visIds,
								 vector<int>& connIds,
								 vector<GraphNode*> nodes);

#endif // MODEL_H
