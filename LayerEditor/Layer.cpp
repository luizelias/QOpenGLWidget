#include "Layer.h"
#include "SculptDeformer.h"
#include "Global.h"
#include "Mesh.h"
#include "Types.h"
#include "Viewport.h"

#include <maya/MObjectHandle.h>
#include <maya/MGlobal.h>
#include <maya/MFnDependencyNode.h>

namespace mush3d{

Layer::Layer(int logicalIndex, MObject deformer) :
    _logicalIndex(logicalIndex), _deformer(deformer)
{
    setText(QString::number(logicalIndex));
}


void Layer::setLayerSelected(bool s)
{
    MObjectHandle handle(_deformer);

    if(handle.isValid() == false)
    {
        MGlobal::displayError("layer has invalid deformer");
        return;
    }

    // get selection plug
    MStatus status;
    MFnDependencyNode fnNode(_deformer);

    MPlug layerPlug = fnNode.findPlug(SculptDeformer::aLayer, false, &status);
    MPlug selPlug = fnNode.findPlug(SculptDeformer::aSelected, false, &status);

    status = selPlug.selectAncestorLogicalIndex(_logicalIndex, layerPlug.attribute());

    if(!status)
    {
        MGlobal::displayError(MString("can't set selection plug value, selectAncestorLogicalIndex error ") + _logicalIndex);
    }

    selPlug.setBool(s);

    // update mesh
    if(s)
    {
        // get topology
        MPlug faceCountsPlug = fnNode.findPlug(SculptDeformer::aFaceCounts, false, &status);
        MPlug faceVertsPlug = fnNode.findPlug(SculptDeformer::aFaceVertices, false, &status);
        MPlug numVertsPlug = fnNode.findPlug(SculptDeformer::aNumVertices, false, &status);

        MIntArray faceCounts;
        MIntArray faceVertices;
        int numVertices = numVertsPlug.asInt();

        {
            MObject data = faceCountsPlug.asMObject();
            if(data.isNull())
            {
                MGlobal::displayError("error getting face counts");
                return;
            }

            MFnIntArrayData fnData(data);
            fnData.copyTo(faceCounts);
        }

        {
            MObject data = faceVertsPlug.asMObject();
            if(data.isNull())
            {
                MGlobal::displayError("error getting face verts");
                return;
            }

            MFnIntArrayData fnData(data);
            fnData.copyTo(faceVertices);
        }

        // get sculpt points
        MPlug offsetPointsPlug = fnNode.findPlug(SculptDeformer::aOffsetPoints, false, &status);
        offsetPointsPlug.selectAncestorLogicalIndex(_logicalIndex, layerPlug.attribute());

        MPointArray points;

        {
            MObject data = offsetPointsPlug.asMObject();
            if(data.isNull())
            {
                MGlobal::displayError("error getting points");
                return;
            }

            MFnPointArrayData fnData(data);
            MGlobal::displayInfo(MString("num offset points: ") + fnData.length());

            fnData.copyTo(points);
        }

        if(numVertices != points.length() || numVertices == 0 || faceCounts.length() == 0 || faceVertices.length() == 0)
        {
            MGlobal::displayError(MString("error creating mesh for layer."));
            return;
        }

        // TODO: change mesh->build to take MIntArray and MPointArray
        uintArray inFaceCounts(faceCounts.length());
        uintArray inFaceVerts(faceVertices.length());
        vec4Array inPoints(points.length());

        for(unsigned int i=0; i<faceCounts.length(); i++)
        {
            inFaceCounts[i] = faceCounts[i];
        }

        for(unsigned int i=0; i<faceVertices.length(); i++)
        {
            inFaceVerts[i] = faceVertices[i];
        }

        for(unsigned int i=0; i<points.length(); i++)
        {
            inPoints[i].setX(points[i].x);
            inPoints[i].setY(points[i].y);
            inPoints[i].setZ(points[i].z);
            inPoints[i].setW(1.0f);
        }


        auto viewport = Global::getInstance()->mainWindow()->viewport();

        viewport->makeCurrent();

            Mesh * mesh = Global::getInstance()->mesh();

            mesh->build(inPoints, inFaceVerts, inFaceCounts);
            mesh->computeNormals();

        viewport->doneCurrent();

        Global::getInstance()->mainWindow()->viewport()->update();
    }
}

}
