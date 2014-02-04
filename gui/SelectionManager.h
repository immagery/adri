#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <vector>
//#include "Handle.h"
#include "DataStructures/Node.h"

class object;

// trabajo en objeto o subobjeto.
enum selectionMode { SM_SUBOBJECT = 0, SM_OBJECT};

enum contextMode { CTX_SELECTION = 0, CTX_MOVE, CTX_ROTATION, CTX_SCALE}; // contexto de transformacion
enum ToolType { CTX_TRANSFORMATION = 0, CTX_CREATE_SKT}; // que herramienta

using namespace std;
class selectionManager : public node
{
public:

    selectionMode mode;

	// Transformation context;	
	contextMode toolCtx;

	// ToolSet for working
	ToolType currentTool;

    vector< object* > selection;

    selectionManager() : node()
    {
       mode = SM_OBJECT;
       toolCtx = CTX_SELECTION;
	   currentTool = CTX_TRANSFORMATION;
    }

    virtual void drawFunc();
    virtual bool update();
};

#endif // SELECTIONMANAGER_H
