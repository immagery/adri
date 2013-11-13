#ifndef SERIALIZATION_H
#define SERIALIZATION_H

using namespace std;
#include <iostream>
#include <fstream>

class serialization
{
public:
	virtual bool saveToFile(FILE* fout){ fout = fout; true; };
	virtual bool loadFromFile(FILE* fout){ fout = fout; return true; };
};

/*
class airRigSerialization : nodeSerialization
{
	string ModeloName;
	string skeletonName;

	double iniTwist; 
	double finTwist; 
	bool enableTwist; 

	virtual bool saveToFile(FILE* fout);
	virtual bool loadFromFile(FILE* fout);
};

class defNodeSerialization : nodeSerialization
{
	int boneId; 
	int rootBoneId;
	int childBoneId;

	Vector3d pos;

	double cuttingThreshold;
	double precomputedDistances;
	float ratio; 
	float expansion;

	bool enableWeightsComputation;

	vector<double> MVCWeights;
	vector<int> weightsSort;

	virtual bool saveToFile(FILE* fout);
	virtual bool loadFromFile(FILE* fout);
};

class defGroupSerialization : nodeSerialization
{
	vector<DefNode> deformers;
	joint* transformation;

	float subdivisionRatio;

	float expansion;
	int smoothingPasses;
	float smoothPropagationRatio;

	DefGroupType type; 

	// For computational pourposes.
	vector<DefGroup*> relatedGroups;


	virtual bool saveToFile(FILE* fout);
	virtual bool loadFromFile(FILE* fout);
};

class defGraphSerialization : nodeSerialization
{
	virtual bool saveToFile(FILE* fout);
	virtual bool loadFromFile(FILE* fout);
};

class contraintSerialization : nodeSerialization
{
	virtual bool saveToFile(FILE* fout);
	virtual bool loadFromFile(FILE* fout);
};

class skinningSerialization : nodeSerialization
{
	virtual bool saveToFile(FILE* fout);
	virtual bool loadFromFile(FILE* fout);
};

*/

#endif