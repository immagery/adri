#pragma once

#include "SurfaceData.h"
#include "Modelo.h"
#include <vector>
#include <map>

using namespace Eigen;

// Skinning manages all the skeleton bindings in the scene. It contains an array of bindings and deals with all computations
// relative to vertex deformation

class Skinning
{
public:
	Skinning();
	~Skinning();
    virtual void loadBindingForModel (Modelo *m, string path, const vector< skeleton* >& skeletons);
 //   virtual void saveBindingToFile (string path);
    virtual void computeRestPositions(const vector< skeleton* >& skeletons);
	virtual void computeDeformations(const vector< skeleton* >& skeletons);
	virtual void computeDeformationsWithSW(const vector< skeleton* >& skeletons);
    
	Vector3d deformVertex ();

	vector<Geometry*> originalModels;
	vector<Geometry*> deformedModels;

	vector< vector<binding*> > bindings;		// index of model -> vector of bindings for that model

	// compact skinning data structures
	// A. Primary
	vector< vector<weight> > weights;
	// B. Secondary
	bool useSecondaryWeights;
	vector< vector< vector<weight> > >secondaryWeights;

	// All the data for Air computations
	binding* bind;

	int quaternionDef;
};

