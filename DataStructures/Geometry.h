#ifndef GEOMETRY_H
#define GEOMETRY_H

#define DEBUG false

#include "Object.h"
#include "DataStructures.h"

#include "SurfaceData.h"

using namespace std;

// GEOMETRY
class Geometry : public object , public SurfaceGraph
{    
    public:
        Geometry();
		Geometry(unsigned int nodeId);
		~Geometry();

        void loadModel(string fileName, string name, string ext, string path);
        void saveModel(string fileName, string name, string ext, string path);
        bool cleanModel();

        virtual void drawFunc();

        virtual void freezeTransformations();

        virtual Eigen::Vector3d getSelCenter();
        virtual bool getBoundingBox(Eigen::Vector3d& minAuxPt,Eigen::Vector3d& maxAuxPt);

		Eigen::Vector3d minBBox;
		Eigen::Vector3d maxBBox;

		void computeFaceNormals();
		void computeVertNormals();
		void computeNormals();

		vector<Eigen::Vector3d> faceNormals;
		vector<Eigen::Vector3d> vertNormals;

		vector<bool> edges;

};

#endif // GEOMETRY_H
