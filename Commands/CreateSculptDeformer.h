#ifndef CREATESCULPTDEFORMER_H
#define CREATESCULPTDEFORMER_H

#include <maya/MSelectionList.h>
#include <maya/MPointArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDGModifier.h>

#include <maya/MItDependencyGraph.h>
#include <maya/MItGeometry.h>
#include <maya/MItSelectionList.h>

#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MPxCommand.h>

#include <maya/MFnIntArrayData.h>



#include <vector>
#include <sstream>

namespace mush3d{

class CreateSculptDeformer : public MPxCommand  {
public:
    CreateSculptDeformer();
    virtual ~CreateSculptDeformer();
    virtual MStatus doIt(const MArgList& argList);
    virtual MStatus redoIt();
    virtual MStatus undoIt();
    virtual bool isUndoable() const;
    static void* creator();

private:
    MStatus getShapeNode(MDagPath& path);
    MStatus getDeformer(MDagPath& pathGeo, MObject& oDeformer);

    MDGModifier dgMod_;
    MDagPathArray meshes_;

};

}

#endif // CREATESCULPTDEFORMER_H
