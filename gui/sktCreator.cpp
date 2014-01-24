#include "sktCreator.h"

void ComputeWithWorldOrientedRotations(joint* jt)
{
	Vector3d dirX = Vector3d(1,0,0);

	// Posicionar solo... dejamos de momento la rot y orient
	if(jt->father)
		jt->pos = jt->worldPosition - jt->father->worldPosition;
	else
		jt->pos = jt->worldPosition;

	for(int i = 0; i < jt->childs.size(); i++)
	{
		//Vector3d vec = jt->childs[i]->worldPosition-jt->worldPosition;
		//Quaterniond rot;
		//rot.setFromTwoVectors(dirX, vec);

		ComputeWithWorldOrientedRotations(jt->childs[i]);
	}
}

void ComputeWithBoneOrientedRotations(joint* jt)
{
	Vector3d dirX = Vector3d(1,0,0);

	// Posicionar solo... dejamos de momento la rot y orient
	if(jt->father)
		jt->pos = jt->worldPosition-jt->father->worldPosition;
	else
		jt->pos = jt->worldPosition;

	for(int i = 0; i < jt->childs.size(); i++)
	{
		//Vector3d vec = jt->childs[i]->worldPosition-jt->worldPosition;
		//Quaterniond rot;
		//rot.setFromTwoVectors(dirX, vec);

		ComputeWithBoneOrientedRotations(jt->childs[i]);
	}
}

void sktCreator::addNewNode(Vector3d point)
{
	if(last != NULL)
	{
		// creation
		DefGroup* df = new DefGroup(scene::getNewId());
		df->transformation = new joint(scene::getNewId());
		joint* newJoint = df->transformation;

		// position
		newJoint->worldPosition = point;

		// setting rotation and orientation?
		newJoint->setRotation(0, 0, 0, false);
		newJoint->setJointOrientation(0,0,0);

		// child/father relation
		last->relatedGroups.push_back(df);
		df->dependentGroups.push_back(last);
		newJoint->father = last->transformation;

		last->transformation->childs.push_back(newJoint);

		//references
		dynRig->defRig.defGroups.push_back(df);
		dynRig->defRig.defGroupsRef[df->nodeId] = df;		
		
		// si solo hemos añadido uno
		if(dynRig->defRig.defGroups.size() == 1 && dynRig->defRig.roots.size() == 0)
			dynRig->defRig.roots.push_back(df);

		newJoint->dirtyFlag = true;
		df->dirtyFlag = true;

		last = df;
	}
	else
	{
		// creation
		DefGroup* df = new DefGroup(scene::getNewId());
		df->transformation = new joint(scene::getNewId());
		joint* newJoint = df->transformation;

		// position
		newJoint->worldPosition = point;

		// setting rotation and orientation?
		newJoint->setRotation(0, 0, 0, false);
		newJoint->setJointOrientation(0,0,0);

		// child/father relation -> no parent relation
		newJoint->father = NULL;

		//references
		dynRig->defRig.defGroups.push_back(df);
		dynRig->defRig.defGroupsRef[df->nodeId] = df;		
		
		// si solo hemos añadido uno
		if(dynRig->defRig.defGroups.size() == 1 && dynRig->defRig.roots.size() == 0)
			dynRig->defRig.roots.push_back(df);

		newJoint->dirtyFlag = true;
		df->dirtyFlag = true;

		last = df;
	}

	if(dynRig->defRig.roots.size() > 0)
		ComputeWithWorldOrientedRotations(dynRig->defRig.roots[0]->transformation);

	dynRig->dirtyFlag = true;
	dynRig->update();

	//ComputeWithBoneOrientedRotations(dynSkt->root);
}

void sktCreator::removeLastNode()
{
	// TODO
	assert(false);
	/*
	if(last != NULL)
	{
		joint* toRemove = last;
		last = last->father;

		for(int i = 0; i< dynSkt->joints.size(); i++)
		{
			if(dynSkt->joints[i]->nodeId == toRemove->nodeId)
			{
				//TODO: eliminar
				assert(false);
			}
		}
	}
	*/
}

void sktCreator::finishRig()
{
	// 1. Attach this rigg in the Air rig, as an appendix or new root.
	if(parentNode)
	{
		//TODO
		assert(false);
	}
	else
	{
		// encolamos el principal
		parentRig->defRig.roots.push_back(dynRig->defRig.roots[0]);
	}

	// 2. Clear the dyn rig
	parentRig->defRig.roots.clear();
	parentRig->defRig.defGroupsRef.clear();

	// 3. Setup the states for further creation
	state = SKT_CR_IDDLE;
	last = NULL;
	parentNode = NULL;

	// este puntero no lo tocamos.
	//parentRig = ...;
}