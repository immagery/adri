#include "Skinning.h"
//#include <QtCore/qfile.h>
//#include <QtCore/qtextstream.h>
//#include <QtCore/qstringlist.h>
//#include <QtCore/qregexp.h>
#include <set>

#include <iostream>
#include <fstream>
#include <string>

#include <dataStructures/scene.h>

using namespace Eigen;

Skinning::Skinning() {
    bind = NULL;
	originalModel = NULL;
	deformedModel = NULL;
}


Skinning::~Skinning()
{
	bind = NULL;
	originalModel = NULL;
	deformedModel = NULL;
}

void Skinning::loadBindingForModel(Modelo *m, const vector< skeleton* >& skeletons) {

	/*
	binding* bd = m->bind;

	ifstream file();
	string line;
	vector<string> elems;

		getline (in , line);
		split(line, ' ', elems);

		boneId = atoi(elems[0].c_str());
		rootBoneId = atoi(elems[1].c_str());
		childBoneId = atoi(elems[2].c_str());
		ratio = atof(elems[3].c_str());
		expansion = atof(elems[4].c_str());
		precomputedDistances = atof(elems[5].c_str());
		cuttingThreshold = atof(elems[6].c_str());
		pos.x() = atof(elems[7].c_str());
		pos.y() = atof(elems[8].c_str());
		pos.z() = atof(elems[9].c_str());
		relPos.x() = atof(elems[10].c_str());
		relPos.y() = atof(elems[11].c_str());
		relPos.z() = atof(elems[12].c_str());
		enableWeightsComputation = atoi(elems[13].c_str());

		int MVCsize = 0, weightSize =0;
		getline (in , line);
		split(line, ' ', elems);
		MVCsize = atoi(elems[0].c_str());
		weightSize = atoi(elems[1].c_str());

		getline (in , line);
		split(line, ' ', elems);

		MVCWeights.resize(MVCsize);
		for(int i = 0; i< MVCWeights.size(); i++)
		{
			MVCWeights[i] = atof(elems[i].c_str());
		} 
		
		getline (in , line);
		split(line, ' ', elems);

		weightsSort.resize(weightSize);
		for(int i = 0; i< weightsSort.size(); i++)
		{
			weightsSort[i] = atoi(elems[i].c_str());
		}

    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    QString line;

	//set<int> vertices;

	int counter = 0;
    while (!in.atEnd()) {
        line = in.readLine();
        QStringList list = line.split(QRegExp(" "));
		//PointData point;
		//point.node = new GraphNode();
		//point.node->id = list.at(0).toInt();
		//printf("Read binding for vertex %d:", point.node->id);
		//vertices.insert(point.node->id);

		PointData& point = bd->pointData[counter];
		counter++;

        for (int i = 1; i < list.size(); i += 2) {
			int skID = -1;
			QString jointName = list.at(i);

			// DEBUG: we now search always in the last added skeleton, does this always work?
			//for(int skIdx = 0; skIdx < skeletons.size(); skIdx++) { }
			skeleton* sk = skeletons[skeletons.size()-1];
			for (int j = 0; j < sk->joints.size(); ++j) {
				joint* jt = sk->joints[j];
				if (jt->sName == jointName.toStdString()) {
					skID = jt->nodeId;
					break;
				}
			}	
			
			if (skID == -1) {
				printf("Something bad happened: joint does not exist!\n");
				assert(false);
			}

			float weightValue = list.at(i+1).toFloat();
			weight w;
			w.label = skID;
			w.weightValue = weightValue;
			point.influences.push_back(w);
        }

		line = in.readLine();
        QStringList secondList = line.split(QRegExp(" "));
		point.secondInfluences.resize(point.influences.size());
		int paramCount = 0;
		for(int infl = 0; infl < point.influences.size(); infl++)
		{
			int secCount = secondList[paramCount].toInt();
			point.secondInfluences[infl].resize(secCount);
			paramCount++;

			for(int sInfl = 0; sInfl < secCount; sInfl++)
			{
				point.secondInfluences[infl][sInfl] = secondList[paramCount].toFloat();
				paramCount++;
			}
		}

		//printf("\n");
		//b->pointData.push_back(point);
    }

	printf("done\n");
	*/
}


void copyModel2Model(Geometry * modelCopy, Geometry *modelReference) 
{
	//Geometry *modelCopy = m;
	//Geometry *modelReference = m->originalModel;

	modelCopy->nodes.resize(modelReference->nodes.size());
	for (int i = 0; i < modelCopy->nodes.size(); ++i) 
	{
		modelCopy->nodes[i] = new GraphNode(i);
		modelCopy->nodes[i]->position = modelReference->nodes[i]->position;
		modelCopy->nodes[i]->connections.resize(modelReference->nodes[i]->connections.size());
		for (int j = 0; j < modelCopy->nodes[i]->connections.size(); ++j) 
		{
			modelCopy->nodes[i]->connections[j] = modelCopy->nodes[modelReference->nodes[i]->connections[j]->id];
		}
	}

	// Copiar caras
	modelCopy->triangles.resize(modelReference->triangles.size());
	for (int i = 0; i < modelCopy->triangles.size(); ++i) 
	{
		modelCopy->triangles[i] = new GraphNodePolygon(i);
		modelCopy->triangles[i]->verts.resize(modelReference->triangles[i]->verts.size());
		for (int j = 0; j < modelCopy->triangles[i]->verts.size(); ++j) 
		{
			modelCopy->triangles[i]->verts[j] = modelCopy->nodes[modelReference->triangles[i]->verts[j]->id];
		}
	}

	modelCopy->shading->visible = false;
	modelCopy->computeNormals();
}

void resetDeformations(Modelo* m) 
{
	// Si no ha sido cargado no se puede hacer reset
	if(m->originalModelLoaded)
	{
		Geometry *modelCopy = m;
		Geometry *modelReference = m->originalModel;
		copyModel2Model( modelCopy, modelReference);
	}
}

void initModelForDeformation(Modelo* m) 
{
	Geometry *modelCopy = m->originalModel;
	Geometry *modelReference = m;
	copyModel2Model( modelCopy, modelReference);
	m->originalModelLoaded = true;

	/*
	m->originalModel = new Geometry(scene::getNewId());
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
	*/
}

/*
To compute rest positions: for each model, loop through all their vertices. For each vertex, compute its rest position
against all joints and store it.
*/

void Skinning::computeRestPositions(const vector< skeleton* >& skeletons) {
	printf("Computing rest positions...");

	//for (int i = 0; i < skeletons.size(); ++i)
	// This should only be done on the newly added skeleton!
	for(int i = 0; i< skeletons.size(); i++)
	{
		skeletons[i]->root->computeRestPos();
	}

	printf(" done\n");
}

void Skinning::computeDeformations2(skeleton* s) 
{

	// Check if there's at least one dirty skeleton. In that case, proceed
	bool updated = false;

	// It's a bad way to ensure that we are deforming the right mdoel.
	if (!deformedModel->shading->visible) return;

	Geometry *m = deformedModel;

	// loop through all bindings
	binding * b = bind;

	#pragma omp parallel for
	for (int k = 0; k < b->pointData.size(); ++k) 
	{ 
		// and for each binding, loop over all its points
		PointData& data = b->pointData[k];
		GraphNode* node = data.node;
		int vertexID = node->id;

		Vector3d finalPosition (0,0,0);
		float totalWeight = 0;

		Vector3d rotDir; 
		for (int kk = 0; kk < data.influences.size(); ++kk) // and check all joints associated to them
		{   
			int skID = data.influences[kk].label;
			joint* jt = s->jointRef[skID];

			float currentWeight = data.influences[kk].weightValue;

			Vector3d tempValue = originalModel->nodes[vertexID]->position - jt->rTranslation;
			Vector3d finalPos2 =  jt->rotation._transformVector(jt->rRotation.inverse()._transformVector(tempValue));
			finalPos2 += jt->translation;

			finalPosition += finalPos2 * currentWeight;
			totalWeight += data.influences[kk].weightValue;
		}

		finalPosition = finalPosition / totalWeight;
		if (m->nodes[vertexID]->position != finalPosition) 
			updated = true;

		m->nodes[vertexID]->position = finalPosition;
	}
	
	if (updated)
	{		
		m->computeNormals();
	}
}

void Skinning::computeDeformations(const vector< skeleton* >& skeletons) {
	//for (int v = 0; v < originalModels.size(); ++v) originalModels[v]->shading->visible = false;	 // keep it false ALWAYS

	// Check if there's at least one dirty skeleton. In that case, proceed
	bool oneDirtySkeleton = false;
	for (int i = 0; i < skeletons.size(); ++i) 
	{
		if (skeletons[i]->root->dirtyFlag) 
		{
			oneDirtySkeleton = true;
			skeletons[i]->root->computeWorldPos();
		}
	}
	if (!oneDirtySkeleton) return;
	
	bool updated = false;

	//for (int i = 0; i < deformedModels.size(); ++i) 
	{		
		// It's a bad way to ensure that we are deforming the right mdoel.
		if (!deformedModel->shading->visible) return;

		Geometry *m = deformedModel;

		// loop through all bindings
		binding * b = bind;

		for (int k = 0; k < b->pointData.size(); ++k) 
		{ 
			// and for each binding, loop over all its points
			PointData& data = b->pointData[k];
			GraphNode* node = data.node;
			int vertexID = node->id;

			Vector3d finalPosition (0,0,0);
			float totalWeight = 0;

			Vector3d rotDir; 
			for (int kk = 0; kk < data.influences.size(); ++kk) // and check all joints associated to them
			{   
				int skID = data.influences[kk].label;
				//joint& jt = deformersRestPosition[skID];
				
				joint* jt = skeletons[0]->jointRef[skID];

				Vector3d& restPosition = originalModel->nodes[vertexID]->position;
				Vector3d restPos2(restPosition.x(), restPosition.y(), restPosition.z());

				float currentWeight = data.influences[kk].weightValue;

				Vector3d finalPos2 =  jt->rotation._transformVector(jt->rRotation.inverse()._transformVector(restPos2-jt->rTranslation)) + jt->translation;
				finalPosition = finalPosition + Vector3d(finalPos2(0), finalPos2(1), finalPos2(2)) * currentWeight;

				totalWeight += data.influences[kk].weightValue;
					
			}

			finalPosition = finalPosition / totalWeight;
			if (m->nodes[vertexID]->position != finalPosition) 
				updated = true;

			m->nodes[vertexID]->position = finalPosition;
		}

	
		if (updated)
		{		
			m->computeNormals();
		}
	}
}


void Skinning::computeDeformationsWithSW(const vector< skeleton* >& skeletons) 
{
	
}

