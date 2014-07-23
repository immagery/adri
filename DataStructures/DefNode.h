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

// This class represents an influence
class weight
{
public:

	weight()
	{
		label = -1;
		relativeLabel = -1;
		weightValue = 0;
	}

	weight(const weight& temp)
	{
		label = temp.label;
		relativeLabel = temp.relativeLabel;
		weightValue = temp.weightValue;
	}

	weight(int label_in, float weight_in)
	{
		label = label_in;
		relativeLabel = -1;
		weightValue = weight_in;
	}

	weight(int label_in, int relative_label_in, float weight_in)
	{
		label = label_in;
		relativeLabel = relative_label_in;
		weightValue = weight_in;
	}

	int label;
	int relativeLabel; // For secondaryWeights
	float weightValue;
};

// This class represents an influence
class secWeight
{
public:

	secWeight()
	{
		wideBone = 1;
		alongBone = 0;
	}

	secWeight(const secWeight& temp)
	{
		wideBone = temp.wideBone;
		alongBone = temp.alongBone;
	}

	secWeight(float wideBone_in, int alongBone_in)
	{
		wideBone = wideBone_in;
		alongBone = alongBone_in;
	}

	float wideBone; // Weight for fight between childs
	float alongBone; // Weight over a bone
};

int findWeight(vector<weight>& weights, int label);

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
		relPos = Eigen::Vector3d(0,0,0);

		precomputedDistances = 0;

		// This flag enables weight computation.
		enableWeightsComputation = true;

		// This structures takes the weights and a sort for better computation
		MVCWeights.clear();
		weightsSort.clear();

		// This value is used to cut the weights and improve the computations.
		cuttingThreshold = ADAPTATIVE_CUTTING_VALUE;

		dirtyFlag = true;
		segmentationDirtyFlag = true;
		expansionDirtyFlag = true;
		smoothDirtyFlag = true;

		addedToComputations = false;

		freeNode = true;

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
		relPos = def_orig.relPos;
		precomputedDistances = def_orig.precomputedDistances;

		segmentationDirtyFlag = def_orig.segmentationDirtyFlag;
		freeNode = def_orig.freeNode;
		addedToComputations = def_orig.addedToComputations;


	}

	void copyKeyInfoFrom(const DefNode& def_orig)
	{
		boneId = def_orig.boneId;
		rootBoneId = def_orig.rootBoneId;
		childBoneId = def_orig.childBoneId;

		ratio = def_orig.ratio;
		expansion = def_orig.expansion;

		pos = def_orig.pos;
		relPos = def_orig.relPos;
		precomputedDistances = def_orig.precomputedDistances;		
	}

	virtual bool propagateDirtyness()
	{
		node::propagateDirtyness();
		return true;
	}

	// TODEBUG maybe there is redundant data.
	int rootBoneId;

	// For faster computation
	Eigen::Vector3d pos;

	// For drawing
	Eigen::Vector3d relPos;

	int boneId; // Who is my parent?
	int childBoneId;

	float ratio; // position over the bone (for blending through the bone)
	float expansion; // influence expansion (for segmentation)

	// Aux variable that encapsulates precomputed part af distance calculus.
	double precomputedDistances;

	// This flag enables weight computation.
	bool enableWeightsComputation;

	// This structures takes the weights and a sort for better computation
	vector<double> MVCWeights;
	vector<int> weightsSort;

	// This value is used to cut the weights and improve the computations.
	double cuttingThreshold;

	bool segmentationDirtyFlag;
	bool addedToComputations;

	bool creationDirtyFlag;

	bool expansionDirtyFlag;
	bool smoothDirtyFlag;

	bool freeNode;

	// Serialization
	bool saveToFile(FILE* fout)
	{
		if(!fout)
		{
			printf("There is no file to print!\n [DEF_NODE_SAVE]\n");
			return false;
		}
		
		node::saveToFile(fout);

		fprintf(fout, "%d %d %d ", boneId, rootBoneId, childBoneId); fflush(fout);
		fprintf(fout, "%f %f ", ratio, expansion); fflush(fout);
		fprintf(fout, "%f %f ", precomputedDistances, cuttingThreshold); fflush(fout);

		// TOFIX: Could be computed later, but for been faster I save it
		fprintf(fout, "%f %f %f ", pos.x(), pos.y(), pos.z()); fflush(fout);
		fprintf(fout, "%f %f %f ", relPos.x(), relPos.y(), relPos.z()); fflush(fout);

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
	bool loadFromFile(ifstream& in)
	{
		node::loadFromFile(in);

		string line;
		vector<string> elems;

		getline (in , line);
		split(line, ' ', elems);

		boneId = atoi(elems[0].c_str());
		rootBoneId = atoi(elems[1].c_str());
		childBoneId = atoi(elems[2].c_str());
		ratio = atof(elems[3].c_str());
		expansion = atof(elems[4].c_str());
		precomputedDistances = atof(elems[5].c_str());
		cuttingThreshold = atof(elems[6].c_str());
		pos.x() = atof(elems[7].c_str());
		pos.y() = atof(elems[8].c_str());
		pos.z() = atof(elems[9].c_str());
		relPos.x() = atof(elems[10].c_str());
		relPos.y() = atof(elems[11].c_str());
		relPos.z() = atof(elems[12].c_str());
		enableWeightsComputation = atoi(elems[13].c_str());

		int MVCsize = 0, weightSize =0;
		getline (in , line);
		split(line, ' ', elems);
		MVCsize = atoi(elems[0].c_str());
		weightSize = atoi(elems[1].c_str());

		getline (in , line);
		split(line, ' ', elems);

		MVCWeights.resize(MVCsize);
		for(int i = 0; i< MVCWeights.size(); i++)
		{
			MVCWeights[i] = atof(elems[i].c_str());
		} 
		
		getline (in , line);
		split(line, ' ', elems);

		weightsSort.resize(weightSize);
		for(int i = 0; i< weightsSort.size(); i++)
		{
			weightsSort[i] = atoi(elems[i].c_str());
		}

		return true;
	}

};

#endif // DEFNODE_H
