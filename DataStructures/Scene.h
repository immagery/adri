#ifndef SCENE_H
#define SCENE_H

#include "Deformer.h"
#include "Cage.h"
#include "skeleton.h"
#include "Modelo.h"
#include "Skinning.h"
#include "rig.h"

// reservamos unos cuantos números para elementos especiales
#define FIRST_OBJECT_ID		100000
#define FIRST_DEFNODE_ID	200000
#define FIRST_DEFGROUP_ID	300000
#define FIRST_POINTDATA_ID	400000
#define FIRST_BONE_ID		500000
#define FIRST_GNRAL_ID		600000

// it's important the concordance from the prior defines to this enum, because
// the object type is equal to CONTANT%100000
enum nodeType{T_OBJECT = 1, T_DEFNODE, T_DEFGROUP, T_POINTDATA, T_BONE, T_GNRAL};

enum procState{CREATED = 0, RIGG, ANIMATION, INTERACTIVE};

class sceneUpdatingFlags
{
public:
	bool updateDefNodes;
	bool updateSkinningFlag;
	vector<int> defNodesToUpdate;
	bool all;

	sceneUpdatingFlags()
	{
		updateDefNodes = false;
		updateSkinningFlag = false;
		all = false;
		defNodesToUpdate.clear();
	}
};

class defInfo
{
	public:

	int id;
	Eigen::Vector3d position;
	float smooth;
	float expansion;
	int relation[2];

	defInfo()
	{
		id = -1;
		smooth = 1;
		expansion = 1;
		position = Eigen::Vector3d(0,0,0);
		relation[0] = -1;
		relation[1] = -1;
	}

	defInfo(const defInfo& inD)
	{	
		smooth = inD.smooth;
		expansion = inD.expansion;
		id = inD.id;
		position = inD.position;
		relation[0] = inD.relation[0];
		relation[1] = inD.relation[1];
	}
};

class inData
{
	public:
	bool firstTime;

	float globalScale;
	float globalSmooth;

	procState mode;

	vector<defInfo> deformers;

	inData()
	{
		deformers.clear();
		firstTime = true;
		globalScale = -1;
		globalSmooth = -1;
	}
};

// Total Scene
class scene
{
    public:
    scene()
    {
		//skinner = new Skinning();
		modelLoaded = false;
		skeletonLoaded = false;
		embeddingLoaded = false;
		weightsUpdated = false;
		initialized = false;
		evaluate = false;
		gridLoadedFromDisc = false;
		state = CREATED;
		weightsThreshold = -10;

		iVisMode = 0;
		desiredVertex = 0;

		sceneScale = 1;

		rig = NULL;
		rigsArray = vector<Rig*>();
    }

    ~scene()
    {

		printf("Destruccion de la escena\n"); fflush(0);

        for(unsigned int i = 0; i< models.size(); i++)
            delete models[i];

        for(unsigned int i = 0; i< skeletons.size(); i++)
            delete skeletons[i];

        for(unsigned int i = 0; i< deformers.size(); i++)
            delete deformers[i];

        for(unsigned int i = 0; i< shaders.size(); i++)
            delete shaders[i];

        models.clear();
        skeletons.clear();
        deformers.clear();
        shaders.clear();
    }

    static unsigned int getNewId(nodeType type = T_GNRAL)
	{
		switch(type)
		{
			case T_DEFNODE:
				scene::defNodeIds++; 
				return scene::defNodeIds-1;
			case T_OBJECT:
				scene::objectIds++; 
				return scene::objectIds-1;
			case T_DEFGROUP:
				scene::defGroupIds++; 
				return scene::defGroupIds-1;
			case T_BONE:
				scene::boneIds++; 
				return scene::boneIds-1;
			case T_POINTDATA:
				scene::pointDataIds++; 
				return scene::pointDataIds-1;
			default:
				scene::generalIds++; 
				return scene::generalIds-1;
		}
	}


	void setSceneScale( float sceneScale);
	void setGlobalSmoothness(float globalSmooth);

    void selectElements(vector<unsigned int > lst)
    {
        for(unsigned int j = 0; j < lst.size(); j++)
        {
            for(unsigned int i = 0; i< models.size(); i++)
                ((Modelo*)(models[i]))->select(true, lst[j]);

            for(unsigned int i = 0; i< skeletons.size(); i++)
                ((skeleton*)(skeletons[i]))->select(false, lst[j]);

			rig->select(false, -1);
        }
    }

    void removeSelection()
    {
        for(unsigned int i = 0; i< models.size(); i++)
            ((Modelo*)(models[i]))->select(false, -1);

        for(unsigned int i = 0; i< skeletons.size(); i++)
            ((skeleton*)(skeletons[i]))->select(false, -1);

		rig->select(false, -1);
    }

	int findIdByName(string name) {
		for (int i = 0; i < skeletons[0]->joints.size(); ++i) {
			if (skeletons[0]->joints[i]->sName == name) return skeletons[0]->joints[i]->nodeId;
		}
		return -1;
	}

	string findNameById(int id) {
				for (int i = 0; i < skeletons[0]->joints.size(); ++i) {
			if (skeletons[0]->joints[i]->nodeId == id) return skeletons[0]->joints[i]->sName;
		}
		return "";
	}

	// Scene info
	string sSceneName;
    string sGlobalPath;
    string sPath;

	//Models
    vector< object*   > models;
    vector< deformer* > deformers;

	// Render
    vector< shadingNode*   > visualizers;
    vector< shadingNode* > shaders;

	// Scene management
	static unsigned int objectIds;
	static unsigned int defNodeIds;
	static unsigned int defGroupIds;
	static unsigned int pointDataIds;
	static unsigned int boneIds;
	static unsigned int generalIds;

	map<int, int> defIds; // tranlation for change IDs
	shadingNode* currentSelection;

	// Skeleton
	map<int, joint*> deformerRef; // A reference to all the joints from id.
	vector< skeleton* > skeletons;

	// Relation of Ids for best searching
	map<int,  object*> IDSRelation;

	// AirRig
	Rig* rig;
	vector<Rig*> rigsArray;

	procState state;

	// Scene state flags
	bool modelLoaded;
	bool skeletonLoaded;
	bool embeddingLoaded;
	bool weightsUpdated;

	// Global scene scale, for unifying all the processes
	float sceneScale;

	// Scene draw flags
	static bool drawDefNodes;
	static float drawingNodeStdSize;

	double weightsThreshold;

	bool gridLoadedFromDisc;
	bool initialized;
	bool evaluate;

	void ( *fPrintText)(char*);
	void ( *fPrintProcessStatus)(int);

	int iVisMode;
	int desiredVertex;
};

class camera : object
{
    public:
        Eigen::Vector3d target;
};

#endif // SCENE_H
