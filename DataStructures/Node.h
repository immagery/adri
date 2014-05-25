#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <DataStructures\Serialization.h>

#include <utils/util.h>

using namespace std;

enum nodeTypes {NODE = 0, RENDERNODE_NODE, GRIDRENDERER_NODE, PLANERENDERER_NODE, OBJECT_NODE, DEFORMER_NODE, SKELETON_NODE, JOINT_NODE,
				DEFGROUP_NODE, DEFGRAPH_NODE, DEFNODE_NODE};


class nodeSerialization
{
public:
	int nodeId;
	bool dirtyFlag;
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

	void copyFrom(node* obj)
	{
		dirtyFlag = obj->dirtyFlag;
		iam = obj->iam;
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
		fprintf(fout, "%d %d %d ",dirtyFlag,nodeId,(int)iam);
		//fprintf(fout, "%s\n",sPath.c_str());
		
		if(sName.length() > 0)
			fprintf(fout, "%s\n",sName.c_str());
		else
			fprintf(fout, "_Node_NoName_\n",sName.c_str());

		return true;
	}

	virtual bool loadFromFile(ifstream& in)
	{
		sNode = new nodeSerialization();
	
		string line;
		vector<string> elems;
		getline (in , line);
		split(line, ' ', elems);
		sNode->dirtyFlag = atoi(elems[0].c_str());
		sNode->nodeId = atoi(elems[1].c_str());
		iam = (nodeTypes)atoi(elems[2].c_str());
		
		if(elems[3].compare("_Node_NoName_") != 0)
			sName = elems[3];
		
		//sPath = str;
		//fscanf(fout, "%s", &str);
		//sName = str;

		return true;
	}

};

#endif // NODE_H
