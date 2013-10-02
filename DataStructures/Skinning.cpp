#include "Skinning.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <set>

using namespace Eigen;

Skinning::Skinning() {
    bindings = vector<binding*>(0);
	originalModels = vector<Modelo*>(0);
	deformedModels = vector<Modelo*>(0);
}


Skinning::~Skinning()
{
}

void Skinning::loadBindingForModel (Modelo *m, Modelo* m2, string path, const vector< skeleton* >& skeletons) {
	printf("Loading binding data...");
    QFile file(path.c_str());
	if (!file.exists()) {
		printf("\nNo binding file found! Proceeding to load model without bindings\n");
		return;
	}

	// Make the original model invisible
	m->shading->visible = false;
	m2->shading->visible = true;
	originalModels.push_back(m);
	deformedModels.push_back(m2);

    binding * b = new binding();
	b->pointData.resize(0);
    bindings.push_back(b);

    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    QString line;

	set<int> vertices;

    while (!in.atEnd()) {
        line = in.readLine();
        QStringList list = line.split(QRegExp(" "));
		PointData point;
		point.node = new GraphNode();
		point.node->id = list.at(0).toInt();
		//printf("Read binding for vertex %d:", point.node->id);
		vertices.insert(point.node->id);

        for (int i = 1; i < list.size(); i += 2) {
			int skID = -1;
			QString skeletonName = list.at(i);

			skeleton* sk = skeletons[0];
			for (int j = 0; j < sk->joints.size(); ++j) {
				joint* jt = sk->joints[j];
				if (jt->sName == skeletonName.toStdString()) {
					skID = jt->nodeId;
					break;
				}
			}


			if (skID == -1) {
				printf("Something bad happened: bone does not exist!\n");
				assert(false);
			}

			float weightValue = list.at(i+1).toFloat();
			weight w;
			w.label = skID;
			w.weightValue = weightValue;
			point.influences.push_back(w);
			//printf("%d %f ", skID, weightValue);
        }

		//printf("\n");
		b->pointData.push_back(point);
    }

	printf("done\n");
}

/*
To compute rest positions: for each model, loop through all their vertices. For each vertex, compute its rest position
against all joints and store it.
*/

void Skinning::computeRestPositions(const vector< skeleton* >& skeletons) {
	printf("Computing rest positions...");

	skeletons[0]->joints[0]->computeRestPos();

	printf(" done\n");
}

void Skinning::computeDeformations(const vector< skeleton* >& skeletons) {
	for (int v = 0; v < originalModels.size(); ++v) originalModels[v]->shading->visible = false;	 // keep it false ALWAYS

	if (skeletons.size() > 0) skeletons[0]->joints[0]->computeWorldPos();
	bool updated = false;

	for (int i = 0; i < deformedModels.size(); ++i) {		// for every deformed model
		Modelo *m = deformedModels[i];
		for (int j = 0; j < bindings.size(); ++j) {			// loop through all bindings
			binding * b = bindings[j];
			for (int k = 0; k < b->pointData.size(); ++k) {     // and for each binding, loop over all its points
				PointData data = b->pointData[k];
				GraphNode* node = data.node;
				int vertexID = node->id;
				Point3d finalPosition (0,0,0);
				float totalWeight = 0;
				for (int kk = 0; kk < data.influences.size(); ++kk) {   // and check all joints associated to them
					int skID = data.influences[kk].label;
					for (int s = 0; s < skeletons.size(); ++s) {
						for (int t = 0; t < skeletons[s]->joints.size(); ++t) {
							joint* joint = skeletons[s]->joints[t];
							if (joint->nodeId == skID) {
								Point3d restPosition = originalModels[i]->nodes[vertexID]->position;
								MatrixXf restPos(4,1); restPos << restPosition.X(), restPosition.Y(), restPosition.Z(), 1;
								MatrixXf finalPos =  joint->W * joint->iT * restPos;
								finalPosition = finalPosition + Point3d(finalPos(0,0), finalPos(1,0), finalPos(2,0)) * data.influences[kk].weightValue;
								totalWeight += data.influences[kk].weightValue;
								break;
							}
						}
					}
				}
				finalPosition = finalPosition / totalWeight;
				if (m->nodes[vertexID]->position != finalPosition) updated = true;
				m->nodes[vertexID]->position = finalPosition;
			}
		}
		if (updated) m->computeNormals();
	}

}
