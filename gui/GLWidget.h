#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "AdriViewer.h"

#include "DataStructures/DataStructures.h"
#include "DataStructures/Scene.h"
#include "DataStructures/Object.h"
#include "DataStructures/skeleton.h"
#include "DataStructures/Modelo.h"
#include "DataStructures/grid3D.h"

#include "render/gridRender.h"
#include "render/clipingPlaneRender.h"
#include "gui/mainwindow.h"

using namespace std;

/*class MainWindow;
class Object;
class Cage;
class scene;
class skeleton;
class joint;
//class gridRenderer;
class Modelo;
class grid3d;*/

class GLWidget : public AdriViewer
{
public:
    GLWidget(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags flags = 0);

    gridRenderer* currentProcessGrid;

    // Métodos específicos
    void paintModelWithData();
    // void paintPlaneWithData(bool compute = false);
    void doTests(string fileName, string name, string path);
    void computeProcess();
    void VoxelizeModel(Modelo *m, bool onlyBorders = true);
    void exportWeightsToMaya();
    void UpdateVertexSource(int id);
    void importSegmentation(QString fileName);
    void updateColorLayersWithSegmentation(int maxIdx);


    // CAGES
    bool processGreenCoordinates();
    bool processHarmonicCoordinates();
    bool processMeanValueCoordinates();
    void active_GC_vs_HC(int tab);
    bool processAllCoords();
    bool deformMesh();
    void loadSelectableVertex(Cage* cage /* MyMesh& cage*/);
    void showDeformedModelSlot();
    void showHCoordinatesSlot();
    void ChangeStillCage(int id);

    float calculateDistancesForISOLines(grid3d* grid, vector<double>&  embeddedPoint);
    void paintGrid(gridRenderer* grRend);
    void updateGridVisualization();
    void changeVisualizationMode(int);

    void cleanWeights(gridRenderer* grRend);
    void changeSmoothPropagationDistanceRatio(float smoothRatioValue);

    void PropFunctionConf();
    void setPlaneData(bool drawPlane, int pointPos, int mode, float sliderPos, int orient);

    void paintModelWithGrid();
    void setThreshold(double value);

    void nextProcessStep();
    void allNextProcessSteps();

    void getStatisticsFromData(string fileName, string name, string path);
    void readDistances(QString fileName);

    void toogleToShowSegmentation(bool toogle);
    void changeExpansionFromSelectedJoint(float expValue);

    void drawModel();
    void drawWithDistances();
    void drawWithCages();

    void ChangeViewingMode(int);
    void updateGridRender();


protected:
    virtual void postSelection(const QPoint& point);

public slots:

    // GENERAL
    void ChangeSliceXZ(int slice);
    void ChangeSliceXY(int slice);

};

#endif // GLWIDGET_H
