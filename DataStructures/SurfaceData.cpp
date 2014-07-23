#include "SurfaceData.h"

#include <iostream>
#include <fstream>
#include <string>

#include <utils\util.h>


bool SaveOFFFromSurface(SurfaceGraph& graph, string& sFileName)
{
	FILE* fout;
    fout = fopen(sFileName.c_str(), "w");

	if(!fout) return false;

	fprintf(fout, "OFF\n");
	fprintf(fout, "%d %d %d\n", graph.nodes.size(), graph.triangles.size(), 0);

	for(int i = 0; i< graph.nodes.size(); i++)
	{
		fprintf(fout, "%f %f %f\n", graph.nodes[i]->position.x(), graph.nodes[i]->position.y(), graph.nodes[i]->position.z() );
	}

	for(int i = 0; i< graph.triangles.size(); i++)
	{
		assert( graph.triangles[i]->verts[0]->pieceId  < graph.nodes.size());
		assert( graph.triangles[i]->verts[1]->pieceId  < graph.nodes.size());
		assert( graph.triangles[i]->verts[2]->pieceId  < graph.nodes.size());

		fprintf(fout, "3 %d %d %d\n", graph.triangles[i]->verts[0]->pieceId, 
									  graph.triangles[i]->verts[1]->pieceId, 
									  graph.triangles[i]->verts[2]->pieceId);
	}

	fclose(fout);

	 return true;
}


bool MergeMeshes(vector<string> sFileList, string result)
{
	int globalCountTriangles = 0;
	int globalCountVertexes = 0;
	vector<SurfaceGraph> graphs;
	graphs.resize(sFileList.size());

	for(int fidx = 0; fidx< sFileList.size(); fidx++)
	{
		string sFileName = sFileList[fidx];
		SurfaceGraph& graph = graphs[fidx];

		ifstream file;
		file.open(sFileName.c_str(),  ios::in );

		string line;
		getline(file,line); // Format flag

		if(line.compare("OFF") != 0) return false;
		getline(file , line);
		getline(file , line);

		vector <string> elems;
		getline(file , line);
		split(line, ' ', elems);

		int num1 = atoi(elems[0].c_str());
		int num2 = atoi(elems[1].c_str());
		int num3 = atoi(elems[2].c_str());

		globalCountVertexes += num1;
		globalCountTriangles += num2;

		//Leemos los vertices
		graph.nodes.resize(num1);
		float value1, value2, value3;
		for(int i = 0; i< num1; i++)
		{
			getline(file , line);
			split(line, ' ', elems);
			value1 = atof(elems[0].c_str());
			value2 = atof(elems[1].c_str());
			value3 = atof(elems[2].c_str());

			graph.nodes[i] = new GraphNode(i);
			graph.nodes[i]->position = Eigen::Vector3d(value1,value2,value3);
		}

		// Leemos las caras.
		graph.triangles.resize(num2);
		int polygonCount = 0;
		int vertIdx = 0;
		for(int i = 0; i< num2; i++)
		{
			getline(file , line);
			split(line, ' ', elems);
			polygonCount = atoi(elems[0].c_str());

			graph.triangles[i] = new GraphNodePolygon(i);
			graph.triangles[i]->verts.resize(polygonCount);

			vector<int> indexes;
			for(int j = 0; j< polygonCount; j++)
			{
				vertIdx = atoi(elems[j+1].c_str());
				graph.triangles[i]->verts[j] = graph.nodes[vertIdx];
				indexes.push_back(vertIdx);
			}

			for(int j = 0; j< polygonCount; j++)
			{
				int connNodeId = graph.nodes[indexes[(j+1)%polygonCount]]->id;
				int founded = -1;
				int conSize = graph.nodes[indexes[j]]->connections.size();
				for(int conIdx = 0; conIdx < conSize && founded < 0; conIdx++)
					if(graph.nodes[indexes[j]]->connections[conIdx]->id == connNodeId)
						founded = conIdx;
			
				if(founded < 0)
				{
					graph.nodes[indexes[j]]->connections.push_back(graph.nodes[indexes[(j+1)%polygonCount]]);
					graph.nodes[indexes[(j+1)%polygonCount]]->connections.push_back(graph.nodes[indexes[j]]);
				}
			}
		}

		file.close();
	}

	FILE* fout;
    fout = fopen(result.c_str(), "w");

	if(!fout) return false;

	fprintf(fout, "OFF\n");
	fprintf(fout, "%d %d %d\n", globalCountVertexes, globalCountTriangles, 0);

	vector<int> startingIdx(graphs.size());
	startingIdx[0] = 0;
	 
	// Primero todos los vertices
	for(int igIdx = 0; igIdx< graphs.size(); igIdx++)
	{
		if(igIdx > 0) 
		{
			startingIdx[igIdx] = startingIdx[igIdx-1] + graphs[igIdx-1].nodes.size();
		}

		SurfaceGraph& graph = graphs[igIdx];
		for(int i = 0; i< graph.nodes.size(); i++)
		{
			fprintf(fout, "%f %f %f\n", graph.nodes[i]->position.x(), 
										graph.nodes[i]->position.y(), 
										graph.nodes[i]->position.z() );
		}
	}

	// Despues todos los triangulos
	for(int igIdx = 0; igIdx< graphs.size(); igIdx++)
	{
		SurfaceGraph& graph = graphs[igIdx];
		for(int i = 0; i< graph.triangles.size(); i++)
		{
			fprintf(fout, "3 %d %d %d\n", graph.triangles[i]->verts[0]->id + startingIdx[igIdx], 
										  graph.triangles[i]->verts[1]->id + startingIdx[igIdx], 
										  graph.triangles[i]->verts[2]->id + startingIdx[igIdx]);
		}
	}

	fclose(fout);

	return true;
}

bool BuildSurfaceFromOFFFile(SurfaceGraph& graph, string& sFileName)
{
	FILE* fin;
    fin = fopen(sFileName.c_str(), "r");

	if(!fin) return false;

	char str[1000];
    fscanf(fin, "%s", str); // Format flag
	string formatStr(str);
	if(formatStr.compare("OFF") != 0) return false;

	int num1, num2, num3;
    fscanf(fin, "%d", &num1);
    fscanf(fin, "%d", &num2);
	fscanf(fin, "%d", &num3);

	//Leemos los vertices
	graph.nodes.resize(num1);
	float value1, value2, value3;
	for(int i = 0; i< num1; i++)
	{
		fscanf(fin, "%f", &value1);
		fscanf(fin, "%f", &value2);
		fscanf(fin, "%f", &value3);
		graph.nodes[i] = new GraphNode(i);
		graph.nodes[i]->position = Eigen::Vector3d(value1,value2,value3);
	}

	// Leemos las caras.
	graph.triangles.resize(num2);
	int polygonCount = 0;
	int vertIdx = 0;
	for(int i = 0; i< num2; i++)
	{
		fscanf(fin, "%i", &polygonCount);
		graph.triangles[i] = new GraphNodePolygon(i);
		graph.triangles[i]->verts.resize(polygonCount);

		vector<int> indexes;
		for(int j = 0; j< polygonCount; j++)
		{
			fscanf(fin, "%i", &vertIdx);
			graph.triangles[i]->verts[j] = graph.nodes[vertIdx];
			indexes.push_back(vertIdx);
		}

		for(int j = 0; j< polygonCount; j++)
		{
			int connNodeId = graph.nodes[indexes[(j+1)%polygonCount]]->id;
			int founded = -1;
			for(int conIdx = 0; conIdx < graph.nodes[indexes[j]]->connections.size() && founded < 0; conIdx++)
				if(graph.nodes[indexes[j]]->connections[conIdx]->id == connNodeId)
					founded = conIdx;
			
			if(founded < 0)
			{
				graph.nodes[indexes[j]]->connections.push_back(graph.nodes[indexes[(j+1)%polygonCount]]);
				graph.nodes[indexes[(j+1)%polygonCount]]->connections.push_back(graph.nodes[indexes[j]]);
			}
		}
	}

	 return true;
}

PointData::PointData()
{
    influences.clear();
    auxInfluences.clear();

    embedding.clear();
    itPass =-1;

	secondInfluences.clear();

    confidenceLevel = 0;

    ownerLabel = -1;
    ownerWeight = 0;

    segmentId = -1;
	segmentDistance = 99999999;

    domain = 0.0;
    domainId = -1;

    component = -1;
    assigned = false;
    validated = false;

	//modelVert = -1;

    color = Eigen::Vector3f(0,0,0);

	isBorder = false;
}

PointData::PointData(const PointData& pd)
{
	influences.resize(pd.influences.size());
	for(int i = 0; i< pd.influences.size(); i++)
		influences[i] = pd.influences[i]; 

	auxInfluences.resize(pd.auxInfluences.size());
	for(int i = 0; i< pd.auxInfluences.size(); i++)
		auxInfluences[i] = pd.auxInfluences[i]; 

	secondInfluences.resize(pd.secondInfluences.size());
	for(int i = 0; i< pd.secondInfluences.size(); i++)
	{
		secondInfluences[i].resize(pd.secondInfluences[i].size());
		for(int j = 0; j< pd.secondInfluences[i].size(); j++)
			secondInfluences[i][j] = pd.secondInfluences[i][j]; 
	}

	embedding.resize(pd.embedding.size());
	for(int i = 0; i< pd.embedding.size(); i++)
		embedding[i] = pd.embedding[i]; 

    itPass =-1;

    confidenceLevel = pd.confidenceLevel;

    ownerLabel = pd.ownerLabel;
    ownerWeight = pd.ownerWeight;

    segmentId = pd.segmentId;
	segmentDistance =  pd.segmentId;

    domain = pd.domain;
    domainId = pd.domainId;

    component = pd.component;
    assigned = pd.assigned;
    validated = pd.validated;

	//modelVert = pd.modelVert;

    color = pd.color;
	node = pd.node;

	isBorder = pd.isBorder;

}

PointData::PointData(int weightsSize)
{
    if(weightsSize > 0)
        influences.resize(weightsSize);
    else
        influences.clear();

    auxInfluences.clear();
    itPass =-1;
    ownerLabel = -1;
    confidenceLevel = 0;

	secondInfluences.clear();

    component = -1;
    assigned = false;
    validated = false;

    color = Eigen::Vector3f(0,0,0);

	isBorder = false;

	segmentId = -1;
	segmentDistance = 99999;
}

// Clear no del todo, no quitamos el labeling de la voxelizacion
void PointData::clear()
{
    auxInfluences.clear();
    influences.clear();
	secondInfluences.clear();

    component = -1;
    assigned = false;

    confidenceLevel = 0;
    itPass =-1;

    // TO DEBUG
    //vertexContainer;
    //Vector3f color;
    color = Eigen::Vector3f(0,0,0);

    // No tocaria
    //vector<double> embedding;

    ownerLabel = -1;
    ownerWeight = 0;
    confidenceLevel = 0;

    segmentId = -1;
	segmentDistance = 99999;

    domain = 0.0;
    domainId = -1;

    component = -1;
    assigned = false;
    validated = false;
}

void PointData::clearAll()
{
    clear();
    embedding.clear();
}

/*
void PointData::SaveToFile(ofstream& myfile)
{
    // bool vertexContainer
    myfile.write((const char*) &vertexContainer, sizeof(bool));

    // vector<weight> influences
    vector<int> labels;
    vector<float> weights;

    labels.resize(influences.size());
    weights.resize(influences.size());
    for(int i = 0; i< influences.size(); i++)
    {
        labels[i] = influences[i].label;
        weights[i] = influences[i].weightValue;
    }

    // vector<float> weights
    int size = weights.size();
    myfile.write((const char*) &size, sizeof(unsigned int));
    if(size > 0)
        myfile.write((const char*) &weights[0], sizeof(float)*weights.size());

    // vector<int> labels
    size = labels.size();
    myfile.write((const char*) &size, sizeof(unsigned int));
    if(size > 0)
        myfile.write((const char*) &labels[0], sizeof(int)*labels.size());

    // vector<double> embedding
    size = embedding.size();
    myfile.write((const char*) &size, sizeof(unsigned int));
    if(size > 0)
        myfile.write((const char*) &embedding[0], sizeof(double)*embedding.size());

    //int segmentId;
    myfile.write((const char*) &segmentId, sizeof(int));

}

void PointData::LoadFromFile(ifstream& myfile)
{
    myfile.read( (char*) &vertexContainer, sizeof(bool) );

    int size;
    vector<int> al;
    vector<float> aw;

    // Weights
    myfile.read( (char*) &size, sizeof(int) );
    if(size > 0)
    {
        aw.resize(size);
        myfile.read( (char*) &aw[0], sizeof(float)*size);
    }
    else aw.clear();

    // labels
    myfile.read( (char*) &size, sizeof(int) );
    if(size > 0)
    {
        al.resize(size);
        myfile.read( (char*) &al[0], sizeof(int)*size);
    }
    else al.clear();

    if(size > 0)
    {
        influences.resize(size);
        for(int i = 0; i< influences.size(); i++)
        {
            influences[i].label = al[i];
            influences[i].weightValue = aw[i];
        }
    }
    else
        influences.clear();

    myfile.read( (char*) &size, sizeof(int) );
    if(size > 0)
    {
        embedding.resize(size);
        myfile.read( (char*) &embedding[0], sizeof(double)*size);
    }
    else embedding.clear();

    myfile.read( (char*) &segmentId, sizeof(int) );

}
*/

void cleanZeroInfluences(binding* bd)
{
	float err = 1/pow(10,5);
    for(int i = 0; i< bd->pointData.size();i++)
    {
        PointData* pd = &(bd->pointData[i]);

		vector<weight> tempInfluences(pd->influences.size());
		int elements = 0;

        // Eliminamos los pesos igual a 0 o negativos
        pd->auxInfluences.clear();
		
		double weightsSum = 0;
		int count = 0; 
        for(int infl = 0; infl < pd->influences.size(); infl++)
        {
            if(pd->influences[infl].weightValue > err)
			{
				tempInfluences[elements] = pd->influences[infl];
				elements++;
				weightsSum += pd->influences[infl].weightValue;
				count++;
			}

			else if(pd->influences[infl].weightValue < 0)
			{
				//Comentado temporalmente
				printf("[cleanZeroInfluences] - Pesos negativos...no deberia haber.\n");
				fflush(0);
			}
        }

		if(count > 0)
			weightsSum /=count;

		// Si todos los elementos estan bien, pasamos
		if (elements == pd->influences.size()) return;

        pd->influences.clear();
		pd->influences.resize(elements);
        
		for (int infl = 0; infl < elements; infl++)
			pd->influences[infl] = weight(tempInfluences[infl].label, tempInfluences[infl].weightValue / weightsSum);

		tempInfluences.clear();
    }
}

void PointData::saveToFile(FILE* fout)
{
	fprintf(fout, "%d %d %f\n", node->id, segmentId, segmentDistance); fflush(fout);

	//fprintf(fout, "%f %f %f\n", color.x(), color.y(), color.z()); fflush(0);
	//fprintf(fout, "%d \n", vertexContainer); fflush(0);

    // Segmentation labels
    // vector<double> embedding;

	// En principio no es necesario.
	// fprintf(fout, "%d %f %f %f\n", ownerLabel, ownerWeight, tempOwnerWeight, confidenceLevel); fflush(0);

    // Domain of influcence
    //float domain;
    //int domainId;

    // Iteration pass
    //int itPass;

	fprintf(fout, "%d %d %d %d\n", validated, component, assigned, isBorder); fflush(fout);

    // ConnexComponents
    
	// Reference to model vertex index
	//int modelVert;

	// Influences
	fprintf(fout, "%d ", influences.size()); fflush(fout);
	for(int inflIdx = 0; inflIdx < influences.size(); inflIdx++)
	{
		fprintf(fout, "%d %f ", influences[inflIdx].label, influences[inflIdx].weightValue); fflush(fout);
	}
	fprintf(fout, "\n"); fflush(fout);


	// SecondInfluences
	fprintf(fout, "%d ", secondInfluences.size()); fflush(fout);
	for(int inflIdx = 0; inflIdx < secondInfluences.size(); inflIdx++)
	{
		fprintf(fout, "%d ", secondInfluences[inflIdx].size());
		for(int inflSecIdx = 0; inflSecIdx < secondInfluences[inflIdx].size(); inflSecIdx++)
		{
			fprintf(fout, "%f ", secondInfluences[inflIdx][inflSecIdx]); fflush(fout);
		}
	}
	fprintf(fout, "\n"); fflush(fout);
}

float PointData::getDomain(int fatherId)
{
    for(unsigned int i = 0; i< influences.size(); i++)
    {
        if(influences[i].label == fatherId)
            return influences[i].weightValue;
    }

    return 0;
}

void PointData::loadFromFile(ifstream& in)
{
	int nodeReference = 0;
	string line;
	vector<string> elems;

	getline (in , line);
    split(line, ' ', elems);

	nodeReference = atoi(elems[0].c_str());
	segmentId = atoi(elems[1].c_str());
	segmentDistance = atof(elems[2].c_str());

	getline (in, line);
    split(line, ' ', elems);

	validated = atoi(elems[0].c_str());
	component = atoi(elems[1].c_str());
	assigned = atoi(elems[2].c_str());
	isBorder = atoi(elems[3].c_str());

	getline (in, line);
    split(line, ' ', elems);

	influences.resize(atoi(elems[0].c_str()));
	for(int inflIdx = 0; inflIdx < influences.size(); inflIdx++)
	{
		influences[inflIdx] = weight( atoi(elems[inflIdx*2+1].c_str()), atof(elems[(inflIdx+1)*2].c_str()));
	}

	//Secondary weights
	getline (in, line);
    split(line, ' ', elems);

	int id = 0;
	secondInfluences.resize(atoi(elems[id].c_str())); id++;
	for(int inflIdx = 0; inflIdx < secondInfluences.size(); inflIdx++)
	{
		secondInfluences[inflIdx].resize(atoi(elems[id].c_str())); id++;
		for(int inflSecIdx = 0; inflSecIdx < secondInfluences[inflIdx].size(); inflSecIdx++)
		{
			secondInfluences[inflIdx][inflSecIdx].alongBone = (float)atof(elems[id].c_str()); id++;
			//assert(false); // TODO: wideBone read
		}
	}
}

void loadBinding(binding* bd, string fileName, vector<skeleton*> skts)
{
	map<string, int> jointsMap;
	for(int sktIdx = 0; sktIdx < skts.size(); sktIdx++)
	{
		for(int jointIdx = 0; jointIdx < skts[sktIdx]->joints.size(); jointIdx++)
		{
			jointsMap[skts[sktIdx]->joints[jointIdx]->sName] = skts[sktIdx]->joints[jointIdx]->nodeId;
		}
	}
	std::printf("\nCargando binding%s\n",fileName.c_str());
	
	ifstream file;
	file.open(fileName.c_str(),  ios::in );

	for(int pt = 0; pt< bd->pointData.size(); pt++)
	{
		if(file.eof())
			break;

		string line;
		vector <string> elems;
		getline(file , line);
		split(line, ' ', elems);

		int in_nodeId = atoi(elems[0].c_str());

		assert(elems.size()%2 == 1);

		bd->pointData[in_nodeId].influences.resize((elems.size()-1)/2);

		int realSize = 0;

		for(int infl = 0; infl< bd->pointData[pt].influences.size(); infl++)
		{
			string nodeName = elems[infl*2+1];
			float weightValue = atof(elems[(infl+1)*2].c_str());

			if(weightValue > 0)
			{
				bd->pointData[in_nodeId].influences[realSize].label = jointsMap[nodeName];
				bd->pointData[in_nodeId].influences[realSize].weightValue = weightValue;
				realSize++;
			}

			//int idInfl = bd->pointData[pt].influences[infl].label;
			//float inflValue = bd->pointData[pt].influences[infl].weightValue;
		}

		bd->pointData[in_nodeId].influences.resize(realSize);

		getline(file , line);
		/*
		for(int infl = 0; infl< bd->pointData[pt].secondInfluences.size(); infl++)
		{
			fprintf(fout, "%d ", bd->pointData[pt].secondInfluences[infl].size());
			for(int child = 0; child < bd->pointData[pt].secondInfluences[infl].size(); child++)
			{
				fprintf(fout, "%f ", bd->pointData[pt].secondInfluences[infl][child]);
			}
		}
		*/
	}	
}

void binding::saveCompactBinding(string fileName, map<int, string>& deformersRelation)
{
	binding* bd = this;
	std::printf("\nGuardando %s\n",fileName.c_str());
	FILE* fout = fopen(fileName.c_str(), "w");

	for(int pt = 0; pt< bd->pointData.size(); pt++)
	{
		fprintf(fout, "%d", pt);

		for(int infl = 0; infl< bd->pointData[pt].influences.size(); infl++)
		{
			int idInfl = bd->pointData[pt].influences[infl].label;
			float inflValue = bd->pointData[pt].influences[infl].weightValue;
			string inflName = "dummy";
			
			inflName = deformersRelation[idInfl];
			fprintf(fout, " %s %f", inflName.c_str(), inflValue);
		}

		fprintf(fout, "\n"); fflush(fout);

		for(int infl = 0; infl< bd->pointData[pt].secondInfluences.size(); infl++)
		{
			fprintf(fout, "%d ", bd->pointData[pt].secondInfluences[infl].size());
			for(int child = 0; child < bd->pointData[pt].secondInfluences[infl].size(); child++)
			{
				fprintf(fout, "%f ", bd->pointData[pt].secondInfluences[infl][child]);
			}
		}

		fprintf(fout, "\n"); fflush(fout);
	}

	fclose(fout);
}

void normalizeWeightsByDomain(binding *bd, int surfIdx)
{

	int length = bd->surfaces[surfIdx].nodes.size();
	int numBlocks = 8;
	int valuesPerBlock = (int)ceil((float)length / (float)numBlocks);
	
	#pragma omp parallel for
	for (int idxBlock = 0; idxBlock < numBlocks; idxBlock++)
	{
		int iniBlock = idxBlock*valuesPerBlock;
		int finBlock = iniBlock + valuesPerBlock;

		for (int i = iniBlock; i < finBlock && i < length; i++)
		///for (int i = 0; i < length; i++)
		{
			PointData* pd = &(bd->pointData[bd->surfaces[surfIdx].nodes[i]->id]);

			float childGain = 0;
			for (int infl = 0; infl < pd->auxInfluences.size(); infl++)
			{
				// DEBUG info -> to comment
				float err = (1 / pow(10.0, 5.0));
				if (pd->auxInfluences[infl].weightValue < 0 || (pd->auxInfluences[infl].weightValue - 1.0)>err)
				{
					// Algo está pasando...
					printf("hay algun problema en la asignacion de pesos.\n");
				}

				childGain += pd->auxInfluences[infl].weightValue;
			}

			if (childGain == 0)
			{
				pd->auxInfluences.clear();
				continue;
			}

			if (pd->domain > childGain)
			{
				//printf("\n\nEn principio aqui no entra, porque el padre tambien juega.\n\n"); fflush(0);
				if (pd->domainId >= 0)
				{
					// Obtener la influencia del padre y quitar lo que tocaria.
					int fatherId = findWeight(pd->influences, pd->domainId);
					if (fatherId >= 0)
					{
						//float rest = pd->auxInfluences.size()-childGain+;
						pd->influences[fatherId].weightValue = pd->domain - childGain;

						for (int infl = 0; infl < pd->auxInfluences.size(); infl++)
						{
							int l = pd->auxInfluences[infl].label;
							float w = pd->auxInfluences[infl].weightValue;
							pd->influences.push_back(weight(l, w));
						}
					}
					else
					{
						printf("No hay padre. Si no padre, no hijo...jeje\n"); fflush(0);
						assert(false);
					}
				}
				else
				{
					// Si es caso base tendría que cubrir todo el domain
					assert(false);
				}
			}
			else
			{
				// Eliminamos el peso del padre, porque lo repartiremos.
				if (pd->domainId >= 0)
				{
					int fatherId = findWeight(pd->influences, pd->domainId);
					if (fatherId >= 0)
					{
						if (pd->influences[fatherId].weightValue != pd->domain)
						{
							assert(false);
							printf("Hay un problema de inicializacion del dominio...\n");
							fflush(0);
						}

						pd->influences[fatherId].weightValue = 0;
					}
				}

				for (int infl = 0; infl < pd->auxInfluences.size(); infl++)
				{
					int l = pd->auxInfluences[infl].label;
					float w = (pd->auxInfluences[infl].weightValue / childGain)*pd->domain;
					pd->influences.push_back(weight(l, w));
				}

			}

			pd->auxInfluences.clear();
		}

	}

	//clock_t fin = clock();
	//printf("Fin Normalizacion:%fms\n", ((double)(fin-ini)) / CLOCKS_PER_SEC * 1000);
}
