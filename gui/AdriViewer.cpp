#include <utils/utilGL.h>

//#define VERBOSE false

// Qt libraries
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtGui/QColor>

#include "AdriViewer.h"

#include <DataStructures/Scene.h>
//TODELETE
//#include <computation/GreenCoords.h>
//#include <computation/HarmonicCoords.h>

#include <utils/util.h>
#include "adrimainwindow.h"

#include <DataStructures/Scene.h>
#include <DataStructures/Cage.h>
#include <DataStructures/Modelo.h>
#include <DataStructures/Rig.h>
#include <render/ShadingNode.h>
#include <render/GeometryRender.h>

#include <render/gridRender.h>
#include <render/clipingPlaneRender.h>

#include "manipulatedFrameSetConstraint.h"

#include "DataStructures/InteriorDistancesData.h"

#include "utils/ioWeights.h"

//#include <Eigen/Dense>

//#include <vcg/simplex/face/jumping_pos.h>

#include <iostream>
#include <fstream>

#include <QtCore/QTextStream>

using namespace qglviewer;

AdriViewer::AdriViewer(QWidget * parent , const QGLWidget * shareWidget, Qt::WindowFlags flags)
        : QGLViewer(parent, shareWidget, flags)
{
    selectionMode_ = NONE;

    // Scene variables.
	m_ptCenter.setZero();
    m_sceneRadius = 500;

    m_sceneMin[0] = -m_sceneRadius;
    m_sceneMin[1] = -m_sceneRadius;
    m_sceneMin[2] = -m_sceneRadius;

    m_sceneMax[0] = m_sceneRadius;
    m_sceneMax[1] = m_sceneRadius;
    m_sceneMax[2] = m_sceneRadius;

    //initScene();

    // State flags
    loadedModel = false;
    firstInit = true;
    refreshFlag = true;
    firstDrawing = true;
    showDeformedModel = false;

    // Transform
//    this->setAnimationPeriod(0);

    // Drawing flags.
//    drawCage = true;
//    shadeCoordInfluence = true;
//    bGCcomputed = false;
//    bHComputed = false;

//    //m_bHCGrid = false;

//    bGCcomputed = false;
//    bHComputed = false;
//    bMVCComputed = false;

//    influenceDrawingIdx = 0;

//    //activeCoords = HARMONIC_COORDS;
//    //m.sModelPath = "";

//    stillCageAbled = false;
//    stillCageSelected = -1;

    viewingMode = SimpleModel_Mode;
    ShadingModeFlag = SH_MODE_SMOOTH; //smooth

    // Guardamos un directorio de trabajo para ir m�s r�pido
    QFile file(QDir::currentPath()+"/WorkDir.txt");
    file.open(QFile::ReadOnly);
    if(file.exists())
    {
        QTextStream in(&file);
        sPathGlobal = in.readLine();
        QFile path(sPathGlobal);
        if(!path.exists())
        {
            printf("el directorio de trabajo no existe\n");
            sPathGlobal = QDir::currentPath();
        }
    }
    else
         sPathGlobal = QDir::currentPath();

    printf("Path de trabajo: %s\n", sPathGlobal.toStdString().c_str()); fflush(0);

    file.close();

    setTextIsEnabled(false);
    setFPSIsDisplayed(true);
    setAxisIsDrawn(false);

    colorsLoaded= false;
    colorLayers = false;
    colorLayerIdx = -1;

    escena = new scene();
	escena->rig = new Rig(scene::getNewId());

	CurrentProcessJoints.clear();

	interiorPoint = Eigen::Vector3d(0,0,0);

    //ctxMode = CTX_SELECTION;

    frame = 0;
	aniManager.simulationEnabled = false;
	aniManager.animationEnabled = false;
	particles = new Particles();
	//particles->bake(1500, 1/24.0);

	setSceneCenter(Vec(0,0,0));
    setSceneRadius(400);
	//ReBuildScene();
}

 AdriViewer::~AdriViewer()
 {
    //makeCurrent();
    //releaseKeyboard();
 }

 void AdriViewer::init()
 {
    // A ManipulatedFrameSetConstraint will apply displacements to the selection
    /*
    setManipulatedFrame(new ManipulatedFrame());
    manipulatedFrame()->setConstraint(new ManipulatedFrameSetConstraint());
    ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
    mfsc->newCage = &m.dynCage;
    */
	 
	// A ManipulatedFrameSetConstraint will apply displacements to the selection
	setManipulatedFrame(new ManipulatedFrame());
	manipulatedFrame()->setConstraint(new ManipulatedFrameSetConstraint());

    // Used to display semi-transparent relection rectangle
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDepthFunc(GL_LEQUAL);

    restoreStateFromFile();
    setAnimationPeriod(20);
    startAnimation();

	// Define 'Control+Q' as the new exit shortcut (default was 'Escape')
	setShortcut(EXIT_VIEWER, Qt::CTRL+Qt::Key_Q);

    //testScene();
 }

 void AdriViewer::animate()
 {
     /*
     ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
     if(!mfsc)
         return;

     if(mfsc->transformed)
         deformMesh();

     */

	 if (!aniManager.animationEnabled) return;

     for (unsigned int i = 0; i < escena->skeletons.size(); ++i) {
         skeleton* skt = ((skeleton*) escena->skeletons[i]);
         for (unsigned int j = 0; j < skt->joints.size(); ++j) {
             int id = skt->joints[j]->nodeId;

             if (aniManager.objectHasAnimation(id)) {
                Eigen::Vector3d position = aniManager.getPosition(id, frame);
                Eigen::Vector3d rotation = aniManager.getRotation(id, frame);
                //TOFIX ((joint*)skt->joints[j])->rot = rotation;
             }
         }
     }
     ++frame;
     if (frame == 150) aniManager.animationEnabled = false;
     emit changedFrame(frame);
 }


void AdriViewer::selectElements(vector<unsigned int > lst)
{
    escena->removeSelection();
    escena->selectElements(lst);

    selMgr.selection.clear();
    for(unsigned int i = 0; i< lst.size(); i++)
    {
		// If it is a model
        for(unsigned int m = 0; m< escena->models.size(); m++)
        {
            if(((object*)escena->models[m])->nodeId == lst[i])
            {
               selMgr.selection.push_back(escena->models[m]);
            }
        }

		// If it is a skeleton
        for(unsigned int m = 0; m< escena->skeletons.size(); m++)
        {
            skeleton* skt = ((skeleton*)escena->skeletons[m]);
            for(unsigned int j = 0; j< skt->joints.size(); j++)
            {
                if( skt->joints[j]->nodeId == lst[i] )
                {
                   selMgr.selection.push_back(skt->joints[j]);
                   emit jointDataShow(((joint*)skt->joints[j])->expansion,
                                      ((joint*)skt->joints[j])->nodeId);

				   double alfa,beta,gamma;

				   toEulerAngles(((joint*)skt->joints[j])->qrot, alfa, beta, gamma);

				   emit jointTransformationValues(((joint*)skt->joints[j])->pos[0],
												  ((joint*)skt->joints[j])->pos[1],
												  ((joint*)skt->joints[j])->pos[2],
												  alfa,
												  beta,
												  gamma);

                }
            }
        }
	}
}


 bool AdriViewer::updateInfo()
 {
     printf("Info function TODO\n"); fflush(0);
     /*
     QString texto = "<b>Escena:</b><br>";
     texto.append(QString("<br><b>-:: Modelo ::-</b><br>%1 vertices<br> %2 triangulos<br>").arg(m.modeloOriginal.vn).arg(m.modeloOriginal.fn));
     texto.append(QString("<br><b>-:: Caja ::-</b><br>%1 vertices<br> %2 triangulos<br>").arg(m.cage.vn).arg(m.cage.fn));
     parent->ui->infoData->setText(texto);

     */

    return true;
 }

 void AdriViewer::testScene()
 {
     /*
     skeleton* skt = new skeleton();

     skt->root = new joint(escena->getNewId());
     skt->root->sName = "root";
     skt->root->father = NULL;

     joint* jt = new joint(skt->root, escena->getNewId());
     jt->sName = "jt1";
     skt->root->childs.push_back(jt);
     jt->addTranslation(12,0,0);

     joint* jt2 = new joint(jt, escena->getNewId());
     jt2->sName = "jt2";
     jt->childs.push_back(jt2);
     //jt2->addRotation(45,0,0);
     jt2->addTranslation(0,10,0);

     joint* jt3 = new joint(jt, escena->getNewId());
     jt3->sName = "jt3";
     jt->childs.push_back(jt3);
     jt3->addTranslation(4,0,0);

     joint* jt4 = new joint(jt3, escena->getNewId());
     jt4->sName = "jt4";
     jt3->childs.push_back(jt4);
     jt4->addTranslation(8,0,0);

     joint* jt5 = new joint(jt2, escena->getNewId());
     jt5->sName = "jt5";
     jt2->childs.push_back(jt5);
     jt5->addTranslation(8,0,0);


     escena->skeletons.push_back((object*)skt);

     //ReBuildScene();
     updateInfo();
     emit updateSceneView();
     */}

 // Lee una lista de puntos.

 bool AdriViewer::readNodes(vector< string >& nodeNames,
                          vector< Eigen::Vector3d >& nodePoints,
                          QString sFile)
 {
     nodePoints.clear();
     nodeNames.clear();

     QFile inFile(sFile);
     if(!inFile.exists()) return false;

     inFile.open(QFile::ReadOnly);
     QTextStream in(&inFile);
     unsigned int idxNumber;
     idxNumber= in.readLine().toInt();

     nodePoints.resize(idxNumber);
     nodeNames.resize(idxNumber);

     int element = 0;

     while(!in.atEnd())
     {
         QString str = in.readLine();

         QStringList lstr = str.split(" ");

         nodeNames[element] = lstr[0].toStdString();

         Eigen::Vector3d pt;
         for(int i = 0; i< 3; i++)
             pt[i] = lstr[i+1].toDouble();

         nodePoints[element] = pt;

         element++;
     }

     inFile.close();

     return true;
 }


 // Lee una lista de puntos.
 bool AdriViewer::readPoints(vector< Eigen::Vector3d >& points,
                           QString sFile)
 {
     points.clear();

     QFile inFile(sFile);
     if(!inFile.exists()) return false;

     inFile.open(QFile::ReadOnly);
     QTextStream in(&inFile);
     unsigned int idxNumber;
     idxNumber= in.readLine().toInt();

     while(!in.atEnd())
     {
         QString str = in.readLine();

         QStringList lstr = str.split(" ");
         assert(lstr.size() == 3);

         Eigen::Vector3d pt;
         for(int i = 0; i< lstr.size(); i++)
             pt[i] = lstr[i].toDouble();

         points.push_back(pt);
     }

     inFile.close();

     assert(points.size() == idxNumber);
     return points.size() == idxNumber;
 }

void AdriViewer::readSkeleton(string fileName)
{
    readSkeletons(fileName, escena->skeletons);

    emit updateSceneView();
 }

 void AdriViewer::readScene(string fileName, string name, string path)
 {
     QFile modelDefFile(fileName.c_str());
     if(modelDefFile.exists())
     {
        modelDefFile.open(QFile::ReadOnly);
        QTextStream in(&modelDefFile);

        QString sSceneName = in.readLine(); in.readLine();
        QString sGlobalPath = in.readLine(); in.readLine();
        QString sPath = in.readLine(); in.readLine(); in.readLine();

		escena->sSceneName = sSceneName.toStdString();
		escena->sGlobalPath = sGlobalPath.toStdString();
		escena->sPath = sPath.toStdString();

		QString sModelFile = "", sSkeletonFile = "", sEmbeddingFile = "", sBindingFile = "", sRiggingFile = "";

		QStringList flags = in.readLine().split(" "); in.readLine(); in.readLine();
		if(flags.size() < 5)
			return;

		if(flags[0].toInt() != 0 && !in.atEnd())
		{
			sModelFile = in.readLine(); in.readLine(); in.readLine();
		}

		if(flags[1].toInt() != 0 && !in.atEnd())
		{
			sSkeletonFile = in.readLine(); in.readLine(); in.readLine();
		}

		if(flags[2].toInt() != 0 && !in.atEnd())
		{
			sEmbeddingFile = in.readLine(); in.readLine(); in.readLine();
		}

		if(flags[3].toInt() != 0 && !in.atEnd())
		{
			sBindingFile = in.readLine(); in.readLine(); in.readLine();
		}

		if(flags[4].toInt() != 0 && !in.atEnd())
		{
			sRiggingFile = in.readLine(); in.readLine(); in.readLine();
		}

		modelDefFile.close();

		QString newPath( path.c_str());
        newPath = newPath +"/";
        if(!sPath.isEmpty())
            newPath = newPath+"/"+sPath +"/";

        // Leer modelo
        readModel( (newPath+sModelFile).toStdString(), sSceneName.toStdString(), newPath.toStdString());

		// Constuir datos sobre el modelo
        Modelo* m = ((Modelo*)escena->models.back());
        m->sPath = newPath.toStdString(); // importante para futuras referencias
		BuildSurfaceGraphs(m);

        // Leer esqueleto
		if(!sSkeletonFile.isEmpty())
		{
			string sSkeletonFileFullPath = (newPath+sSkeletonFile).toStdString();
			readSkeletons(sSkeletonFileFullPath, escena->skeletons);
		}

		bool skinLoaded = false, riggLoaded = false;
		// Skinning
		if(!sBindingFile.isEmpty())
		{
			string sBindingFileFullPath = (newPath+sBindingFile).toStdString();//path.toStdString();
			
			//Copia del modelo para poder hacer deformaciones
			if(!m->originalModelLoaded)
				initModelForDeformation(m);
				
			loadBinding(m->bind, sBindingFileFullPath, escena->skeletons);
			skinLoaded = true;

			//escena->rig->skin->computeRestPositions(escena->skeletons);
		}

		// Load Rigging
		if(!sRiggingFile.isEmpty())
		{
			string sRiggingFileFullPath = (newPath+sRiggingFile).toStdString();//path.toStdString();
			escena->rig->loadRigging(sRiggingFileFullPath);

			// By now is with the skeleton, but soon will be alone
			//escena->rig->bindLoadedRigToScene(m, escena->skeletons);
			riggLoaded = true;
		}

		if(riggLoaded)
		{
			// By now is with the skeleton, but soon will be alone
			escena->rig->bindRigToModelandSkeleton(m, escena->skeletons);
		}

		if(skinLoaded)
		{
			// Now it's time to do a correspondence with the loaded data and the scene.
			escena->rig->skin->loadBindingForModel(m, escena->skeletons);

			escena->rig->skin->computeRestPositions(escena->skeletons);

			escena->rig->enableDeformation = true;
		}
	 }
 }


 void AdriViewer::readModel(string fileName, string name, string path)
 {

     // Desglosamos el fichero en 3 partes:
     //    1. Path donde esta el modelo
     //    2. Prefijo del fichero
     //    3. Extension del fichero.

     QString ext = QString(fileName.c_str()).right(3);

     if(ext == QString("off") || ext == QString("obj")) // cargar modelo simple
     {
         escena->models.push_back((object*)new Modelo(escena->getNewId()));
         ((Modelo*)escena->models.back())->loadModel(fileName, name, ext.toStdString(), path);
     }
     else if(ext == QString("txt")) // Cargamos una configuracion
     {
         QFile modelDefFile(fileName.c_str());
         if(modelDefFile.exists())
         {
             modelDefFile.open(QFile::ReadOnly);
             QTextStream in(&modelDefFile);

             QString sPath, modelFileName, cageFileName;
             vector< QString > cageStills;
             QString stillCage;

             // Lectura del modelo
             in >> sPath;
             in >> modelFileName;

             QFileInfo sPathAux(fileName.c_str());
             QString absPath = sPathAux.canonicalPath();

             sPath = absPath+sPath;

             escena->models.push_back((object*)new Modelo(escena->getNewId()));
             Modelo* m = (Modelo*)escena->models.back();

             QString ext2 = modelFileName.right(3);
             QString fileNameAbs = (sPath+"/"+modelFileName);
             QString nameAbs = modelFileName;
             nameAbs.chop(4);


             // Modelo
             m->loadModel(fileNameAbs.toStdString(),
                          nameAbs.toStdString(),
                          ext2.toStdString(),
                          sPath.toStdString());

             if(!in.atEnd())
             {
                 in >> cageFileName;

                 ext2 = cageFileName.right(3);
                 nameAbs = cageFileName;
                 nameAbs.chop(4);
                 fileNameAbs = (sPath+"/"+cageFileName);

				 /*
                 // Caja
                 m->modelCage = new Cage(escena->getNewId());
                 m->modelCage->loadModel(fileNameAbs.toStdString(),
                                         nameAbs.toStdString(),
                                         ext2.toStdString(),
                                         sPath.toStdString());
                 m->modelCage->type = MODEL_CAGE;

                 // Caja dinamica
                 m->dynCage = new Cage(escena->getNewId());
                 m->dynCage->loadModel(fileNameAbs.toStdString(),
                                       nameAbs.append("_dyn").toStdString(),
                                       ext2.toStdString(),
                                       sPath.toStdString());
                 m->dynCage->type = DYN_CAGE;

                 Cage* cg = m->dynCage;
                 //loadSelectableVertex(cg);

                 while(!in.atEnd())
                 {
                     in >> stillCage;
                     cageStills.push_back(stillCage);
                 }

                 if((int)cageStills.size() > 0 )
                 {
                     // Still cages
                     //m->stillCages.resize(cageStills.size());
                     for(unsigned int i = 0; i< cageStills.size(); i++)
                     {
                         ext2 = cageStills[i].right(3);
                         nameAbs = cageStills[i];
                         nameAbs.chop(4);
                         fileNameAbs = (sPath+"/"+cageStills[i]);

                         m->stillCages.push_back(new Cage(escena->getNewId()));

                         m->stillCages.back()->loadModel(fileNameAbs.toStdString(),
                                                         nameAbs.toStdString(),
                                                         ext2.toStdString(),
                                                         sPath.toStdString());
                         m->stillCages.back()->type = STILL_CAGE;

                         //parent->ui->cagesComboBox->addItem(cageStills[i].left(cageStills[i].length()-4), i);
                     }
                 }
				 */
             }

             modelDefFile.close();
         }
     }

     ReBuildScene();
     updateInfo();
     emit updateSceneView();
 }

// This function inits the scene using the general viewing parameters.
 void AdriViewer::initScene(){
     setGridIsDrawn(false);
     setAxisIsDrawn(true);
     setFPSIsDisplayed(true);

     // Restore previous viewer state.
     //restoreStateFromFile();
 }

 // Inicializates the scene taking in account all the models of the scene.
void AdriViewer::ReBuildScene(){

     double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
     bool init_ = false;
     // Recomponemos la bounding box de la escena
     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         Eigen::Vector3d minAuxPt, maxAuxPt;
         escena->models[i]->getBoundingBox(minAuxPt, maxAuxPt);


         if(!init_)
         {
             minX = minAuxPt.x();
             minY = minAuxPt.y();
             minZ = minAuxPt.z();

             maxX = maxAuxPt.x();
             maxY = maxAuxPt.y();
             maxZ = maxAuxPt.z();
             init_  = true;
         }
         else
         {
             minX = min(minAuxPt.x(), minX);
             minY = min(minAuxPt.y(), minY);
             minZ = min(minAuxPt.y(), minY);

             maxX = max(maxAuxPt.x(), maxX);
             maxY = max(maxAuxPt.y(), maxY);
             maxZ = max(maxAuxPt.z(), maxZ);
         }
     }

     for(unsigned int i = 0; i< escena->skeletons.size(); i++)
     {
         // TODO
         // Queda mirar esto... lo que tengo previsto es pedir al esqueleto... como con el modelo.

         Eigen::Vector3d minAuxPt, maxAuxPt;
         escena->skeletons[i]->getBoundingBox(minAuxPt, maxAuxPt);

         if(!init_)
         {
             minX = minAuxPt.x();
             minY = minAuxPt.y();
             minZ = minAuxPt.z();

             maxX = maxAuxPt.x();
             maxY = maxAuxPt.y();
             maxZ = maxAuxPt.z();
             init_ = true;
         }
         else
         {
             minX = min(minAuxPt.x(), minX);
             minY = min(minAuxPt.y(), minY);
             minZ = min(minAuxPt.y(), minY);

             maxX = max(maxAuxPt.x(), maxX);
             maxY = max(maxAuxPt.y(), maxY);
             maxZ = max(maxAuxPt.z(), maxZ);
         }
     }

     Vector3d minPt(minX,minY,minZ);
     Vector3d maxPt(maxX,maxY,maxZ);

     for(int i = 0; i< 3; i++) m_sceneMin[i] = (float)minPt[i] ;
     for(int i = 0; i< 3; i++) m_sceneMax[i] = (float)maxPt[i] ;
     m_ptCenter = Vector3d((minX+maxX)/2, (minY+maxY)/2, (minZ+maxZ)/2);

    // definimos las condiciones de la escena para el GlViewer.
    setSceneBoundingBox(Vec(m_sceneMin[0],m_sceneMin[1],m_sceneMin[2]),Vec(m_sceneMax[0],m_sceneMax[1],m_sceneMax[2]));

    Vector3d minPoint(m_sceneMin[0],m_sceneMin[1],m_sceneMin[2]);

    setSceneCenter(Vec(m_ptCenter.x(),m_ptCenter.y(),m_ptCenter.z()));
    setSceneRadius((m_ptCenter - minPoint).norm());

    printf("SceneMinMax: (%f,%f,%f)-(%f,%f,%f)\n",m_sceneMin[0],m_sceneMin[1],m_sceneMin[2],m_sceneMax[0],m_sceneMax[1],m_sceneMax[2]);
    printf("SceneCenter: (%f,%f,%f)\n",m_ptCenter.x(),m_ptCenter.y(),m_ptCenter.z());

    showEntireScene();

    // Actualizamos la vista.
    updateGL();

    // Guardamos la vista.
    saveStateToFile();

    //firstInit = false;
}

 void AdriViewer::drawWithNames()
 {
	for(unsigned int i = 0; i< escena->models.size(); i++)
	{
		if(!escena->models[i] || !escena->models[i]->shading->visible)
			continue;

		glPushName(escena->models[i]->nodeId);
		((Modelo*)escena->models[i])->shading->shMode = SH_MODE_SELECTION;
		((Modelo*)escena->models[i])->drawFunc();
		glPopName();
	}
 }

 // limpiamos variables de shading sobre los seleccionados
 // (ahora sobre todos)
 void AdriViewer::cleanShadingVariables()
 {
     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         if(!escena->models[i])
             continue;

         escena->models[i]->shading->shtype = T_POLY;
     }
 }

 // activamos el modo lineas.
 void AdriViewer::toogleModelToLines()
 {
     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         if(!escena->models[i])
             continue;

         escena->models[i]->shading->shtype = T_LINES;
     }
 }

 // activamos el modo invisible.
 void AdriViewer::toogleVisibility(bool toogle)
 {
     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         if(!escena->models[i] || !escena->models[i]->shading)
             continue;

          escena->models[i]->shading->visible = toogle;
     }
 }

 // activamos el modo xray.
 void AdriViewer::toogleModelToXRay()
 {
     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         if(!escena->models[i])
             continue;

         escena->models[i]->shading->shtype = T_XRAY;
     }
 }

 // This function draw the elements using the state parameters.
 void AdriViewer::draw()
 {

	 //if (escena->skeletons.size() > 0) escena->skeletons[0]->joints[0]->computeWorldPos();
     if(ShadingModeFlag == SH_MODE_SMOOTH)
	 {
        glShadeModel(GL_SMOOTH);
	 }
     else if(ShadingModeFlag == SH_MODE_FLAT)
	 {
        glShadeModel(GL_FLAT);
	 }


	if (aniManager.simulationEnabled) ++frame;
	/*
	if (aniManager.simulationEnabled) {
		 double fps = 1.0/this->animationPeriod()*1000;
		 double currentTime = (double)frame/fps;
		 int numReps = 10;
		 for (int k = 0; k < numReps; ++k) particles->solve(currentTime + ((double)k / numReps)*this->animationPeriod()/1000.0);
		 ++frame;
	 }
	 particles->drawFunc();
	 */

     /*
     for(unsigned int i = 0; i< escena->shaders.size(); i++)
     {
         if(!escena->shaders[i] || !escena->shaders[i]->visible)
             continue;

         escena->shaders[i]->drawFunc();
     }
     */
     for(unsigned int i = 0; i< escena->visualizers.size(); i++)
     {
         if(!escena->visualizers[i] || !escena->visualizers[i]->visible)
             continue;

         ((gridRenderer*)escena->visualizers[i])->drawFunc(NULL);
     }


     for(unsigned int i = 0; i< escena->skeletons.size(); i++)
     {
         if(!escena->skeletons[i] || !escena->skeletons[i]->shading->visible)
             continue;

		 ((skeleton*)escena->skeletons[i])->drawFunc();
     }

     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         if(!escena->models[i] || !escena->models[i]->shading->visible)
             continue;

		 ((Modelo*)escena->models[i])->shading->shMode = ShadingModeFlag;
         ((Modelo*)escena->models[i])->drawFunc();
     }


     glDisable(GL_LIGHTING);
     
	   // Draws manipulatedFrame (the set's rotation center)
	  if (manipulatedFrame()->isManipulated())
	  {
		  glPushMatrix();
		  glMultMatrixd(manipulatedFrame()->matrix());
		  drawAxis(0.5);
		  glPopMatrix();
	 }
	 
	// Draws rectangular selection area. Could be done in postDraw() instead.
	//if (selectionMode_ != NONE)
	//	drawSelectionRectangle();

	 //drawPointLocator(interiorPoint, 1, true);
     glEnable(GL_LIGHTING);

     // Pintamos el handle en el caso de que haya selecci�n.
     selMgr.drawFunc();


    // Pintamos el handle en el caso de que haya selecci�n.
     /*if (selMgr.ctx != CTX_SELECTION)
       drawSelectionRectangle();
     else
        selMgr.drawFunc();*/

 }

//void AdriViewer::postSelection(const QPoint& point)
//{

	/*
  qglviewer::Vec orig, dir, selectedPoint;
  
  // Compute orig and dir, used to draw a representation of the intersecting line
  camera()->convertClickToLine(point, orig, dir);

  // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
  bool found;
  selectedPoint = camera()->pointUnderPixel(point, found);

  // Construimos un rayo suficientemente largo
  Ray r;
  Vector3d origenPt = Vector3d(orig[0],orig[1],orig[2]);
  Vector3d selPt = Vector3d(selectedPoint[0],selectedPoint[1],selectedPoint[2]);
  r.P0 = origenPt;
  r.P1 = (selPt-origenPt)*10+origenPt;

  Vector3d rayDir(dir.x, dir.y, dir.z); 

  // Recorremos todos los triángulos y evaluamos si intersecta, en ese caso lo apilamos.
  vector<unsigned int> ids;
  TriangleAux tr;

  vector<float> dephts;
  vector<Vector3d> intersecPoints;
  vector<int> triangleIdx;

  Geometry* geom = (Geometry*)(escena->models[0]);
  GeometryRender* geomRender = (GeometryRender*)(escena->models[0]->shading);
  for(int trIdx = 0; trIdx< geom->triangles.size(); trIdx++ )
  {
	int pIdx = geom->triangles[trIdx]->verts[0]->id;
	tr.V0 = geom->nodes[pIdx]->position;

	pIdx = geom->triangles[trIdx]->verts[1]->id;
	tr.V1 = geom->nodes[pIdx]->position;

 	pIdx = geom->triangles[trIdx]->verts[2]->id;
	tr.V2 = geom->nodes[pIdx]->position; 

	Vector3d I;
	int res = intersect3D_RayTriangle(r, tr, I);
  
	if(res > 0 && res < 2)
	{
		printf("Interseccion con: %d\n", trIdx);
		
		float depth = (origenPt-I).norm();

		vector<float>::iterator it = dephts.begin();
		vector<Vector3d>::iterator it2 = intersecPoints.begin();
		vector<int>::iterator it3 = triangleIdx.begin();

		bool inserted = false;
		// Lo anadimos a la lista de intersecciones, pero ordenado por profundidad.
		for(int dIdx = 0; dIdx< dephts.size(); dIdx++)
		{
			if(dephts[dIdx] > depth)
			{
				dephts.insert(it, depth);
				intersecPoints.insert(it2, I);
				triangleIdx.insert(it3, trIdx);
				inserted = true;
				break;
			}

			++it2;
			++it;
		}

		if(!inserted)
		{
			dephts.push_back(depth);
			intersecPoints.push_back(I);
			triangleIdx.push_back(trIdx);
		}
	}
  }

  int firstIn = -1;
  int firstOut = -1;
  // A partir del vector ordenado por profundidades cojo el primer rango in/out, para eso usamos la normal
  // del triangulo -vs- la direccion del rayo
  for(int intrIdx = 0; intrIdx < triangleIdx.size(); intrIdx++)
  {
	  float orient = rayDir.dot(geom->faceNormals[triangleIdx[intrIdx]]);
	  if(orient >= 0)
	  {
		  if(firstIn >= 0)
			  firstOut = intrIdx;
		  
		  if(firstIn >= 0 && firstOut > 0)
			  break;
	  }
	  else
	  {
		  if(firstIn < 0) firstIn = intrIdx;
	  }
  }

  if(firstIn>=0 && firstOut>0)
  {
	printf("El punto seleccionado es...");

	Vector3d intermedio((intersecPoints[firstIn] + intersecPoints[firstOut])/2);
	sktCr->addNewJoint(intermedio);

	printf("%f %f %f a profundidad %f", intermedio.x(), intermedio.y(), intermedio.z(), dephts[firstIn]+dephts[firstOut]/2);
  }
  else
  {
	  printf("No hay punto...\n");
  }

  */

//}


void AdriViewer::endSelection(const QPoint&)
{
  // Flush GL buffers
  //glFlush();

  // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
  //GLint nbHits = glRenderMode(GL_RENDER);

  /*
  if (nbHits > 0)
    {
      // Interpret results : each object created 4 values in the selectBuffer().
      // (selectBuffer())[4*i+3] is the id pushed on the stack.
      for (int i=0; i<nbHits; ++i)
	switch (selectionMode_)
	  {
	  case ADD    : addIdToSelection((selectBuffer())[4*i+3]); break;
	  case REMOVE : removeIdFromSelection((selectBuffer())[4*i+3]);  break;
	  default : break;
	  }
    }
	*/

  //selectionMode_ = NONE;
}

// Pinta un grid de l�neas que abarca un cuadrado de lado width con
// tantas l�nes como indica lines.
void AdriViewer::drawSceneGrid(int lines, double width) {

    double incr = width/(double)lines;
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
    glColor3f(1.0,1.0,1.0);
    for(int i = 0; i<=lines; i++)
    {
        glVertex3f(-width/2+incr*i+m_ptCenter[0], -width/2+m_ptCenter[1], -5);
        glVertex3f(-width/2+incr*i+m_ptCenter[0], width/2+m_ptCenter[1], -5);
    }

    for(int i = 0; i<=lines; i++)
    {
        glVertex3f(-width/2+m_ptCenter[0], -width/2+incr*i+m_ptCenter[1], -5);
        glVertex3f(width/2+m_ptCenter[0], -width/2+incr*i +m_ptCenter[1], -5);
    }

    glEnd();
}

//   S e l e c t i o n   t o o l s

void AdriViewer::addIdToSelection(int id)
{
  if (!selection_.contains(id))
    selection_.push_back(id);
}

void AdriViewer::removeIdFromSelection(int id)
{
  selection_.removeAll(id);
}

void AdriViewer::drawSelectionRectangle() const
{
  startScreenCoordinatesSystem();
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);

  glColor4f(0.0, 0.0, 0.3f, 0.3f);
  glBegin(GL_QUADS);
  glVertex2i(rectangle_.left(),  rectangle_.top());
  glVertex2i(rectangle_.right(), rectangle_.top());
  glVertex2i(rectangle_.right(), rectangle_.bottom());
  glVertex2i(rectangle_.left(),  rectangle_.bottom());
  glEnd();

  glLineWidth(2.0);
  glColor4f(0.4f, 0.4f, 0.5f, 0.5f);
  glBegin(GL_LINE_LOOP);
  glVertex2i(rectangle_.left(),  rectangle_.top());
  glVertex2i(rectangle_.right(), rectangle_.top());
  glVertex2i(rectangle_.right(), rectangle_.bottom());
  glVertex2i(rectangle_.left(),  rectangle_.bottom());
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  stopScreenCoordinatesSystem();
}

 //   C u s t o m i z e d   m o u s e   e v e n t s

 void AdriViewer::mousePressEvent(QMouseEvent* e)
 {
	/*
   selection_.clear();

   // Start selection. Mode is ADD with Shift key and TOGGLE with Alt key.
   rectangle_ = QRect(e->pos(), e->pos());

   if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
   {
     selectionMode_ = ADD;
   }
   else
     if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::AltModifier))
     {
       selectionMode_ = REMOVE;
     }
      else
       {
        // if (e->modifiers() == Qt::ControlModifier)
        //    startManipulation();

            QGLViewer::mousePressEvent(e);
       }
	   */

	QGLViewer::mousePressEvent(e);

 }

 void AdriViewer::mouseMoveEvent(QMouseEvent* e)
 {
   /*
   if (selectionMode_ != NONE)
     {
       // Updates rectangle_ coordinates and redraws rectangle
       rectangle_.setBottomRight(e->pos());
       updateGL();
     }
   else
   */

   QGLViewer::mouseMoveEvent(e);
 }

 void AdriViewer::mouseReleaseEvent(QMouseEvent* e)
 {
	//if(selMgr.ctx == CTX_CREATE_SKT)
	//	postSelection(e->pos());

	/*
   if (selectionMode_ != NONE) // Seleccion con cuadrado
     {
       // Actual selection on the rectangular area.
       // Possibly swap left/right and top/bottom to make rectangle_ valid.
       rectangle_ = rectangle_.normalized();
       // Define selection window dimensions
       setSelectRegionWidth(rectangle_.width());
       setSelectRegionHeight(rectangle_.height());
       // Compute rectangle center and perform selection
       select(rectangle_.center());
       // Update display to show new selected objects

       printf("Seleccion:\n");
       for (QList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
       {
           printf("%d ", *it);
       }
       printf("\n\n"); fflush(0);

       //startManipulation();
       updateGL();
     }
   else
   {*/
	
	 
     QGLViewer::mouseReleaseEvent(e);
   //}
 }

 void AdriViewer::startManipulation()
{
  Vec averagePosition;
  ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
  mfsc->clearSet();

  for (QList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
    {
      //mfsc->addObjectToSet(objects_[*it]);
      //averagePosition += objects_[*it]->frame.position();
    }

  if (selection_.size() > 0)
    manipulatedFrame()->setPosition(averagePosition / selection_.size());
}

 void AdriViewer::ChangeStillCage(int id)
{
    //stillCageSelected = parent->ui->cagesComboBox->itemData(id).toInt();
}

void AdriViewer::loadSelectableVertex(Cage* cage /* MyMesh& cage*/)
{
    // TODO
    /*
    MyMesh::VertexIterator vi;
    for(vi = cage.vert.begin(); vi!=cage.vert.end(); ++vi )
    {
        DrawObject* o = new DrawObject();
        o->frame.setPosition(Vec(vi->P()[0], vi->P()[1], vi->P()[2])); // Posicion
        o->id = vi->IMark(); // id
        objects_.append(o);
    }
    */
}


 //void AdriViewer::loadSelectVertexCombo(MyMesh& cage)
 //{
     //QString vertName("Vertice %1");
     //parent->ui->selectElementComboLabel->setText("Vertices de la caja");

     //MyMesh::VertexIterator vi;
     //int idx = 0;
     //for(vi = cage.vert.begin(); vi!=cage.vert.end(); ++vi )
     //    parent->ui->selectElementCombo->addItem(vertName.arg(idx), QVariant(idx++));

     //connect(parent->ui->selectElementCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeVertexSelection(int)));
 //}

 void AdriViewer::changeVertexSelection(int id)
 {
	
	//influenceDrawingIdx = parent->ui->selectElementCombo->itemData(id).toInt();
    //updateGL();
 }

 void AdriViewer::paintModelWithData() {

	//AirRig* rig = escena->rig;

    for(unsigned int modelIdx = 0; modelIdx < escena->models.size(); modelIdx++)
    {
        Modelo* m = (Modelo*)escena->models[modelIdx];

		if(m->shading->colors.size() != m->vn())
            m->shading->colors.resize(m->vn());

        m->cleanSpotVertexes();

        double maxdistance = 0.001;
        vector<double> pointdistances;
        vector<double> maxdistances;
        maxdistances.resize(m->bind->surfaces.size());

        vector<double> weights;
        vector<int> weightssort(m->vn());
        vector<bool> weightsrepresentative(m->vn());

        vector<double> completedistances;
        double threshold = escena->weightsThreshold;//1/pow(10.0, 3);

		if(escena->iVisMode == VIS_ISODISTANCES || escena->iVisMode == VIS_POINTDISTANCES || escena->iVisMode == VIS_ERROR || escena->iVisMode == VIS_WEIGHT_INFLUENCE)
        {
			/*
            printf("tiempos de computacion para %d vertices: \n", m->vn()); fflush(0);

            clock_t ini, fin;
            ini = clock();

            weights.resize(m->vn());
            int currentbinding = 1;
            pointdistances.resize(m->vn());
            //pointdistances.resize(m->bindings[currentbinding]->pointdata.size());
            //mvcsinglebinding(interiorpoint, weights, m->bindings[currentbinding], *m);
            mvcAllBindings(interiorPoint, weights, m->bindings, *m);

            fin = clock();
            printf("mean value coordinates: %f ms\n", timelapse(fin,ini)*1000); fflush(0);
            ini = clock();

            //for(int si = 0; si < weights.size(); si++)
            //{
            //	weightssort[si] = si;
            //}
            //doublearrangeelements(weights, weightssort, true, threshold);
            vector<double> stats;
            doubleArrangeElements_withStatistics(weights, weightssort, stats, threshold);


            fin = clock();
            printf("ordenacion: %f ms\n", timelapse(fin,ini)*1000); fflush(0);
            ini = clock();

            double prevalue = 0;
            for(int currentbinding = 0; currentbinding < m->bindings.size(); currentbinding++)
            {
                int iniidx = m->bindings[currentbinding]->globalIndirection.front();
                int finidx = m->bindings[currentbinding]->globalIndirection.back();
                prevalue += PrecomputeDistancesSingular_sorted(weights, weightssort,  m->bindings[currentbinding]->BihDistances, threshold);
            }

            fin = clock();
            printf("precomputo: %f ms\n", timelapse(fin,ini)*1000); fflush(0);
            ini = clock();

            for(int i = 0; i< m->modelVertexBind.size(); i++)
            {
                int ibind = m->modelVertexBind[i];
                int iniidx = m->bindings[ibind]->globalIndirection.front();
                int finidx = m->bindings[ibind]->globalIndirection.back();

                int ivertexbind = m->modelVertexDataPoint[i];

                pointdistances[i] = -BiharmonicDistanceP2P_sorted(weights, weightssort, ivertexbind , m->bindings[ibind], 1.0, prevalue, threshold);

                maxdistances[ibind] = max(maxdistances[ibind],pointdistances[i]);
                maxdistance = max(pointdistances[i], maxdistance);
            }

            fin = clock();
            printf("calculo distancias: %f ms\n", timelapse(fin,ini)*1000); fflush(0);
            ini = clock();

            for(int md = 0; md< maxdistances.size(); md++)
            {
                printf("bind %d con maxdist: %f\n", md, maxdistances[md]);
                fflush(0);
            }
			*/

            if(escena->iVisMode == VIS_ERROR)
            {
				/*
                completedistances.resize(m->vn());
                double prevalue2 = 0;
                for(int currentbinding = 0; currentbinding < m->bindings.size(); currentbinding++)
                {
                    int iniidx = m->bindings[currentbinding]->globalIndirection.front();
                    int finidx = m->bindings[currentbinding]->globalIndirection.back();
                    prevalue2 += PrecomputeDistancesSingular_sorted(weights, weightssort,  m->bindings[currentbinding]->BihDistances, -10);
                }

                for(int i = 0; i< m->modelVertexBind.size(); i++)
                {
                    int ibind = m->modelVertexBind[i];
                    int iniidx = m->bindings[ibind]->globalIndirection.front();
                    int finidx = m->bindings[ibind]->globalIndirection.back();

                    int ivertexbind = m->modelVertexDataPoint[i];
                    completedistances[i] = -BiharmonicDistanceP2P_sorted(weights, weightssort, ivertexbind , m->bindings[ibind], 1.0, prevalue2, -10);
                }
				*/
            }
        }

        double maxError = -9999;
        if(!m->bind) continue;
		//for(int currentbinding = 0; currentbinding < m->bind->surfaces.size(); currentbinding++)
        //{
            for(int count = 0; count< m->bind->pointData.size(); count++)
            {
                if(m->bind->pointData[count].isBorder)
                    m->addSpotVertex(m->bind->pointData[count].node->id);

                float value = 0.0;
                // deberia ser al reves, recorrer el binding y este pinta los puntos del modelo.
                // hacer un reset antes con el color propio del modelo.
                binding* bd = m->bind;
                PointData& pd = bd->pointData[count];
                int newvalue = 0;
                if(escena->iVisMode == VIS_LABELS)
                {
					value = (float)pd.component/(float)m->bind->surfaces.size();
                }
                else if(escena->iVisMode == VIS_SEGMENTATION)
                {
                    newvalue = (pd.segmentId-100)*13;
                    value = ((float)(newvalue%50))/50.0;
                }
                else if(escena->iVisMode == VIS_BONES_SEGMENTATION)
                {
					newvalue = (bd->nodeIds[pd.segmentId]->boneId-100)*13;
                    //newvalue = (newvalue * 25) % 100;
                    value = ((float)(newvalue%25))/25.0;
                }
                else if(escena->iVisMode == VIS_WEIGHTS)
                {
                    //float sum = 0;
                    value = 0.0;

                    int searchedindex = -1;
                    for(unsigned int ce = 0; ce < pd.influences.size(); ce++)
                    {
                        if(pd.influences[ce].label == escena->desiredVertex)
                        {
                            searchedindex = ce;
                            break;
                        }
                        //sum += pd.influences[ce].weightvalue;
                    }
                    if(searchedindex >= 0)
                            value = pd.influences[searchedindex].weightValue;

                    //if (sum < 1)
                    //	printf("no se cumple la particion de unidad: %f.\n", sum);
                }
				else if(escena->iVisMode == VIS_SECW_WIDE)
                {
                    assert(false);
                }
                else if(escena->iVisMode == VIS_SECW_ALONG)
                {
					value = 0.0;

                    int searchedindex = -1;
                    for(unsigned int ce = 0; ce < pd.influences.size(); ce++)
                    {
                        if(pd.influences[ce].label == escena->desiredVertex)
                        {
                            searchedindex = ce;
                            break;
                        }
                        //sum += pd.influences[ce].weightvalue;
                    }
                    if(searchedindex >= 0)
					{
						if(pd.secondInfluences[searchedindex].size()> 0)
						{
							if(valueAux < pd.secondInfluences[searchedindex].size())
								value = pd.secondInfluences[searchedindex][valueAux].alongBone;
							else
								value = pd.secondInfluences[searchedindex][pd.secondInfluences[searchedindex].size()-1].alongBone;
						}
						else
						{
							value = 1.0;
						}
					}

					/*
                    value = 0.0;
                    if(count == escena->desiredVertex)
                    {
                        value = 1.0;
                    }
                    else
                    {
                        for(int elemvecino = 0; elemvecino < bd->surface.nodes[count]->connections.size() ; elemvecino++)
                        {
                            if(bd->surface.nodes[count]->connections[elemvecino]->id == escena->desiredVertex)
                            {
                                value = 0.5;
                                break;
                            }
                        }
                    }
					*/
                }
                else if(escena->iVisMode == VIS_ISODISTANCES)
                {
                    if(escena->desiredVertex <0 || escena->desiredVertex >= bd->pointData.size())
                        value = 0;
                    else
                    {
                        if(maxdistance <= 0)
                            value = 0;
                        else
                            value = m->bind->BihDistances[0].get(count,escena->desiredVertex) / maxdistance;
                    }
                }
                else if(escena->iVisMode == VIS_POINTDISTANCES)
                {
                    if(maxdistance <= 0)
                        value = 0;
                    else
                    {
                        int modelvert = m->bind->pointData[count].node->id;
                        value = pointdistances[modelvert] ;/// maxdistances[currentbinding];
                    }
                }
                else if(escena->iVisMode == VIS_ERROR)
                {
					value = 0.0;

                    int searchedindex = -1;
                    for(unsigned int ce = 0; ce < pd.influences.size(); ce++)
                    {
                        if(pd.influences[ce].label == escena->desiredVertex)
                        {
                            searchedindex = ce;
                            break;
                        }
                        //sum += pd.influences[ce].weightvalue;
                    }
                    if(searchedindex >= 0)
					{
						if(pd.secondInfluences[searchedindex].size()> 0)
						{
							if(valueAux < pd.secondInfluences[searchedindex].size())
								value = pd.secondInfluences[searchedindex][valueAux].alongBone*pd.influences[searchedindex].weightValue;
							else
								value = pd.secondInfluences[searchedindex][pd.secondInfluences[searchedindex].size()-1].alongBone*pd.influences[searchedindex].weightValue;
						}
						else
						{
							value = 1.0*pd.influences[searchedindex].weightValue;
						}
					}
                    /*int modelvert = m->bindings[currentbinding]->pointData[count].node->id;

                    if(completedistances[modelvert] > 0)
                        value = pointdistances[modelvert] - completedistances[modelvert];

                    maxError = max((double)maxError, (double)value);
					*/

                }
                else if(escena->iVisMode == VIS_WEIGHT_INFLUENCE)
                {
                    int modelVert = m->bind->pointData[count].node->id;
                    value = weights[modelVert];
                    //if(maxDistance <= 0)
                    //	value = 0;
                    //else
                    //{
                    //	int modelVert = m->bindings[currentBinding]->pointData[count].modelVert;
                    //	value = pointDistances[modelVert] / maxDistances[currentBinding];
                    //}
                }
                else
                {
                    value = 1.0;
                }

                float r,g,b;
                GetColourGlobal(value,0.0,1.0, r, g, b);
                //QColor c(r,g,b);
                m->shading->colors[count].resize(3);
                m->shading->colors[count][0] = r;
                m->shading->colors[count][1] = g;
                m->shading->colors[count][2] = b;
            }
        //}

        //printf("Corte:%f\n", threshold); fflush(0);
        //printf("Error max:%f\n", maxError); fflush(0);

    }
}


 void AdriViewer::paintModelWithGrid()
{
   /*
   if(VERBOSE)
       printf(">> PaintModelWithGrid (for testing)\n");

   // Cada grid renderer actualizara sus modelos.
   for(unsigned int vis = 0; vis < escena->visualizers.size(); vis++)
   {
       gridRenderer* grRend = (gridRenderer*)escena->visualizers[vis];
       if(grRend == NULL) continue;

       Modelo* m = grRend->model;
       if(m == NULL) continue;

       if(grRend == NULL || !grRend->Initialized) return;

       m->shading->colors.resize(m->vn());

       if(grRend->iVisMode == VIS_SEGMENTATION || grRend->iVisMode == VIS_WEIGHTS)
       {
           int count = 0;
           MyMesh::VertexIterator vi;
           for(vi = m->vert.begin(); vi!=m->vert.end(); ++vi ) {
               m->shading->colors[count].resize(3);
               Eigen::Vector3d pt = vi->P();

               Vector3i idx = grRend->grid->cellId(pt);

               float value = 0;

               if(grRend->iVisMode == VIS_WEIGHTS)
               {
                   int searchedIndex = -1;
                   for(unsigned int ce = 0; ce < grRend->grid->cells[idx.X()][idx.Y()][idx.Z()]->data->influences.size(); ce++)
                   {
                       if(grRend->grid->cells[idx.X()][idx.Y()][idx.Z()]->data->influences[ce].label == grRend->desiredVertex)
                       {
                           searchedIndex = ce;
                           break;
                       }
                   }

                   if(searchedIndex >= 0)
                       value = grRend->grid->cells[idx.X()][idx.Y()][idx.Z()]->data->influences[searchedIndex].weightValue;

               }
               else if(grRend->iVisMode == VIS_SEGMENTATION)
               {
                   if(grRend->grid->cells[idx.X()][idx.Y()][idx.Z()]->data->segmentId >= 0 &&
                      grRend->grid->cells[idx.X()][idx.Y()][idx.Z()]->data->segmentId < grRend->grid->valueRange)
                   {
                       int newValue = (grRend->grid->cells[idx.X()][idx.Y()][idx.Z()]->data->segmentId * 13) % (int)ceil(grRend->grid->valueRange);
                       value = newValue/grRend->grid->valueRange;
                   }
                   else value = 0;
               }

               assert((grRend->grid->valueRange-1) >= 0);
               float r,g,b;
               GetColour(value,0,1, r, g, b);
               QColor c(r,g,b);
               m->shading->colors[count][0] = c.redF();
               m->shading->colors[count][1] = c.blueF();
               m->shading->colors[count][2] = c.greenF();

               count++;
           }
       }
   }
   */
}

 float AdriViewer::calculateDistancesForISOLines(grid3d* grid, vector<double>&  embeddedPoint)
{
    float maxDistance = 0;
    for(int i = 0; i < grid->cells.size(); i++)
    {
        for(int j = 0; j < grid->cells[i].size(); j++)
        {
            for(int k = 0; k < grid->cells[i][j].size(); k++)
            {
                cell3d* cell = grid->cells[i][j][k];

                if(cell->getType() == EXTERIOR || cell->getType() == INTERIOR) continue;

                // TOFIX
                //cell->data->distanceToPos = distancesFromEbeddedPoints(embeddedPoint, cell->data->embedding);
                if(cell->data->distanceToPos > maxDistance)
                    maxDistance = cell->data->distanceToPos;
            }
        }
    }

    return maxDistance;
}


 void AdriViewer::paintGrid(gridRenderer* grRend)
{

   float maxDistance = 0;
   if( grRend->iVisMode == VIS_ISODISTANCES || grRend->iVisMode == VIS_ISOLINES)
   {
       // Calcular distancia maxima.
       int findIndex = -1;
       for(int i = 0; i< grRend->grid->v.embeddedPoints.size(); i++)
       {
           if(grRend->grid->v.intPoints[i].boneId == grRend->desiredVertex)
           {
               findIndex = i;
               break;
           }
       }

       if(findIndex >= 0)
       {
           vector<double>& embeddedPoint = grRend->grid->v.embeddedPoints[findIndex];
           maxDistance = calculateDistancesForISOLines(grRend->grid, embeddedPoint);
           //maxDistance = 4000;
       }
       if(maxDistance <= 0)
           maxDistance = 1;
   }

   for(int i = 0; i < grRend->grid->cells.size(); i++)
   {
       for(int j = 0; j < grRend->grid->cells[i].size(); j++)
       {
           for(int k = 0; k < grRend->grid->cells[i][j].size(); k++)
           {
               int newValue;
               float colorValue;
               int idx;
               float nInterval;
               float dif;
               float confidenceLimit;

               cell3d* cell = grRend->grid->cells[i][j][k];
               if(cell->getType() == EXTERIOR || cell->getType() == INTERIOR) continue;

               switch(grRend->iVisMode)
               {
               case VIS_SEGMENTATION:
                      /* newValue = (cell->data->segmentId * 25) % (int)ceil(grRend->grid->valueRange);
                       colorValue = newValue/grRend->grid->valueRange;*/
                   break;
               case VIS_BONES_SEGMENTATION:
				   /*
                       newValue = grRend->grid->v.nodeIds[cell->data->segmentId]->boneId;
                       newValue = (newValue * 25) % (int)ceil(grRend->grid->valueRange);
                       colorValue = newValue/grRend->grid->valueRange;
					   */

                       //if(!cell->data->validated)
                       //	colorValue = -1;
                   break;
               case VIS_SECW_WIDE:
				   /*
                       if(cell->data->ownerLabel>0 && cell->data->ownerLabel< grRend->grid->valueRange)
                       {
                           newValue = grRend->grid->v.nodeIds[cell->data->ownerLabel]->boneId;
                           newValue = (newValue * 13) % (int)ceil(grRend->grid->valueRange);
                           colorValue = newValue/grRend->grid->valueRange;
                       }
                       else
                       {
                           colorValue = 1;
                       }
					   */
                   break;
               case VIS_WEIGHTS:
                       idx = findWeight(cell->data->influences, grRend->desiredVertex);

                       if(idx >= 0)
                           colorValue = cell->data->influences[idx].weightValue;
                       else
                           colorValue = -1;
               break;
               case VIS_ISODISTANCES:
                       //colorValue = cell->data->distanceToPos / maxDistance;
                   break;

               case VIS_ISOLINES:
				   /*
                       nInterval = maxDistance/20.0;
                       dif = cell->data->distanceToPos/nInterval;
                       dif = dif*nInterval - floor(dif)*nInterval;
                       if( dif < maxDistance/128)
                       {
                           colorValue = -1;
                       }
                       else
                       {
                           colorValue = cell->data->distanceToPos / maxDistance;
                       }
					   */
                   break;

               case VIS_SECW_ALONG:
                       colorValue = cell->data->confidenceLevel;
                       confidenceLimit = grRend->grid->minConfidenceLevel;
                       if(colorValue > confidenceLimit )
                           colorValue = confidenceLimit;
                       if(colorValue < 0)
                           colorValue = 0;
                       colorValue = colorValue/confidenceLimit;
                   break;

               default:
                       colorValue = 1;
                   break;
               }

			   float errorColor = -1;

               // En escala de grises para parecernos mas a maya
               float r,g,b;
               if(colorValue < 0)
               {
                   cell->data->color[0] = 0.0;
                   cell->data->color[1] = 0.0;
                   cell->data->color[2] = 0.0;
               }
               else if(colorValue > 1)
               {
                   cell->data->color[0] = 1.0;
                   cell->data->color[1] = 1.0;
                   cell->data->color[2] = 1.0;
               }
               else
               {
                   GetColour(colorValue,0,1,r,g,b);
                   cell->data->color[0] = r;
                   cell->data->color[1] = g;
                   cell->data->color[2] = b;
               }

           }
       }
   }
}

void AdriViewer::changeVisualizationMode(int mode)
{
    escena->iVisMode = mode;

	paintModelWithData();   // TODO paint model

    for(unsigned int i = 0; i< escena->visualizers.size(); i++)
    {
        if(!escena->visualizers[i] || escena->visualizers[i]->iam != GRIDRENDERER_NODE)
            continue;

         ((gridRenderer*)escena->visualizers[i])->iVisMode = mode;
         paintGrid((gridRenderer*)escena->visualizers[i]);
         paintModelWithGrid();
         ((gridRenderer*)escena->visualizers[i])->propagateDirtyness();
    }
}

void AdriViewer::updateGridRender()
{
    for(unsigned int i = 0; i< escena->visualizers.size(); i++)
    {
        if(escena->visualizers[i]->iam == GRIDRENDERER_NODE)
        {

            gridRenderer* gr = ((gridRenderer*)escena->visualizers[i]);
            gr->m_bShowAllGrid = parent->ui->allGrid_button->isChecked();
            //gr->m_bBorders = parent->ui->GridDraw_interior->isChecked();
			gr->m_bShow_interior = parent->ui->GridDraw_interior->isChecked();
            gr->m_bShow_exterior = parent->ui->GridDraw_exterior->isChecked();
            gr->m_bShow_boundary = parent->ui->GridDraw_boundary->isChecked();

			gr->m_iWeightsId = parent->ui->dataSource->value();

            gr->grid->res = parent->ui->gridResolutionIn->text().toInt();
            gr->grid->worldScale = parent->ui->sceneScale->text().toInt();

            gr->desiredVertex = parent->ui->DistancesVertSource->text().toInt();

            gr->setSliceXY(parent->ui->SliceSelectorXY->value());
            gr->setSliceXZ(parent->ui->SliceSelectorXZ->value());
						
			gr->m_iWeightsId = parent->ui->auxValueInt->value();
			
			gr->updateGridColorsAndValues();
			//gr->updateGridColorsAndValuesRGB();

            gr->propagateDirtyness();
			
       }
    }
}

 void AdriViewer::UpdateVertexSource(int id)
{
    escena->desiredVertex = id;
	paintModelWithData();   // TODO paint model
	updateGridRender();
    //paintPlaneWithData();

    //updateGridRender();

    //TODO
    /*
   double vmin = 9999999, vmax = -9999999;
   for(int i = 0; i < m.modeloOriginal.vn; i++)
   {
       vmin = min(BHD_distancias[i][id], vmin);
       vmax = max(BHD_distancias[i][id], vmax);
   }

   printf("Distancias-> max:%f min:%f\n", vmax, vmin); fflush(0);

   vertexColors.resize(m.modeloOriginal.vn);
   for(int i = 0; i < m.modeloOriginal.vn; i++)
   {
       vertexColors[i] = GetColour(BHD_distancias[i][id], vmin, vmax);
   }

   colorsLoaded = true;

   updateGL();
   */
}

 void AdriViewer::doTests(string fileName, string name, string path) 
 {
 
 }
