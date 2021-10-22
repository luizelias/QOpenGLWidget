#ifndef SCULPTDEFORMER_H
#define SCULPTDEFORMER_H


#include <maya/MPxDeformerNode.h>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MGlobal.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MTypeId.h>
#include <maya/MFnStringData.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MItGeometry.h>
#include <maya/MFloatPointArray.h>


#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>

namespace mush3d{

class SculptDeformer : public MPxDeformerNode {
public:
    SculptDeformer();
    ~SculptDeformer();

    static void *creator();
    static MStatus initialize();

    MStatus deform(MDataBlock &block, MItGeometry &iter, const MMatrix &mat,
                   unsigned int multiIndex) override;

public:
    static MTypeId id;
    static MObject aLayer;
    static MObject aSelected;
    static MObject aIntensity;
    static MObject aBasePoints;
    static MObject aOffsetPoints;

    static MObject aFaceCounts;
    static MObject aFaceVertices;
    static MObject aNumVertices;
};

}


#endif // SCULPTDEFORMER_H
