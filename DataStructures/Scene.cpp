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

void scene::loadBindingForModel(Modelo* m, string path) {

	m->originalModel = new Geometry(getNewId());
	Geometry *modelCopy = m->originalModel;

	modelCopy->nodes.resize(m->nodes.size());
	for (int i = 0; i < modelCopy->nodes.size(); ++i) 
	{
		modelCopy->nodes[i] = new GraphNode(i);
		modelCopy->nodes[i]->position = m->nodes[i]->position;
		modelCopy->nodes[i]->connections.resize(m->nodes[i]->connections.size());
		for (int j = 0; j < modelCopy->nodes[i]->connections.size(); ++j) 
		{
			modelCopy->nodes[i]->connections[j] = modelCopy->nodes[m->nodes[i]->connections[j]->id];
		}
	}

	// Copiar caras
	modelCopy->triangles.resize(m->triangles.size());
	for (int i = 0; i < modelCopy->triangles.size(); ++i) 
	{
		modelCopy->triangles[i] = new GraphNodePolygon(i);
		modelCopy->triangles[i]->verts.resize(m->triangles[i]->verts.size());
		for (int j = 0; j < modelCopy->triangles[i]->verts.size(); ++j) 
		{
			modelCopy->triangles[i]->verts[j] = modelCopy->nodes[m->triangles[i]->verts[j]->id];
		}
	}

	modelCopy->shading->visible = false;
	modelCopy->computeNormals();
	
	rig->skinning->loadBindingForModel(m, path, skeletons);
}


bool bindRigToScene(Modelo& model, vector<skeleton*>skeletons, AirRig* rig)
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

	//rig->skinning.bindings[0] = vector<binding*> ();
	//for(int i = 0; i< rig->model->bindings.size(); i++)
	//	rig->skinning.bindings[0].push_back(rig->model->bindings[i]);

	rig->skinning->deformedModels.push_back(rig->model);
	rig->skinning->originalModels.push_back(rig->model->originalModel);
	rig->skinning->rig = rig;

	// Default initialization
	rig->skinning->bind = new binding();

	return false;
}
