#ifndef SUBDIVISIONLEVEL_H
#define SUBDIVISIONLEVEL_H

#include "Types.h"
#include "GLBuffer.h"

namespace mush3d{

class SubdivisionLevel
{
public:
    SubdivisionLevel();

    void build(int numVertices,
               const uintArray & faceCountsIn,
               const uintArray & faceVertsIn);

    GLBuffer * borders();
    GLBuffer * topologyTags();
    GLBuffer * faceIndices();
    GLBuffer * faceNormals();
    GLBuffer * faceRingsSC();
    GLBuffer * faceRings();
    GLBuffer * vertexRingsSC();
    GLBuffer * vertexRings();
    GLBuffer * particlesIn();
    GLBuffer * particlesOut();
    GLBuffer * deltas();
    GLBuffer * mask();
    GLBuffer * triangles();
    GLBuffer * visibilityMask();

    GLBuffer * falloff1();
    GLBuffer * falloff2();
    GLBuffer * brush();
    GLBuffer * faceHits();
    GLBuffer * area();
    GLBuffer * waveIndices();

    int getNumVertices();
    int getNumFaces();
    int getNumTriangles();

private:
    uint _numVertices;
    uint _numFaces;
    uint _numTriangles;

    GLBuffer _border;
    GLBuffer _topologyTags;
    GLBuffer _faceIndices;
    GLBuffer _faceNormals;
    GLBuffer _faceRingsSC;
    GLBuffer _faceRings;
    GLBuffer _vertexRingsSC;
    GLBuffer _vertexRings;
    GLBuffer _particlesIn;
    GLBuffer _particlesOut;
    GLBuffer _triangles;
    GLBuffer _visibilityMask;

    GLBuffer _deltas;
    GLBuffer _mask;
    GLBuffer _brush;
    GLBuffer _falloff1;
    GLBuffer _falloff2;
    GLBuffer _faceHits;
    GLBuffer _area;
    GLBuffer _waveIndices;
};

}

#endif // SUBDIVISIONLEVEL_H



