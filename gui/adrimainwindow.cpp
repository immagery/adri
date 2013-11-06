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

void AdriMainWindow::connectSignals() {
	// conexiones
    connect(ui->action_importModel, SIGNAL(triggered()), this, SLOT(ImportNewModel()) );
    connect(ui->actionAction_openScene, SIGNAL(triggered()), this, SLOT(OpenNewScene()));

    connect(ui->import_cage_s, SIGNAL(triggered()), this, SLOT(ImportCages()) );
    connect(ui->import_distances, SIGNAL(triggered()), this, SLOT(ImportDistances()) );

    connect(ui->shadingModeSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(ShadingModeChange(int)) );

    connect(ui->colorLayersCheck, SIGNAL(toggled(bool)), this, SLOT(EnableColorLayer(bool)) );
    connect(ui->ColorLayerSeletion, SIGNAL(valueChanged(int)), this, SLOT(ChangeColorLayerValue(int)) );

    connect(ui->actionImportSegementation, SIGNAL(triggered()), this, SLOT(DoAction()) );

    //connect(ui->prop_function_updt, SIGNAL(released()), this, SLOT(ChangeSourceVertex()));

    connect(ui->DistancesVertSource, SIGNAL(valueChanged(int)), this, SLOT(distancesSourceValueChange(int)));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(CloseApplication()) );
    //connect(ui->processGC,  SIGNAL(triggered()), ui->glCustomWidget, SLOT(processGreenCoordinates()) );
    //connect(ui->processHC,  SIGNAL(triggered()), ui->glCustomWidget, SLOT(processHarmonicCoordinates()));
    //connect(ui->processMVC, SIGNAL(triggered()), ui->glCustomWidget, SLOT(processMeanValueCoordinates()));
   // connect(ui->processAll, SIGNAL(triggered()), ui->glCustomWidget, SLOT(processAllCoords()));
    //connect(ui->deformedMeshCheck, SIGNAL(released()), ui->glCustomWidget, SLOT(showDeformedModelSlot()));

	connect(ui->GridDraw_interior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->GridDraw_exterior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->allGrid_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->gridSlices_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->SliceSelectorXY, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXY(int)));
    connect(ui->SliceSelectorXZ, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXZ(int)));

    connect(ui->cagesComboBox, SIGNAL(currentIndexChanged(int)), ui->glCustomWidget, SLOT(ChangeStillCage(int)));
    connect(ui->enableStillCage, SIGNAL(toggled(bool)), this, SLOT(enableStillCage(bool)));

    connect(ui->HCGridDraw, SIGNAL(released()), ui->glCustomWidget, SLOT(showHCoordinatesSlot()));
    connect(ui->GridDraw_boundary, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));

    // Actualizaciones del grid.
    //connect(ui->GridDraw_interior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->GridDraw_exterior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->allGrid_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->gridSlices_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->SliceSelectorXY, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXY(int)));
    //connect(ui->SliceSelectorXZ, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXZ(int)));

	//connect(ui->nextStep_button, SIGNAL(released()), ui->glCustomWidget, SLOT(nextProcessStep()));
	//connect(ui->allNextStep_button, SIGNAL(released()), ui->glCustomWidget, SLOT(allNextProcessSteps()));

    connect(ui->prop_function_updt, SIGNAL(released()), ui->glCustomWidget, SLOT(PropFunctionConf()));

    connect(ui->paintModel_btn, SIGNAL(released()), ui->glCustomWidget, SLOT(paintModelWithGrid()));
    connect(ui->metricUsedCheck, SIGNAL(released()), ui->glCustomWidget, SLOT(PropFunctionConf()));

    connect(ui->drawInfluences_check, SIGNAL(released()), ui->glCustomWidget, SLOT(showHCoordinatesSlot()));

    connect(ui->coordTab, SIGNAL(currentChanged(int)), ui->glCustomWidget, SLOT(active_GC_vs_HC(int)));

    connect(ui->glCustomWidget, SIGNAL(updateSceneView()), this, SLOT(updateSceneView()));
    connect(ui->outlinerView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectObject(QModelIndex)));

    connect(ui->actionMove, SIGNAL(triggered()), this, SLOT(toogleMoveTool()));
    connect(ui->actionSelection, SIGNAL(triggered()), this, SLOT(toogleSelectionTool()));
    connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(toogleRotateTool()));
    connect(ui->visibility_btn, SIGNAL(toggled(bool)), this, SLOT(toogleVisibility(bool)));

	connect(ui->actionDoTests, SIGNAL(triggered()), this, SLOT(LaunchTests()));

    connect(ui->segmentation_btn, SIGNAL(toggled(bool)), this, SLOT(toogleToShowSegmentation(bool)));
    connect(ui->DataVisualizationCombo, SIGNAL(currentIndexChanged(int)),this, SLOT(DataVisualizationChange(int)) );

    connect(ui->exportWeights_btn, SIGNAL(released()), ui->glCustomWidget, SLOT(exportWeightsToMaya()));

    connect(ui->expansionSlider, SIGNAL(valueChanged(int)), this, SLOT(updateExpansionSlidervalue(int)));

	connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(updateThresholdSlidervalue(int)));
	connect(ui->threshold_enable, SIGNAL(toggled(bool)), this, SLOT(enableThreshold(bool)));
	

    connect(ui->smoothPropagationSlider, SIGNAL(sliderReleased()), this, SLOT(changeSmoothSlider()));
    connect(ui->smoothPropagationSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSmoothSlidervalue(int)));

	//connect(ui->smoothingPasses, SIGNAL(valueChanged(int)), this, SLOT(changeSmoothingPasses(int)));

	//connect(ui->auxValueInt, SIGNAL(valueChanged(int)), this, SLOT(changeAuxValueInt(int)));

    connect(ui->glCustomWidget, SIGNAL(jointDataShow(float, int)), this , SLOT(jointDataUpdate(float,int)));
	connect(ui->glCustomWidget, SIGNAL(jointTransformationValues(float,float,float,float,float,float)), this , SLOT(jointTransformUpdate(float,float,float,float,float,float)));

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

AdriMainWindow::AdriMainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
	rotateTranlsateFlag = false;

	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;

    /*ui->setupUi(this);
    ui->glCustomWidget->parent = this;
	connectSignals();*/

	/*
    // conexiones
    connect(ui->action_importModel, SIGNAL(triggered()), this, SLOT(ImportNewModel()) );
    connect(ui->actionAction_openScene, SIGNAL(triggered()), this, SLOT(OpenNewScene()));

    connect(ui->import_cage_s, SIGNAL(triggered()), this, SLOT(ImportCages()) );
    connect(ui->import_distances, SIGNAL(triggered()), this, SLOT(ImportDistances()) );

    connect(ui->shadingModeSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(ShadingModeChange(int)) );

    connect(ui->colorLayersCheck, SIGNAL(toggled(bool)), this, SLOT(EnableColorLayer(bool)) );
    connect(ui->ColorLayerSeletion, SIGNAL(valueChanged(int)), this, SLOT(ChangeColorLayerValue(int)) );

    connect(ui->actionImportSegementation, SIGNAL(triggered()), this, SLOT(DoAction()) );

    //connect(ui->prop_function_updt, SIGNAL(released()), this, SLOT(ChangeSourceVertex()));

    connect(ui->DistancesVertSource, SIGNAL(valueChanged(int)), this, SLOT(distancesSourceValueChange(int)));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(CloseApplication()) );
    //connect(ui->processGC,  SIGNAL(triggered()), ui->glCustomWidget, SLOT(processGreenCoordinates()) );
    //connect(ui->processHC,  SIGNAL(triggered()), ui->glCustomWidget, SLOT(processHarmonicCoordinates()));
    //connect(ui->processMVC, SIGNAL(triggered()), ui->glCustomWidget, SLOT(processMeanValueCoordinates()));
   // connect(ui->processAll, SIGNAL(triggered()), ui->glCustomWidget, SLOT(processAllCoords()));
    //connect(ui->deformedMeshCheck, SIGNAL(released()), ui->glCustomWidget, SLOT(showDeformedModelSlot()));

	connect(ui->GridDraw_interior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->GridDraw_exterior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->allGrid_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->gridSlices_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    connect(ui->SliceSelectorXY, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXY(int)));
    connect(ui->SliceSelectorXZ, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXZ(int)));

    connect(ui->cagesComboBox, SIGNAL(currentIndexChanged(int)), ui->glCustomWidget, SLOT(ChangeStillCage(int)));
    connect(ui->enableStillCage, SIGNAL(toggled(bool)), this, SLOT(enableStillCage(bool)));

    connect(ui->HCGridDraw, SIGNAL(released()), ui->glCustomWidget, SLOT(showHCoordinatesSlot()));
    connect(ui->GridDraw_boundary, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));

    // Actualizaciones del grid.
    //connect(ui->GridDraw_interior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->GridDraw_exterior, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->allGrid_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->gridSlices_button, SIGNAL(released()), ui->glCustomWidget, SLOT(updateGridRender()));
    //connect(ui->SliceSelectorXY, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXY(int)));
    //connect(ui->SliceSelectorXZ, SIGNAL(valueChanged(int)), ui->glCustomWidget, SLOT(ChangeSliceXZ(int)));

	//connect(ui->nextStep_button, SIGNAL(released()), ui->glCustomWidget, SLOT(nextProcessStep()));
	//connect(ui->allNextStep_button, SIGNAL(released()), ui->glCustomWidget, SLOT(allNextProcessSteps()));

    connect(ui->prop_function_updt, SIGNAL(released()), ui->glCustomWidget, SLOT(PropFunctionConf()));

    connect(ui->paintModel_btn, SIGNAL(released()), ui->glCustomWidget, SLOT(paintModelWithGrid()));
    connect(ui->metricUsedCheck, SIGNAL(released()), ui->glCustomWidget, SLOT(PropFunctionConf()));

    connect(ui->drawInfluences_check, SIGNAL(released()), ui->glCustomWidget, SLOT(showHCoordinatesSlot()));

    connect(ui->coordTab, SIGNAL(currentChanged(int)), ui->glCustomWidget, SLOT(active_GC_vs_HC(int)));

    connect(ui->glCustomWidget, SIGNAL(updateSceneView()), this, SLOT(updateSceneView()));
    connect(ui->outlinerView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectObject(QModelIndex)));

    connect(ui->actionMove, SIGNAL(triggered()), this, SLOT(toogleMoveTool()));
    connect(ui->actionSelection, SIGNAL(triggered()), this, SLOT(toogleSelectionTool()));
    connect(ui->actionRotate, SIGNAL(triggered()), this, SLOT(toogleRotateTool()));
    connect(ui->visibility_btn, SIGNAL(toggled(bool)), this, SLOT(toogleVisibility(bool)));

	connect(ui->actionDoTests, SIGNAL(triggered()), this, SLOT(LaunchTests()));

    connect(ui->segmentation_btn, SIGNAL(toggled(bool)), this, SLOT(toogleToShowSegmentation(bool)));
    connect(ui->DataVisualizationCombo, SIGNAL(currentIndexChanged(int)),this, SLOT(DataVisualizationChange(int)) );

    connect(ui->exportWeights_btn, SIGNAL(released()), ui->glCustomWidget, SLOT(exportWeightsToMaya()));

    connect(ui->expansionSlider, SIGNAL(valueChanged(int)), this, SLOT(updateExpansionSlidervalue(int)));

	connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(updateThresholdSlidervalue(int)));
	connect(ui->threshold_enable, SIGNAL(toggled(bool)), this, SLOT(enableThreshold(bool)));
	

    connect(ui->smoothPropagationSlider, SIGNAL(sliderReleased()), this, SLOT(changeSmoothSlider()));
    connect(ui->smoothPropagationSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSmoothSlidervalue(int)));

	//connect(ui->smoothingPasses, SIGNAL(valueChanged(int)), this, SLOT(changeSmoothingPasses(int)));

	//connect(ui->auxValueInt, SIGNAL(valueChanged(int)), this, SLOT(changeAuxValueInt(int)));

    connect(ui->glCustomWidget, SIGNAL(jointDataShow(float, int)), this , SLOT(jointDataUpdate(float,int)));

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

	// Transform
    connect(ui->rotationAmountX, SIGNAL(valueChanged(int)), this, SLOT(changeTransformRotateAmountX(int)));
    connect(ui->rotationAmountY, SIGNAL(valueChanged(int)), this, SLOT(changeTransformRotateAmountY(int)));
    connect(ui->rotationAmountZ, SIGNAL(valueChanged(int)), this, SLOT(changeTransformRotateAmountZ(int)));
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
	//connect(ui->speedDmp, SIGNAL(valueChanged(int)), this, SLOT(changeSpeedDampingSlider(int)));*/
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

void AdriMainWindow::OpenNewScene()
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

    ui->glCustomWidget->readScene(fileNames[0].toStdString(), sModelPrefix.toStdString(), sModelPath.toStdString());
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

void AdriMainWindow::updateSmoothSlidervalue(int)
{
    float valueAux = ui->smoothPropagationSlider->value();
    float value = valueAux/10.0;

    ui->smoothPropagationEdit->setText(QString("%1").arg(value));
}

void AdriMainWindow::updateExpansionSlidervalue(int)
{
    float valueAux = ui->expansionSlider->value();
    float value = 0;
    if(valueAux <= 100)
        value = ((float)ui->expansionSlider->value())/100.0;
    else
    {
        value = (((valueAux-100)/100)*9)+1.0;
    }

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

	ui->glCustomWidget->interiorPoint = Point3d(valueAuxX,valueAuxY,valueAuxZ);
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

void AdriMainWindow::jointDataUpdate(float fvalue, int id)
{
    if(fvalue <=1)
    {
		ui->expansionSlider->setValue((int)round(fvalue*100));
    }
    else
    {
        int value = ((int)round((fvalue-1)/9*100)+100);
        ui->expansionSlider->setValue(value);
    }

    ui->expansionValueEdit->setText(QString("%1").arg(fvalue));

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
    if(toolSelected == T_MOVETOOL)
    {
        ui->actionMove->setChecked(true);
        return;
    }

    ui->actionRotate->setChecked(false);
    ui->actionSelection->setChecked(false);
    changeTool(T_MOVETOOL);
}

void AdriMainWindow::toogleRotateTool()
{
    if(toolSelected == T_ROTATETOOL)
    {
       ui->actionRotate->setChecked(true);
        return;
    }

    ui->actionMove->setChecked(false);
    ui->actionSelection->setChecked(false);
    changeTool(T_ROTATETOOL);

}

void AdriMainWindow::toogleSelectionTool()
{
    if(toolSelected == T_SELECTTOOL)
    {
        ui->actionSelection->setChecked(true);
        return;
    }

    ui->actionMove->setChecked(false);
    ui->actionRotate->setChecked(false);
    changeTool(T_SELECTTOOL);
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
    default:
        printf("Hay algún problema con la seleccion de contexto.\n"); fflush(0);
        ui->glCustomWidget->setContextMode(CTX_SELECTION);
        break;
    }

    printf("ha habido un cambio\n"); fflush(0);

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

void AdriMainWindow::toogleVisibility(bool toogle)
{
   ui->glCustomWidget->toogleVisibility(toogle);
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
		DataVisualizationChange(VIS_SEG_PASS);
        ui->DataVisualizationCombo->setCurrentIndex(VIS_SEG_PASS);
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
	ui->glCustomWidget->particles->xvalue = ui->translationAmountX->value();
    if (selectedObject != NULL) {
        selectedObject->pos.X() = ui->translationAmountX->value();
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
		selectedObject->pos.Y() = ui->translationAmountY->value();
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
        selectedObject->pos.Z() = ui->translationAmountZ->value();
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
		Quaternion<double> qaux;
		//qaux.FromEulerAngles(Deg2Rad(ui->rotationAmountX->value()-rotationX),0,0);
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
		Quaternion<double> qaux;

		// guardamos el nuevo valor
		rotationY =  Deg2Rad((float)ui->dialY->value()/10.0);
		//qaux.FromEulerAngles(rotationX, rotationY, rotationZ);

		// Lo aplicamos como incremento
		//selectedObject->qrot = qaux;
		selectedObject->setRotation(rotationX, rotationY, rotationZ);
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

		Quaternion<double> qaux;

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

