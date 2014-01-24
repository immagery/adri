#ifndef SKT_CREATOR_H
#define SKT_CREATOR_H

#include <DataStructures\AirRig.h>
#include <DataStructures\scene.h>
#include <DataStructures/DataStructures.h>

enum sktCrState { SKT_CR_IDDLE = 0, SKT_CR_SELECTED};

class sktCreator
{
public:
	AirRig* dynRig; // The skeleton that is being created on the fly

	DefGroup* last; // The last joint created in the process

	DefGroup* parentNode; // Parent joint where the new skeleton will be attached
	AirRig* parentRig; // Parent joint where the new skeleton will be attached

	sktCrState state; // The state

	sktCreator()
	{
		state = SKT_CR_IDDLE;

		dynRig = NULL; 
		last = NULL;
		parentNode = NULL;
		parentRig = NULL;

		dynRig = new AirRig(scene::getNewId());

	}

	void selectJoint(DefGroup* jt, AirRig* skt)
	{
		// no se si es necesario hacer algo mas.
		parentNode = jt;
		parentRig = skt;

		last = NULL;

		state = SKT_CR_SELECTED;
	}

	// adds a new joint to the skeleton
	void addNewNode(Vector3d point);

	// Deletes the last joint created (undo)
	void removeLastNode();

	// Atach the dynSkeleton to the scene skeleton through parentJoint and parentSkeleton
	void finishRig();

};

#endif