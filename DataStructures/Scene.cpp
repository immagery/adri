#include "Scene.h"
#include "..\render\gridRender.h"

#include <DataStructures\AirRig.h>

unsigned int scene::sceneIds = FIRST_NODE_ID;


void scene::setSceneScale( float sceneScale)
{
	for(int model = 0; model < models.size(); model++)
	{
		//for(int bd = 0; bd < ((Modelo*)models[model])->bindings.size(); bd++)
		//{
			binding* bind = ((Modelo*)models[model])->bind;
			bind->worldScale = sceneScale;
			printf("Global Smoothness: %f\n", sceneScale); 
			fflush(0);
		//}
	}
	/*
	if(DEBUG) printf("setSceneScale %f\n", sceneScale); fflush(0);

	for(unsigned int i = 0; i< visualizers.size(); i++)
	{
		((gridRenderer*)visualizers[i])->grid->worldScale = sceneScale;
	}
	*/
}

void scene::setGlobalSmoothness(float globalSmooth)
{
	for(int model = 0; model < models.size(); model++)
	{
		//for(int bd = 0; bd < ((Modelo*)models[model])->bindings.size(); bd++)
		//{
			binding* bind = ((Modelo*)models[model])->bind;
			bind->smoothPropagationRatio = globalSmooth;
			printf("Global Smoothness: %f\n", globalSmooth); 
			fflush(0);
		//}
	}

	/*
	if(DEBUG) printf("setGlobalSmoothness %f\n", globalSmooth); fflush(0);

	for(unsigned int i = 0; i< visualizers.size(); i++)
	{
		((gridRenderer*)visualizers[i])->grid->smoothPropagationRatio = globalSmooth;
	}
	*/
}


bool bindRigToScene(Modelo* model, vector<skeleton*>skeletons, AirRig* rig)
{
	// bind model
	rig->bindModel(model);
	
	//bind skeletons
	for(int i = 0; i< skeletons.size(); i++)
		rig->skeletons.push_back(skeletons[i]);

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
	for(int defgroupIdx = 0; defgroupIdx< rig->defRig.defGroups.size(); defgroupIdx++)
	{
		// get correspondence
		nodeCorrespondence[rig->defRig.defGroups[defgroupIdx]->sNode->nodeId] = rig->defRig.defGroups[defgroupIdx]->nodeId;
		rig->defRig.defGroupsRef[rig->defRig.defGroups[defgroupIdx]->nodeId] = rig->defRig.defGroups[defgroupIdx];

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

	rig->skin->deformedModel = rig->model;
	rig->skin->originalModel = rig->model->originalModel;

	// Default initialization
	rig->skin->bind = rig->model->bind;

	return false;
}
