#include "Scene.h"
#include "..\render\gridRender.h"

//#include <DataStructures\AirRig.h>

unsigned int scene::objectIds = FIRST_OBJECT_ID;
unsigned int scene::defNodeIds = FIRST_DEFNODE_ID;
unsigned int scene::defGroupIds = FIRST_DEFGROUP_ID;
unsigned int scene::pointDataIds = FIRST_POINTDATA_ID;
unsigned int scene::boneIds = FIRST_BONE_ID;
unsigned int scene::generalIds = FIRST_GNRAL_ID;

// Global Drawing scene flags
bool scene::drawDefNodes = true;
float scene::drawingNodeStdSize = 1.0;

void scene::setSceneScale( float _sceneScale)
{
	sceneScale = _sceneScale;
	scene::drawingNodeStdSize = sceneScale;

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
