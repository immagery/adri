#include "Geometry.h"
#include "..\render\geometryRender.h"


Geometry::Geometry() : object()
{
	shading = new GeometryRender(this);
}

Geometry::Geometry(unsigned int nodeId) : object(nodeId)
{
	shading = new GeometryRender(this);
}

Geometry::~Geometry()
{
	delete shading;
}

void Geometry::drawFunc()
{
    // transformaciones
	if(!shading)
		return;
		
	//shading->beforeDraw(this);
	((GeometryRender*)shading)->drawFunc(this);
    //shading->afterDraw(this);
}

void Geometry::freezeTransformations()
{
	for(unsigned int vi = 0; vi < nodes.size(); vi++ ) {
		Eigen::Vector4d prod = tMatrix*Eigen::Vector4d(nodes[vi]->position.x(), nodes[vi]->position.y(), nodes[vi]->position.z(), 1);
		nodes[vi]->position = Eigen::Vector3d(prod.x(), prod.y(), prod.z());
	}

    loadIdentity();
}

bool Geometry::cleanModel()
{
	nodes.clear();
	triangles.clear();

	return true;
}

// Guarda un modelo de disco
// filename: fichero de ruta completa.
void Geometry::saveModel(string fileName, string name, string ext, string path)
{

    if(ext.compare("off") == 0)
    {
        // tri::io::ExporterOFF<MyMesh>::Save(*this,fileName.c_str());
    }
    else if(ext.compare("ply") == 0)
    {
        printf("PlyFile: to do SaveModel\n"); fflush(0);
           //model.Clear();
           // tri::io::ImporterPLY<MyMesh>::Open(model,fileName.toStdString().c_str());
           //gpUpdateNormals(model, false);
           //loadedModel = true;
    }
    else if(ext.compare("3ds") == 0)
    {
        printf("3DsFile: to do SaveModel\n"); fflush(0);
        /*
        m_.modeloOriginal.Clear();
        m_.dynCage.Clear();
        m_.cage.Clear();
        // tri::io::ImporterOBJ<MyMesh>::Open(model,fileName.toStdString().c_str());
        gpUpdateNormals(m_.modeloOriginal, false);
        loadedModel = true;
        */
    }
    else
    {
        printf("Aviso: La extension del ficehero no es conocida\n"); fflush(0);
        return;
    }

}

// Lee un modelo de disco
// filename: fichero de ruta completa.
// model: MyMesh donde se almacenara el modelo leido.
void Geometry::loadModel(string fileName, string name, string ext, string path)
{
    this->sPath = path;
    this->sName = name;

    dirtyFlag = true;

	if(ext.compare("obj") == 0)
    {
		/*
		 tri::io::ImporterOBJ<MyMesh>::Info infoImp;
		 tri::io::ImporterOBJ<MyMesh>::Open(*this,fileName.c_str(), infoImp);

		 tri::UpdateTopology<MyMesh>::VertexFace(*this);
         tri::UpdateBounding<MyMesh>::Box(*this);

        gpUpdateNormals(*this, true);
        dirtyFlag = false;
		*/
	}
    else if(ext.compare("off") == 0)
    {
		BuildSurfaceFromOFFFile(*this, fileName);
		computeNormals();
        dirtyFlag = false;
    }
    else if(ext.compare("ply") == 0)
    {
           //model.Clear();
           // tri::io::ImporterPLY<MyMesh>::Open(model,fileName.toStdString().c_str());
           //gpUpdateNormals(model, false);
           //loadedModel = true;
    }
    else if(ext.compare("3ds") == 0)
    {
        printf("3DsFile: to do LoadModel\n"); fflush(0);
        /*
        m_.modeloOriginal.Clear();
        m_.dynCage.Clear();
        m_.cage.Clear();
        // tri::io::ImporterOBJ<MyMesh>::Open(model,fileName.toStdString().c_str());
        gpUpdateNormals(m_.modeloOriginal, false);
        loadedModel = true;
        */
    }
    else
    {
        printf("Aviso: La extension del ficehero no es conocida\n"); fflush(0);
        return;
    }

}


// Return a bool if there is a bounding box computable
bool Geometry::getBoundingBox(Eigen::Vector3d& minAuxPt,Eigen::Vector3d& maxAuxPt)
{
	if(nodes.size() < 0)
	{
		minBBox = maxBBox = minAuxPt = maxAuxPt = Eigen::Vector3d(0,0,0);
		return false;
	}

	double minValue[3], maxValue[3];	

	for(int i = 0; i< 3; i++)
		minValue[i] = maxValue[i] = nodes[0]->position[i];

	for(unsigned int i = 1; i< nodes.size(); i++)
	{
		for(int j = 0; j< 3; j++)
		{
			minValue[j] = min(minValue[j], nodes[i]->position[j]);
			maxValue[j] = max(maxValue[j], nodes[i]->position[j]);
		}
	}

    minBBox = minAuxPt = Eigen::Vector3d(minValue[0], minValue[1], minValue[2]);
    maxBBox = maxAuxPt = Eigen::Vector3d(maxValue[0], maxValue[1], maxValue[2]);
    return true;
}

void Geometry::computeFaceNormals()
{
	int trianglesSize = triangles.size();
	faceNormals.resize(trianglesSize);
	for(unsigned int i = 0; i < triangles.size(); i++)
	{
		Eigen::Vector3d v1 = triangles[i]->verts[1]->position - triangles[i]->verts[0]->position;
		Eigen::Vector3d v2 = triangles[i]->verts[2]->position - triangles[i]->verts[0]->position;
		faceNormals[i] = (v1.normalized().cross(v2.normalized())).normalized();		
	}
}

void Geometry::computeVertNormals()
{
	if(triangles.size() != faceNormals.size())
		computeFaceNormals();

	vertNormals.resize(nodes.size());
	vector<int> vertTriCounter;
	vertTriCounter.resize(nodes.size(),0);

	for(unsigned int i = 0; i < nodes.size();i++)
		vertNormals[i] = Eigen::Vector3d(0,0,0);

	for(unsigned int i = 0; i < triangles.size(); i++)
	{
		for(unsigned int j = 0; j < triangles[i]->verts.size(); j++)
		{
			vertTriCounter[triangles[i]->verts[j]->id]++;
			vertNormals[triangles[i]->verts[j]->id] += faceNormals[i];
		}
	}

	for(unsigned int i = 0; i < nodes.size();i++)
		vertNormals[i] = (vertNormals[i]/vertTriCounter[i]).normalized();

	//faceNormals.clear();
}

void Geometry::computeNormals()
{
	computeFaceNormals();
	computeVertNormals();
}

Eigen::Vector3d Geometry::getSelCenter()
{
    if(shading->subObjectmode)
    {
        Eigen::Vector3d center;
        for(unsigned int i = 0; i>shading->selectedIds.size(); i++)
        {
			assert(false);
            //if(i == 0)
            //    center = vert[shading->selectedIds[i]].P();
            //else
            //    center += vert[shading->selectedIds[i]].P();
        }

        if(shading->selectedIds.size() > 0)
            center = center/shading->selectedIds.size();

        return center;
    }

    return pos;
}
