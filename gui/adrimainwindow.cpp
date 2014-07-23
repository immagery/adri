#include "adrimainwindow.h"

#include <QtWidgets/QToolButton>
#include <QtWidgets/QToolBar>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>

#include <QtWidgets/QTreeView>
#include <QtGui/QStandardItem>

#include <gui/treeitem.h>
#include <gui/treemodel.h>

#include <gui/outliner.h>
#include <DataStructures/modelo.h>

#include <utils/util.h>
#include <globalDefinitions.h>

#include "AdriViewer.h"

using namespace std;
using namespace Eigen;

void AdriMainWindow::connectSignals() {
	// conexiones
    connect(ui->action_importModel, SIGNAL(triggered()), this, SLOT(ImportNewModel()) );
    connect(ui->actionAction_openScene, SIGNAL(triggered()), this, SLOT(OpenNewScene()));
	connect(ui->actionAction_cleanScene, SIGNAL(triggered()), this, SLOT(ClearScene()));
	connect(ui->actionAction_saveScene, SIGNAL(triggered()), this, SLOT(SaveScene()));
	connect(ui->actionAction_saveScene_compact, SIGNAL(triggered()), this, SLOT(SaveSceneCompact()));

    connect(ui->import_cage_s, SIGNAL(triggered()), this, SLOT(ImportCages()) );
    connect(ui->import_distances, SIGNAL(triggered()), this, SLOT(ImportDistances()) );

    connect(ui->shadingModeSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(ShadingModeChange(int)) );
    connect(ui->colorLayersCheck, SIGNAL(toggled(bool)), this, SLOT(EnableColorLayer(bool)) );
    connect(ui->ColorLayerSeletion, SIGNAL(valueChanged(int)), this, SLOT(ChangeColorLayerValue(int)) );
    connect(ui->actionImportSegementation, SIGNAL(triggered()), this, SLOT(DoAction()) );
    connect(ui->DistancesVertSource, SIGNAL(valueChanged(int)), this, SLOT(distancesSourceValueChange(int)));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(CloseApplication()) );


	connect(ui->GridDraw_interior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->GridDraw_exterior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->allGrid_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->gridSlices_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->SliceSelectorXY, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXY(int)));
    connect(ui->SliceSelectorXZ, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXZ(int)));

    connect(ui->cagesComboBox, SIGNAL(currentIndexChanged(int)), ui->glCustomWidget, SLOT(ChangeStillCage(int)));
    connect(ui->enableStillCage, SIGNAL(toggled(bool)), this, SLOT(enableStillCage(bool)));
    connect(ui->GridDraw_boundary, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->paintModel_btn, SIGNAL(released()), ui->glCustomWidget, SLOT(paintModelWithGrid()));
    connect(ui->glCustomWidget, SIGNAL(updateSceneView()), this, SLOT(updateSceneView()));
    connect(ui->outlinerView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectObject(QModelIndex)));
	connect(ui->actionSelection, SIGNAL(triggered()), this, SLOT(toogleSelectionTool()));
    connect(ui->actionMove, SIGNAL(triggered()), this, SLOT(toogleMoveTool()));
    connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(toogleRotateTool()));

	// Air create skeletons tools
	connect(ui->actionCreateSkeletonTool, SIGNAL(triggered()), this, SLOT(toogleCreateSkeletonTool()));

	connect(ui->CreateToolBtn, SIGNAL(released()), this, SLOT(setCreateTool()));
	connect(ui->RiggingToolBtn, SIGNAL(released()), this, SLOT(setRiggingTool()));
	connect(ui->AnimToolBtn, SIGNAL(released()), this, SLOT(setAnimTool()));
	connect(ui->TestToolBtn, SIGNAL(released()), this, SLOT(setTestTool()));

	connect(ui->glCustomWidget, SIGNAL(setCreateToolUI()), this, SLOT(setCreateToolUI()));
	connect(ui->glCustomWidget, SIGNAL(setRiggingToolUI()), this, SLOT(setRiggingToolUI()));
	connect(ui->glCustomWidget, SIGNAL(setAnimToolUI()), this, SLOT(setAnimToolUI()));
	connect(ui->glCustomWidget, SIGNAL(setTestToolUI()), this, SLOT(setTestToolUI()));

    connect(ui->visibility_btn, SIGNAL(stateChanged(int)), this, SLOT(toogleVisibility(int)));
	connect(ui->drawSupportInfo, SIGNAL(stateChanged(int)), this, SLOT(toogleVisibility(int)));
	connect(ui->defNodesSize, SIGNAL(valueChanged(int)), this, SLOT(toogleVisibility(int)));

	connect(ui->Analisis_check_box, SIGNAL(clicked()), this, SLOT(analisisDataShow()));

	connect(ui->vertex_colors_btn, SIGNAL(stateChanged(int)), this, SLOT(toogleVisibility(int)));

	connect(ui->actionDoTests, SIGNAL(triggered()), this, SLOT(LaunchTests()));

    connect(ui->segmentation_btn, SIGNAL(toggled(bool)), this, SLOT(toogleToShowSegmentation(bool)));
    connect(ui->DataVisualizationCombo, SIGNAL(currentIndexChanged(int)),this, SLOT(DataVisualizationChange(int)) );

    connect(ui->exportWeights_btn, SIGNAL(released()), ui->glCustomWidget, SLOT(exportWeightsToMaya()));

    connect(ui->expansionSlider, SIGNAL(valueChanged(int)), this, SLOT(updateExpansionSlidervalue(int)));
	connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(updateThresholdSlidervalue(int)));
	connect(ui->threshold_enable, SIGNAL(toggled(bool)), this, SLOT(enableThreshold(bool)));
	connect(ui->glCustomWidget, SIGNAL(defGroupData(float, float, bool, bool, int)), this,
							    SLOT(NodeDataUpdate(float, float, bool, bool, int)));

	connect(ui->glCustomWidget, SIGNAL(defGroupValueUpdate(float ,bool , bool ,int ,float ,float )), this,
								  SLOT(defGroupValueUpdate(float ,bool , bool ,int ,float ,float )));

	connect(ui->glCustomWidget, SIGNAL(labelsFromSelectedUpdate(int ,string )), this,
								  SLOT(labelsFromSelectedUpdate(int ,string )));

	connect(ui->glCustomWidget, SIGNAL(jointTransformationValues(float,float,float,float,float,float)), this , 
								SLOT(jointTransformUpdate(float,float,float,float,float,float)));

	connect(ui->ip_axisX, SIGNAL(valueChanged(int)), this, SLOT(changeInteriorPointPosition()));
	connect(ui->ip_axisY, SIGNAL(valueChanged(int)), this, SLOT(changeInteriorPointPosition()));
	connect(ui->ip_axisZ, SIGNAL(valueChanged(int)), this, SLOT(changeInteriorPointPosition()));

	connect(ui->paintModel, SIGNAL(clicked()), this, SLOT(updateModelColors()));
	connect(ui->paintPlane, SIGNAL(clicked()), this, SLOT(updateClipingPlaneColor()));

	connect(ui->drawPlaneCheck, SIGNAL(clicked()), this, SLOT(updateClipingPlaneData()));
	connect(ui->PlaneOrientX, SIGNAL(clicked()), this, SLOT(updateClipingPlaneData()));
	connect(ui->planeOrientY, SIGNAL(clicked()), this, SLOT(updateClipingPlaneData()));
	connect(ui->planeOrientZ, SIGNAL(clicked()), this, SLOT(updateClipingPlaneData()));
	connect(ui->PlaneDataCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeVisModeForPlane(int)));
	
	connect(ui->dataSource, SIGNAL(valueChanged(int)), this, SLOT(distancesSourceValueChange(int)));
	connect(ui->positionPlaneSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeSelPointForPlane(int)));

	// Rotation
	connect(ui->dialX, SIGNAL(valueChanged(int)), this, SLOT(changeTransformRotateAmountX(int)));
	connect(ui->dialY, SIGNAL(valueChanged(int)), this, SLOT(changeTransformRotateAmountY(int)));
    connect(ui->dialZ, SIGNAL(valueChanged(int)), this, SLOT(changeTransformRotateAmountZ(int)));

	// Twist control
	connect(ui->twistEnableCheck, SIGNAL(clicked()), this, SLOT(changeTwistParameters()));
	connect(ui->smoothTwistCheck, SIGNAL(clicked()), this, SLOT(changeTwistParameters()));
	connect(ui->twist_fin_slider, SIGNAL(sliderMoved(int)), this, SLOT(changeTwistParameters(int)));
	connect(ui->twist_ini_slider, SIGNAL(sliderMoved(int)), this, SLOT(changeTwistParameters(int)));

	// Bulge control
	connect(ui->bulgeEffectActivation, SIGNAL(clicked()), this, SLOT(changeBulgeParameters()));

	// Transform 
	connect(ui->translationAmountX, SIGNAL(valueChanged(int)), this, SLOT(changeTransformTranslateAmountX(int)));
    connect(ui->translationAmountY, SIGNAL(valueChanged(int)), this, SLOT(changeTransformTranslateAmountY(int)));
    connect(ui->translationAmountZ, SIGNAL(valueChanged(int)), this, SLOT(changeTransformTranslateAmountZ(int)));
	
	connect(ui->resetRotation, SIGNAL(clicked()), this, SLOT(resetRotationValues()));

    // Animation
    connect(ui->addKeyframe, SIGNAL(clicked()), this, SLOT(addAnimationKeyframe()));
    connect(ui->toggleAnim, SIGNAL(clicked()), this, SLOT(toggleAnimation()));
    connect(ui->animSlider, SIGNAL(valueChanged(int)), this, SLOT(changeFrame(int)));
    connect(ui->glCustomWidget, SIGNAL(changedFrame(int)), this, SLOT(changeAnimSlider(int)));
	connect(ui->saveAnim, SIGNAL(clicked()), this, SLOT(saveAnimation()));
	connect(ui->loadAnim, SIGNAL(clicked()), this, SLOT(loadAnimation()));

	// Simulation
	connect(ui->toggleSim, SIGNAL(clicked()), this, SLOT(toggleSimulation()));
}

void AdriMainWindow::analisisDataShow()
{
	bool enableAnalisisShow = ui->Analisis_check_box->isChecked();

	ui->glCustomWidget->m_bShowAnalisis = enableAnalisisShow;

}

void AdriMainWindow::changeTwistParameters()
{
	// enable/disable
	//ui->twist_fin_slider->setEnabled(ui->twistEnableCheck->isChecked());
	//ui->twist_ini_slider->setEnabled(ui->twistEnableCheck->isChecked());
	ui->glCustomWidget->setTwistParams(ui->twist_ini_slider->value()/1000.0, 
		ui->twist_fin_slider->value()/1000.0, 
		ui->twistEnableCheck->isChecked(),
		ui->smoothTwistCheck->isChecked());
}

void AdriMainWindow::changeTwistParameters(int value)
{
	changeTwistParameters();
}

void AdriMainWindow::changeBulgeParameters()
{
	ui->glCustomWidget->setBulgeParams(ui->bulgeEffectActivation->isChecked());
}

AdriMainWindow::AdriMainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
	rotateTranlsateFlag = false;

	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
}


void AdriMainWindow::changeSlider(int)
{

}

void AdriMainWindow::ImportNewModel()
{
    QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.off *.txt");
    inFileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    QFileInfo sPathAux(fileNames[0]);
    QString aux = sPathAux.canonicalPath();
    QString sModelPath = aux;
    int ini = fileNames[0].indexOf("/",aux.length());
    aux = fileNames[0].right(fileNames[0].length()-ini);
    QString sModelPrefix = aux.left(aux.length()-4);

    ui->glCustomWidget->readModel(fileNames[0].toStdString(), sModelPrefix.toStdString(), sModelPath.toStdString());
}

void AdriMainWindow::SaveScene()
{
    QFileDialog inFileDialog(0, "Especifica el directorio", ui->glCustomWidget->sPathGlobal, "*.*");
	inFileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    QFileInfo sPathAux(fileNames[0]);
    QString aux = sPathAux.canonicalPath();
    QString sModelPath = aux;
    int ini = fileNames[0].indexOf("/",aux.length());
    aux = fileNames[0].right(fileNames[0].length()-ini);
    QString sModelPrefix = aux.left(aux.length()-4);

    ui->glCustomWidget->saveScene(fileNames[0].toStdString(), sModelPrefix.toStdString(), sModelPath.toStdString());
}

void AdriMainWindow::SaveSceneCompact()
{
    QFileDialog inFileDialog(0, "Especifica el directorio", ui->glCustomWidget->sPathGlobal, "*.*");
	inFileDialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    QFileInfo sPathAux(fileNames[0]);
    QString aux = sPathAux.canonicalPath();
    QString sModelPath = aux;
    int ini = fileNames[0].indexOf("/",aux.length());
    aux = fileNames[0].right(fileNames[0].length()-ini);
    QString sModelPrefix = aux.left(aux.length()-4);

    ui->glCustomWidget->saveScene(fileNames[0].toStdString(), sModelPrefix.toStdString(), sModelPath.toStdString(), true);
}

void AdriMainWindow::ClearScene()
{

}

void AdriMainWindow::OpenNewScene()
{
    //QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.txt, *.snk");
	QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "");
    inFileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    QFileInfo sPathAux(fileNames[0]);
    QString aux = sPathAux.canonicalPath();
    QString sModelPath = aux;
    int ini = fileNames[0].indexOf("/",aux.length());
    aux = fileNames[0].right(fileNames[0].length()-ini);
    QString sModelPrefix = aux.left(aux.length()-4);

    ui->glCustomWidget->readScene(fileNames[0].toStdString(), sModelPrefix.toStdString(), sModelPath.toStdString());

	updateSceneView();
}

void AdriMainWindow::LaunchTests()
{
    QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.txt");
    inFileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    QFileInfo sPathAux(fileNames[0]);
    QString aux = sPathAux.canonicalPath();
    QString sModelPath = aux;
    int ini = fileNames[0].indexOf("/",aux.length());
    aux = fileNames[0].right(fileNames[0].length()-ini);
    QString sModelPrefix = aux.left(aux.length()-4);

	ui->glCustomWidget->doTests(fileNames[0].toStdString(), sModelPrefix.toStdString(), sModelPath.toStdString());
}

/*
void AdriMainWindow::updateSmoothSlidervalue(int)
{
    float valueAux = ui->smoothPropagationSlider->value();
    float value = valueAux/10.0;

    ui->smoothPropagationEdit->setText(QString("%1").arg(value));
}
*/
void AdriMainWindow::updateExpansionSlidervalue(int)
{
    float valueAux = ui->expansionSlider->value();
    float value = 0;
    
	/*if(valueAux <= 100)
        value = ((float)ui->expansionSlider->value())/100.0;
    else
    {
        value = (((valueAux-100)/100)*9)+1.0;
    }*/

	value = valueAux / 1000.0;

    ui->expansionValueEdit->setText(QString("%1").arg(value));
}

void AdriMainWindow::changeVisModeForPlane(int)
{	
	//updateClipingPlaneData();
}

void AdriMainWindow::changeSelPointForPlane(int)
{
	//updateClipingPlaneData();
}

void AdriMainWindow::changeInteriorPointPosition()
{
    float valueAuxX = ui->ip_axisX->value();
	float valueAuxY = ui->ip_axisY->value();
	float valueAuxZ = ui->ip_axisZ->value();

	ui->axisX_edit->setText(QString("%1").arg(valueAuxX));
	ui->axisY_edit->setText(QString("%1").arg(valueAuxY));
	ui->axisZ_edit->setText(QString("%1").arg(valueAuxZ));

	ui->glCustomWidget->interiorPoint = Eigen::Vector3d(valueAuxX,valueAuxY,valueAuxZ);
	ui->glCustomWidget->setPlanePosition(valueAuxX,valueAuxY,valueAuxZ);
}

void AdriMainWindow::jointTransformUpdate(float x,float y,float z,float alpha,float beta,float gamma)
{
	/*
	ui->translationAmountX->setValue(x*10.0);
	ui->translationEditX->setText(QString("%1").arg(x));

	ui->translationAmountY->setValue(y*10.0);
	ui->translationEditY->setText(QString("%1").arg(y));

	ui->translationAmountZ->setValue(z*10.0);
	ui->translationEditZ->setText(QString("%1").arg(z));
	*/


	ui->dialX->setValue(alpha*10.0);
	ui->dialX->setSliderPosition(alpha*10.0);
	ui->rotationEditX->setText(QString("%1").arg(alpha));

	ui->dialY->setValue(beta*10.0);
	ui->dialY->setSliderPosition(beta*10.0);
	ui->rotationEditY->setText(QString("%1").arg(beta));

	ui->dialZ->setValue(gamma*10.0);
	ui->dialZ->setSliderPosition(gamma*10.0);
	ui->rotationEditZ->setText(QString("%1").arg(gamma));

	rotationX = Deg2Rad(alpha);
	rotationY = Deg2Rad(beta);
	rotationZ = Deg2Rad(gamma);
}

void AdriMainWindow::labelsFromSelectedUpdate(int nodeId, string sName)
{
	ui->def_group_id->setText(QString("%1").arg(nodeId));
	ui->def_groupName->setText(QString("%1").arg(sName.c_str()));
}

void AdriMainWindow::defGroupValueUpdate(float expansion, bool twistEnabled, bool bulgeEnabled, 
							int localSmoothPases, float twistIni, float twistFin)
{
	ui->twistEnableCheck->setChecked(twistEnabled);

	ui->expansionValueEdit->setText(QString("%1").arg(expansion));
	ui->expansionSlider->setValue(expansion*1000);

	ui->bulgeEffectActivation->setChecked(bulgeEnabled);

	ui->twist_ini_slider->setValue((int)(twistIni*1000));
	ui->twist_fin_slider->setValue((int)(twistFin*1000));

	ui->localSmoothingPasses->setValue(localSmoothPases);

}

void AdriMainWindow::NodeDataUpdate(float iniTw, float finTw, bool enableTw, bool smoothTw, int smooth)
{
	ui->twistEnableCheck->setChecked(enableTw);

	ui->smoothTwistCheck->setChecked(smoothTw);

	ui->twist_ini_slider->setValue((int)(iniTw*1000));
	ui->twist_fin_slider->setValue((int)(finTw*1000));

	ui->localSmoothingPasses->setValue(smooth);
}

void AdriMainWindow::jointDataUpdate(float fvalue, int id)
{
	/*
    if(fvalue <=1)
    {
		ui->expansionSlider->setValue((int)round(fvalue*100));
    }
    else
    {
        int value = ((int)round((fvalue-1)/9*100)+100);
        ui->expansionSlider->setValue(value);
    }*/

	int value = (int)round(fvalue*1000.0);
	if(value > 3000) value = 3000;
	if(value < 1) value = 1;

	ui->expansionSlider->setValue(value);

    ui->expansionValueEdit->setText(QString("%1").arg((float)value/1000.0));
	
    ui->DistancesVertSource->setValue(id);
    distancesSourceValueChange(id);
}

void AdriMainWindow::changeAuxValueInt(int value)
{
	ui->glCustomWidget->valueAux = value;
	ui->glCustomWidget->updateGridRender();
	ui->glCustomWidget->paintModelWithData();
}

void AdriMainWindow::ImportCages()
{
    /*
    QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.off , *.txt");
    inFileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    ui->glCustomWidget->readCage(fileNames[0], ui->glCustomWidget->m);
    */

    printf("Function deprecated\n"); fflush(0);
}

void AdriMainWindow::toogleMoveTool()
{
    ui->actionMove->setChecked(true);
    ui->actionRotate->setChecked(false);
    ui->actionSelection->setChecked(false);
    changeTool(T_MOVETOOL);
}

void AdriMainWindow::toogleRotateTool()
{
    ui->actionRotate->setChecked(true);
    ui->actionMove->setChecked(false);
    ui->actionSelection->setChecked(false);
    changeTool(T_ROTATETOOL);

}

void AdriMainWindow::toogleSelectionTool()
{
    ui->actionSelection->setChecked(true);
    ui->actionMove->setChecked(false);
    ui->actionRotate->setChecked(false);

    changeTool(T_SELECTTOOL);
}

void AdriMainWindow::toogleCreateSkeletonTool()
{
	ui->actionCreateSkeletonTool->setChecked(true);

	ui->actionSelection->setChecked(false);
    ui->actionMove->setChecked(false);
    ui->actionRotate->setChecked(false);

	if(ui->actionCreateSkeletonTool->isChecked())
	{
		changeTool(T_CREATE_SKELETON_TOOL);
		setRiggingTool();
		ui->RiggingToolBtn->setEnabled(false);
		ui->RiggingToolBtn->setEnabled(true);
		ui->AnimToolBtn->setEnabled(true);
		ui->TestToolBtn->setEnabled(true);
	}
	else
	{
		changeTool(T_TRANSFORMATION_TOOL);

		ui->CreateToolBtn->setEnabled(false);		
		ui->RiggingToolBtn->setEnabled(false);
		ui->AnimToolBtn->setEnabled(false);
		ui->TestToolBtn->setEnabled(false);
	}
}

void AdriMainWindow::setCreateToolUI()
{
	ui->CreateToolBtn->setChecked(true);
	ui->RiggingToolBtn->setChecked(false);
	ui->AnimToolBtn->setChecked(false);
	ui->TestToolBtn->setChecked(false);
}

void AdriMainWindow::setRiggingToolUI()
{
	ui->CreateToolBtn->setChecked(false);
	ui->RiggingToolBtn->setChecked(true);
	ui->AnimToolBtn->setChecked(false);
	ui->TestToolBtn->setChecked(false);
}

void AdriMainWindow::setAnimToolUI()
{
	ui->CreateToolBtn->setChecked(false);
	ui->RiggingToolBtn->setChecked(false);
	ui->AnimToolBtn->setChecked(true);
	ui->TestToolBtn->setChecked(false);
}

void AdriMainWindow::setTestToolUI()
{
	ui->CreateToolBtn->setChecked(false);
	ui->RiggingToolBtn->setChecked(false);
	ui->AnimToolBtn->setChecked(false);
	ui->TestToolBtn->setChecked(true);
}

void AdriMainWindow::setCreateTool()
{
	changeTool(T_CREATION_TOOL);
	ui->CreateToolBtn->setChecked(true);
	ui->RiggingToolBtn->setChecked(false);
	ui->AnimToolBtn->setChecked(false);
	ui->TestToolBtn->setChecked(false);
}

void AdriMainWindow::setRiggingTool()
{
	changeTool(T_RIGGING_TOOL);
	ui->RiggingToolBtn->setChecked(true);
	ui->AnimToolBtn->setChecked(false);
	ui->TestToolBtn->setChecked(false);
	ui->CreateToolBtn->setChecked(false);
}

void AdriMainWindow::setAnimTool()
{
	changeTool(T_ANIMATION_TOOL);
	ui->AnimToolBtn->setChecked(true);
	ui->RiggingToolBtn->setChecked(false);
	ui->TestToolBtn->setChecked(false);
	ui->CreateToolBtn->setChecked(false);
}

void AdriMainWindow::setTestTool()
{
	changeTool(T_TESTING_TOOL);
	ui->TestToolBtn->setChecked(true);
	ui->RiggingToolBtn->setChecked(false);
	ui->AnimToolBtn->setChecked(false);
	ui->CreateToolBtn->setChecked(false);
}

void AdriMainWindow::changeTool(toolmode newtool)
{
    switch(newtool)
    {
    case T_MOVETOOL:
        ui->glCustomWidget->setContextMode(CTX_MOVE);
        break;
    case T_SELECTTOOL:
        ui->glCustomWidget->setContextMode(CTX_SELECTION);
        break;
    case T_ROTATETOOL:
        ui->glCustomWidget->setContextMode(CTX_ROTATION);
        break;
	case T_SCALETOOL:
        ui->glCustomWidget->setContextMode(CTX_SCALE);
        break;

	case T_TRANSFORMATION_TOOL:
		ui->glCustomWidget->setTool(CTX_TRANSFORMATION);
        break;

	case T_CREATE_SKELETON_TOOL:
		ui->glCustomWidget->setTool(CTX_CREATE_SKT);
        break;
    case T_CREATION_TOOL:
        ui->glCustomWidget->setToolCrtMode(BTN_CREATE);
        break;
    case T_ANIMATION_TOOL:
		ui->glCustomWidget->setToolCrtMode(BTN_ANIM);
        break;
    case T_RIGGING_TOOL:
        ui->glCustomWidget->setToolCrtMode(BTN_RIGG);
        break;
    case T_TESTING_TOOL:
        ui->glCustomWidget->setToolCrtMode(BTN_TEST);
        break;

	default:
        printf("Hay algún problema con la seleccion de contexto.\n"); fflush(0);
        ui->glCustomWidget->setContextMode(CTX_SELECTION);
        break;
    }

    //printf("ha habido un cambio\n"); fflush(0);

}

void AdriMainWindow::ShadingModeChange(int option)
{
    ui->glCustomWidget->cleanShadingVariables();
    if (option == 0)
    {
        ui->glCustomWidget->ShadingModeFlag = SH_MODE_SMOOTH;
        ui->glCustomWidget->cleanShadingVariables();
    }

    if (option == 1)
    {
        ui->glCustomWidget->ShadingModeFlag = SH_MODE_FLAT;
        ui->glCustomWidget->cleanShadingVariables();
    }

    if (option == 2)
    {
        ui->glCustomWidget->ShadingModeFlag = SH_MODE_SMOOTH;
        ui->glCustomWidget->toogleModelToXRay();
    }

    if (option == 3)
    {
        ui->glCustomWidget->ShadingModeFlag = SH_MODE_FLAT;
        ui->glCustomWidget->toogleModelToLines();
    }
}

void AdriMainWindow::toogleVisibility(int)
{
	ui->glCustomWidget->toogleVisibility(ui->visibility_btn->isChecked());

	scene::drawDefNodes = ui->drawSupportInfo->isChecked();
	scene::drawingNodeStdSize = ui->defNodesSize->value()/1000.0;

	/*if(ui->vertex_colors_btn->isChecked())
		ui->glCustomWidget->preferredType = SHD_VERTEX_COLORS);
	else
		ui->glCustomWidget->setShaderConfiguration(SHD_BASIC);
*/
}

void AdriMainWindow::DataVisualizationChange(int mode)
{
	ui->glCustomWidget->changeVisualizationMode(mode);
}


void AdriMainWindow::EnableColorLayer(bool _b)
{
    ui->glCustomWidget->colorLayers = _b;
        if (_b) ui->glCustomWidget->colorLayerIdx = ui->ColorLayerSeletion->value();
}

void AdriMainWindow::ChangeColorLayerValue(int value)
{
    ui->glCustomWidget->colorLayerIdx = value;
}

void AdriMainWindow::updateSceneView(TreeItem* treeitem, treeNode* treenode)
{
    for(unsigned int i = 0; i< treenode->childs.size(); i++)
    {
        QList<QVariant> columnData;
        columnData << QString(((treeNode*)treenode->childs[i])->sName.c_str());// << ((treeNode*)treenode->childs[i])->nodeId;

        TreeItem* elem = new TreeItem(columnData, treeitem);
        elem->sName = ((treeNode*)treenode->childs[i])->sName;
        elem->item_id = ((treeNode*)treenode->childs[i])->nodeId;
        elem->type = ((treeNode*)treenode->childs[i])->type;

        updateSceneView(elem,treenode->childs[i]);

        treeitem->appendChild(elem);
    }

}

void AdriMainWindow::updateSceneView()
{
    //treeViewModel->clear(); // Limpiamos la lista.

    TreeModel* treeViewModel;
    treeViewModel = new TreeModel();
    ui->outlinerView->setModel(treeViewModel);

    treeNode* root = new treeNode();
    outliner outl(ui->glCustomWidget->escena);
    outl.getSceneTree(root);

    QList<TreeItem*> parents;
    parents << treeViewModel->rootItem;
    updateSceneView(parents.back(), root);

    emit ui->outlinerView->dataChanged(ui->outlinerView->rootIndex(), ui->outlinerView->rootIndex());
    repaint();
    // Hay que actualizar la vista para que aparezca todo.
}


void AdriMainWindow::UpdateScene()
{

}

void AdriMainWindow::unSelectObject(QModelIndex idx)
{

}

void AdriMainWindow::selectObject(QModelIndex idx)
{
    //TreeModel* model = (TreeModel*)ui->outlinerView->model();

    if (!idx.isValid()) return;

    TreeItem *item = static_cast<TreeItem*>(idx.internalPointer());

    //item->checked = !item->checked;
    //return item->checked;

    if(item->item_id == 0)
        return;

    // Listado de elementos seleccionados.
    vector<unsigned int > lst;
    lst.push_back(item->item_id);
    ui->glCustomWidget->selectElements(lst);

	ui->def_group_id->setText(QString("%1").arg(item->item_id));
	ui->def_groupName->setText(QString("%1").arg(item->sName.c_str()));

	//ui->glCustomWidget->updateGridVisualization();

    printf("Selected: %s con id: %d\n", item->sName.c_str(), item->item_id); fflush(0);
    repaint();

	UpdateScene();
}

void AdriMainWindow::ImportDistances()
{
    QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.txt");
    inFileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    //ui->glCustomWidget->readDistances(fileNames[0]);
}

void AdriMainWindow::DoAction()
{
    QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.txt");
    inFileDialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
    if (inFileDialog.exec())
        fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    //ui->glCustomWidget->importSegmentation(fileNames[0]);
}

void AdriMainWindow::ChangeSourceVertex()
{
    int ind = ui->DistancesVertSource->text().toInt();
    ui->glCustomWidget->UpdateVertexSource(ind);
}

void AdriMainWindow::distancesSourceValueChange(int ind)
{
    ui->glCustomWidget->UpdateVertexSource(ind);
}

void AdriMainWindow::enableStillCage(bool state)
{
    ui->cagesComboBox->setEnabled(ui->enableStillCage->isEnabled());
    ui->glCustomWidget->stillCageAbled = ui->enableStillCage->isEnabled();

    if(ui->cagesComboBox->count()>0)
        ui->glCustomWidget->ChangeStillCage(ui->cagesComboBox->currentIndex());
}

void AdriMainWindow::CloseApplication()
{
    exit(0);
}

AdriMainWindow::~AdriMainWindow()
{
    delete ui;
}

void AdriMainWindow::keyPressEvent(QKeyEvent* event)
{
    int current = 0;
    bool checked = false;
    switch(event->key())
    {
    case Qt::Key_Plus:
        current = ui->DistancesVertSource->value();
        ui->DistancesVertSource->setValue(current+1);
        distancesSourceValueChange(current+1);
        break;

    case Qt::Key_Minus:
        current = ui->DistancesVertSource->value();
        if(current >=1)
        {
            ui->DistancesVertSource->setValue(current-1);
            distancesSourceValueChange(current-1);
        }
        break;

    case Qt::Key_0:
        checked = ui->visibility_btn->isChecked();
        toogleVisibility(!checked);
        ui->visibility_btn->setChecked(!checked);
        break;
    case Qt::Key_1:
        DataVisualizationChange(VIS_LABELS);
        ui->DataVisualizationCombo->setCurrentIndex(VIS_LABELS);
        break;
    case Qt::Key_2:
        DataVisualizationChange(VIS_SEGMENTATION);
        ui->DataVisualizationCombo->setCurrentIndex(VIS_SEGMENTATION);
        break;
    case Qt::Key_3:
        DataVisualizationChange(VIS_BONES_SEGMENTATION);
        ui->DataVisualizationCombo->setCurrentIndex(VIS_BONES_SEGMENTATION);
        break;
	case Qt::Key_4:
		DataVisualizationChange(VIS_SECW_WIDE);
        ui->DataVisualizationCombo->setCurrentIndex(VIS_SECW_WIDE);
        break;
	case Qt::Key_5:
        DataVisualizationChange(VIS_WEIGHTS);
        ui->DataVisualizationCombo->setCurrentIndex(VIS_WEIGHTS);
        break;
    case Qt::Key_6:
        ShadingModeChange(0);
        ui->shadingModeSelection->setCurrentIndex(0);
        ui->infoData->setText("Smooth shading mode");
        break;
    case Qt::Key_7:
        ShadingModeChange(1);
        ui->shadingModeSelection->setCurrentIndex(1);
        ui->infoData->setText("Flat shading Mode");
        break;
    case Qt::Key_8:
        ShadingModeChange(2);
        ui->shadingModeSelection->setCurrentIndex(2);
        ui->infoData->setText("Blend shading mode");
        break;
    case Qt::Key_9:
        ShadingModeChange(3);
        ui->shadingModeSelection->setCurrentIndex(3);
        ui->infoData->setText("Lines shading mode");
        break;
    default:
        break;
    }
}

void AdriMainWindow::changeTransformTranslateAmountX(int) 
{

	// Obtiene el objeto a trasladar
	object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();

	// Aplica la traslacion
	ui->glCustomWidget->particles->xvalue  = ui->translationAmountX->value();
    if (selectedObject != NULL) {
        selectedObject->pos.x() = ui->translationAmountX->value();
		selectedObject->dirtyFlag = true;

		for(int i = 0; i< ui->glCustomWidget->escena->skeletons.size(); i++)
			ui->glCustomWidget->escena->skeletons[i]->dirtyFlag = true;
	}

    QString msg = QString::number(ui->translationAmountX->value());
	ui->translationEditX->setText(msg);

}

void AdriMainWindow::changeTransformTranslateAmountY(int) 
{

	// Obtiene el objeto a trasladar
	object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();

	// Aplica la traslacion
	ui->glCustomWidget->particles->yvalue = ui->translationAmountY->value();
    if (selectedObject != NULL) {
		selectedObject->pos.y() = ui->translationAmountY->value();
		selectedObject->dirtyFlag = true;

		for(int i = 0; i< ui->glCustomWidget->escena->skeletons.size(); i++)
			ui->glCustomWidget->escena->skeletons[i]->dirtyFlag = true;
	}

    QString msg = QString::number(ui->translationAmountY->value());
    ui->translationEditY->setText(msg);
}

void AdriMainWindow::changeTransformTranslateAmountZ(int) {
  
	// Obtiene el objeto a trasladar
	object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();

	// Aplica la traslacion
	ui->glCustomWidget->particles->zvalue = ui->translationAmountZ->value();
    if (selectedObject != NULL) {
        selectedObject->pos.z() = ui->translationAmountZ->value();
		selectedObject->dirtyFlag = true;

		for(int i = 0; i< ui->glCustomWidget->escena->skeletons.size(); i++)
			ui->glCustomWidget->escena->skeletons[i]->dirtyFlag = true;
	}

    QString msg = QString::number(ui->translationAmountZ->value());
    ui->translationEditZ->setText(msg);
}

void AdriMainWindow::changeTransformRotateAmountX(int x) 
{

    object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();

	if (selectedObject != NULL) {
		rotationX =  Deg2Rad((float)ui->dialX->value()/10.0);	
		//qaux.FromEulerAngles(rotationX, rotationY, rotationZ);

		//qaux.FromEulerAngles(Deg2Rad(ui->rotationAmountX->value()),Deg2Rad(ui->rotationAmountY->value()),Deg2Rad(ui->rotationAmountZ->value()));
		//selectedObject->qrot = qaux;
		
		selectedObject->setRotation(rotationX, rotationY, rotationZ);
		selectedObject->dirtyFlag = true;

		for(int i = 0; i< ui->glCustomWidget->escena->skeletons.size(); i++)
			ui->glCustomWidget->escena->skeletons[i]->dirtyFlag = true;
	
	}

    QString msg = QString::number(Rad2Deg(rotationX));
    ui->rotationEditX->setText(msg);

}

void AdriMainWindow::changeTransformRotateAmountY(int y) 
{

    object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();

    if (selectedObject != NULL)
	{
		Eigen::Quaternion<double> qaux;

		// guardamos el nuevo valor
		rotationY =  Deg2Rad((float)ui->dialY->value()/10.0);
		//qaux.FromEulerAngles(rotationX, rotationY, rotationZ);

		// Lo aplicamos como incremento
		//selectedObject->qrot = qaux;
		selectedObject->setRotation(rotationX, rotationY, rotationZ);
		// TOFIX qaux.FromEulerAngles(rotationX, rotationY, rotationZ);

		// Lo aplicamos como incremento
		//selectedObject->qrot = qaux;
		selectedObject->dirtyFlag = true;

		for(int i = 0; i< ui->glCustomWidget->escena->skeletons.size(); i++)
			ui->glCustomWidget->escena->skeletons[i]->dirtyFlag = true;
	}

    QString msg = QString::number(Rad2Deg(rotationY));
    ui->rotationEditY->setText(msg);


}

void AdriMainWindow::changeTransformRotateAmountZ(int z) 
{

    object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();
	
	if (selectedObject != NULL)
	{

		Eigen::Quaternion<double> qaux;

		rotationZ =  Deg2Rad((float)ui->dialZ->value()/10.0);
		//qaux.FromEulerAngles(rotationX, rotationY, rotationZ);

		//selectedObject->qrot = qaux;
		selectedObject->setRotation(rotationX, rotationY, rotationZ);
		selectedObject->dirtyFlag = true;

		for(int i = 0; i< ui->glCustomWidget->escena->skeletons.size(); i++)
			ui->glCustomWidget->escena->skeletons[i]->dirtyFlag = true;
	}

    QString msg = QString::number(Rad2Deg(rotationZ));
    ui->rotationEditZ->setText(msg);

}


void AdriMainWindow::resetRotationValues() {
    object *selectedObject = NULL;

    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();


    //if (selectedObject != NULL) selectedObject->rot.SetZero();
	ui->dialX->setValue(0);
    ui->dialY->setValue(0);
    ui->dialZ->setValue(0);
	
	ui->translationAmountX->setValue(0);
    ui->translationAmountY->setValue(0);
    ui->translationAmountZ->setValue(0);

	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;

}

void AdriMainWindow::addAnimationKeyframe() {


    object *selectedObject = NULL;
    if (ui->glCustomWidget->selMgr.selection.size() > 0)
        selectedObject = ui->glCustomWidget->selMgr.selection.back();

    if (selectedObject == NULL) return;

    int id = selectedObject->nodeId;
    int frame = ui->animSlider->value();
    AdriViewer * viewer = ui->glCustomWidget;
    if (!viewer->aniManager.objectHasAnimation(id)) viewer->aniManager.addAnimation(id);
    viewer->aniManager.addKeyFrame(id, frame, 0, 0, 0,
								ui->translationAmountX->value(),
                                ui->translationAmountY->value(),
                                ui->translationAmountZ->value(),
                                0, 0, 0);

	ui->kfBar->addKeyframe(frame, 150);
}

void AdriMainWindow::toggleAnimation() {
    ui->glCustomWidget->aniManager.animationEnabled = !ui->glCustomWidget->aniManager.animationEnabled;
	if (ui->glCustomWidget->aniManager.animationEnabled) ui->toggleSim->setEnabled(false);
	else ui->toggleSim->setEnabled(true);
}

void AdriMainWindow::toggleSimulation() {
    ui->glCustomWidget->aniManager.simulationEnabled = !ui->glCustomWidget->aniManager.simulationEnabled;
	if (ui->glCustomWidget->aniManager.simulationEnabled) ui->toggleAnim->setEnabled(false);
	else ui->toggleAnim->setEnabled(true);
}

void AdriMainWindow::changeFrame(int frame) {
    // Change the frame label to its correct value
    QString msg = QString::number(frame);
    ui->frameLabel->setText(msg);

    // Tell animManager to load that frame info
    ui->glCustomWidget->frame = frame;

    // Update UI values accordingly
    for (unsigned int i = 0; i < ui->glCustomWidget->escena->skeletons.size(); ++i) {
        skeleton* skt = ((skeleton*) ui->glCustomWidget->escena->skeletons[i]);
        for (unsigned int j = 0; j < skt->joints.size(); ++j) {
            if (ui->glCustomWidget->aniManager.objectHasAnimation(skt->joints[j]->nodeId)) {
                object * joint = (object *)skt->joints[j];
                //joint->rot = ui->glCustomWidget->aniManager.getRotation(skt->joints[j]->nodeId, frame);

				ui->translationAmountX->blockSignals(true);
                //ui->rotationAmountX->setValue(joint->rot.X());
                //ui->rotationEditX->setText(QString("%1").arg(joint->rot.X(), 3, 'g', 3));
                ui->translationAmountX->blockSignals(false);

                ui->translationAmountY->blockSignals(true);
                //ui->rotationAmountY->setValue(joint->rot.Y());
                //ui->rotationEditY->setText(QString("%1").arg(joint->rot.Y(), 3, 'g', 3));
                ui->translationAmountY->blockSignals(false);

                ui->translationAmountZ->blockSignals(true);
                //ui->rotationAmountZ->setValue(joint->rot.Z());
                //ui->rotationEditZ->setText(QString("%1").arg(joint->rot.Z(), 3, 'g', 3));
                ui->translationAmountZ->blockSignals(false);

            }
        }
    }
}

void AdriMainWindow::changeAnimSlider(int val) {
    ui->animSlider->setValue(val);
}

void AdriMainWindow::saveAnimation() {
	string path = ui->glCustomWidget->sPathGlobal.toStdString();
	ui->glCustomWidget->aniManager.saveAnimation("../models/animation.txt", ui->glCustomWidget->escena);
}

void AdriMainWindow::loadAnimation() {
	QFileDialog inFileDialog(0, "Selecciona un fichero", ui->glCustomWidget->sPathGlobal, "*.off *.txt");
	inFileDialog.setFileMode(QFileDialog::ExistingFile);
	    QStringList fileNames;
     if (inFileDialog.exec())
         fileNames = inFileDialog.selectedFiles();

    if(fileNames.size() == 0)
        return;

    QFileInfo sPathAux(fileNames[0]);
    QString aux = sPathAux.canonicalPath();

	ui->glCustomWidget->aniManager.loadAnimations(fileNames[0].toStdString(), ui->glCustomWidget->escena);
	ui->kfBar->addListOfKeyframes(ui->glCustomWidget->aniManager.getFrames(), 150);
	ui->kfBar->repaint();
}

