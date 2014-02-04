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

enum toolmode{T_MOVETOOL, T_SCALETOOL, T_ROTATETOOL, T_SELECTTOOL, T_CREATE_SKELETON_TOOL,T_TRANSFORMATION_TOOL, T_ANIMATION_TOOL, T_RIGGING_TOOL, T_TESTING_TOOL};
enum toolmodeSpecific { BTN_RIGG = 0, BTN_ANIM, BTN_TEST};

class AdriMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdriMainWindow(QWidget *parent = 0);
    ~AdriMainWindow();
	void connectSignals();

	double rotationX, rotationY, rotationZ;
	bool rotateTranlsateFlag;

public slots:
    void changeSlider(int);
    void OpenNewScene();

	void SaveScene();
	void SaveSceneCompact();
	void ClearScene();

	void LaunchTests();
    void ImportNewModel();
    void CloseApplication();
    void enableStillCage(bool);

    void ImportCages();
    void ImportDistances();

    void ShadingModeChange(int option);

    void DataVisualizationChange(int idx);
    void toogleVisibility(int);

    void EnableColorLayer(bool _b);
    void ChangeColorLayerValue(int value);

    void DoAction();
    void ChangeSourceVertex();
    void distancesSourceValueChange(int);

    virtual void updateSceneView();
    void updateSceneView(TreeItem* treeitem, treeNode* treenode);
    void selectObject(QModelIndex idx);

    void toogleMoveTool();
    void toogleRotateTool();
    void toogleSelectionTool();
    void toogleCreateSkeletonTool();

	void setRiggingTool();
	void setAnimTool();
	void setTestTool();

	void changeTool(toolmode newtool);
	

   // void updateSmoothSlidervalue(int);

	void changeInteriorPointPosition();
    void updateExpansionSlidervalue(int);
    void jointDataUpdate(float fvalue, int id);
	void NodeDataUpdate(float iniTw, float finTw, bool enableTw, bool smoothTw, int smooth);
	void jointTransformUpdate(float x,float y,float z,float alpha,float beta,float gamma);
	
	virtual void changeAuxValueInt(int value);

	void changeVisModeForPlane(int);
	void changeSelPointForPlane(int);

	void UpdateScene();

	// Transform: Transform values for joints
    void changeTransformRotateAmountX(int);
    void changeTransformRotateAmountY(int);
    void changeTransformRotateAmountZ(int);

	// Rotation: rotation values for joints
	void changeTransformTranslateAmountX(int);
    void changeTransformTranslateAmountY(int);
    void changeTransformTranslateAmountZ(int);

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

	void changeTwistParameters();
	void changeTwistParameters(int value);
	void changeBulgeParameters();

protected:
    virtual void keyPressEvent(QKeyEvent* event);

public:
    Ui::MainWindow *ui;

    toolmode toolSelected;
};

#endif