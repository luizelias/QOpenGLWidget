#include "SculptDeformer.h"

namespace mush3d {

// local attributes
MTypeId SculptDeformer::id(0x80056);
MObject SculptDeformer::aLayer;
MObject SculptDeformer::aIntensity;
MObject SculptDeformer::aSelected;
MObject SculptDeformer::aBasePoints;
MObject SculptDeformer::aOffsetPoints;

MObject SculptDeformer::aFaceCounts;
MObject SculptDeformer::aFaceVertices;
MObject SculptDeformer::aNumVertices;

SculptDeformer::SculptDeformer() {}
SculptDeformer::~SculptDeformer() {}

void *SculptDeformer::creator() { return new SculptDeformer(); }

MStatus SculptDeformer::deform(MDataBlock &block, MItGeometry &iter,
                               const MMatrix &mat, unsigned int multiIndex) {
    MStatus status;

    return status;

    // envelope
    MDataHandle envData = block.inputValue(envelope, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    float env = envData.asFloat();
    if (!(env > 0.0f))
        return status;

    // check if there is any layer to deformer
    MArrayDataHandle layersHandle = block.inputArrayValue( aLayer );
    unsigned int numLayers = layersHandle.elementCount();
    if ( numLayers == 0 )
    {
        return MStatus::kSuccess;
    }

    // get current points
    MPointArray points;
    iter.allPositions(points);

    auto numPoints = points.length();

    // loop trought layers
    MDataHandle layerHandle;
    MDataHandle mapHandle;
    unsigned int ix, iy, iz, iw;
    for ( unsigned int i = 0; i < numLayers; i++ )
    {
        layersHandle.jumpToArrayElement( i );
        layerHandle = layersHandle.inputValue();

        // enabled
//        bool enabled = layersHandle.inputValue().child(aEnabled).asBool();
//        if(enabled == false)
//            continue;

//        // deltas
//        MObject oDeltas = layersHandle.inputValue().child(aDeltas).data();
//        MFnVectorArrayData fnDeltas(oDeltas);

//        // ids
//        MObject oIds = layersHandle.inputValue().child(aIds).data();
//        MFnIntArrayData fnIds(oIds);

////        auto numDeltas = fnDeltas.length();
////        auto numIds = fnDeltas.length();

////        if(numIds != numDeltas)
////            continue;

////        int idx;
////        MVector delta;
////        for(unsigned int j=0; j<numIds; j++)
////        {
////            idx = fnIds[j];
////            delta = fnDeltas[j];

////            points[idx] += delta;
////        }
    }


    iter.setAllPositions(points);


    return status;
}

MStatus SculptDeformer::initialize() {
//    MFnUnitAttribute unitFn;
    MFnTypedAttribute tAttr;
    MFnNumericAttribute nAttr;
    MFnCompoundAttribute cAttr;
//    MFnEnumAttribute eAttr;

    // MERGED TOPOLOGY
    aFaceCounts = tAttr.create("faceCounts", "faceCounts", MFnData::kIntArray);
    addAttribute(aFaceCounts);

    aFaceVertices = tAttr.create("faceVertices", "faceVertices", MFnData::kIntArray);
    addAttribute(aFaceVertices);

    aNumVertices = nAttr.create("numVertices", "numVertices", MFnNumericData::kInt, 0);
    addAttribute(aNumVertices);

    // LAYER
    aIntensity = nAttr.create("layerintensity", "layerintensity", MFnNumericData::kFloat, 1.0f);
    nAttr.setMin(0.0f);
    nAttr.setMax(1.0f);

    aSelected = nAttr.create("layerselected", "layerselected", MFnNumericData::kBoolean);
    nAttr.setDefault(false);

    aBasePoints = tAttr.create("layerbasepoints", "layerbasepoints", MFnData::kPointArray);

    aOffsetPoints =
            tAttr.create("layeroffsetpoints", "layeroffsetpoints", MFnData::kPointArray);

    aLayer = cAttr.create("layer", "layer");
    cAttr.setArray(true);

    cAttr.addChild(aIntensity);
    cAttr.addChild(aSelected);
    cAttr.addChild(aBasePoints);
    cAttr.addChild(aOffsetPoints);

    addAttribute(aLayer);

    attributeAffects(aLayer, outputGeom);
    attributeAffects(aIntensity, outputGeom);
    attributeAffects(aSelected, outputGeom);
    attributeAffects(aBasePoints, outputGeom);
    attributeAffects(aOffsetPoints, outputGeom);

//    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer mush3dSculpt weights;");

    return MStatus::kSuccess;
}


}
