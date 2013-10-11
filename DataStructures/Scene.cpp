#include "Scene.h"
#include "..\render\gridRender.h"

unsigned int scene::sceneIds = FIRST_NODE_ID;


void scene::setSceneScale( float sceneScale)
{
	for(int model = 0; model < models.size(); model++)
	{
		for(int bd = 0; bd < ((Modelo*)models[model])->bindings.size(); bd++)
		{
			binding* bind = ((Modelo*)models[model])->bindings[bd];
			bind->worldScale = sceneScale;
			printf("Global Smoothness: %f\n", sceneScale); 
			fflush(0);
		}
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
		for(int bd = 0; bd < ((Modelo*)models[model])->bindings.size(); bd++)
		{
			binding* bind = ((Modelo*)models[model])->bindings[bd];
			bind->smoothPropagationRatio = globalSmooth;
			printf("Global Smoothness: %f\n", globalSmooth); 
			fflush(0);
		}
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

	// Copiar el modelo para tener una "copia deformable"
	//Modelo* m2 = new Modelo(getNewId());
	//models.push_back(m2);

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
	
	skinner->loadBindingForModel(m, path, skeletons);
}

