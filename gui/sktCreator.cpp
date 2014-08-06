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

DefGroup* sktCreator::addNewNode(Vector3d point)
{
	printf("Add new node...\n");

	int actualRoot = -1;

	// creation
	DefGroup* df = new DefGroup(scene::getNewId(T_DEFNODE));
	df->references = &(dynRig->defRig.defGroupsRef);
	df->transformation = new joint(scene::getNewId(T_BONE));
	joint* newJoint = df->transformation;

	if (last != NULL)
	{
		// child/father relation
		// From DefGroup point of view
		last->relatedGroups.push_back(df);
		df->dependentGroups.push_back(last);

		// I'm not sure
		// From joint point of view
		//newJoint->father = last->transformation;
		//last->transformation->childs.push_back(newJoint);
	}
	else
	{
		// setting rotation and orientation?
		//newJoint->setRotation(0, 0, 0, false);
		//newJoint->setJointOrientation(0,0,0);

		// child/father relation -> no parent relation
		//newJoint->father = NULL;
		dynRig->defRig.roots.push_back(df);
	}

	df->setTranslation(point.x(), point.y(), point.z(), false);


	//references
	dynRig->defRig.defGroups.push_back(df);
	//dynRig->defRig.defGroupsRef[df->nodeId] = df;		
	last = df;

	// si solo hemos añadido uno
	//if(dynRig->defRig.defGroups.size() == 1 && dynRig->defRig.roots.size() == 0)
	//	dynRig->defRig.roots.push_back(df);

	// dirty bits
	dynRig->dirtyFlag = true;
	newJoint->dirtyFlag = true;
	df->dirtyFlag = true;

	// Recompute worldPos
	//for (int rootIdx = 0; rootIdx < dynRig->defRig.roots.size(); rootIdx++)
	//{
	//	dynRig->defRig.roots[rootIdx]->computeWorldPos(dynRig->defRig.roots[rootIdx]);
	//}

	df->computeWorldPos(df);

	df->update();
	dynRig->update();

	//ComputeWithBoneOrientedRotations(dynSkt->root);

	return df;
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


//Esta establece la posición de descanso con la posición y rot actual
void resetRestPose(DefGroup* groupDest)
{
	joint* jt = groupDest->transformation;

	jt->rTranslation = jt->translation;
	jt->rRotation = jt->rotation;
	jt->rTwist = jt->twist;

	jt->restRot = jt->qrot;
	jt->restPos = jt->pos;
	jt->qOrient = jt->qOrient;
}


void copyDefGroup(DefGroup* groupDest, DefGroup* groupOrig)
{
	groupDest->deformers.resize(groupOrig->deformers.size());
	for(int defIdx = 0; defIdx< groupOrig->deformers.size(); defIdx++)
	{
		// TO_DEBUG
		groupDest->deformers[defIdx] = groupOrig->deformers[defIdx];
	}

	if (groupOrig->transformation)
	{
		groupDest->transformation = new joint();
		groupDest->transformation->copyFrom(groupOrig->transformation);
	}
	else
		groupDest->transformation = new joint();

	// I dont know what is it... 
	if (groupOrig->rigTransform)
	{
		groupDest->rigTransform = new joint();
		groupDest->rigTransform->copyFrom(groupOrig->rigTransform);
	}
	else
		groupDest->rigTransform = new joint();
	//

	// Properties... generic
	groupDest->subdivisionRatio = groupDest->subdivisionRatio;
	groupDest->expansion = groupDest->expansion;
	groupDest->smoothingPasses = groupDest->smoothingPasses;
	groupDest->smoothPropagationRatio = groupDest->smoothPropagationRatio;
	groupDest->localSmooth = groupDest->localSmooth; 

	groupDest->iniTwist = groupDest->iniTwist;
	groupDest->finTwist = groupDest->finTwist;
	groupDest->enableTwist = groupDest->enableTwist;
	groupDest->smoothTwist = groupDest->smoothTwist;

	groupDest->parentType = groupDest->parentType;
	groupDest->bulgeEffect = groupDest->bulgeEffect;
	groupDest->type = groupDest->type; 
}

void copyDefGroupsHierarchy(AirRig* strain, DefGroup* link, DefGroup* graft)
{
	DefGroup* sprout = NULL;
	if(link == NULL)
	{
		strain->defRig.roots.push_back(new DefGroup(scene::getNewId(T_DEFGROUP)));
		strain->defRig.roots.back()->references = &(strain->defRig.defGroupsRef);
		sprout = strain->defRig.roots.back();
	}
	else
	{
		link->relatedGroups.push_back(new DefGroup(scene::getNewId(T_DEFGROUP)));
		link->relatedGroups.back()->references = &(strain->defRig.defGroupsRef);
		sprout = link->relatedGroups.back();
	}

	copyDefGroup(sprout, graft);


	resetRestPose(sprout);

	// Setting dirty flags triggers computation
	sprout->dirtyFlag = true;
	sprout->dirtyCreation = true;
	sprout->dirtyTransformation = true;
	sprout->dirtySegmentation = true;

	char name[20];
	sprintf(name, "DefGroup%d", sprout->nodeId);
	sprout->sName = name;

	if (link != NULL)
	{
		link->dirtyFlag = true;
		sprout->dependentGroups.push_back(link);
	}
	else
		link = sprout;

	strain->defRig.defGroups.push_back(sprout);
	strain->defRig.defGroupsRef[sprout->nodeId] = sprout;

	// recorremos los hijos de manera recursiva.
	for(int i = 0; i< graft->relatedGroups.size(); i++)
	{
		copyDefGroupsHierarchy(strain, sprout, graft->relatedGroups[i]);
	}
}

void sktCreator::finishRig()
{
	printf("Finalizar la creacion del esqueleto\n");

	// 1. Attach this rigg in the Air rig, as an appendix or new root.

	int firstIndexToCopy = 0;

	if (parentNode != NULL)
	{
		// Solo copiamos si hay mas de uno, porque sino solo tenemos la base... estariamos duplicando.
		if (dynRig->defRig.defGroups.size() > 1)
		{
			// Copia de la jerarquia
			copyDefGroupsHierarchy(parentRig, parentNode, dynRig->defRig.roots[0]->relatedGroups[0]);

			// Correction for place the new branch in the correct place
			Quaterniond qrotExtra = dynRig->defRig.roots[0]->relatedGroups[0]->transformation->rotation * parentNode->transformation->rotation.inverse();
			parentNode->relatedGroups.back()->transformation->qrot = qrotExtra;
			Vector3d worldPositionRel = dynRig->defRig.roots[0]->relatedGroups[0]->transformation->translation - parentNode->transformation->translation;
			parentNode->relatedGroups.back()->transformation->pos = parentNode->transformation->rotation.inverse()._transformVector(worldPositionRel);

			// Seleccionar el nodo de nuevo
			parentNode->select(true, parentNode->nodeId);
			parentNode->computeWorldPos(parentNode);
		}
	}
	else
	{
		copyDefGroupsHierarchy(parentRig, parentNode, dynRig->defRig.roots[0]);

		parentNode = parentRig->defRig.roots.back();

		parentNode->select(true, parentNode->nodeId);
		parentNode->computeWorldPos(parentNode);
	}

	parentRig->dirtyFlag = true;

	// 2. Clear the dyn rig
	dynRig->defRig.roots.clear();
	dynRig->defRig.defGroupsRef.clear();
	for (int i = 0; i < dynRig->defRig.defGroups.size(); i++)
		delete dynRig->defRig.defGroups[i];
	dynRig->defRig.defGroups.clear();

	// 3. Setup the states for further creation
	state = SKT_CR_IDDLE;
	last = NULL;
	parentNode = NULL;

	// este puntero no lo tocamos.
	//parentRig = ...;
}