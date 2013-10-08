#ifndef ADRIMAINWINDOW_H
#define ADRIMAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QModelIndex>

#include <ui_mainwindow.h>


class TreeModel;
class TreeItem;
class treeNode;

namespace Ui {
    class MainWindow;
}

enum toolmode{T_MOVETOOL, T_ROTATETOOL, T_SELECTTOOL};

class AdriMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdriMainWindow(QWidget *parent = 0);
    ~AdriMainWindow();

public slots:
    void changeSlider(int);
    void OpenNewScene();
	void LaunchTests();
    void ImportNewModel();
    void CloseApplication();
    void enableStillCage(bool);

    void ImportCages();
    void ImportDistances();

    void ShadingModeChange(int option);

    void DataVisualizationChange(int idx);
    void toogleVisibility(bool toogle);

    void EnableColorLayer(bool _b);
    void ChangeColorLayerValue(int value);

    void DoAction();
    void ChangeSourceVertex();
    void distancesSourceValueChange(int);

    void updateSceneView();
    void updateSceneView(TreeItem* treeitem, treeNode* treenode);
    void selectObject(QModelIndex idx);

    void toogleMoveTool();
    void toogleRotateTool();
    void toogleSelectionTool();
    void changeTool(toolmode newtool);

    void updateSmoothSlidervalue(int);

	void changeInteriorPointPosition();
    void updateExpansionSlidervalue(int);
    void jointDataUpdate(float fvalue, int id);
	

	void changeVisModeForPlane(int);
	void changeSelPointForPlane(int);

	void UpdateScene();

	// Transform: rotation values for joints
    void changeTransformRotateAmountX(int);
    void changeTransformRotateAmountY(int);
    void changeTransformRotateAmountZ(int);
    void resetRotationValues();

    // Animation: keyframe managing, animation I/O
    void addAnimationKeyframe();
    void toggleAnimation();
    void changeFrame(int);
    void changeAnimSlider(int);
	void saveAnimation();
	void loadAnimation();

	// Simulation
	void toggleSimulation();

protected:
    virtual void keyPressEvent(QKeyEvent* event);

public:
    Ui::MainWindow *ui;

    toolmode toolSelected;
};

#endif