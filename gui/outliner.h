#ifndef OUTLINER_H
#define OUTLINER_H

#include "vector"
#include "string"

class scene;
class Modelo;
class skeleton;
class joint;

enum outlinesNodeTypes {NODETYPE_MODEL = 0, NODETYPE_CAGE , NODETYPE_SKELETON, NODETYPE_JOINT,
                        NODETYPE_CURVE,     NODETYPE_HANDLE,   NODETYPE_CAM,
                        NODETYPE_MODELROOT, NODETYPE_SKELETONROOT,
						NODETYPE_RIGROOT ,NODETYPE_RIG_DEFGRAPH, NODETYPE_RIG_DEFGROUP, NODETYPE_RIG_DEFNODE,
						NODETYPE_RIG_CONTROLGRP};

using namespace std;

class treeNode
{
public:
    treeNode(){}

    string sName;
    int nodeId;
    outlinesNodeTypes type;
    vector< treeNode* > childs;
};

class outliner
{
public:

    outliner(){}

    outliner(scene* scena){ escena = scena;}

    virtual void getSceneTree(treeNode* root);
    scene* escena;

    void getSceneTree(joint* j,treeNode* root);
    void getSceneTree(skeleton* s,treeNode* root);
    void getSceneTree(Modelo* m  ,treeNode* root);

};

#endif // OUTLINER_H
