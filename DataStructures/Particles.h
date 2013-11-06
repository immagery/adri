#pragma once
#include "DataStructures/Object.h"
#include "DataStructures/SurfaceData.h"

class Particles : public object {
public:
	SurfaceGraph *graph;
	vector<vector<Eigen::Vector3d> > positions;		// baked simulation

	vector<Eigen::Vector3d> restPositions;
	vector<Eigen::Vector3d> currentPositions;
	vector<Eigen::Vector3d> lastPositions;

	double lastTime;

	double stiffness;
	double g;
	double xvalue, yvalue, zvalue;
	double velocityDamping;

	Particles(void);
	~Particles(void);
	void drawFunc(int frame);
	void drawFunc();
	void bake(int maxFrames, double deltaTimePerFrame);
	void solve(double time);
	void applyChange(Eigen::Vector3d deltaPos, double deltaTime, int i);
	Eigen::Vector3d idealRestPosition(int i);
};

