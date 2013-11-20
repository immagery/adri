#include "Scene.h"
#include "..\render\gridRender.h"

//#include <DataStructures\AirRig.h>

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
