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

enum shaderIdx {SHD_BASIC = 0, SHD_XRAY, SHD_VERTEX_COLORS, SHD_NO_SHADE, SHD_LENGTH};

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

    virtual void initViewer(){}

    // LOADING
    void readModel(string fileName, string name, string path);
    virtual void readScene(string fileName, string name, string path);
	virtual void saveScene(string fileName, string name, string path, bool compactMode = false){}
    void readSkeleton(string fileName);

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

	virtual void setShaderConfiguration( shaderIdx type){}
	virtual void setSmoothPasses(int , int ) {}
	virtual void setTwistParams(double , double , bool, bool ){}
	virtual void setBulgeParams( bool ){}

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
	int valueAux;

    // Transform
    // Animation
    AnimationManager aniManager;
    int frame;
	Particles* particles;

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
	void changeVertexSelection(int id);

	void paintGrid(gridRenderer* grRend);

	virtual void doTests(string fileName, string name, string path);
	float calculateDistancesForISOLines(grid3d* grid, vector<double>&  embeddedPoint);

	virtual void paintModelWithData();
    void paintModelWithGrid();
    bool readNodes(vector< string >& nodeNames, vector< Eigen::Vector3d >& nodePoints, QString sFile);
    bool readPoints(vector< Eigen::Vector3d >& points, QString sFile);

	void setPlanePosition(float x, float y, float z)
	{
	
	}

	void updateGridRender();

	virtual void setContextMode(contextMode ctx){}
	virtual void endContextMode(contextMode ctx){}

	virtual void setToolCrtMode(int ctx){}
	virtual void setTool(ToolType ctx){}

	// UI INFO INTERFACE
	void showInfo(string str);
	void showBarProgress(int value);

	virtual void setSceneScale(float scale){}

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

	void defGroupValueUpdate(float ,bool , bool ,int ,float ,float );
	void labelsFromSelectedUpdate(int, string);

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
