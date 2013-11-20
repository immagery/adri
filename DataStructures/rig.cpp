#include <DataStructures\Rig.h>

bool Rig::bindLoadedRigToScene(Modelo* model, vector<skeleton*>& skeletons)
{
	//TODO
	assert(false);
	return false;
}

bool Rig::bindRigToModelandSkeleton(Modelo* in_model, vector<skeleton*>& in_skeletons)
{
	// bind model
	bindModel(in_model);

	// bindSkeletons
	bindSkeletons(in_skeletons);

	/*
	// get joint info
	map<string, joint*> allJoints;
	for(int i = 0; i< skeletons.size(); i++)
	{
		for(int idJoint = 0; idJoint < skeletons[i]->joints.size(); idJoint++)
		{
			allJoints[skeletons[i]->joints[idJoint]->sName] = skeletons[i]->joints[idJoint];
		}
	}
	
	// bind Groups to joints -> to remove???
	map<int, int > nodeCorrespondence;
	for(int defgroupIdx = 0; defgroupIdx< rigPtr->defRig.defGroups.size(); defgroupIdx++)
	{
		// get correspondence
		nodeCorrespondence[rigPtr->defRig.defGroups[defgroupIdx]->sNode->nodeId] = rig->defRig.defGroups[defgroupIdx]->nodeId;
		rigPtr->defRig.defGroupsRef[rigPtr->defRig.defGroups[defgroupIdx]->nodeId] = rig->defRig.defGroups[defgroupIdx];

		// bind to the transformation
		joint* jt = NULL;
		rig->defRig.defGroups[defgroupIdx]->transformation = allJoints[rig->defRig.defGroups[defgroupIdx]->serializedData->sJointName];
		if(!rig->defRig.defGroups[defgroupIdx]->transformation) return false;
		delete rig->defRig.defGroups[defgroupIdx]->serializedData;
		rig->defRig.defGroups[defgroupIdx]->serializedData = NULL;

		// get correspondence
		for(int defIdx = 0; defIdx< rig->defRig.defGroups[defIdx]->deformers.size(); defIdx++)
		{
			nodeCorrespondence[rig->defRig.defGroups[defgroupIdx]->deformers[defIdx].sNode->nodeId] = rig->defRig.defGroups[defgroupIdx]->deformers[defIdx].nodeId;
		}
	}

	// Bind Constraint Relation
	for(int relationIdx = 0; relationIdx< rig->defRig.relations.size(); relationIdx++)
	{
		int childIdx = nodeCorrespondence[rig->defRig.relations[relationIdx]->sConstraint->childId];
		int parentIdx = nodeCorrespondence[rig->defRig.relations[relationIdx]->sConstraint->parentId];

		rig->defRig.relations[relationIdx]->child = rig->defRig.defGroupsRef[childIdx];
		rig->defRig.relations[relationIdx]->parent = rig->defRig.defGroupsRef[parentIdx] ;
	}

	BuildGroupTree(rig->defRig);

	//rig->skinning.bindings[0] = vector<binding*> ();
	//for(int i = 0; i< rig->model->bindings.size(); i++)
	//	rig->skinning.bindings[0].push_back(rig->model->bindings[i]);

	rig->skinning->deformedModels.push_back(rig->model);
	rig->skinning->originalModels.push_back(rig->model->originalModel);
	rig->skinning->rig = rig;

	// Default initialization
	rig->skinning->bind = new binding();

	*/

	return false;
}


bool Rig::bindModel(Modelo* m)
{
	// Asign model to this rig
	model = m;
	skin->bind = model->bind;
	skin->deformedModel = model;

	if(!m->originalModelLoaded)
		initModelForDeformation(model);

	skin->originalModel = model->originalModel;

	// TODEBUG: Load data form the model???
	// for consistency.

	return true;
}

bool Rig::bindSkeletons(vector<skeleton*>& in_skeletons)
{
	// bind skeletons
	skeletons.resize(in_skeletons.size());
	for(int i = 0; i< skeletons.size(); i++)
		skeletons.push_back(in_skeletons[i]);

	return false;
}

bool Rig::loadRigging(string sFile)
{
	FILE* fin = NULL;
	fin = fopen(sFile.c_str(),"r");
	
	// Cargar rig
	// No tiene nada que cargar.

	fclose(fin);
	return true;
}