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
	// Copiar el modelo para tener una "copia deformable"
	Modelo* m2 = new Modelo(getNewId());
	models.push_back(m2);

	// Copiar vertices
	m2->nodes.resize(m->nodes.size());
	for (int i = 0; i < m2->nodes.size(); ++i) {
		m2->nodes[i] = new GraphNode(i);
		m2->nodes[i]->position = m->nodes[i]->position;
		m2->nodes[i]->connections.resize(m->nodes[i]->connections.size());
		for (int j = 0; j < m2->nodes[i]->connections.size(); ++j) {
			m2->nodes[i]->connections[j] = m->nodes[i]->connections[j];
		}
	}

	// Copiar caras
	m2->triangles.resize(m->triangles.size());
	for (int i = 0; i < m2->triangles.size(); ++i) {
		m2->triangles[i] = new GraphNodePolygon(i);
		m2->triangles[i]->verts.resize(m->triangles[i]->verts.size());
		for (int j = 0; j < m2->triangles[i]->verts.size(); ++j) {
			m2->triangles[i]->verts[j] = m->triangles[i]->verts[j];
		}
	}

	m2->shading->visible = true;
	m2->computeNormals();
	skinner->loadBindingForModel(m,m2,path, skeletons);
}

