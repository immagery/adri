#ifndef NODE_H
#define NODE_H

#include <string>
#include <DataStructures\Serialization.h>

enum nodeTypes {NODE = 0, RENDERNODE_NODE, GRIDRENDERER_NODE, PLANERENDERER_NODE, OBJECT_NODE, DEFORMER_NODE, SKELETON_NODE, JOINT_NODE};


class nodeSerialization
{
public:
	int nodeId;
};

class node
{
    public:
    // Dirty flag management
    bool dirtyFlag;

    unsigned int nodeId;

    std::string sPath; // Node path
    std::string sName; // Node name

    nodeTypes iam;

	nodeSerialization* sNode;

    node()
    {
        dirtyFlag = false;
        sName.clear();
        sPath.clear();
        nodeId = 0;
        iam = NODE;
    }

    node(unsigned int id)
    {
        dirtyFlag = false;
        sName.clear();
        sPath.clear();

        nodeId = id;
        iam = NODE;
    }

    virtual bool propagateDirtyness()
    {
        dirtyFlag = false;
        return true;
    }

    virtual bool update()
    {
        if(!dirtyFlag)
            return true;
        else
            return true;
    }

	virtual bool saveToFile(FILE* fout)
	{
		// Dirty flag management
		fprintf(fout, "%d %d %d\n",dirtyFlag,nodeId,iam);
		fprintf(fout, "%s\n",sPath);
		fprintf(fout, "%s\n",sName);

		return true;
	}

	virtual bool loadFromFile(FILE* fout)
	{
		sNode = new nodeSerialization();
		fscanf(fout, "%d %d %d", &dirtyFlag, &sNode->nodeId, &iam);
		fscanf(fout, "%s", &sPath);
		fscanf(fout, "%s", &sName);

		return true;
	}

};

#endif // NODE_H
