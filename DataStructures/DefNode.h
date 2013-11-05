#ifndef DEFNODE_H
#define DEFNODE_H

#include "DataStructures.h"
#include "node.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#define NO_CUTTING_VALUE 10
#define ADAPTATIVE_CUTTING_VALUE 1

using namespace std;
using namespace vcg;

/*
	-	DEFNODE -
	Definition: This class is constructed for compute deformations.
It is linked to a bone or some parental structure that defines a behavior

*/
class DefNode : public node
{
public:
	// This functions is just for compress code.
	void LoadDefaultValues()
	{
		nodeId = -1;

		boneId = -1;
		rootBoneId = -1;
		childBoneId = -1;

		ratio = -1;
		expansion = 1;

		pos = Eigen::Vector3d(0,0,0);
		precomputedDistances = 0;

		// This flag enables weight computation.
		enableWeightsComputation = true;

		// This structures takes the weights and a sort for better computation
		MVCWeights.clear();
		weightsSort.clear();

		// This value is used to cut the weights and improve the computations.
		cuttingThreshold = ADAPTATIVE_CUTTING_VALUE;

		dirtyFlag = true;

	}

	DefNode() : node() 
	{
		LoadDefaultValues();
	}

	DefNode(int id) : node() 
	{
		LoadDefaultValues();
		boneId = id;
	}

	DefNode(Eigen::Vector3d newPos, int id) : node() 
	{
		LoadDefaultValues();

		boneId = id;
		pos = newPos;
	}

	DefNode(const DefNode& def_orig)
	{
		LoadDefaultValues();

		boneId = def_orig.boneId;
		nodeId = def_orig.nodeId;
		rootBoneId = def_orig.rootBoneId;
		childBoneId = def_orig.childBoneId;

		ratio = def_orig.ratio;
		expansion = def_orig.expansion;

		pos = def_orig.pos;
		precomputedDistances = def_orig.precomputedDistances;
	}

	virtual bool propagateDirtyness()
	{
		node::propagateDirtyness();
		return true;
	}

	// TODEBUG maybe there is redundant data.
	int boneId; // Who is my parent?
	int rootBoneId;
	int childBoneId;

	float ratio; // position over the bone (for blending through the bone)
	float expansion; // influence expansion (for segmentation)

	// Aux variable that encapsulates precomputed part af distance calculus.
	double precomputedDistances;

	// For faster computation
	Eigen::Vector3d pos;

	// This flag enables weight computation.
	bool enableWeightsComputation;

	// This structures takes the weights and a sort for better computation
	vector<double> MVCWeights;
	vector<int> weightsSort;

	// This value is used to cut the weights and improve the computations.
	double cuttingThreshold;

};

#endif // DEFNODE_H
