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
    void loadBindingForModel (Modelo * m, Modelo* m2, string path, const vector< skeleton* >& skeletons);
    void saveBindingToFile (string path);
    void computeRestPositions(const vector< skeleton* >& skeletons);
	void computeDeformations(const vector< skeleton* >& skeletons);
    Point3d deformVertex ();

	vector<Modelo*> originalModels;
	vector<Modelo*> deformedModels;

	vector<binding*> bindings;
    map<pair<int, int>, Point3d> restPositions;
    // some way to store triplets vertex - bone - position
};

