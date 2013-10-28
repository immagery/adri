#pragma once

#include "SurfaceData.h"
#include "Modelo.h"
#include <vector>
#include <map>

// Skinning manages all the skeleton bindings in the scene. It contains an array of bindings and deals with all computations
// relative to vertex deformation

class Skinning
{
public:
	Skinning();
	~Skinning();
    void loadBindingForModel (Modelo *m, string path, const vector< skeleton* >& skeletons);
    void saveBindingToFile (string path);
    void computeRestPositions(const vector< skeleton* >& skeletons);
	void computeDeformations(const vector< skeleton* >& skeletons);
	void computeDeformationsWithSW(const vector< skeleton* >& skeletons);
    Point3d deformVertex ();

	vector<Geometry*> originalModels;
	vector<Geometry*> deformedModels;

	map<int, vector<binding*> > bindings;		// index of model -> vector of bindings for that model

	int quaternionDef;
};

