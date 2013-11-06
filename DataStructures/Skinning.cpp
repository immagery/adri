#include "Skinning.h"
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qregexp.h>
#include <set>

using namespace Eigen;

Skinning::Skinning() {
    bindings = map<int,vector<binding*> >();
	originalModels = vector<Geometry*>(0);
	deformedModels = vector<Geometry*>(0);

	quaternionDef = 0;
}


Skinning::~Skinning()
{
}

void Skinning::loadBindingForModel(Modelo *m, string path, const vector< skeleton* >& skeletons) {
	printf("Loading binding data...");
    QFile file(path.c_str());
	
	if (!file.exists()) {
		printf("\nNo binding file found! Proceeding to load model without bindings\n");
		return;
	}

	int modelIndex = deformedModels.size();
	bindings[modelIndex] = vector<binding*> ();

	for(int i = 0; i< m->bindings.size(); i++)
		bindings[modelIndex].push_back(m->bindings[i]);

	// De momento trabajamos con el primero
	// DEBUG: we check the last added binding. What happens if a model has more than two bindings? 
	// TOFIX a model can have more than 1 binding
	binding* bd = bindings[modelIndex][0];

	deformedModels.push_back((Geometry*) m);
	originalModels.push_back(m->originalModel);

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

/*
void Skinning::computeDeformations(const vector< skeleton* >& skeletons) {
	//for (int v = 0; v < originalModels.size(); ++v) originalModels[v]->shading->visible = false;	 // keep it false ALWAYS

	if (skeletons.size() > 0) {
		//if (!skeletons[0]->joints[0]->dirtyFlag) return;
		skeletons[0]->joints[0]->computeWorldPos();
	}
	bool updated = false;

	for (int i = 0; i < deformedModels.size(); ++i) {		// for every deformed model
		Geometry *m = deformedModels[i];
		for (int j = 0; j < bindings.size(); ++j) {			// loop through all bindings
			binding * b = bindings[j];

			for (int k = 0; k < b->pointData.size(); ++k) // and for each binding, loop over all its points
			{     
				PointData data = b->pointData[k];
				GraphNode* node = data.node;
				int vertexID = node->id;
				Point3d finalPosition (0,0,0);
				float totalWeight = 0;
				for (int kk = 0; kk < data.influences.size(); ++kk) {   // and check all joints associated to them
					int skID = data.influences[kk].label;
					for (int s = 0; s < skeletons.size(); ++s) 
					{
						//for (int t = 0; t < skeletons[s]->joints.size(); ++t) {
							joint* joint = skeletons[s]->getJoint(skID);
							if(joint)
							{
							//joint* joint = skeletons[s]->joints[t];
							//if (joint->nodeId == skID) {
								Point3d restPosition = originalModels[i]->nodes[vertexID]->position;
								MatrixXf restPos(4,1); restPos << restPosition.X(), restPosition.Y(), restPosition.Z(), 1;
								MatrixXf finalPos =  joint->W * joint->iT * restPos;
								finalPosition = finalPosition + Point3d(finalPos(0,0), finalPos(1,0), finalPos(2,0)) * data.influences[kk].weightValue;
								totalWeight += data.influences[kk].weightValue;
								break;
							}
							//}
						//}
					}
				}
				finalPosition = finalPosition / totalWeight;
				if (m->nodes[vertexID]->position != finalPosition) 
					updated = true;

				m->nodes[vertexID]->position = finalPosition;
			}

		}
		if (updated)
		{
			m->computeNormals();
		}
	}



}
*/

void Skinning::computeDeformationsWithSW(const vector< skeleton* >& skeletons) {
	// Caluculamos las matrices de Transformacion de cada articulacion

	if (skeletons.size() == 0) return;

	// Check if there's at least one dirty skeleton. In that case, proceed
	bool oneDirtySkeleton = false;
	for (int i = 0; i < skeletons.size(); ++i) {
		if (skeletons[i]->joints[0]->dirtyFlag) {
			oneDirtySkeleton = true;
			break;
		}
	}
	if (!oneDirtySkeleton) return;
	
	bool updated = false;

	for (int i = 0; i < deformedModels.size(); ++i) {		// for every deformed model
		if (skeletons[i]->joints[0]->dirtyFlag) skeletons[i]->joints[0]->computeWorldPos();
		else continue;
		if (!deformedModels[i]->shading->visible) return;
		Geometry *m = deformedModels[i];
		for (int j = 0; j < bindings[i].size(); ++j) {			// loop through all bindings
			binding * b = bindings[i][j];

			for (int k = 0; k < b->pointData.size(); ++k) { // and for each binding, loop over all its points
				PointData& data = b->pointData[k];
				GraphNode* node = data.node;
				int vertexID = node->id;
				Eigen::Vector3d finalPosition (0,0,0);
				float totalWeight = 0;

				Eigen::Vector3d rotDir; 
				for (int kk = 0; kk < data.influences.size(); ++kk) // and check all joints associated to them
				{   
					int skID = data.influences[kk].label;
					joint* jo  = NULL;
					for (int s = 0; s < skeletons.size(); ++s) 
					{
						jo = skeletons[s]->getJoint(skID);
						if(jo) break;
					}	
					if(jo)
					{
						//Eigen::Quaternion<float> eigenQuatAux(1,0,0,0);
						double twistInterpolation = 0; 
							
						if(data.secondInfluences.size()>0) // Si es cero es que no hay hijos
						{
							for(int jointChild = 0; jointChild < jo->childs.size(); jointChild++)
							{

								if(jointChild == 0)
								{
									//rotDir = (jo->childs[jointChild]->pos-jo->pos).Normalize();
									Eigen::Vector3d& ap = jo->childs[jointChild]->worldPosition;
									Eigen::Vector4f ap4 = Vector4f(ap.x(),ap.y(),ap.z(),1);
									ap4 = jo->iT * ap4;
									rotDir = Eigen::Vector3d(ap4[0],ap4[1],ap4[2]).normalized();
								}
								else
								{
									//rotDir += (jo->childs[jointChild]->pos-jo->pos).Normalize();
									Eigen::Vector3d& ap = jo->childs[jointChild]->worldPosition;
									Eigen::Vector4f ap4 = Vector4f(ap.x(),ap.y(),ap.z(),1);
									ap4 = jo->iT * ap4;
									rotDir += Eigen::Vector3d(ap4[0],ap4[1],ap4[2]).normalized();
								}
								//jo->childs[jointChild]->qrot.Normalize();
								// Obtener twist del hijo
								Eigen::Vector3d axis;
								//double twist = 2*acos(jo->childs[jointChild]->qrot.X());

								double twist, angle1, angle2; 
								//jo->childs[jointChild]->qrot.ToAxis(twist2,axis);
								//jo->childs[jointChild]->qrot.ToEulerAngles(twist, angle1, angle2);

								//double val01,val02,val03;
								//jo->childs[jointChild]->qrot.ToEulerAngles(val01,val02,val03);

								//vcg::Quaternion<double> quatAuxInt = vcg::Quaternion<double>(cos(twist/2),0,0,0);
								//quatAux += quatAuxInt * data.secondInfluences[kk][jointChild];
								twistInterpolation += twist*(1-data.secondInfluences[kk][jointChild]);

								//Eigen::Quaternion<float> quatAux2(jo->childs[jointChild]->qrot.X(), jo->childs[jointChild]->qrot.Y(), jo->childs[jointChild]->qrot.Z(),jo->childs[jointChild]->qrot.W());
								//eigenQuatAux.slerp(data.secondInfluences[kk][jointChild], quatAux2);
							}

							if(jo->childs.size() > 0)
							{
								rotDir.normalize();
							}
							else
								rotDir = Eigen::Vector3d(1,0,0);
						}

						//if(k == 95)
						//{
					//		printf("Esta es la interpolacion %f.\n", twistInterpolation); fflush(0);
						//}

						//vcg::Quaternion<float> quatAux = vcg::Quaternion<float>(cos(twistInterpolation/2),0,0,0);
						//vcg::Quaternion<float> quatAux = vcg::Quaternion<float>(1,0,0,0);

						//Matrix44f twistMatrix;
						//quatAux.Normalize();
						//quatAux.ToMatrix(twistMatrix);
						//Eigen::Quaternionf qOp();
						//qOp.(cos(twistInterpolation/2), 0, 0, 0);
						//Eigen::Matrix4f twm; twm << twistMatrix[0][0] ,twistMatrix[0][1], twistMatrix[0][2], twistMatrix[0][3],
						//							twistMatrix[1][0] ,twistMatrix[1][1], twistMatrix[1][2], twistMatrix[1][3],
						//							twistMatrix[2][0] ,twistMatrix[2][1], twistMatrix[2][2], twistMatrix[2][3],
						//							twistMatrix[3][0] ,twistMatrix[3][1], twistMatrix[3][2], twistMatrix[3][3];

						Eigen::Vector3d& restPosition = originalModels[i]->nodes[vertexID]->position;
						Eigen::Vector4f restPos(restPosition.x(), restPosition.y(), restPosition.z(), 1);
						Eigen::Vector4f finalPosAux = jo->iT * restPos;
						
						Eigen::Vector3f auxPos(finalPosAux[0], finalPosAux[1], finalPosAux[2]);

						// Aplicacion del twist.
						
						Eigen::Matrix3f m;
						Eigen::Vector3f pruebas(rotDir.x(),rotDir.y(),rotDir.z());
						m = AngleAxisf(twistInterpolation, pruebas);
						//m = AngleAxisf(twistInterpolation, Vector3f::UnitX());
						//Vector3f pruebas = Vector3f::UnitX();
						//pruebas += Vector3f(rotDir.X(),rotDir.Y(),rotDir.Z());
						//rotDir = Point3d(1.000, 0.0, 0.0);
						
						//Vector3f pruebas(1.0,0,0); 
						//AngleAxisf(twistInterpolation, pruebas);
						//auxPos = m*auxPos;

						Eigen::Vector4f finalPos2(auxPos[0], auxPos[1],auxPos[2],1);
						Eigen::Vector4f finalPos = jo->W * finalPos2;
						

						//auxPos = auxPos.transpose()*qOp.toRotationMatrix();
						//auxPos = qOp.toRotationMatrix()*auxPos.transpose();
						

						/*
						//qOp.normalize();
						//Eigen::Quaternionf qOp2 = qOp.conjugate();
						//qOp2.normalize();
							
						//Eigen::Vector3f finalPos;
						//finalPos << finalPosMatrix(0,0), finalPosMatrix(1,0), finalPosMatrix(2,0); 

						//finalPos = qOp*finalPos;
						*/

						float weight = data.influences[kk].weightValue;
						finalPosition = finalPosition + Eigen::Vector3d(finalPos(0), finalPos(1), finalPos(2)) * weight;
						//finalPosition = finalPosition + Point3d(finalPos(0), auxPos(1), auxPos(2)) * weight;

						totalWeight += data.influences[kk].weightValue;
					}
				}

				// es una comprobacion burda, pero deberia darse.
				//if(fabs(totalWeight-1)> 0.00001)
				//	printf("Algo pasa con los valores de los pesos: %f", totalWeight);
				//assert(fabs(totalWeight-1)< 0.00001);

				finalPosition = finalPosition / totalWeight;
				if (m->nodes[vertexID]->position != finalPosition) 
					updated = true;

				m->nodes[vertexID]->position = finalPosition;
			}

		}
		if (updated)
		{
			
			m->computeNormals();
		}
	}
}