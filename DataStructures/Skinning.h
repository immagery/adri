#pragma once

#include "SurfaceData.h"
#include "Modelo.h"
#include <vector>
#include <map>

using namespace Eigen;

// Skinning manages all the skeleton bindings in the scene. It contains an array of bindings and deals with all computations
// relative to vertex deformation

class Rig;
class Skinning
{
public:
	Skinning();
	~Skinning();


    virtual void loadBindingForModel (Modelo *m, const vector< skeleton* >& skeletons);

    virtual void computeRestPositions(const vector< skeleton* >& skeletons);
	virtual void computeDeformations(const vector< skeleton* >& skeletons);
	virtual void computeDeformations2(skeleton* s);
	virtual void computeDeformationsWithSW(const vector< skeleton* >& skeletons);
    
	Vector3d deformVertex ();

	// Pointer for model deformations
	Geometry* originalModel;
	Geometry* deformedModel;

	// compact skinning data structures

	// A. Primary
	vector< vector<weight> > weights;

	// B. Secondary
	bool useSecondaryWeights;
	vector< vector< vector<weight> > >secondaryWeights;

	// All the data for Air computations
	binding* bind;
};

void initModelForDeformation(Modelo* m);
void resetDeformations(Modelo* m);
void copyModel2Model(Geometry * modelCopy, Geometry *modelReference);
