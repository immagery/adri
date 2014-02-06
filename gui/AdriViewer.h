#ifndef ADRIVIEWER_H
#define ADRIVIEWER_H

//Std libraries
#include <vector>

#include <QtOpenGL/QGLWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QGLViewer/qglviewer.h>

#include "QGLViewer/constraint.h"

#include "DataStructures/DataStructures.h"
#include "DataStructures/Scene.h"
#include "DataStructures/Object.h"
#include "DataStructures/skeleton.h"
#include "DataStructures\Rig.h"
#include "DataStructures\Skinning.h"
#include "DataStructures/Modelo.h"
#include "DataStructures/AnimationManager.h"
#include "DataStructures/Particles.h"

#include <render/gridRender.h>
#include "SelectionManager.h"
#include "DrawObject.h"


#define INFO_STRING "Info > %1"
#define DEBUG_MODE 0
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
using namespace Eigen;

enum viewingModes { DynCage_Mode = 0, Cages_Mode, BHD_Mode, SimpleModel_Mode};

class AdriMainWindow;
class MainWindow;
class Particles;

class AdriViewer : public QGLViewer
{
    Q_OBJECT

public:
    AdriViewer(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags flags = 0);
    ~AdriViewer();

    QSize minimumSizeHint() const { return QSize(50, 50); }
    QSize sizeHint() const { return QSize(400, 400); }

    virtual void init();
    virtual void animate();

    // LOADING
    void readModel(string fileName, string name, string path);
    virtual void readScene(string fileName, string name, string path);
	virtual void saveScene(string fileName, string name, string path, bool compactMode = false){}
//    void readDistances(QString fileName);
    void readSkeleton(string fileName);

    //float calculateDistancesForISOLines(grid3d* grid, vector<double>&  embeddedPoint);

    //void readCage(QString fileName, Modelo& m_);

    // DRAWING FUNCTIONS
    void ReBuildScene();
    void drawSceneGrid(int lines, double width);
    void drawCube(Vector3d o, double cellSize, Vector3f color, bool blend = false);
    void initScene();

    //SELECTION
    virtual void selectElements(vector<unsigned int > lst);

    void testScene();

    void drawColored(QVector<QColor> colors);

    // VARIABLES
    Vector3d m_ptCenter; // centro de la escena
    float m_sceneMin[3];
    float m_sceneMax[3];

    float m_sceneRadius;

    viewingModes viewingMode;
    shadingModes ShadingModeFlag;

	bool bDrawStatistics;

    void cleanShadingVariables();
    void toogleModelToXRay();
    void toogleModelToLines();
    void toogleVisibility(bool toogle);


	virtual void setSmoothPasses(int , int ) {}
	virtual void setTwistParams(double , double , bool, bool ){}
	virtual void setBulgeParams( bool ){}

//    void changeVisualizationMode(int);
//	void updateGridVisualization();
//    void toogleToShowSegmentation(bool toogle);

//    void changeSmoothPropagationDistanceRatio(float smoothRatioValue);
//	void cleanWeights(gridRenderer* grRend);
//    void changeExpansionFromSelectedJoint(float expValue);

    // State flags
    bool loadedModel;
    bool loadedCages;

    bool colorsLoaded;
    bool colorLayers;

    int colorLayerIdx;

    bool firstInit; // Flag por si es necesario hacer refresh
    bool refreshFlag;
    bool firstDrawing;

    bool bGCcomputed;
    bool bHComputed;
    bool bMVCComputed;

	Eigen::Vector3d interiorPoint;

    // Primitives
    //MyMesh simpleCube;
    //MyMesh littleCube;

	int valueAux;

    // Transform
    // Animation
    AnimationManager aniManager;
    int frame;
	Particles* particles;

    //vector< object*> modelos; // Todos los objetos que tendremos en la escena
    scene* escena; // Escena con jerarquía

    selectionManager selMgr;

    bool drawCage;
    bool shadeCoordInfluence;
    bool showDeformedModel;

    unsigned int influenceDrawingIdx;

    vector<float> selectionValuesForColorGC;
    vector<float> selectionValuesForColorHC;
    vector<float> selectionValuesForColorMVC;

    Eigen::Vector2f maxMinGC;
    Eigen::Vector2f maxMinHC;
    Eigen::Vector2f maxMinMVC;

    vector< vector<QColor> > vertexColorLayers;

    vector<QColor> vertexColors;

    vector< vector<double> > BHD_distancias;
    vector< int > BHD_indices;

    bool stillCageAbled;
    int stillCageSelected;

	vector<joint*> CurrentProcessJoints;


    QString sPathGlobal;

    bool updateInfo();

    AdriMainWindow *parent;
protected:
    virtual void drawWithNames();
    virtual void draw();

//    void drawModel();
//    void drawWithDistances();
//    void drawWithCages();

	virtual void postSelection(const QPoint& point){}
	virtual void endSelection(const QPoint&);

    // Mouse events functions
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);


public slots:

	virtual void changeVisualizationMode(int mode);
	void UpdateVertexSource(int id);
	void ChangeStillCage(int id);
	void loadSelectableVertex(Cage* cage);
	//void loadSelectVertexCombo(MyMesh& cage);
	void changeVertexSelection(int id);

	void paintGrid(gridRenderer* grRend);

	void doTests(string fileName, string name, string path);
	float calculateDistancesForISOLines(grid3d* grid, vector<double>&  embeddedPoint);

    // GENERAL
    //void ChangeSliceXZ(int slice);
    //void ChangeSliceXY(int slice);
//protected:
	virtual void paintModelWithData();


    void paintModelWithGrid();
    //void paintPlaneWithData(bool compute = false);
	//void setPlaneData(bool drawPlane, int pointPos, int mode, float sliderPos, int orient);

    bool readNodes(vector< string >& nodeNames, vector< Eigen::Vector3d >& nodePoints, QString sFile);
    bool readPoints(vector< Eigen::Vector3d >& points, QString sFile);

    //void exportWeightsToMaya();

    //void computeProcess();
    //void doTests(string fileName, string name, string path);
//	void getStatisticsFromData(string fileName, string name, string path);

    //void VoxelizeModel(Modelo* m, bool onlyBorders = true);

	void setPlanePosition(float x, float y, float z)
	{
	
	}

//	void setThreshold(double value);

    //void ComputeSkining(Modelo* m, gridRenderer* grRend);
    //void updateSkinning(gridRenderer* grRend);

    //void updateSkinningWithHierarchy(gridRenderer* grRend);

//	void nextProcessStep();
//	void allNextProcessSteps();

	//void PropagateFromSegment(gridRenderer* grRend, int frontId);
	//void initDomainForId(gridRenderer* grRend, int fatherId); // Duplicated -> TODELETE
	//void initGridForHierarchicalskinning(gridRenderer* grRend, int domainId_init);
	//void NormalizeWeights(gridRenderer* grRend, vector<int>& frontIds);

	//void computeHierarchicalSkinning(gridRenderer* grRend); // first Step
    //void computeHierarchicalSkinning(gridRenderer* grRend, joint* jt); // recursive computation.

    //void createTraductionTable(joint* jt, map<int, int> &traductionTable, int idNode);

//    void PropFunctionConf();
	
//    void paintGrid(gridRenderer* grRend);

//    void ChangeViewingMode(int);

    // CAGES
//    bool processGreenCoordinates();
//    bool processHarmonicCoordinates();
//    bool processMeanValueCoordinates();
//    bool processAllCoords();
//    bool deformMesh();
//    void loadSelectableVertex(Cage* cage /* MyMesh& cage*/);
//    void showDeformedModelSlot();
//    void showHCoordinatesSlot();
//    void active_GC_vs_HC(int tab);
//    void ChangeSt illCage(int id);

//    void updateGridRender();

	void updateGridRender();

    // Biharmonic Distances
    //void UpdateVertexSource(int id); // Updates the source for harmonic distances testing
    //void importSegmentation(QString fileName);
    //void updateColorLayersWithSegmentation(int maxIdx);

	virtual void setContextMode(contextMode ctx){}
	virtual void endContextMode(contextMode ctx){}

	virtual void setToolCrtMode(int ctx){}
	virtual void setTool(ToolType ctx){}

    //void loadSelectVertexCombo(MyMesh& cage);
    //void changeVertexSelection(int id);

    // Transform

signals:
    void updateSceneView();
    void jointDataShow(float, int);
	void defGroupData(float, float, bool, bool, int);
	void jointTransformationValues(float, float,float,float,float,float);
    void changedFrame(int);

	void setCreateToolUI();
	void setRiggingToolUI();
	void setAnimToolUI();
	void setTestToolUI();

private :
  void startManipulation();
  void drawSelectionRectangle() const;
  void addIdToSelection(int id);
  void removeIdFromSelection(int id);

  // Current rectangular selection
  QRect rectangle_;

  // Different selection modes
  enum SelectionMode { NONE, ADD, REMOVE };
  SelectionMode selectionMode_;

  QList<DrawObject*> objects_;
  QList<int> selection_;

};

#endif
