#include "Particles.h"

#include <utils/utilGL.h>
#include <DataStructures/Geometry.h> 


Particles::Particles(void)
{
	lastTime = 0;
	graph = new SurfaceGraph();
	int n = 20;

	stiffness = 1;
	g = -50;
	xvalue = yvalue = zvalue = 0;
	velocityDamping = 0.8;

	graph->nodes.resize(n);
	for (int i = 0; i < n; ++i) {
		graph->nodes[i] = new GraphNode(i);
		graph->nodes[i]->position = Eigen::Vector3d(0, 30*i, 0.4*i*i);
		if (i > 0) {
			graph->nodes[i]->connections.push_back(graph->nodes[i-1]);
			graph->nodes[i-1]->connections.push_back(graph->nodes[i]);
		}
		
		restPositions.push_back(graph->nodes[i]->position);
		currentPositions.push_back(graph->nodes[i]->position);
		lastPositions.push_back(graph->nodes[i]->position);
	}
}


Particles::~Particles(void)
{
}

void Particles::drawFunc(int frame) {
	glColor3f(1.0, 0, 0);
	if (frame*100 >= positions.size()) return;
	vector<Eigen::Vector3d> pos = positions[frame*100];
	glPointSize(8);
	glBegin(GL_POINTS);
	for (int i = 0; i < graph->nodes.size(); ++i) {
		glVertex3d(pos[i].x(), pos[i].y(), pos[i].z());
	}
	glEnd();
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < graph->nodes.size(); ++i) {
		if (i < graph->nodes.size()-1) {
			glVertex3d(pos[i].x(), pos[i].y(), pos[i].z());
			glVertex3d(pos[i+1].x(), pos[i+1].y(), pos[i+1].z());
		}
		/*for (int j = 0; j < graph->nodes[i]->connections.size(); ++j) {
			glVertex3d(pos[i].X(), pos[i].Y(), pos[i].Z());
			glVertex3d(pos[j].X(), pos[j].Y(), pos[j].Z());
		}*/
	}
	glEnd();
}

void Particles::drawFunc() {
	glColor3f(1.0, 0, 0);
	vector<Eigen::Vector3d> pos = currentPositions;
	glPointSize(8);
	glBegin(GL_POINTS);
	for (int i = 0; i < graph->nodes.size(); ++i) {
		glVertex3d(pos[i].x(), pos[i].y(), pos[i].z());
	}
	glEnd();
	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < graph->nodes.size(); ++i) {
		if (i < graph->nodes.size()-1) {
			glVertex3d(pos[i].x(), pos[i].y(), pos[i].z());
			glVertex3d(pos[i+1].x(), pos[i+1].y(), pos[i+1].z());
		}
	}
	glEnd();
}

void Particles::applyChange(Eigen::Vector3d deltaPos, double deltaTime, int i) {
	if (i >= currentPositions.size()) return;
	currentPositions[i] += deltaPos*deltaTime;
	applyChange(deltaPos, deltaTime, i+1);
}

Eigen::Vector3d Particles::idealRestPosition(int i) {
	if (i == 0) assert(false);
	return restPositions[i] - restPositions[i-1] + currentPositions[i-1];
	Eigen::Vector3d position = currentPositions[0];
	for (int j = 0; j < i; ++j) {
		Eigen::Vector3d deltaPos = restPositions[j+1] - restPositions[j];
		position = position + deltaPos;
	}
	return position;
}

void Particles::solve(double time) {
	double deltaTime = time - lastTime;
	double tsq = deltaTime * deltaTime;
	lastTime = time;

	// Update points
	for (int i = 1; i < currentPositions.size(); ++i) {
		Eigen::Vector3d velocity = (currentPositions[i] - lastPositions[i]);
		if (velocity.norm() < 0.001 && (currentPositions[i] - idealRestPosition(i)).norm() <= 0.00005) velocity = Eigen::Vector3d(0,0,0);
		velocity *= velocityDamping;
		Eigen::Vector3d acceleration = Eigen::Vector3d(0,g,0);	
		Eigen::Vector3d nextPos = currentPositions[i] + velocity + acceleration * tsq * 0.5;
		lastPositions[i] = currentPositions[i];
		currentPositions[i] = nextPos;
	}


	/*
	double ks, kd, stiff, ks2, kd2, stiff2;
	ks = 5;	kd = 0.5;	stiff = 0.5;	// max: 0.5
	ks2 = 5;	kd2 = 20;	stiff2 = 0.5;	// max: 1
	*/

	double ks, kd, stiff, ks2, kd2, stiff2;
	ks = 5;		kd = 5;		stiff = 0.5;	// max: 0.5
	//ks2 = 5;	kd2 = 20;	stiff2 = 0.1;	// max: 1
	ks2 = 0.5;	kd2 = 2;	stiff2 = 10;
	for (int k = 0; k < 100; ++k) {
		for (int i = 0; i < currentPositions.size(); ++i) {
			// Distance constraints
			for (int j = i-10; j < i; ++j) {
				if (j < 0) continue;
				Eigen::Vector3d restDistance = (restPositions[i] - restPositions[j]);
				Eigen::Vector3d currentDist = currentPositions[i] - currentPositions[j];
				double diff = currentDist.norm() - restDistance.norm();

				Eigen::Vector3d delta1 = currentDist / currentDist.norm() * ks * diff;
				Eigen::Vector3d delta2 = - delta1;
				Eigen::Vector3d vel1 = (currentPositions[i] - lastPositions[i]);
				Eigen::Vector3d vel2 = (currentPositions[j] - lastPositions[j]);
				if (j == 0) vel2 = Eigen::Vector3d(0,0,0);
				double v = (vel1 - vel2).dot(currentDist.normalized());
				Eigen::Vector3d damp1 = currentDist / currentDist.norm() * kd * v;
				Eigen::Vector3d damp2 = - damp1;
				currentPositions[i] -= (delta1+damp1)*stiff*deltaTime;
				if (j > 0) currentPositions[j] -= (delta2+damp2)*stiff*deltaTime;
			} 
			// End of distance constraints

			// "Ideal point" constraints, they try to maintain the angle
			if (i > 0) {
				Eigen::Vector3d idealPoint = idealRestPosition(i);
				Eigen::Vector3d restDistance (0,0,0);
				Eigen::Vector3d currentDist = currentPositions[i] - idealPoint;

				if (currentDist.norm() > 0.05) {
					double diff = currentDist.norm() - restDistance.norm();
					Eigen::Vector3d delta1 = currentDist / currentDist.norm() * ks2 * diff;
					Eigen::Vector3d vel1 = (currentPositions[i] - lastPositions[i]);
					double v = (vel1).dot(currentDist.normalized());
					Eigen::Vector3d damp1 = currentDist / currentDist.norm() * kd2 * v;
					currentPositions[i] -= (delta1+damp1)*stiff2*deltaTime;
				}	
			}
		}
	}

	currentPositions[0] = Eigen::Vector3d(xvalue,yvalue,zvalue);


}

/*void Particles::solve(double time) {
	double deltaTime = time - lastTime;
	double tsq = deltaTime * deltaTime;
	lastTime = time;
	if (lastTime > 4) {
		xvalue = 6;
		currentPositions[0] = Eigen::Vector3d(6,0,0);
	}

	int relaxNumber = 100;
	int neighbourDepth = 3;
	for (int k = 0; k < relaxNumber; ++k) {
			for (int i = 0; i < currentPositions.size(); ++i) {
				
				// Distance constraints
				double ks, kd;
				ks = 1;
				kd = 1;
				stiffness = 0.5;
				for (int j = i-neighbourDepth; j < i; ++j) {
					if (j < 0) continue;
					Eigen::Vector3d restDistance = (restPositions[i] - restPositions[j]);
					Eigen::Vector3d currentDist = currentPositions[i] - currentPositions[j];
					double diff = currentDist.norm() - restDistance.norm();
					Eigen::Vector3d delta1 = currentDist / currentDist.norm() * ks * diff;
					Eigen::Vector3d delta2 = - delta1;
					Eigen::Vector3d vel1 = (currentPositions[i] - lastPositions[i]);
					Eigen::Vector3d vel2 = (currentPositions[j] - lastPositions[j]);
					double v = (vel2 - vel1).dot(currentDist.normalized());
					Eigen::Vector3d damp1 = currentDist / currentDist.norm() * kd * v;
					Eigen::Vector3d damp2 = - damp1;
					currentPositions[i] -= (delta1+damp1)*stiffness*deltaTime;
					//applyChange(-(delta1+damp1), deltaTime, i);
					currentPositions[j] -= (delta2+damp2)*stiffness*deltaTime;
					currentPositions[0] = Eigen::Vector3d(xvalue,0,0);
				} 
				// End of distance constraints

				// Angle constraints, only applicable if node has a neighbour in each side
				if (i < currentPositions.size() - 1) {
					Eigen::Vector3d v1(0,1,0);		// for the first vertex, consider the surface normal as v1
					if (i > 0) v1 = currentPositions[i] - currentPositions[i-1];
					Eigen::Vector3d v2 = currentPositions[i+1] - currentPositions[i];
					double dotProd = v1.dot(v2);
					double norms = (v1.norm() * v2.norm());
					double cosPhi = dotProd / norms;
					if (cosPhi > 1) cosPhi = 1;
					if (cosPhi < -1) cosPhi = -1;
					double angle = acos(cosPhi);
					angle = angle * 180 / 3.141592;
					angle = 180 - angle;

					// If the angle is not in a certain range of the original, then move it!
					if (i > 0) v1 = restPositions[i] - restPositions[i-1];
					v2 = restPositions[i+1] - restPositions[i];
					dotProd = v1.dot(v2);
					cosPhi = dotProd / (v1.norm() * v2.norm());
					if (cosPhi > 1) cosPhi = 1;
					if (cosPhi < -1) cosPhi = -1;
					double restAngle = acos(cosPhi);
					restAngle = restAngle * 180 / 3.141592;
					restAngle = 180 - restAngle;

					double angleks = 1;
					double angleDamping = 1;
					double maxAngle = 2;
					if (abs(restAngle - angle) > maxAngle) {
						double phi = abs(restAngle - angle);
						double ratio = 1.0 - (maxAngle / phi);
						Eigen::Vector3d restDistance(0,0,0);
						Eigen::Vector3d idealPoint = (restPositions[i+1] - restPositions[i]) + currentPositions[i];
						Eigen::Vector3d currentDist = idealPoint - currentPositions[i+1];
						double diff = currentDist.norm() - restDistance.norm();
						Eigen::Vector3d delta1 = currentDist / currentDist.norm() * angleks * diff;
						Eigen::Vector3d delta2 = - delta1;
						Eigen::Vector3d vel1 = (currentPositions[i+1] - lastPositions[i+1]);
						Eigen::Vector3d vel2(0,0,0);
						double v = (vel2 - vel1).dot(currentDist.normalized());
						Eigen::Vector3d damp1 = currentDist / currentDist.norm() * angleDamping * v;
						Eigen::Vector3d damp2 = - damp1;
						//applyChange((delta1+damp1)*ratio, deltaTime, i+1);
						currentPositions[i+1] += (delta1+damp1)*deltaTime;
					}
				} // End of angle constraints


			}

			// "Angle" restriction
			//Eigen::Vector3d originalVector = restPositions[1] - restPositions[0];
			//Eigen::Vector3d currentVector = currentPositions[1] - currentPositions[0];
			//currentPositions[1] += (originalVector - currentVector) * deltaTime;
		}
		
		for (int i = 0; i < currentPositions.size(); ++i) {
			Eigen::Vector3d velocity = (currentPositions[i] - lastPositions[i]);						// velocity = inertia

			Eigen::Vector3d nextPos = currentPositions[i] + velocity + Eigen::Vector3d(0,g,0) * tsq;		// apply gravit

			if (i > 0) {
				Eigen::Vector3d idealPoint = (restPositions[i] - restPositions[i-1]) + currentPositions[i-1];
				if ((nextPos - idealPoint).norm() > (currentPositions[i] - idealPoint).norm()) {
					nextPos = currentPositions[i] + velocity*0.9 + Eigen::Vector3d(0,g,0) * tsq;
					if (velocity.norm() < 2) velocity = Eigen::Vector3d(0,0,0);
				}

			}

			//if ((nextPos - currentPositions[i]).norm() >
			lastPositions[i] = currentPositions[i];
			currentPositions[i] = nextPos;
		}
}*/

void Particles::bake(int maxFrames, double deltaTimePerFrame) {
	int stepsPerFrame = 100;
	positions.resize(maxFrames*stepsPerFrame);
	deltaTimePerFrame /= stepsPerFrame;

	for (int frame = 0; frame < positions.size(); ++frame) {

		positions[frame] = (vector<Eigen::Vector3d> (currentPositions.size()));

		int relaxNumber = 24;
		int neighbourDepth = 2;
		for (int k = 0; k < relaxNumber; ++k) {
			for (int i = 0; i < currentPositions.size(); ++i) {
				// Calculate distance to its neighbours
				for (int j = i-neighbourDepth; j <= i+neighbourDepth; ++j) {
					if (j < 0 || j >= currentPositions.size() || j == i) continue;
					Eigen::Vector3d restDistance = restPositions[i] - restPositions[j];
					Eigen::Vector3d currentDist = currentPositions[i] - currentPositions[j];
					double diff = (restDistance.norm() - currentDist.norm()) / restDistance.norm();
					double scalarP1 = 0.5;		
					double scalarP2 = 0.5;		// masses are 1
					diff = currentDist.norm() - restDistance.norm();
					Eigen::Vector3d delta1 = -(currentDist / restDistance.norm()) * (scalarP1 * diff);
					Eigen::Vector3d delta2 = -delta1;
					//currentPositions[i] -= currentDist * scalarP1 * diff * deltaTimePerFrame;
					//currentPositions[j] += currentDist * scalarP2 * diff * deltaTimePerFrame;
					currentPositions[i] += delta1 * deltaTimePerFrame;
					currentPositions[j] += delta2 * deltaTimePerFrame;
					currentPositions[0] = Eigen::Vector3d(frame / 10000.0,0,0);
					//currentPositions[0] = Eigen::Vector3d(0,0,0);
				}

				if (i == 1) {
					Eigen::Vector3d originalVector = restPositions[1] - restPositions[0];
					Eigen::Vector3d currentVector = currentPositions[1] - currentPositions[0];
					currentPositions[1] += (originalVector - currentVector) * deltaTimePerFrame;
				}
			}
		}

		double tsq = deltaTimePerFrame * deltaTimePerFrame;

		for (int i = 0; i < currentPositions.size(); ++i) {
			Eigen::Vector3d velocity = (currentPositions[i] - lastPositions[i]);						// velocity = inertia
			Eigen::Vector3d nextPos = currentPositions[i] + velocity + Eigen::Vector3d(0,g,0) * tsq;		// apply gravit
			lastPositions[i] = currentPositions[i];
			currentPositions[i] = nextPos;

			positions[frame][i] = nextPos;

		}

	}

	currentPositions.clear();
	lastPositions.clear();
	for (int i = 0; i < restPositions.size(); ++i) {
		currentPositions.push_back(restPositions[i]);
		lastPositions.push_back(restPositions[i]);
	}

}


					/*if (j < 0 || j >= currentPositions.size() || j == i) continue;
					Eigen::Vector3d restDistance = restPositions[i] - restPositions[j];
					Eigen::Vector3d currentDist = currentPositions[i] - currentPositions[j];
					double diff = (restDistance.norm() - currentDist.norm()) / restDistance.norm();
					double scalarP1 = 0.1;		
					double scalarP2 = 0.1;		// masses are 1
					diff = currentDist.norm() - restDistance.norm();
					Eigen::Vector3d delta1 = -(currentDist / restDistance.norm()) * (scalarP1 * diff);
					Eigen::Vector3d delta2 = -delta1;
					currentPositions[i] += delta1 * deltaTime;
					currentPositions[j] += delta2 * deltaTime;
					currentPositions[0] = Eigen::Vector3d(xvalue,0,0);*/

						// Enforce a distance constraint between the current point and the "ideal point"
						/*Eigen::Vector3d idealPoint = (restPositions[i+1] - restPositions[i]) + currentPositions[i];
						Eigen::Vector3d p1 = currentPositions[i+1];
						Eigen::Vector3d vel1 = (currentPositions[i+1] - lastPositions[i+1]);
						Eigen::Vector3d deltaPos = idealPoint - p1;
						double v = (vel1).dot(deltaPos.normalized());
						
						deltaPos *= angleDamping;
						//deltaPos *= ratio;
						//deltaPos *= -v;
						deltaPos /= deltaPos.norm();

						currentPositions[i+1] += deltaPos * deltaTime;*/