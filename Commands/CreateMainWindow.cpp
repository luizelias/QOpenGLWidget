#include "CreateMainWindow.h"
#include "Global.h"
#include "MainWindow.h"
#include "Viewport.h"
#include "Mesh.h"
#include "GPU.h"
#include "SculptDeformer.h"

#include <math.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MItMeshPolygon.h>

namespace mush3d{

MSyntax CreateMainWindow::newSyntax()
{
    MSyntax syntax;
    syntax.addFlag(deformerFlag, deformerFlagLong, MSyntax::kString);
    return syntax;
}

MStatus CreateMainWindow::doIt(const MArgList & args)
{
    /*
    // get flags
    MStatus status;
    MArgDatabase argData(syntax(), args, &status);

    if (!status)
    {
        MGlobal::displayError("error creating flag for CreateMainWindow");
        return MStatus::kFailure;
    }

    // get deformer name
    MString deformerName;
    if (argData.isFlagSet(deformerFlag))
    {
        status = argData.getFlagArgument(deformerFlag, 0, deformerName);

        if (!status)
        {
            MGlobal::displayError("error getting flag for deformerName");
            return MStatus::kFailure;
        }
    }

    // get deformer MObject
    MSelectionList selList;
    status = selList.add(deformerName);

    if (!status)
    {
        MGlobal::displayError("error getting deformer MObject");
        return MStatus::kFailure;
    }

    MObject oDeformer;
    status = selList.getDependNode(0, oDeformer);

    if (!status)
    {
        MGlobal::displayError("error getting deformer MObject");
        return MStatus::kFailure;
    }

    // check if deformer MObject is valid
    MFnDependencyNode fnNode(oDeformer);

    if (fnNode.typeId() != SculptDeformer::id)
    {
        MGlobal::displayError("name passed to -deformer flag is not a m3dSculpt deformer");
        return MStatus::kFailure;
    }
    */


    auto w = Global::getInstance()->mainWindow();
    w->show();
    w->showNormal();
    w->raise();

    /*
    // load mesh
    MSelectionList selected;
    MGlobal::getActiveSelectionList(selected);

    MObject oMesh;

    for (unsigned int i = 0; i < selected.length(); i++) {
        MDagPath path;
        selected.getDagPath(i, path);
        path.extendToShape();

        MObject node = path.node();

        if(node.hasFn(MFn::kMesh))
        {
            oMesh = node;
            break;
        }
    }

    if(oMesh.isNull() == false)
    {
        // get points
        MFnMesh fnMesh(oMesh);

        MFloatPointArray pts;
        fnMesh.getPoints(pts);

        int numPoints = pts.length();
        vec4Array points(numPoints);

        for(int i=0; i<numPoints; i++)
        {
            MFloatPoint & p = pts[i];
            points[i] = vec4(p.x, p.y, p.z, 1.0f);
        }

        // faces
        uintArray faceVertices;
        uintArray faceCounts;

        MItMeshPolygon piter(oMesh);
        while(!piter.isDone())
        {
            MIntArray vertices;
            piter.getVertices(vertices);

            for(int i=0; i<vertices.length(); i++)
            {
                faceVertices.append(vertices[i]);
            }

            faceCounts.append(vertices.length());

            piter.next();
        }

        // build mesh and level 0
        Mesh * mesh = Global::getInstance()->mesh();
        mesh->build(points, faceVertices, faceCounts);

        // compute normals for level 0
        auto level = mesh->getCurrentLevel();
        GPU::getInstance()->computeFaceNormals(mesh->positions(), level->faceIndices(), level->faceNormals(), level->getNumFaces());
        GPU::getInstance()->computeVertexNormals(mesh->normals(), level->faceNormals(), level->faceRingsSC(), level->faceRings(), level->getNumVertices());

        // set morph points
        Global::getInstance()->setMorphPoints(points);
    }
    */

    return MS::kSuccess;
}


}
