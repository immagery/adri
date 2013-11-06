#include "..\DataStructures\grid3D.h"
#include "..\DataStructures\DefNode.h"
#include "..\utils\util.h"

#include <DataStructures/Modelo.h>

#define DEBUG false

#include <iostream>
#include <fstream>

using namespace std;
using namespace vcg;

int findWeight(vector<weight>& weights, int label)
{
	for(unsigned int i = 0; i< weights.size(); i++)
	{
		if(weights[i].label == label)
			return i;
	}

	return -1;
}

//////////////////////////////////////
///////////// CELL DATA //////////////

cellData::cellData()
{
    influences.clear();
    auxInfluences.clear();

    embedding.clear();
    vertexContainer = false;
    pos = Eigen::Vector3i(-1,-1,-1);
    itPass =-1;

	confidenceLevel = 0;
    
	ownerLabel = -1;
	ownerWeight = 0;
	confidenceLevel = 0;

	distanceToPos = -1;

	segmentId = -1;

	domain = 0.0;
	domainId = -1;

	component = -1;
	assigned = false;
	validated = false;

	color = Eigen::Vector3f(0,0,0);
}

cellData::cellData(int weightsSize)
{
    if(weightsSize > 0)
        influences.resize(weightsSize);
    else
        influences.clear();

    vertexContainer = false;

    auxInfluences.clear();
    pos = Eigen::Vector3i(-1,-1,-1);
    itPass =-1;
    ownerLabel = -1;
	confidenceLevel = 0;

	component = -1;
	assigned = false;
	validated = false;

	color = Eigen::Vector3f(0,0,0);
}

// Clear no del todo, no quitamos el labeling de la voxelizacion
void cellData::clear()
{
    auxInfluences.clear();
    influences.clear();

	component = -1;
	assigned = false;

	confidenceLevel = 0;

    pos = Eigen::Vector3i(-1,-1,-1);
    itPass =-1;

    // Flags for better computation
    distanceToPos = -1;

	// TO DEBUG
	//vertexContainer;
	//Eigen::Vector3f color;
	color = Eigen::Vector3f(0,0,0);

	// No tocaria
    //vector<double> embedding;
	
	ownerLabel = -1;
	ownerWeight = 0;
	confidenceLevel = 0;

	segmentId = -1;

	domain = 0.0;
	domainId = -1;

	component = -1;
	assigned = false;
	validated = false;
}

void cellData::clearAll()
{
    clear();
    embedding.clear();
}

void cellData::SaveToFile(ofstream& myfile)
{  
    // bool vertexContainer
    myfile.write((const char*) &vertexContainer, sizeof(bool));

	// vector<weight> influences 
	vector<int> labels;
	vector<float> weights;

	labels.resize(influences.size());
	weights.resize(influences.size());
	for(unsigned int i = 0; i< influences.size(); i++)
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

void cellData::LoadFromFile(ifstream& myfile)
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
		for(unsigned int i = 0; i< influences.size(); i++)
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

/////////////////////////////////////
/////////////   CELL   //////////////

cell3d::cell3d()
{
    setType(UNTYPED);
    data = new cellData();
    frontier = 0;
}

cell3d::cell3d(bool noData)
{
    setType(UNTYPED);
    if(!noData) 
		data = new cellData();
	else
		data = NULL;

    frontier = 0;
}

cell3d::cell3d(int weightsSize)
{
    setType(UNTYPED);
    data = new cellData(weightsSize);
    frontier = 0;
}

cell3d::cell3d(T_cell tipo, int weightsSize)
{
    setType(tipo);
    data = new cellData(weightsSize);
    frontier = 0;
}

cell3d::~cell3d()
{
    data->clear();
    delete data;
    data = NULL;
}

/////////////////////////////////////
////////////   GRID   ///////////////

void grid3d::initBasicData()
{
	cells.clear();
	dimensions = Eigen::Vector3i(0,0,0);
	cellSize = 0;
	weightSize = 0;
	totalCellsCounts = 0;
	borderCellsCounts = 0;
	inCellsCounts = 0;
	outCellsCounts = 0;
	valueRange = 0;

	smoothPropagationRatio = 1;
	worldScale = 1;
	minConfidenceLevel = 0.5;

	kValue = 100;
	alphaValue = 9;

	tradIds.clear();
	bindedSkeletons.clear();

	res = 7;

	fPrintText = NULL;
	fPrintProcessStatus = NULL;
	//loadedFunctions = false;
}

grid3d::grid3d()
{
    initBasicData();
}

void grid3d::initwithNoData(Box3d bounding_, Eigen::Vector3i divisions)
{
	initBasicData();

    // comprobamos que las dimensiones tengan sentido
    assert(divisions.x() != 0 || divisions.y() != 0 || divisions.z() != 0);

    bool withData = false;
    // reservamos el espacio necesario para hacer los c√°lculos
    cells.resize(divisions.x());
    for(int i = 0; i< divisions.x(); i++)
    {
        cells[i].resize(divisions.y());
        for(int j = 0; j< divisions.y(); j++)
        {
            cells[i][j].resize(divisions.z());
            for(int k = 0; k< divisions.z(); k++)
            {
                //cells[i][j][k] = new cell3d(_weightsSize);
                cells[i][j][k] = new cell3d(withData);
            }
        }
    }

    dimensions = divisions;
    bounding = bounding_;

    double cell01 = bounding.DimX()/divisions.x();
    double cell02 = bounding.DimY()/divisions.y();
    double cell03 = bounding.DimZ()/divisions.z();

    // Comprobamos de las celdas son regulares.
    double error = 0.0000000001;
    assert(fabs(cell01-cell02)<error && fabs(cell03-cell02)<error);

    cellSize = bounding.DimX()/divisions.x();
}

Eigen::Vector3d grid3d::getCenterOfCell(int i, int j, int k)
{
	// TOFIX
	Eigen::Vector3d min(bounding.min.X(), bounding.min.Y(), bounding.min.Z());
	return min + Eigen::Vector3d(((float)i+0.5)*cellSize, ((float)j+0.5)*cellSize,((float)k+0.5)*cellSize);
}

void interpolateLinear(vector<weight>& result, vector<weight>& ptminWeights, vector<weight>& ptmaxWeights, float interpolationValue)
{
	int maxWeights = ptminWeights.size() + ptmaxWeights.size();

	vector<vector<float>> weightsAux;
	weightsAux.resize(maxWeights);
	map<int,int> redirection;

	int counter = 0;
	// Insertamos los pesos de una celda
	for(int wind = 0; wind < ptminWeights.size(); wind++)
	{
		weightsAux[counter].push_back(ptminWeights[wind].weightValue);
		redirection[ptminWeights[wind].label] = counter;
		counter++;
	}

	for(int wind = counter ; wind < weightsAux.size(); wind++)
	{
		weightsAux[wind].push_back(0);
	}

	// Insertamos los pesos de la otra celda
	for(int wind = 0; wind < ptmaxWeights.size(); wind++)
	{
		int label = ptmaxWeights[wind].label;
		float weightValue = ptmaxWeights[wind].weightValue;
		if(redirection.count(label) == 0)
		{
			redirection[label] = counter;
			weightsAux[counter].push_back(weightValue);
			counter++;
		}
		else
		{
			weightsAux[redirection[label]].push_back(weightValue);
		}
	}

	result.resize(counter);
	counter = 0;
	// Calculamos la media de los pesos
	for (std::map<int,int>::iterator it=redirection.begin(); it!=redirection.end(); ++it)
	{
		float weightValue = 0;
		int label = it->first;
		int idx = it->second;
		if(weightsAux[idx].size() == 1)
		{
			weightValue = weightsAux[idx][0]*(1-interpolationValue);
		}
		else if(weightsAux[idx].size() == 2)
		{
			weightValue = weightsAux[idx][0]*(1-interpolationValue) + weightsAux[idx][1]*interpolationValue;
		}

		result[counter] = weight(it->first, weightValue);
		counter++;
	}
}

void interpolateBiLinear(vector<weight>& result, 
						 vector<weight>& pt01, 
						 vector<weight>& pt02,
						 vector<weight>& pt03, 
						 vector<weight>& pt04,
						 float interpolationValue1,
						 float interpolationValue2)
{
		vector<weight> weights1;
		vector<weight> weights2;

		interpolateLinear(weights1, pt01, pt02, interpolationValue1);
		interpolateLinear(weights2, pt04, pt03, interpolationValue1);
		interpolateLinear(result, weights1, weights2, interpolationValue2);
}

bool grid3d::isOut(Eigen::Vector3i& pt)
{
	return (pt.x() < 0 || pt.y() < 0 || pt.z() < 0 ||
		    pt.x() >= dimensions.x() || pt.y() >=  dimensions.y()  || pt.z() >=  dimensions.z());

}

bool grid3d::hasData(Eigen::Vector3i& pt)
{
	return cells[pt.x()][pt.y()][pt.z()]->data != NULL;
}

void grid3d::copyValues(vector<weight>& weights, vector<weight>& weights_out)
{
	weights_out.resize(weights.size());
	//float sum = 0;
	for(int i = 0; i< weights_out.size(); i++)
	{
		weights_out[i] = weights[i];
		//sum += weights[i].weightValue;
	}

	//if (fabs(sum - 1) > 0.0005 )
	//	printf("There is a problem with that set of weights %f\n", sum); fflush(0);
}

void interpolateTriLinear(vector<weight>& result,
						  vector<vector<weight>* >& pts,
						 float interpolationValue1,
						 float interpolationValue2,
						 float interpolationValue3)
{
	vector<weight> weights1;
	vector<weight> weights2;
	interpolateBiLinear(weights1, 
						*pts[0],
						*pts[1], 
						*pts[2],
						*pts[3],
						interpolationValue1, 
						interpolationValue2);

	interpolateBiLinear(weights2, 
						*pts[4],
						*pts[5], 
						*pts[6],
						*pts[7],
						interpolationValue1, 
						interpolationValue2);

	interpolateLinear(  result, 
						weights1, 
						weights2, 
						interpolationValue3);
}

void grid3d::getCoordsFromPointSimple(Eigen::Vector3d& pt, vector<weight>& weights)
{
	weights.clear();

	Eigen::Vector3i originCell = cellId(pt);
	int i = originCell.x();
	int j = originCell.y();
	int k = originCell.z();

	if(i < 0 || j < 0 || k < 0 || i >= dimensions.x() || j >= dimensions.y() || k >= dimensions.z()) return;

	if(cells[i][j][k]->getType() != EXTERIOR)
	{
		if(cells[i][j][k]->data)
		{
			copyValues(cells[i][j][k]->data->influences, weights);
			return;
		}
	}
}

void grid3d::getCoordsFromPoint(Eigen::Vector3d& pt, vector<weight>& weights)
{
	weights.clear();

	Eigen::Vector3i originCell = cellId(pt);
	int i = originCell.x();
	int j = originCell.y();
	int k = originCell.z();

	if(i < 0 || j < 0 || k < 0 || i >= dimensions.x() || j >= dimensions.y() || k >= dimensions.z()) return;

	if(cells[i][j][k]->getType() != EXTERIOR)
	{
		if(cells[i][j][k]->data)
		{
			//copyValues(cells[i][j][k]->data->influences, weights);
			//return;

			Eigen::Vector3d c = getCenterOfCell(i,j,k);
			// Obtenemos el cuadrante en que cae el punto.
			Eigen::Vector3d dpt = pt - c;

			Eigen::Vector3d ptmin, ptmax;
			Eigen::Vector3i ptminIdx, ptmaxIdx;
			for(int comp = 0; comp < 3; comp++)
			{
				if(dpt[comp]>=0)
				{
					ptmin[comp] = c[comp];
					ptmax[comp] = c[comp]+cellSize;

					ptminIdx[comp] = originCell[comp];
					ptmaxIdx[comp] = originCell[comp]+1;
				}
				else
				{
					ptmax[comp] = c[comp];
					ptmin[comp] = c[comp]-cellSize;

					ptmaxIdx[comp] = originCell[comp];
					ptminIdx[comp] = originCell[comp]-1;
				}
			}


			if(isOut(ptmaxIdx) || isOut(ptminIdx))
			{
				if(isOut(ptmaxIdx))
				{
					if(!isOut(ptminIdx))
					{
						if(hasData(ptminIdx))
							copyValues(cells[ptminIdx[0]][ptminIdx[1]][ptminIdx[2]]->data->influences, weights);
					}
					else
					{
						// Buscar una celda cerca
					}
					return;
				}
				else
				{
					if(isOut(ptminIdx))
					{
						if(hasData(ptmaxIdx))
							copyValues(cells[ptmaxIdx[0]][ptmaxIdx[1]][ptmaxIdx[2]]->data->influences, weights);
					}
					else
					{
						// Buscar una celda cerca
					}
					return;
				}
			}

			if(!hasData(ptminIdx) || !hasData(ptmaxIdx))
				return;

			float margen = 0.00001;
			int maxCoincidences = 0;
			int minCoincidences = 0;
			bool coincidences[6];
			for(int comp = 0; comp < 3; comp++)
			{
				coincidences[comp] = false;
				coincidences[comp+3] = false; 
				if(fabs(ptmin[comp] - pt[comp]) < margen)
				{
					minCoincidences++;
					coincidences[comp] = true;
				}

				if(fabs(ptmax[comp] - pt[comp]) < margen)
				{
					maxCoincidences++;
					coincidences[comp+3] = true;
				}
			}

			// No puede ser mas.
			assert(minCoincidences+maxCoincidences <= 3);

			int coin= minCoincidences+maxCoincidences;
			if(coin == 0)
			{
				// dentro
				Eigen::Vector3i CoordPt1, CoordPt2;
				int interpCoord1 = -1;
				int interpCoord2 = -1;

				vector<Eigen::Vector3i> facePoints1(4); // Cara 1
				facePoints1[0] = Eigen::Vector3i(ptminIdx[0], ptminIdx[1], ptminIdx[2]);
				facePoints1[1] = Eigen::Vector3i(ptmaxIdx[0], ptminIdx[1], ptminIdx[2]);
				facePoints1[2] = Eigen::Vector3i(ptmaxIdx[0], ptmaxIdx[1], ptminIdx[2]);
				facePoints1[3] = Eigen::Vector3i(ptminIdx[0], ptmaxIdx[1], ptminIdx[2]);

				vector<Eigen::Vector3i> facePoints2(4); // Cara 2
				facePoints2[0] = Eigen::Vector3i(ptminIdx[0], ptminIdx[1], ptmaxIdx[2]);
				facePoints2[1] = Eigen::Vector3i(ptmaxIdx[0], ptminIdx[1], ptmaxIdx[2]);
				facePoints2[2] = Eigen::Vector3i(ptmaxIdx[0], ptmaxIdx[1], ptmaxIdx[2]);
				facePoints2[3] = Eigen::Vector3i(ptminIdx[0], ptmaxIdx[1], ptmaxIdx[2]);
				
				vector<float> interpolationValues(3);
				
				for(int iCoord = 0; iCoord < 3; iCoord++)
					interpolationValues[iCoord] = (pt[iCoord]-ptmin[iCoord])/(ptmax[iCoord]-ptmin[iCoord]);

				vector< vector<weight>* > AllWeights;
				AllWeights.resize(8);
				for(int ptIdx = 0; ptIdx < 4; ptIdx++)
				{
					AllWeights[ptIdx] = &(cells[facePoints1[ptIdx][0]][facePoints1[ptIdx][1]][facePoints1[ptIdx][2]]->data->influences);
					AllWeights[ptIdx+4] = &(cells[facePoints2[ptIdx][0]][facePoints2[ptIdx][1]][facePoints2[ptIdx][2]]->data->influences);
				}

				interpolateTriLinear(weights, AllWeights,
									interpolationValues[1],
									interpolationValues[0],
									interpolationValues[2]);
				/*
				vector<weight> weights1;
				vector<weight> weights2;
				interpolateBiLinear(weights1, 
									*AllWeights[0],*AllWeights[1],*AllWeights[2], *AllWeights[3],
									interpolationValues[0], interpolationValues[1]);

				interpolateBiLinear(weights2, 
									*AllWeights[4],*AllWeights[5],*AllWeights[6], *AllWeights[7],
									interpolationValues[0], interpolationValues[1]);

				interpolateLinear(weights, weights1, weights2, interpolationValues[2]);*/


			}
			else if(coin == 1)
			{
				// cara
				Eigen::Vector3i CoordPt1, CoordPt2;
				int interpCoord1 = -1;
				int interpCoord2 = -1;

				for(int comp = 0; comp< 3; comp++)
				{
					if(coincidences[comp])
					{
						CoordPt1[comp] = ptmaxIdx[comp];
						CoordPt2[comp] = ptmaxIdx[comp];
					}
					else if(coincidences[comp+3])
					{
						CoordPt1[comp] = ptminIdx[comp];
						CoordPt2[comp] = ptminIdx[comp];
					}
					else
					{
						CoordPt1[comp] = ptminIdx[comp];
						CoordPt2[comp] = ptmaxIdx[comp];
						
						if(interpCoord1 < 0)
						{
							interpCoord1 = comp;
						}
						else
						{
							interpCoord2 = comp;
						}
					}
				}

				vector<Eigen::Vector3i> facePoints(4);
				facePoints[0] = CoordPt1;
				facePoints[1] = CoordPt1;
				facePoints[2] = CoordPt2;
				facePoints[3] = CoordPt2;

				facePoints[1][interpCoord1] = CoordPt2[interpCoord1];
				facePoints[2][interpCoord1] = CoordPt1[interpCoord1];
				
				float interpolationValue1 = (pt[interpCoord1]-ptmin[interpCoord1])/(ptmax[interpCoord1]-ptmin[interpCoord1]);
				float interpolationValue2 = (pt[interpCoord2]-ptmin[interpCoord2])/(ptmax[interpCoord2]-ptmin[interpCoord2]);

				interpolateBiLinear(weights, 
									cells[facePoints[0][0]][facePoints[0][1]][facePoints[0][2]]->data->influences,
									cells[facePoints[1][0]][facePoints[1][1]][facePoints[1][2]]->data->influences,
									cells[facePoints[2][0]][facePoints[2][1]][facePoints[2][2]]->data->influences, 
									cells[facePoints[3][0]][facePoints[3][1]][facePoints[3][2]]->data->influences,
									interpolationValue1, interpolationValue2);

			}
			else if(coin == 2)
			{
				// arista
				Eigen::Vector3i CoordPt1;
				Eigen::Vector3i CoordPt2;
				int interpCoord = -1;
				for(int comp = 0; comp< 3; comp++)
				{
					if(coincidences[comp])
					{
						CoordPt1[comp] = ptmaxIdx[comp];
						CoordPt2[comp] = ptmaxIdx[comp];
					}
					else if(coincidences[comp+3])
					{
						CoordPt1[comp] = ptminIdx[comp];
						CoordPt2[comp] = ptminIdx[comp];
					}
					else
					{
						CoordPt1[comp] = ptminIdx[comp];
						CoordPt2[comp] = ptmaxIdx[comp];
						interpCoord = comp;
					}
				}
					
				float interpolationValue = (pt[interpCoord]-ptmin[interpCoord])/(ptmax[interpCoord]-ptmin[interpCoord]);	

				interpolateLinear(weights, cells[CoordPt1[0]][CoordPt1[1]][CoordPt1[2]]->data->influences, 
										   cells[CoordPt2[0]][CoordPt2[1]][CoordPt2[2]]->data->influences,
										   interpolationValue);
				
			}
				
			else if(coin == 3)
			{

					Eigen::Vector3i CoordPt;
					for(int comp = 0; comp< 3; comp++)
					{
						if(coincidences[comp])
							CoordPt[comp] = ptmaxIdx[comp];
						else
							CoordPt[comp] = ptminIdx[comp];
					}
					
					weights.resize(cells[CoordPt[0]][CoordPt[1]][CoordPt[2]]->data->influences.size());
					for(int wind = 0; wind < weights.size(); wind++)
						weights[wind] = cells[CoordPt[0]][CoordPt[1]][CoordPt[2]]->data->influences[wind];

			}
			else
			{
					// No puede ser que haya menos de 0 coincidencias, ni mas de 4.
			}



		}
		else
		{
			printf("Este punto cae en algun sitio curioso\n");
			if(cells[i][j][k]->getType() == BOUNDARY)
			{
				printf("Es borde...\n");
			}
			else
			{
				printf("??? algo esta pasando...\n");
			}
			fflush(0);
		}
	}
}

void grid3d::init(Box3d bounding_, Eigen::Vector3i divisions, int _weightsSize)
{
	initBasicData();

    // comprobamos que las dimensiones tengan sentido
    assert(divisions.x() != 0 || divisions.y() != 0 || divisions.z() != 0);

    // reservamos el espacio necesario para hacer los c√°lculos
    cells.resize(divisions.x());
    for(int i = 0; i< divisions.x(); i++)
    {
        cells[i].resize(divisions.y());
        for(int j = 0; j< divisions.y(); j++)
        {
            cells[i][j].resize(divisions.z());
            for(int k = 0; k< divisions.z(); k++)
            {
                //cells[i][j][k] = new cell3d(_weightsSize);
                cells[i][j][k] = new cell3d(true);
            }
        }
    }

    dimensions = divisions;
    bounding = bounding_;

    double cell01 = bounding.DimX()/divisions.x();
    double cell02 = bounding.DimY()/divisions.y();
    double cell03 = bounding.DimZ()/divisions.z();

    // Comprobamos de las celdas son regulares.
    double error = 0.0000000001;
    assert(fabs(cell01-cell02)<error && fabs(cell03-cell02)<error);

    cellSize = bounding.DimX()/divisions.x();
}

grid3d::grid3d(Box3d bounding_, Eigen::Vector3i divisions, int _weightsSize)
{
    // comprobamos que las dimensiones tengan sentido
    assert(divisions.x() != 0 || divisions.y() != 0 || divisions.z() != 0);

    // reservamos el espacio necesario para hacer los c√°lculos
    cells.resize(divisions.x());
    for(int i = 0; i< divisions.x(); i++)
    {
        cells[i].resize(divisions.y());
        for(int j = 0; j< divisions.y(); j++)
        {
            cells[i][j].resize(divisions.z());
            for(int k = 0; k< divisions.z(); k++)
            {
                //cells[i][j][k] = new cell3d(_weightsSize);
                cells[i][j][k] = new cell3d();
            }
        }
    }

    weightSize = 0;
    //weightSize = _weightsSize;

    /*
    for(int i = 0; i< cells.size(); i++)
    {
        for(int j = 0; j<  cells[i].size(); j++)
        {
            for(int k = 0; k< cells[i][j].size(); k++)
            {
                assert(cells[i][j][k]->tipo == UNTYPED);
            }
        }
    }
    */

    dimensions = divisions;
    bounding = bounding_;

    double cell01 = bounding.DimX()/divisions.x();
    double cell02 = bounding.DimY()/divisions.y();
    double cell03 = bounding.DimZ()/divisions.z();

    // Comprobamos de las celdas son regulares.
    assert(cell01 == cell02 && cell02 == cell03);

    cellSize = bounding.DimX()/divisions.x();
}

void grid3d::updateStatistics()
{
    totalCellsCounts = 0;
    borderCellsCounts = 0;
    inCellsCounts = 0;
    outCellsCounts = 0;

    int untyped= 0;

    for(unsigned int i = 0; i< cells.size(); i++)
    {
        for(unsigned int j = 0; j< cells[i].size(); j++)
        {
            for(unsigned int k = 0; k< cells[i][j].size(); k++)
            {
                switch(cells[i][j][k]->getType())
                {
                case BOUNDARY:
                    borderCellsCounts++;
                    break;
                case INTERIOR:
                    inCellsCounts++;
                    break;
                case EXTERIOR:
                    outCellsCounts++;
                    break;
                default:
                    untyped++;
                    break;
                }
                totalCellsCounts++;
            }
        }
    }
}

void grid3d::clear()
{
    // Celdas del grid
    for(unsigned int i = 0; i< cells.size(); i++)
    {
        for(unsigned int j = 0; j< cells[i].size(); j++)
        {
            for(unsigned int k = 0; k< cells[i][j].size(); k++)
            {
                delete cells[i][j][k];
                cells[i][j][k] = NULL;
            }
            cells[i][j].clear();
        }
        cells[i].clear();
    }
    cells.clear();

    dimensions = Eigen::Vector3i(0,0,0);
    bounding.SetNull();
    weightSize = 0;
    cellSize = 0;
}

int grid3d::fillFromCorners()
{
    vector<Eigen::Vector3i> listForProcess;

    int cellsCount = 0;

    // Encolamos los 8 extremos.
    listForProcess.push_back(Eigen::Vector3i(0,0,0));
    listForProcess.push_back(Eigen::Vector3i(0,dimensions.y()-1,0));
    listForProcess.push_back(Eigen::Vector3i(0,0,dimensions.z()-1));
    listForProcess.push_back(Eigen::Vector3i(0,dimensions.y()-1,dimensions.z()-1));
    listForProcess.push_back(Eigen::Vector3i(dimensions.x()-1,0,0));
    listForProcess.push_back(Eigen::Vector3i(dimensions.x()-1,dimensions.y()-1,0));
    listForProcess.push_back(Eigen::Vector3i(dimensions.x()-1,0,dimensions.z()-1));
    listForProcess.push_back(Eigen::Vector3i(dimensions.x()-1,dimensions.y()-1,dimensions.z()-1));

    // Los posibles vecinos de expansion
    vector<Eigen::Vector3i> neighbours;
    neighbours.push_back(Eigen::Vector3i(0,0,-1));
    neighbours.push_back(Eigen::Vector3i(0,0,1));
    neighbours.push_back(Eigen::Vector3i(0,-1,0));
    neighbours.push_back(Eigen::Vector3i(0,1,0));
    neighbours.push_back(Eigen::Vector3i(-1,0,0));
    neighbours.push_back(Eigen::Vector3i(1,0,0));

    // Hasta que se vacie la cola recorremos los cubos vecinos.
    while(!listForProcess.empty())
    {
        Eigen::Vector3i pos = listForProcess.back(); listForProcess.pop_back();

        if(cells[pos.x()][pos.y()][pos.z()]->getType() == UNTYPED)
        {
            cells[pos.x()][pos.y()][pos.z()]->setType(EXTERIOR);
            cellsCount++;
        }
        else
            continue;

        for(unsigned int l = 0; l< neighbours.size(); l++)
        {
            int i = neighbours[l].x();
            int j = neighbours[l].y();
            int k = neighbours[l].z();

            if(pos.x() + i >= 0 && pos.y() + j >= 0 && pos.z() + k >= 0 &&
               pos.x() + i < dimensions.x() && pos.y() + j < dimensions.y() && pos.z() + k  < dimensions.z())
            {
                if(cells[pos.x() + i][pos.y() + j][pos.z() + k]->getType() == UNTYPED)
                    listForProcess.push_back(Eigen::Vector3i(pos.x() + i,pos.y() + j,pos.z() + k));
            }
        }
    }

    return cellsCount;
}

int grid3d::fillInside()
{
    int cellsCount = 0;
    for(int i = 0; i< dimensions.x(); i++)
    {
        for(int j = 0; j< dimensions.y(); j++)
        {
            for(int k = 0; k< dimensions.z(); k++)
            {
                if(cells[i][j][k]->getType() == UNTYPED)
                {
                    cells[i][j][k]->setType(INTERIOR);
					cells[i][j][k]->data = new cellData();
                    cellsCount++;
                }
            }
        }
    }

    return cellsCount;
}

#define TERMINATE_CRITERION 0.0001


void grid3d::cleanZeroInfluences()
{
	for(int i = 0; i< dimensions.x(); i++)
	{
		for(int j = 0; j< dimensions.y(); j++)
		{
			for(int k = 0; k< dimensions.z(); k++)
			{
				if(cells[i][j][k]->getType() != BOUNDARY) continue;

				cellData* cellD = cells[i][j][k]->data;
					
				// Eliminamos los pesos igual a 0
				cellD->auxInfluences.clear();
				for(unsigned int infl = 0; infl < cellD->influences.size(); infl++)
				{
					if(cellD->influences[infl].weightValue != 0)
						cellD->auxInfluences.push_back(cellD->influences[infl]);
				}

				cellD->influences.clear();
				for(unsigned int infl = 0; infl < cellD->auxInfluences.size(); infl++)
				{
					cellD->influences.push_back(cellD->auxInfluences[infl]);
				}
			}
		}
	}

}

// Importante: tiene que estar bien inicializado el dominio
// y los vectores de influencias auxiliares.
void grid3d::normalizeWeightsByDomain()
{
	for(int i = 0; i< dimensions.x(); i++)
    {
        for(int j = 0; j< dimensions.y(); j++)
        {
            for(int k = 0; k< dimensions.z(); k++)
            {
				if(cells[i][j][k]->getType() != BOUNDARY) 
					continue;

				cellData* cellD = cells[i][j][k]->data;

				float childGain = 0;
				for(unsigned int infl = 0; infl< cellD->auxInfluences.size(); infl++)
				{

					if(cellD->auxInfluences[infl].weightValue < 0 ||
						cellD->auxInfluences[infl].weightValue > 1)
						printf("hay algun problema en la asignacion de pesos.\n");
					childGain += cellD->auxInfluences[infl].weightValue;
				}

				if(childGain == 0) 
				{
					cellD->auxInfluences.clear();
					continue;
				}

				if(cellD->domain > childGain)
				{
					//printf("\n\nEn principio aquÌ no entra, porque el padre tambiÈn juega.\n\n"); fflush(0);
					if(cellD->domainId >= 0)
					{						
						// Obtener la influencia del padre y quitar lo que tocarÌa.
						int fatherId = findWeight(cellD->influences, cellD->domainId);
						if(fatherId >= 0)
						{
							cellD->influences[fatherId].weightValue = cellD->domain - childGain;
						}
					}

					for(unsigned int infl = 0; infl< cellD->auxInfluences.size(); infl++)
					{
						int l = cellD->auxInfluences[infl].label;
						float w = cellD->auxInfluences[infl].weightValue;
						cellD->influences.push_back(weight(l,w));
					}					
				}
				else
				{
					// Eliminamos el peso del padre, porque lo repartiremos.
					if(cellD->domainId >= 0)
					{				
						int fatherId = findWeight(cellD->influences, cellD->domainId);
						if(fatherId >= 0)
						{
							if(cellD->influences[fatherId].weightValue != cellD->domain)
							{
								printf("Hay un problema de inicializacion del dominio...\n");
								fflush(0);
							}
							
							cellD->influences[fatherId].weightValue = 0;

							// Quitamos la influencia del padre
							//vector<weight> tempWeights;
							//for(int tw = 0; tw < cellD->influences.size(); tw++)
							//{
							//	if(tw == fatherId) continue;
							//	tempWeights.push_back(cellD->influences[tw]);
							//}
							//cellD->influences.clear();

							//for(int tw = 0; tw < tempWeights.size(); tw++)
							//{
							//	cellD->influences.push_back(tempWeights[tw]);
							//}
						}
					}

					for(unsigned int infl = 0; infl< cellD->auxInfluences.size(); infl++)
					{
						int l = cellD->auxInfluences[infl].label;
						float w = (cellD->auxInfluences[infl].weightValue/childGain)*cellD->domain;

						float a = w;
						if(a != w)
						{
							int problema = 1;
							printf("Tenemos un problema curioso.\n");
						}

						cellD->influences.push_back(weight(l,w));
					}	
				}

				cellD->auxInfluences.clear();
            }
        }
    }
}


void grid3d::normalizeWeightsOptimized()
{
	int counterTotalCells = 0;
	int counterZeroCells = 0;
	float maxValue = -9999, minValue = 9999;
	for(int i = 0; i< dimensions.x(); i++)
	{
		for(int j = 0; j< dimensions.y(); j++)
		{
			for(int k = 0; k< dimensions.z(); k++)
			{
				if(cells[i][j][k]->getType() != EXTERIOR)
				{
					if(cells[i][j][k]->getType() == INTERIOR)
						counterTotalCells++;

					// normalizamos tambiÈn lo vertices.
					//if(cells[i][j][k]->data->vertexContainer)
					//	continue;

					double sum = 0;
					for(int w = 0; w < cells[i][j][k]->data->influences.size(); w++)
					{
						sum += cells[i][j][k]->data->influences[w].weightValue;
						maxValue = max(cells[i][j][k]->data->influences[w].weightValue, maxValue);
						minValue = min(cells[i][j][k]->data->influences[w].weightValue, minValue);
					}

					if(sum > 0)
					{
						for(int w = 0; w < cells[i][j][k]->data->influences.size(); w++)
						{
							cells[i][j][k]->data->influences[w].weightValue /= sum;
						}
					}
					else
					{
						counterZeroCells++;
						// Limpiamos, es posible que se haya quedado en cero?
						if(cells[i][j][k]->data->influences.size() > 0)
						{
							if(cells[i][j][k]->getType() == INTERIOR)
								counterZeroCells++;
							printf("Hay un problema con este peso... es igual a cero la suma.\n"); fflush(0);
							cells[i][j][k]->data->influences.clear();
						}
					}
				}
			}
		}
	}

	printf("Total: %d; Zero: %d\n", counterTotalCells, counterZeroCells); fflush(0);
	printf("MinValue: %f; MaxValue: %f\n", minValue, maxValue); fflush(0);
}

void grid3d::normalizeWeights()
{
	/*
    if(DEBUG)
    {
        float maxValue = -9999, minValue = 9999;
        for(int i = 0; i< dimensions.x(); i++)
        {
            for(int j = 0; j< dimensions.y(); j++)
            {
                for(int k = 0; k< dimensions.z(); k++)
                {
                    if(cells[i][j][k]->getType() == INTERIOR)
                    {
                        if(cells[i][j][k]->data->vertexContainer)
                            continue;

                        for(int w = 0; w < weightSize; w++)
                        {
                            maxValue = max(cells[i][j][k]->data->weights[w], maxValue);
                            minValue = min(cells[i][j][k]->data->weights[w], minValue);
                        }
                    }
                }
            }
        }

        printf("MinValue: %f; MaxValue: %f\n", minValue, maxValue); fflush(0);
    }

    for(int i = 0; i< dimensions.x(); i++)
    {
        for(int j = 0; j< dimensions.y(); j++)
        {
            for(int k = 0; k< dimensions.z(); k++)
            {
                if(cells[i][j][k]->getType() == INTERIOR)
                {
                    if(cells[i][j][k]->data->vertexContainer)
                        continue;

                    double totalValue = 0;
                    for(int w = 0; w < weightSize; w++)
                    {
                        totalValue += cells[i][j][k]->data->weights[w];
                    }

                    if(totalValue != 0)
                    {
                        for(int w = 0; w < weightSize; w++)
                        {
                            cells[i][j][k]->data->weights[w] = cells[i][j][k]->data->weights[w]/totalValue;
                        }
                    }
                }
            }
        }
    }

    if(DEBUG)
    {
        float maxValue = -9999, minValue = 9999;
        for(int i = 0; i< dimensions.x(); i++)
        {
            for(int j = 0; j< dimensions.y(); j++)
            {
                for(int k = 0; k< dimensions.z(); k++)
                {
                    if(cells[i][j][k]->getType() == INTERIOR)
                    {
                        if(cells[i][j][k]->data->vertexContainer)
                            continue;

                        for(int w = 0; w < weightSize; w++)
                        {
                            maxValue = max(cells[i][j][k]->data->weights[w], maxValue);
                            minValue = min(cells[i][j][k]->data->weights[w], minValue);
                        }
                    }
                }
            }
        }

        printf("MinValue: %f; MaxValue: %f; weightSize:%d\n", minValue, maxValue, weightSize); fflush(0);
    }
	*/
}

void grid3d::expandWeightsOptimized(Modelo* m)
{
    // Inicializamos los posibles vecinos en un array para
    // Simplificar el codigo -> 6-conectado
    vector<Eigen::Vector3i> positions;
    positions.resize(6);
    positions[0] = Eigen::Vector3i(1,0,0);
    positions[1] = Eigen::Vector3i(-1,0,0);
    positions[2] = Eigen::Vector3i(0,1,0);
    positions[3] = Eigen::Vector3i(0,-1,0);
    positions[4] = Eigen::Vector3i(0,0,1);
    positions[5] = Eigen::Vector3i(0,0,-1);

    float iterationVariation = 9999;
    int iterations = 0;

    for(int i = 0; i< dimensions.x(); i++)
    {
        for(int j = 0; j< dimensions.y(); j++)
        {
            for(int k = 0; k< dimensions.z(); k++)
            {
                if(cells[i][j][k]->getType() != EXTERIOR )
                {
                    cells[i][j][k]->data->pos = Eigen::Vector3i(i,j,k);
                    cells[i][j][k]->changed = false;
					cells[i][j][k]->data->validated = false;
					cells[i][j][k]->data->tempOwnerWeight = 0.0;
					cells[i][j][k]->data->ownerWeight = 0.0;
                }
            }
        }
    }
	
    vector<bool> expandedWeight;
    vector<float> interationVariationArray;
    expandedWeight.resize(weightSize, false);
    interationVariationArray.resize(weightSize, 0);

	printf("\nIncializado\n");fflush(0);

	vector<cell3d*> stepProcessCells;
	vector<cell3d*> cellsToAdd;

	// Procesamos los pesos por separado, expandiendo seg˙n convenga.
	for(int weightIdx = 0; weightIdx < m->nodes.size(); weightIdx++)
	{
		printf("\nPeso %d\n", weightIdx);fflush(0);

		stepProcessCells.clear();
		cellsToAdd.clear();

		// Inicializamos
		//cell3d* currentCell = processCells[cellId(m->nodes[weightIdx]->position)];
		Eigen::Vector3i originCell = cellId(m->nodes[weightIdx]->position);
        int i = originCell.x();
        int j = originCell.y();
        int k = originCell.z();

        assert(cells[i][j][k]->getType() != EXTERIOR);

		cells[i][j][k]->data->ownerLabel = weightIdx;
		cells[i][j][k]->data->segmentId = weightIdx;
		cells[i][j][k]->data->validated = true;
		cells[i][j][k]->data->ownerWeight = 1.0;
		cells[i][j][k]->data->tempOwnerWeight = 1.0;

		// Anadimos los vecinos con peso 0
        for(unsigned int p = 0; p < positions.size(); p++)
        {
            int newI = i+positions[p].x();
            int newJ = j+positions[p].y();
            int newK = k+positions[p].z();

            // Comprobamos que no se salga del grid.
            if(newI <0 || newI >= dimensions.x()) continue;
            if(newJ <0 || newJ >= dimensions.y()) continue;
            if(newK <0 || newK >= dimensions.z()) continue;

            if(cells[newI][newJ][newK]->getType() == EXTERIOR) continue;

			if(!cells[newI][newJ][newK]->data->validated)
			{
				cells[newI][newJ][newK]->data->tempOwnerWeight = 0;
				cells[newI][newJ][newK]->data->ownerWeight = 0;
				cells[newI][newJ][newK]->data->validated = true;
				stepProcessCells.push_back(cells[newI][newJ][newK]);
			}
		}
		
		printf("\nExpansion\n"); fflush(0);
		iterationVariation = TERMINATE_CRITERION*2;
		int iterationsDone = 0;
		bool cellsAddedFlag = true;
		float finishValue = 0;
		while(iterationVariation >= TERMINATE_CRITERION && cellsAddedFlag)
		{
			float localVar = 0;
			for(unsigned int cellCount = 0; cellCount< stepProcessCells.size(); cellCount++)
			{
				cell3d* currentCell = stepProcessCells[cellCount];
				int nexti = currentCell->data->pos.x();
				int nextj = currentCell->data->pos.y();
				int nextk = currentCell->data->pos.z();

				float tempValue = 0;
				float inPos = 0;

				for(unsigned int p = 0; p < positions.size(); p++)
				{
					int newI = nexti+positions[p].x();
					int newJ = nextj+positions[p].y();
					int newK = nextk+positions[p].z();

					// Comprobamos que no se salga del grid.
					if(newI <0 || newI >= dimensions.x()) continue;
					if(newJ <0 || newJ >= dimensions.y()) continue;
					if(newK <0 || newK >= dimensions.z()) continue;

					if(cells[newI][newJ][newK]->getType() == EXTERIOR) continue;

					if(!cells[newI][newJ][newK]->data->validated)
					{
						cells[newI][newJ][newK]->data->tempOwnerWeight = 0;
						cells[newI][newJ][newK]->data->ownerWeight = 0;
						cells[newI][newJ][newK]->data->validated = true;
						cellsToAdd.push_back(cells[newI][newJ][newK]);
					}

					tempValue += cells[newI][newJ][newK]->data->ownerWeight;
                    inPos++;
				}

				if(inPos > 0)
                {
                    tempValue = tempValue / inPos;

                    // Si ha habido cambio...vemos cuanto.
                    if(tempValue > 0)
                    {
						cells[nexti][nextj][nextk]->data->tempOwnerWeight = tempValue;
						localVar = max(localVar, fabs(cells[nexti][nextj][nextk]->data->tempOwnerWeight - cells[nexti][nextj][nextk]->data->ownerWeight));
                    }
                }
			}

			for(unsigned int cellCount = 0; cellCount< stepProcessCells.size(); cellCount++)
			{
				cell3d* currentCell = stepProcessCells[cellCount];
				int nexti = currentCell->data->pos.x();
				int nextj = currentCell->data->pos.y();
				int nextk = currentCell->data->pos.z();

				float owner001 = cells[nexti][nextj][nextk]->data->ownerWeight;
				float owner002 = cells[nexti][nextj][nextk]->data->tempOwnerWeight;

				cells[nexti][nextj][nextk]->data->ownerWeight = cells[nexti][nextj][nextk]->data->tempOwnerWeight;
			}

			for(unsigned int cellCount = 0; cellCount< cellsToAdd.size(); cellCount++)
			{
				cell3d* currentCell = cellsToAdd[cellCount];
				int nexti = currentCell->data->pos.x();
				int nextj = currentCell->data->pos.y();
				int nextk = currentCell->data->pos.z();

				// No deberia ser necesario... pero lo hacemos.
				//cells[nexti][nextj][nextk]->data->ownerWeight = cells[nexti][nextj][nextk]->data->tempOwnerWeight;

				// Se podria hacer de golpe con un resize 
				stepProcessCells.push_back(cellsToAdd[cellCount]);
			}

			cellsAddedFlag = cellsToAdd.size() > 0;
			cellsToAdd.clear();

			iterationVariation = min(iterationVariation, localVar);
			finishValue = localVar;
			//printf("\n%d: Valor de iteracion:%f y acaba en %f\n",iterationsDone, localVar,TERMINATE_CRITERION );fflush(0);
			iterationsDone++;
		}

		printf("\nIteraciones: %d con valor: %f\n", iterationsDone, finishValue );fflush(0);
		printf("\nLimpieza\n" );fflush(0);

		double weightThreshold = 1/pow(10,4);

		int discarted = 0;
		for(unsigned int cellCount = 0; cellCount< stepProcessCells.size(); cellCount++)
		{
			cell3d* currentCell = stepProcessCells[cellCount];
			int nexti = currentCell->data->pos.x();
			int nextj = currentCell->data->pos.y();
			int nextk = currentCell->data->pos.z();

			if(cells[nexti][nextj][nextk]->data->ownerWeight > weightThreshold)
				cells[nexti][nextj][nextk]->data->influences.push_back(weight(weightIdx,cells[nexti][nextj][nextk]->data->ownerWeight));
			else
				discarted++;

			cells[nexti][nextj][nextk]->data->validated = false;
			cells[nexti][nextj][nextk]->data->tempOwnerWeight = 0;
			cells[nexti][nextj][nextk]->data->ownerWeight = 0;
		}
		
		cells[i][j][k]->data->ownerLabel = -1;
		cells[i][j][k]->data->segmentId = -1;
		cells[i][j][k]->data->validated = false;
		cells[i][j][k]->data->ownerWeight = 0.0;
		cells[i][j][k]->data->tempOwnerWeight = 0.0;
		cells[i][j][k]->data->influences.push_back(weight(weightIdx,1.0));

        iterations++;
    }
}

void grid3d::expandWeights()
{
	assert(false);
	// esto probablemente no fucnione porque no est· adaptado bien.

	/*
    // Inicializamos los posibles vecinos en un array para
    // Simplificar el codigo -> 6-conectado
    vector<Eigen::Vector3i> positions;
    positions.resize(6);
    positions[0] = Eigen::Vector3i(1,0,0);
    positions[1] = Eigen::Vector3i(-1,0,0);
    positions[2] = Eigen::Vector3i(0,1,0);
    positions[3] = Eigen::Vector3i(0,-1,0);
    positions[4] = Eigen::Vector3i(0,0,1);
    positions[5] = Eigen::Vector3i(0,0,-1);

    float iterationVariation = 9999;
    int iterations = 0;

    vector< cell3d* > processCells;
    for(int i = 0; i< dimensions.x(); i++)
    {
        for(int j = 0; j< dimensions.y(); j++)
        {
            for(int k = 0; k< dimensions.z(); k++)
            {
                if(cells[i][j][k]->getType() != EXTERIOR )
                {
                    processCells.push_back(cells[i][j][k]);
                    cells[i][j][k]->data->pos = Eigen::Vector3i(i,j,k);
                    cells[i][j][k]->changed = false;
                }
            }
        }
    }

    printf("\nProcesar con %d celdas interiores o borde\n", processCells.size());fflush(0);

    vector<bool> expandedWeight;
    vector<float> interationVariationArray;
    expandedWeight.resize(weightSize, false);
    interationVariationArray.resize(weightSize, 0);

    while(iterationVariation >= TERMINATE_CRITERION)
    {
        for(unsigned int cellCount = 0; cellCount< processCells.size(); cellCount++)
        {

            cell3d* currentCell = processCells[cellCount];
            int i = currentCell->data->pos.x();
            int j = currentCell->data->pos.y();
            int k = currentCell->data->pos.z();

            if(cells[i][j][k]->getType() == INTERIOR || cells[i][j][k]->getType() == BOUNDARY)
            {

                if(cells[i][j][k]->data->vertexContainer) continue;

                // limpiamos del anterior c√°lculo.
                cells[i][j][k]->data->auxInfluences.clear();
                cells[i][j][k]->data->auxInfluences.resize(cells[i][j][k]->data->influences.size());

                for(int w = 0; w < weightSize; w++)
                {
                    // Si ese peso ya se ha expandido suficiente, lo dejamos.
                    //if(expandedWeight[w]) continue;

                    // Iniciamos el valor para el calculo de la media.
                    float tempValue = 0;
                    //cells[i][j][k]->auxWeights[w] = 0;

                    float inPos = 0;
                    // Para cada uno de los vecinos 6-conectado.
                    for(unsigned int p = 0; p < positions.size(); p++)
                    {
                        int newI = i+positions[p].x();
                        int newJ = j+positions[p].y();
                        int newK = k+positions[p].z();

                        // Comprobamos que no se salga del grid.
                        if(newI <0 || newI >= dimensions.x()) continue;
                        if(newJ <0 || newJ >= dimensions.y()) continue;
                        if(newK <0 || newK >= dimensions.z()) continue;

                        if(cells[newI][newJ][newK]->getType() == EXTERIOR) continue;

                        // Acumulamos el valor de los vecinos.
						float v1 = cells[newI][newJ][newK]->data->influences[w].weightValue;
                        tempValue += v1;
                        inPos++;
                        //someChanged |= cells[newI][newJ][newK]->weights[w] > 0 || cells[i][j][k]->weights[w] > 0;
                    }

                    // Hacemos la media
                    if(inPos > 0)
                    {
                        tempValue = tempValue / inPos;

                        // Si ha habido cambio...vemos cuanto.
                        if(tempValue > 0)
                        {
							cells[i][j][k]->data->auxInfluences[w].weightValue = tempValue;
                        }
                    }
                }
            }
        }


        // Actualizamos los valores del gris con los que hemos calculado.
        int counter = 0;
        float meanVar = 0;

        for(unsigned int cellCount = 0; cellCount< processCells.size(); cellCount++)
        {
            cell3d* currentCell = processCells[cellCount];
            int i = currentCell->data->pos.x();
            int j = currentCell->data->pos.y();
            int k = currentCell->data->pos.z();

            if(cells[i][j][k]->getType() == INTERIOR || cells[i][j][k]->getType() == BOUNDARY)
            {

                if(cells[i][j][k]->data->vertexContainer)
                    continue;

                float diference = 0;
                for(int w = 0; w < weightSize; w++)
                {
                    //if(expandedWeight[w]) continue;

                    diference = fabs(cells[i][j][k]->data->auxWeights[w]- cells[i][j][k]->data->weights[w]);
                    interationVariationArray[w] += diference;
                    meanVar += diference;
					
                    if(cells[i][j][k]->data->auxWeights[w] > 0)
                        cells[i][j][k]->data->weights[w] =  cells[i][j][k]->data->auxWeights[w] ;
                }
                counter++;
            }
        }

        int expanded = 0;
        if(counter > 0)
        {
            for(int cc = 0; cc< weightSize; cc++)
            {
                if(expandedWeight[cc])
                {
                    expanded++;
                    //continue;
                }

                interationVariationArray[cc] = interationVariationArray[cc]/counter;

                if(interationVariationArray[cc] < TERMINATE_CRITERION) expandedWeight[cc] = true;
            }
            meanVar = meanVar/(counter*weightSize);
        }
        else
            printf("El contador está a cero... no ha habido nigún cambio.");

        // Nos quedamos con la media mayor. Se irá iterando hasta que todas las medias estén por debajo, pero no se modificará si ya lo está.
        //iterationVariation = 0;

       iterationVariation = min(meanVar, iterationVariation);

        printf("Iteracion: %d, con valor: %f (%d pesos expandidos).\n", iterations, iterationVariation, expanded);fflush(0);
        iterations++;
    }
	*/
}

int grid3d::typeCells(Modelo* mesh)
{
    // Comprobar que la caja contenedora de la maya est√° contenida dentro del grid.
    //assert(mesh.bbox.min > bounding.min);
    //assert(mesh.bbox.max < bounding.max);

    int TypedCells = 0;

    // recorremos todas las caras y las anadimos al grid -> BOUNDARIES
    //for(MyMesh::FaceIterator fi = mesh.face.begin(); fi!=mesh.face.end(); ++fi )
    //    TypedCells += typeCells(*fi);

	for(int i = 0; i< mesh->triangles.size(); i++ )
	{
        TypedCells += typeCells(mesh, i);
	}

    // Marcamos lo que es externo.
    fillFromCorners();

    // Marcamos lo que es interno.
    fillInside();

    return TypedCells;
}

/*
int grid3d::typeCells(MyMesh& mesh)
{
    // Comprobar que la caja contenedora de la maya est√° contenida dentro del grid.
    //assert(mesh.bbox.min > bounding.min);
    //assert(mesh.bbox.max < bounding.max);

    int TypedCells = 0;

    // recorremos todas las caras y las anadimos al grid -> BOUNDARIES
    for(MyMesh::FaceIterator fi = mesh.face.begin(); fi!=mesh.face.end(); ++fi )
        TypedCells += typeCells(*fi);

    // Marcamos lo que es externo.
    fillFromCorners();

    // Marcamos lo que es interno.
    fillInside();

    return TypedCells;
}*/

Eigen::Vector3i grid3d::cellId(Eigen::Vector3d pt)
{
	// TOFIX

    Eigen::Vector3d aux = pt - Eigen::Vector3d(bounding.min.X(), bounding.min.Y(), bounding.min.Z());
    int x = (int)floor(aux.x()/cellSize);
    int y = (int)floor(aux.y()/cellSize);
    int z = (int)floor(aux.z()/cellSize);
    return Eigen::Vector3i(x,y,z);
}

Eigen::Vector3d grid3d::cellCenter(int i,int j,int k)
{
	// TOFIX
    Eigen::Vector3d aux = Eigen::Vector3d(bounding.min.X(), bounding.min.Y(), bounding.min.Z());
    aux += Eigen::Vector3d(cellSize*(i+0.5),cellSize*(j+0.5),cellSize*(k+0.5));
    return aux;
}

int grid3d::typeCells(Modelo* m, int triIdx)
{
    // A. Anadimos los vertices.
    Eigen::Vector3i cell[3];
    //Box3i boundingCells;

    int boundaryCells = 0;

	GraphNodePolygon* tri = m->triangles[triIdx];
	vector<Eigen::Vector3d> points;
	points.resize(tri->verts.size());
	for(int i = 0; i< tri->verts.size(); i++)
		points[i] = tri->verts[i]->position;

	assert(tri->verts.size() == 3);

    // recogemos los vertices y los marcamos.
	for(int i = 0; i<points.size(); i++)
    {
		cell[i] = cellId(points[i]); // Obtenemos la celda en la que cae el v√©rtice
        //boundingCells.Add(cell[i]);

		int x = cell[i].x();
		int y = cell[i].y();
		int z = cell[i].z();

        if(cells[x][y][z]->getType() != BOUNDARY)
        {
            cells[x][y][z]->setType(BOUNDARY);
            boundaryCells++;
        }

		if(cells[x][y][z]->data == NULL)
			cells[x][y][z]->data = new cellData();

        cells[x][y][z]->data->vertexContainer = true;
        //cells[cell[i].x()][cell[i].y()][cell[i].z()]->weights[face.V(i)->IMark()] = 1.0;
    }

    // Ahora recorremos celda por celda y vamos pintando el contorno.

    float processCellSize = cellSize/3;

    // 3D Rendering de arestas
    // cellSize -> lado de cada cubo.
    // ponemos el valor para las arestas también.
    for(int k = 0; k<tri->verts.size(); k++)
    {
        Eigen::Vector3d v = ( points[(k+1)%3] - points[k] );

        //int idvert1 = face.V(k)->IMark();
        //int idvert2 = face.V((k+1)%3)->IMark();
        Eigen::Vector3d v1 = points[k];
        //Eigen::Vector3d v2 = face.V((k+1)%3)->P();
        //float edgeLength = (v2-v1).norm();

        int divisions = (int)floor(v.norm()/processCellSize);
        Eigen::Vector3d vDir = v/v.norm();
        for(int i = 0; i< divisions ; i++)
        {
            Eigen::Vector3d intPoint = vDir*i*processCellSize + v1;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
            {
				cell3d* cellT = cells[cell.x()][cell.y()][cell.z()];

                if(cellT->getType() != BOUNDARY)
                {
					cellT->setType(BOUNDARY);
					if(cellT->data == NULL) 
						cellT->data = new cellData();
                }
            }
            boundaryCells++;
        }
    }

    // buscamos la aresta mas larga.
    int largeIdx = 0;
	Eigen::Vector3d v = (points[(largeIdx+1)%3]-points[largeIdx]);
    float edgeLong = v.norm();
    for(int k = 1; k<3; k++)
    {
        v = (points[(k+1)%3]-points[k]);
        if(edgeLong < v.norm())
        {
            largeIdx = k;
            edgeLong = v.norm();
        }
    }

    Eigen::Vector3d v1 = Eigen::Vector3d(points[(largeIdx+1)%3] - points[largeIdx]);
    Eigen::Vector3d v2 = Eigen::Vector3d(points[(largeIdx+2)%3] - points[largeIdx]);
    Eigen::Vector3d v3 = Eigen::Vector3d(points[(largeIdx+2)%3] - points[(largeIdx+1)%3]);

    Eigen::Vector3d aux = v1.cross(v2);
    Eigen::Vector3d normal = aux.cross(v1);

    float v1Norm = (float)v1.norm();
    float v2Norm = (float)v2.norm();
    float v3Norm = (float)v3.norm();
    float normalNorm = (float)normal.norm();

    Eigen::Vector3d v1Dir = v1/v1Norm;
    Eigen::Vector3d v2Dir = v2/v2Norm;
    Eigen::Vector3d v3Dir = v3/v3Norm;
    Eigen::Vector3d normalDir = normal/normalNorm;

    // TOCHECK Eigen::Vector3d edgeCenter = v1Dir*(v1Dir*v2) + points[largeIdx];
	Eigen::Vector3d edgeCenter = v1Dir * (v1Dir.dot(v2)) + points[largeIdx];
    int div1 = (int)ceil((points[largeIdx]-edgeCenter).norm()/processCellSize);
    int div2 = (int)ceil((points[(largeIdx+1)%3]-edgeCenter).norm()/processCellSize);

    for(int i = 1; i< div1 ; i++) // Saltamos el 0 porque es el mismo vertice.
    {
        Eigen::Vector3d minPt = v1Dir*i*processCellSize + points[largeIdx];
        Eigen::Vector3d maxPt = v2Dir*((float)i/(float)div1)*v2Norm + points[largeIdx]; // Suponemos que al ser triangulo rectangulo mantiene proporciones.

        Eigen::Vector3d line = maxPt-minPt;
        int Ydivs = (int)floor(line.norm()/processCellSize);

        for(int j = 1; j< Ydivs ; j++) // Saltamos el 0 porque es el mismo v√©rtice.
        {
            Eigen::Vector3d intPoint = normalDir*j*processCellSize + minPt;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
            {
				if(cells[cell.x()][cell.y()][cell.z()]->data == NULL)
					cells[cell.x()][cell.y()][cell.z()]->data = new cellData();

                cells[cell.x()][cell.y()][cell.z()]->setType(BOUNDARY);
            }

            boundaryCells++;
        }
    }


    for(int i = 1; i< div2 ; i++) // Saltamos el 0 porque es el mismo vertice.
    {
        Eigen::Vector3d minPt = -v1Dir*i*processCellSize + points[(largeIdx+1)%3];
        Eigen::Vector3d maxPt = v3Dir*((float)i/(float)div2)*v3Norm + points[(largeIdx+1)%3]; // Suponemos que al ser tri√°ngulo rect√°ngulo mantiene proporciones.

        Eigen::Vector3d line = maxPt-minPt;
        int Ydivs = (int)floor(line.norm()/processCellSize);

        for(int j = 1; j< Ydivs ; j++) // Saltamos el 0 porque es el mismo v√©rtice.
        {
            Eigen::Vector3d intPoint = normalDir*j*processCellSize + minPt;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
			{
				if(cells[cell.x()][cell.y()][cell.z()]->data == NULL)
					cells[cell.x()][cell.y()][cell.z()]->data = new cellData();

                cells[cell.x()][cell.y()][cell.z()]->setType(BOUNDARY);
			}

            boundaryCells++;
        }
    }

    
    // Podemos ampliar el contorno para estabilizar más el cálculo.
    /*
	for(int i = 0; i< cells.size(); i++)
    {
        for(int j = 0; j< cells[i].size(); j++)
        {
            for(int k = 0; k< cells[i][j].size(); k++)
            {
                if(cells[i][j][k]->tipo == BOUNDARY && !cells[i][j][k]->changed)
                {

                    for(int l = -1; l< 2; l++)
                    {
                        for(int m = -1; m< 2; m++)
                        {
                            for(int n = -1; n< 2; n++)
                            {
                                if(l == 0 && m == 0 && n == 0) continue;


                                if(l+i < 0 || m+j < 0 || n+k < 0 || dimensions.x() <= l+i || dimensions.y() <= m+j || dimensions.z() <= n+k )  continue;

                                if(cells[i][j][k]->tipo == BOUNDARY) continue;

                                if(cells[i+l][j+m][k+n])
                                {
                                    cells[i+l][j+m][k+n]->tipo = BOUNDARY;
                                    cells[i][j][k]->changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    */

     return boundaryCells;
}

/*
int grid3d::typeCells(MyFace& face)
{
    // A. Anadimos los vertices.
    Eigen::Vector3i cell[3];
    //Box3i boundingCells;

    int boundaryCells = 0;

    // recogemos los vertices y los marcamos.
    for(int i = 0; i<3; i++)
    {
        cell[i] = cellId(face.P(i)); // Obtenemos la celda en la que cae el v√©rtice
        //boundingCells.Add(cell[i]);

        if(cells[cell[i].x()][cell[i].y()][cell[i].z()]->getType() != BOUNDARY)
        {
            cells[cell[i].x()][cell[i].y()][cell[i].z()]->setType(BOUNDARY);
            boundaryCells++;
        }

		if(cells[cell[i].x()][cell[i].y()][cell[i].z()]->data == NULL)
			cells[cell[i].x()][cell[i].y()][cell[i].z()]->data = new cellData();

        cells[cell[i].x()][cell[i].y()][cell[i].z()]->data->vertexContainer = true;
        //cells[cell[i].x()][cell[i].y()][cell[i].z()]->weights[face.V(i)->IMark()] = 1.0;
    }

    // Ahora recorremos celda por celda y vamos pintando el contorno.

    float processCellSize = cellSize/3;

    // 3D Rendering de arestas
    // cellSize -> lado de cada cubo.
    // ponemos el valor para las arestas también.
    for(int k = 0; k<3; k++)
    {
        Eigen::Vector3d v = (face.V((k+1)%3)->P()-face.V(k)->P());

        //int idvert1 = face.V(k)->IMark();
        //int idvert2 = face.V((k+1)%3)->IMark();
        Eigen::Vector3d v1 = face.V(k)->P();
        //Eigen::Vector3d v2 = face.V((k+1)%3)->P();
        //float edgeLength = (v2-v1).norm();

        int divisions = (int)floor(v.norm()/processCellSize);
        Eigen::Vector3d vDir = v/v.norm();
        for(int i = 0; i< divisions ; i++)
        {
            Eigen::Vector3d intPoint = vDir*i*processCellSize + v1;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
            {
				cell3d* cellT = cells[cell.x()][cell.y()][cell.z()];

                if(cellT->getType() != BOUNDARY)
                {
					cellT->setType(BOUNDARY);
					if(cellT->data == NULL) cellT->data = new cellData();
                }
            }
            boundaryCells++;
        }
    }

    // buscamos la aresta mas larga.
    int largeIdx = 0;
    Eigen::Vector3d v = (face.V((largeIdx+1)%3)->P()-face.V(largeIdx)->P());
    float edgeLong = v.norm();
    for(int k = 1; k<3; k++)
    {
        v = Eigen::Vector3d(face.V((k+1)%3)->P()-face.V(k)->P());
        if(edgeLong < v.norm())
        {
            largeIdx = k;
            edgeLong = v.norm();
        }
    }

    Eigen::Vector3d v1 = Eigen::Vector3d(face.V((largeIdx+1)%3)->P()-face.V(largeIdx)->P());
    Eigen::Vector3d v2 = Eigen::Vector3d(face.V((largeIdx+2)%3)->P()-face.V(largeIdx)->P());
    Eigen::Vector3d v3 = Eigen::Vector3d(face.V((largeIdx+2)%3)->P()-face.V((largeIdx+1)%3)->P());

    Eigen::Vector3d aux = v1.cross(v2); 
    Eigen::Vector3d normal = aux.cross(v1);

    float v1Norm = (float)v1.norm();
    float v2Norm = (float)v2.norm();
    float v3Norm = (float)v3.norm();
    float normalNorm = (float)normal.norm();

    Eigen::Vector3d v1Dir = v1/v1Norm;
    Eigen::Vector3d v2Dir = v2/v2Norm;
    Eigen::Vector3d v3Dir = v3/v3Norm;
    Eigen::Vector3d normalDir = normal/normalNorm;

    Eigen::Vector3d edgeCenter = v1Dir*(v1Dir*v2) + face.V(largeIdx)->P();
    int div1 = (int)ceil((face.V(largeIdx)->P()-edgeCenter).norm()/processCellSize);
    int div2 = (int)ceil((face.V((largeIdx+1)%3)->P()-edgeCenter).norm()/processCellSize);

    for(int i = 1; i< div1 ; i++) // Saltamos el 0 porque es el mismo vertice.
    {
        Eigen::Vector3d minPt = v1Dir*i*processCellSize + face.V(largeIdx)->P();
        Eigen::Vector3d maxPt = v2Dir*((float)i/(float)div1)*v2Norm + face.V(largeIdx)->P(); // Suponemos que al ser triangulo rectangulo mantiene proporciones.

        Eigen::Vector3d line = maxPt-minPt;
        int Ydivs = (int)floor(line.norm()/processCellSize);

        for(int j = 1; j< Ydivs ; j++) // Saltamos el 0 porque es el mismo v√©rtice.
        {
            Eigen::Vector3d intPoint = normalDir*j*processCellSize + minPt;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
            {
				if(cells[cell.x()][cell.y()][cell.z()]->data == NULL)
					cells[cell.x()][cell.y()][cell.z()]->data = new cellData();

                cells[cell.x()][cell.y()][cell.z()]->setType(BOUNDARY);
            }

            boundaryCells++;
        }
    }


    for(int i = 1; i< div2 ; i++) // Saltamos el 0 porque es el mismo vertice.
    {
        Eigen::Vector3d minPt = -v1Dir*i*processCellSize + face.V((largeIdx+1)%3)->P();
        Eigen::Vector3d maxPt = v3Dir*((float)i/(float)div2)*v3Norm + face.V((largeIdx+1)%3)->P(); // Suponemos que al ser tri√°ngulo rect√°ngulo mantiene proporciones.

        Eigen::Vector3d line = maxPt-minPt;
        int Ydivs = (int)floor(line.norm()/processCellSize);

        for(int j = 1; j< Ydivs ; j++) // Saltamos el 0 porque es el mismo v√©rtice.
        {
            Eigen::Vector3d intPoint = normalDir*j*processCellSize + minPt;
            Eigen::Vector3i cell = cellId(intPoint);

            if(dimensions.x() <= cell.x() || dimensions.y() <= cell.y() || dimensions.z() <= cell.z() ||
               0 > cell.x() || 0 > cell.y() || 0 > cell.z()      )
            {
                printf("Tenemos un punto fuera?? (%d, %d, %d)\n", cell.x(), cell.y(), cell.z());
            }
            else
			{
				if(cells[cell.x()][cell.y()][cell.z()]->data == NULL)
					cells[cell.x()][cell.y()][cell.z()]->data = new cellData();

                cells[cell.x()][cell.y()][cell.z()]->setType(BOUNDARY);
			}

            boundaryCells++;
        }
    }

    
    // Podemos ampliar el contorno para estabilizar más el cálculo.
    
	for(int i = 0; i< cells.size(); i++)
    {
        for(int j = 0; j< cells[i].size(); j++)
        {
            for(int k = 0; k< cells[i][j].size(); k++)
            {
                if(cells[i][j][k]->tipo == BOUNDARY && !cells[i][j][k]->changed)
                {

                    for(int l = -1; l< 2; l++)
                    {
                        for(int m = -1; m< 2; m++)
                        {
                            for(int n = -1; n< 2; n++)
                            {
                                if(l == 0 && m == 0 && n == 0) continue;


                                if(l+i < 0 || m+j < 0 || n+k < 0 || dimensions.x() <= l+i || dimensions.y() <= m+j || dimensions.z() <= n+k )  continue;

                                if(cells[i][j][k]->tipo == BOUNDARY) continue;

                                if(cells[i+l][j+m][k+n])
                                {
                                    cells[i+l][j+m][k+n]->tipo = BOUNDARY;
                                    cells[i][j][k]->changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    

     return boundaryCells;
}
*/

// Guardamos en binario el grid entero, para no tener que recalcular cada vez
void grid3d::LoadGridFromFile(string sFileName)
{
    ifstream myfile;
	const char* charFileName = sFileName.c_str();
    myfile.open(charFileName, ios::in |ios::binary);
    if (myfile.is_open())
    {
        /* DATOS A GUARDAR
        vector< vector< vector< cell3d* > > > cells;
        Eigen::Vector3i dimensions;
        Box3d bounding;
        int weightSize;
        float cellSize;
        */

        // Datos generales
        myfile.read((char*)  &res, sizeof(int) );
        myfile.read( (char*) &dimensions[0], sizeof(int)*3 );
        myfile.read( (char*) &cellSize, sizeof(float) );
        myfile.read( (char*) &weightSize, sizeof(int));

        Eigen::Vector3d minPt,maxPt ;
        myfile.read( (char*) &minPt, sizeof(double)*3);
        myfile.read( (char*) &maxPt, sizeof(double)*3);
		// TOFIX
		Point3d minPtt(minPt.x(), minPt.y(), minPt.z());
		Point3d maxPtt(maxPt.x(), maxPt.y(), maxPt.z());
        bounding = Box3d(minPtt, maxPtt);

        myfile.read( (char*) &valueRange, sizeof(float));

        cells.resize(dimensions[0]);

        //Datos de cada una de las celdas.
        for(unsigned int i = 0; i< cells.size(); i++)
        {
            cells[i].resize(dimensions[1]);
            for(unsigned int j = 0; j< cells[i].size(); j++)
            {
                cells[i][j].resize(dimensions[2]);
                for(unsigned int k = 0; k< cells[i][j].size(); k++)
                {
                    cells[i][j][k] = new cell3d(true);
                    cells[i][j][k]->LoadFromFile(myfile);
                }
            }
        }

    }
    myfile.close();

}

// Cargamos de disco el grid entero.
void grid3d::SaveGridToFile(string sFileName)
{
    ofstream myfile;
    myfile.open(sFileName.c_str(), ios::binary);
    if (myfile.is_open())
    {
        /* DATOS A GUARDAR
            vector< vector< vector< cell3d* > > > cells;
            Eigen::Vector3i dimensions;
            Box3d bounding;
            int weightSize;
            float cellSize;
        */

        // Datos generales
        myfile.write((const char*) &res, sizeof(int));
        myfile.write((const char*) &dimensions, sizeof(int)*3);
        myfile.write((const char*) &cellSize, sizeof(float));
        myfile.write((const char*) &weightSize, sizeof(int));
        myfile.write((const char*) &bounding.min, sizeof(double)*3);
        myfile.write((const char*) &bounding.max, sizeof(double)*3);
        myfile.write((const char*) &valueRange, sizeof(float));


        //Datos de cada una de las celdas.
        for(unsigned int i = 0; i< cells.size(); i++)
        {
            for(unsigned int j = 0; j< cells[i].size(); j++)
            {
                for(unsigned int k = 0; k< cells[i][j].size(); k++)
                {
                    cells[i][j][k]->SaveToFile(myfile);
                }
            }
        }

    }
    myfile.close();


}

void cell3d::SaveToFile(ofstream& myfile)
{
    /* DATOS A GUARDAR
    // tipo de la celda
    T_cell tipo;

    // vertexContainer
    bool vertexContainer;

    // valores de pesos de cada v√©rtice de la caja envolvente
    vector<float> weights;
    */

    int auxType = getType();
    myfile.write((const char*) &auxType, sizeof(int));

	// Solo guardamos si es boundary
	if(getType() == BOUNDARY || getType() == INTERIOR)
		data->SaveToFile(myfile);
}

void cell3d::LoadFromFile(ifstream& myfile)
{
    int tipoAux;
    myfile.read( (char*) &tipoAux, sizeof(int) );
    setType((T_cell)tipoAux);

	// Solo cargamos si es boundary
	if(getType() == BOUNDARY || getType() == INTERIOR)
	{
		if(data == NULL) 
			data = new cellData();

		data->LoadFromFile(myfile);
	}
}
