#ifndef RIG_H
#define RIG_H

#include <DataStructures\Modelo.h>
#include <DataStructures\skeleton.h>
#include <DataStructures\Skinning.h>

class rig : public node 
{
public:

	//rig() : node(){}
	rig(unsigned int id) : node(id) 
	{
		skinning = new Skinning();
		model = NULL;
		skeletons.clear();
	}

	rig(Modelo* in_model, unsigned int id) : node(id) 
	{
		skinning = new Skinning();
		model = in_model;
	}

	rig(Modelo* in_model, vector<skeleton*> skts, unsigned int id) : node(id) 
	{
		skinning = new Skinning();
		model = in_model;
		skeletons.resize(skts.size()); 
		for(int sktIdx = 0; sktIdx< skts.size(); sktIdx++)
		{
			skeletons[sktIdx] = skts[sktIdx];
		}
	}

	// The base model
	Modelo* model;

	// The skeletons used to compute skinning, or used as output
	vector<skeleton*> skeletons;

	//The deformer
	Skinning* skinning;

	virtual bool bindModel(Modelo& m);
	virtual bool bindSkeletons(vector<skeleton*>& skts);

	// Load the rig defined in the file
	virtual bool loadRigging(string sFile);
	
	virtual bool bindRigToScene(Modelo& model, vector<skeleton*>& skeletons);

};

#endif // RIG_H