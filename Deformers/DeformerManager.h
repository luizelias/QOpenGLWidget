#ifndef DEFORMERMANAGER_H
#define DEFORMERMANAGER_H

/*
This class is used to manage the current deformer on the ui
checks if the deformer has been deleted
has a mesh object which is the combination of all meshes of the deformer
updates the mesh based on the selected plug
*/

#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MSelectionList.h>
#include <maya/MObjectHandle.h>

namespace mush3d{

class Mesh;

class DeformerManager
{
private:
    DeformerManager();

public:
    // remove copy and assign operator
    DeformerManager(const DeformerManager&) = delete;
    void operator=(DeformerManager const&) = delete;

    static DeformerManager * getInstance();

    MStatus createNewDeformer();
    void createLayer();

private:
    MDagPath _path;
    Mesh * mesh;
    MObject _deformer;
};



}
#endif // DEFORMERMANAGER_H
