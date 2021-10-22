#ifndef CREATELAYER_H
#define CREATELAYER_H

//#include <maya/MSelectionList.h>
//#include <maya/MPointArray.h>
//#include <maya/MDagPath.h>
//#include <maya/MDagPathArray.h>
//#include <maya/MPlug.h>
//#include <maya/MPlugArray.h>
//#include <maya/MIntArray.h>
//#include <maya/MFloatArray.h>
//#include <maya/MDoubleArray.h>
//#include <maya/MString.h>
//#include <maya/MStringArray.h>
//#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
//#include <maya/MString.h>
//#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
//#include <maya/MObject.h>
//#include <maya/MObjectArray.h>
#include <maya/MDGModifier.h>

//#include <maya/MItDependencyGraph.h>
//#include <maya/MItGeometry.h>
//#include <maya/MItSelectionList.h>

//#include <maya/MFnDagNode.h>
//#include <maya/MFnDependencyNode.h>
//#include <maya/MFnMesh.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MPxCommand.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatArray.h>
#include <maya/MFnPointArrayData.h>





#include <sstream>

namespace mush3d{

class CreateLayer : public MPxCommand {
public:
    CreateLayer();
    virtual ~CreateLayer();
    virtual MStatus doIt(const MArgList& argList);
    virtual bool isUndoable() const;
    static void* creator();
    static MSyntax newSyntax();

    static const char* kDeformerNameFlag;
    static const char* kDeformerNameFlagLong;
private:
    MDGModifier _dgMod;
};

}

#endif // CREATELAYER_H
