#ifndef DEFNODE_H
#define DEFNODE_H

#include "DataStructures.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "node.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#define NO_CUTTING_VALUE 10
#define ADAPTATIVE_CUTTING_VALUE 1

using namespace std;

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

	// Serialization
	bool saveToFile(FILE* fout)
	{
		if(!fout)
		{
			printf("There is no file to print!\n [AIR_RIG]");
			return false;
		}
		
		fprintf(fout, "%d %d %d\n", boneId, rootBoneId, childBoneId); fflush(fout);
		fprintf(fout, "%f %f\n", ratio, expansion); fflush(fout);
		fprintf(fout, "%f %f\n", precomputedDistances, cuttingThreshold); fflush(fout);

		// TOFIX: Could be computed later, but for been faster I save it
		fprintf(fout, "%f %f %f\n", pos.x(), pos.y(), pos.z()); fflush(fout);
		fprintf(fout, "%d\n", enableWeightsComputation); fflush(fout);

		fprintf(fout, "%d %d\n", MVCWeights.size(), weightsSort.size()); fflush(fout);
		
		for(int i = 0; i< MVCWeights.size(); i++)
		{
			fprintf(fout, "%f ", MVCWeights[i]);
		}
		fprintf(fout, "\n");

		for(int i = 0; i< weightsSort.size(); i++)
		{
			fprintf(fout, "%d ", weightsSort[i]);
		}
		fprintf(fout, "\n");

		return true;
	}

	// Loads data from this file, it is important to bind
	// this data with the model and skeleton after this function.
	bool loadFromFile(FILE* fout)
	{
		fout = fout; // for delete warning
		return false;
	}

};

#endif // DEFNODE_H
