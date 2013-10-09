#ifdef WIN32
#include <GL/glew.h>
#endif

#ifdef WIN64
#include <GL/glew.h>
#endif

#define VERBOSE false

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
//#include "ui_mainwindow.h"

#include <DataStructures/Scene.h>
#include <DataStructures/Cage.h>
#include <DataStructures/Modelo.h>
#include <render/ShadingNode.h>

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

//#include "solvers/SolverSinusoidal.h"
//#include "solvers/SolverStatic.h"
//#include "solvers/SolverVerlet.h"


using namespace qglviewer;
using namespace vcg;

AdriViewer::AdriViewer(QWidget * parent , const QGLWidget * shareWidget, Qt::WindowFlags flags)
        : QGLViewer(parent, shareWidget, flags)
{
    selectionMode_ = NONE;

    // Scene variables.
    m_ptCenter.SetZero();
    m_sceneRadius = 30;

    m_sceneMin[0] = -m_sceneRadius;
    m_sceneMin[1] = -m_sceneRadius;
    m_sceneMin[2] = -m_sceneRadius;

    m_sceneMax[0] = m_sceneRadius;
    m_sceneMax[1] = m_sceneRadius;
    m_sceneMax[2] = m_sceneRadius;

    initScene();

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

	CurrentProcessJoints.clear();

	interiorPoint = Point3d(0,0,0);

    //ctxMode = CTX_SELECTION;

    frame = 0;
	aniManager.simulationEnabled = false;
	aniManager.animationEnabled = false;
	//particles = new Particles();
	//particles->bake(1500, 1/24.0);

	setSceneCenter(Vec(0,0,0));
    setSceneRadius(40);
	ReBuildScene();
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

    // Used to display semi-transparent relection rectangle
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDepthFunc(GL_LEQUAL);

    restoreStateFromFile();
    setAnimationPeriod(40);
    startAnimation();

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
                Point3d position = aniManager.getPosition(id, frame);
                Point3d rotation = aniManager.getRotation(id, frame);
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
        for(unsigned int m = 0; m< escena->models.size(); m++)
        {
            if(((object*)escena->models[m])->nodeId == lst[i])
            {
               selMgr.selection.push_back(escena->models[m]);
            }
        }

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
                          vector< Point3d >& nodePoints,
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

         Point3d pt;
         for(int i = 0; i< 3; i++)
             pt[i] = lstr[i+1].toDouble();

         nodePoints[element] = pt;

         element++;
     }

     inFile.close();

     return true;
 }


 // Lee una lista de puntos.
 bool AdriViewer::readPoints(vector< Point3d >& points,
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

         Point3d pt;
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
        QString sModelFile = in.readLine(); in.readLine(); in.readLine();
        QString sSkeletonFile = in.readLine(); in.readLine(); in.readLine();
        QString sEmbeddingFile = in.readLine(); in.readLine(); in.readLine();
        QString newPath(path.c_str());
        newPath = newPath +"/";
        if(!sPath.isEmpty())
            newPath = newPath+"/"+sPath +"/";

        // Leer modelo
        readModel( (newPath+sModelFile).toStdString(), sSceneName.toStdString(), newPath.toStdString());

        Modelo* m = ((Modelo*)escena->models.back());
        m->sPath = newPath.toStdString(); // importante para futuras referencias

        // Leer esqueleto
        readSkeleton((newPath+sSkeletonFile).toStdString());

        // Leer embedding
        //ReadEmbedding((newPath+sEmbeddingFile).toStdString(), m->embedding);
		
        modelDefFile.close();
		
		// Skinning
		QString path = (sGlobalPath.append(sPath).append("binding.txt"));
		string spath = path.toStdString();
		escena->loadBindingForModel(m,(newPath.append("binding.txt").toStdString()));
        escena->skinner->computeRestPositions(escena->skeletons);

		
		/*SolverSinusoidal* sinY = new SolverSinusoidal(0.1,1.5,0);	sinY->dimension = 1;
		SolverSinusoidal* sinZ = new SolverSinusoidal(0.2,1,0);	sinZ->dimension = 2;
		SolverSinusoidal* mouthSolver = new SolverSinusoidal(0.3,1,0); mouthSolver->dimension = 2;
		SolverStatic* solverS = new SolverStatic();
		SolverVerlet* verlet = new SolverVerlet();
		int n = escena->skeletons[0]->joints.size();*/
		escena->skeletons[0]->joints[0]->computeWorldPos();

		// Verlet
		//for (int i = 0; i < n; ++i) verlet->addJoint(escena->skeletons[0]->joints[i], i);

		/*for (int i = 0; i < n; ++i) {

			// Set the static pos
			if (i >= n-7) escena->skeletons[0]->joints[i]->rot.Z() -= 10;
			else if (i < n-5) escena->skeletons[0]->joints[i]->rot.Z() -= 3;

			// Sinusoidals
			if (i > 0 && i < n-5 && i%2 == 0) {
				sinY->addJoint(escena->skeletons[0]->joints[i], i);
				sinZ->addJoint(escena->skeletons[0]->joints[i], i);
			}
			if (i >= n-6 && i % 2 == 0) mouthSolver->addJoint(escena->skeletons[0]->joints[i], i);

			// Static
			solverS->addJoint(escena->skeletons[0]->joints[i], i);
			
		}*/
		
		//solverS->setStatic();
		//verlet->setPositions();
		//escena->solverManager->addSolver(sinY);
		//escena->solverManager->addSolver(sinZ);
		//escena->solverManager->addSolver(solverS);
		//escena->solverManager->addSolver(mouthSolver);
		//escena->solverManager->addSolver(verlet);
		//aniManager.animationEnabled = true;
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
     restoreStateFromFile();
 }

 // Inicializates the scene taking in account all the models of the scene.
                                             void AdriViewer::ReBuildScene(){

     double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
     bool init_ = false;
     // Recomponemos la bounding box de la escena
     for(unsigned int i = 0; i< escena->models.size(); i++)
     {
         Point3d minAuxPt, maxAuxPt;
         escena->models[i]->getBoundingBox(minAuxPt, maxAuxPt);


         if(!init_)
         {
             minX = minAuxPt.X();
             minY = minAuxPt.Y();
             minZ = minAuxPt.Z();

             maxX = maxAuxPt.X();
             maxY = maxAuxPt.Y();
             maxZ = maxAuxPt.Z();
             init_  = true;
         }
         else
         {
             minX = min(minAuxPt.X(), minX);
             minY = min(minAuxPt.Y(), minY);
             minZ = min(minAuxPt.Y(), minY);

             maxX = max(maxAuxPt.X(), maxX);
             maxY = max(maxAuxPt.Y(), maxY);
             maxZ = max(maxAuxPt.Z(), maxZ);
         }
     }

     for(unsigned int i = 0; i< escena->skeletons.size(); i++)
     {
         // TODO
         // Queda mirar esto... lo que tengo previsto es pedir al esqueleto... como con el modelo.

         Point3d minAuxPt, maxAuxPt;
         escena->skeletons[i]->getBoundingBox(minAuxPt, maxAuxPt);

         if(!init_)
         {
             minX = minAuxPt.X();
             minY = minAuxPt.Y();
             minZ = minAuxPt.Z();

             maxX = maxAuxPt.X();
             maxY = maxAuxPt.Y();
             maxZ = maxAuxPt.Z();
             init_ = true;
         }
         else
         {
             minX = min(minAuxPt.X(), minX);
             minY = min(minAuxPt.Y(), minY);
             minZ = min(minAuxPt.Y(), minY);

             maxX = max(maxAuxPt.X(), maxX);
             maxY = max(maxAuxPt.Y(), maxY);
             maxZ = max(maxAuxPt.Z(), maxZ);
         }
     }

     Point3d minPt(minX,minY,minZ);
     Point3d maxPt(maxX,maxY,maxZ);

     for(int i = 0; i< 3; i++) m_sceneMin[i] = (float)minPt[i] ;
     for(int i = 0; i< 3; i++) m_sceneMax[i] = (float)maxPt[i] ;
     m_ptCenter = Point3d((minX+maxX)/2, (minY+maxY)/2, (minZ+maxZ)/2);

    // definimos las condiciones de la escena para el GlViewer.
    setSceneBoundingBox(Vec(m_sceneMin[0],m_sceneMin[1],m_sceneMin[2]),Vec(m_sceneMax[0],m_sceneMax[1],m_sceneMax[2]));

    Point3d minPoint(m_sceneMin[0],m_sceneMin[1],m_sceneMin[2]);

    setSceneCenter(Vec(m_ptCenter.X(),m_ptCenter.Y(),m_ptCenter.Z()));
    setSceneRadius((m_ptCenter - minPoint).Norm());

    printf("SceneMinMax: (%f,%f,%f)-(%f,%f,%f)\n",m_sceneMin[0],m_sceneMin[1],m_sceneMin[2],m_sceneMax[0],m_sceneMax[1],m_sceneMax[2]);
    printf("SceneCenter: (%f,%f,%f)\n",m_ptCenter.X(),m_ptCenter.Y(),m_ptCenter.Z());

    showEntireScene();

    // Actualizamos la vista.
    updateGL();

    // Guardamos la vista.
    saveStateToFile();

    //firstInit = false;
}

 void AdriViewer::drawWithNames()
 {
     //TODO
     /*

     //MyMesh *currentModelo;
     MyMesh *currentCage;

     currentCage = &m.dynCage;


     //if(!showDeformedModel)
     //    currentModelo = &m.modeloOriginal;
     //else
     //{
     //    if(activeCoords == HARMONIC_COORDS)
     //        currentModelo = &m.newModeloHC;
     //    else
     //        currentModelo = &m.newModeloGC;
     //}


     if(drawCage)
     {
         glPointSize(5);
         glColor3f(0.1, 1.0, 0.1);
         glDisable(GL_LIGHTING);
         MyMesh::VertexIterator vi;
         for(vi = currentCage->vert.begin(); vi!=currentCage->vert.end(); ++vi )
         {
             int id = vi->IMark();
             glPushName(id);
             glBegin(GL_POINTS);
             glVertex3dv(&(*vi).P()[0]);
             glEnd();
             glPopName();
         }
         glEnd();
         glEnable(GL_LIGHTING);
     }
     */
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

          escena->models[i]->shading->visible = !escena->models[i]->shading->visible;
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
     if(ShadingModeFlag == SH_MODE_SMOOTH)
        glShadeModel(GL_SMOOTH);
     else if(ShadingModeFlag == SH_MODE_FLAT)
        glShadeModel(GL_FLAT);

	 // SKINNING I SOLVERS
	 if (escena->skeletons.size() > 0) {
		/*vector<Point3d> rots = escena->solverManager->computeSolvers(frame,escena->skeletons);
		for (int i = 0; i < escena->skeletons[0]->joints.size(); ++i) {
			 // TOFIX escena->skeletons[0]->joints[i]->rot += rots[i];
		}*/
		escena->skinner->computeDeformations(escena->skeletons);
	 }


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

         ((Modelo*)escena->models[i])->drawFunc();
     }

	 /*if (aniManager.simulationEnabled) {
		 double fps = 1.0/this->animationPeriod()*1000;
		 double currentTime = (double)frame/fps;
		 int numReps = fps;
		 for (int k = 0; k < numReps; ++k) particles->solve(currentTime + ((double)k / numReps)*this->animationPeriod()/1000.0);
		 ++frame;
	 }

	 if (aniManager.animationEnabled) particles->drawFunc(frame);
	 else particles->drawFunc();*/

     glDisable(GL_LIGHTING);
     drawPointLocator(interiorPoint, 1, true);
     glEnable(GL_LIGHTING);

     // Pintamos el handle en el caso de que haya selecci�n.
     selMgr.drawFunc();


    // Pintamos el handle en el caso de que haya selecci�n.
     if (selMgr.ctx != CTX_SELECTION)
       drawSelectionRectangle();
     else
        selMgr.drawFunc();

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

 void AdriViewer::setContextMode(contextMode ctx)
 {
    selMgr.ctx = ctx;
 }

void AdriViewer::startManipulation()
{
  Vec averagePosition;
  ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
  mfsc->clearSet();

  for (QList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
    {
      mfsc->addObjectToSet(objects_[*it]);
      averagePosition += objects_[*it]->frame.position();
    }

  if (selection_.size() > 0)
    manipulatedFrame()->setPosition(averagePosition / selection_.size());
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

void AdriViewer::endSelection(const QPoint&)
{
  // Flush GL buffers
  glFlush();

  // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
  GLint nbHits = glRenderMode(GL_RENDER);

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
  selectionMode_ = NONE;
}

 //   C u s t o m i z e d   m o u s e   e v e n t s

 void AdriViewer::mousePressEvent(QMouseEvent* e)
 {
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

 }

 void AdriViewer::mouseMoveEvent(QMouseEvent* e)
 {
   if (selectionMode_ != NONE)
     {
       // Updates rectangle_ coordinates and redraws rectangle
       rectangle_.setBottomRight(e->pos());
       updateGL();
     }
   else
     QGLViewer::mouseMoveEvent(e);
 }

 void AdriViewer::mouseReleaseEvent(QMouseEvent* e)
 {
   if (selectionMode_ != NONE)
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
     QGLViewer::mouseReleaseEvent(e);
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


 void AdriViewer::loadSelectVertexCombo(MyMesh& cage)
 {
     //QString vertName("Vertice %1");
     //parent->ui->selectElementComboLabel->setText("Vertices de la caja");

     //MyMesh::VertexIterator vi;
     //int idx = 0;
     //for(vi = cage.vert.begin(); vi!=cage.vert.end(); ++vi )
     //    parent->ui->selectElementCombo->addItem(vertName.arg(idx), QVariant(idx++));

     //connect(parent->ui->selectElementCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeVertexSelection(int)));
 }

 void AdriViewer::changeVertexSelection(int id)
 {
	
	//influenceDrawingIdx = parent->ui->selectElementCombo->itemData(id).toInt();
    //updateGL();
 }


 void AdriViewer::changeVisualizationMode(int mode)
{
    escena->iVisMode = mode;
    // TOFIX
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

			gr->updateGridColorsAndValues();

            gr->propagateDirtyness();
			
       }
    }
}

 void AdriViewer::UpdateVertexSource(int id)
{
    escena->desiredVertex = id;
	updateGridRender();
    //paintModelWithData();   // TODO paint model
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
