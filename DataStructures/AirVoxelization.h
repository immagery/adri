#ifndef AIR_VOXELIZATION_H
#define AIR_VOXELIZATION_H

#include <DataStructures\SurfaceData.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "Box3.h"

using namespace std;

enum T_vox { /*00*/VT_UNTYPED = -1, /*01*/VT_BOUNDARY = -2, /*10*/VT_EXTERIOR = -3, /*11*/VT_INTERIOR = -4 };

// Voxel for each position
class vox3D
{
public:
	int pieceId;
	bool type[2];

	vox3D()
	{
		pieceId = -1;
		setType(VT_UNTYPED);
	}

	vox3D(T_vox t)
	{
		pieceId = -1;
		setType(t);
	}

	vox3D(int idx, T_vox t)
	{
		pieceId = idx;
		setType(t);
	}

	T_vox getType()
	{
	    if(type[0])
        {
            if(type[1]) return VT_INTERIOR;
            else        return VT_EXTERIOR;
        }
        else
        {
            if(type[1])  return VT_BOUNDARY;
            else         return VT_UNTYPED;
        }
	}

    void setType(T_vox t)
    {
        if(t == VT_INTERIOR)
        {
            type[0] = true; type[1] = true;
        }
        else if(t == VT_EXTERIOR)
        {
            type[0] = true; type[1] = false;
        }
        else if(t == VT_BOUNDARY)
        {
            type[0] = false; type[1] = true;
        }
        else if(t == VT_UNTYPED)
        {
            type[0] = false; type[1] = false;
        }
    }
};

// Uniform grid
class voxGrid3d
{
public:

	// Cells that contains the voxels procesed (4 dimensions)
	 vector< vector< vector< vector< vox3D* > > > > cells;

    // Dimensiones del grid en los tres ejes
    Vector3i dimensions;
    float cellSize;

    //Box3d bounding;
	MyBox3 bounding;

    // constructor
    voxGrid3d();

    void init(MyBox3 bounding, Vector3i divisions);

	void mergeResults(voxGrid3d* grid, int idx);

	void clear();
	void clearData();

	bool hasData(Vector3i& pt);
	bool isOut(Vector3i& pt);

    // Resolution
    int res;

	float worldScale;

	// Marca las celdas con los diferentes tipos segun la geometria de entrada.
	// Con la nueva estructura de grafo
	int typeCells(SurfaceGraph* mesh);
	int typeCells(SurfaceGraph* mesh, int i);

    // Rellena las celdas que quedan dentro de los contornos.
    // Se llama cuando el contorno ya está inicializado.
    int fillFromCorners();

    // Rellena las celdas que quedan dentro de los contornos.
    // Se llama cuando el resto de celdas ya están inicializadas.
    int fillInside();

    // Devuelve la direccion de la celda a la que pertenece el punto.
    Vector3i cellId(Eigen::Vector3d pt);

	// Devuelve el centro de la celda en coordenadas de mundo.
    Vector3d cellCenter(int i,int j,int k);

	Vector3d getCenterOfCell(int i, int j, int k);

    // Guardamos en binario el grid entero, para no tener que recalcular cada vez
    void SaveGridToFile(string sFileName);

    // Cargamos de disco el grid entero.
    void LoadGridFromFile(string sFileName);

	void LoadFromFile(ifstream& myfile);
	void SaveToFile(ofstream& myfile);

};


#endif // AIR_VOXELIZATION_H
