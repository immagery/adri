#ifndef RIG_H
#define RIG_H

#include <DataStructures\Modelo.h>
#include <DataStructures\skeleton.h>
#include <DataStructures\Skinning.h>

class Rig : public node 
{
public:

	//rig() : node(){}
	Rig(unsigned int id) : node(id) 
	{
		skin = new Skinning();
		model = NULL;
		skeletons.clear();
		enableDeformation = false;
		binded = false;
	}

	// The base model
	Modelo* model;

	// The skeletons used to compute skinning, or used as output
	vector<skeleton*> skeletons;

	//The deformer
	Skinning* skin;

	bool enableDeformation;
	bool binded;

	// Load the rig defined in the file
	virtual bool loadRigging(string sFile);
	
	// Connects the Rig to this data set
	virtual bool bindLoadedRigToScene(Modelo* model, vector<skeleton*>& skeletons);
	virtual bool bindRigToModelandSkeleton(Modelo* in_model, vector<skeleton*>& in_skeletons);

	virtual bool bindModel(Modelo* m);
	virtual bool bindSkeletons(vector<skeleton*>& skts);

	virtual void getNodeData(int nodeId, string& sName){}

	virtual void getDataFromDefGroup(int element, float& expansion, bool& twistEnabled,
									 bool& bulgeEnabled,int& localSmoothPases,
									 float& twistIni,float& twistFin){}

	virtual void select(bool bSelect, int element) {}
	virtual bool isSelectable(int element){ return true; }

};

#endif // RIG_H