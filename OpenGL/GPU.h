#ifndef GPU_H
#define GPU_H

#ifndef WORKGROUPSIZE
#define WORKGROUPSIZE 1024
#endif

#include "Types.h"

namespace mush3d{

class Mesh;
class GLBuffer;
class Shader;

// singleton
class GPU
{
private:
    GPU();

public:
    // remove copy and assign operator
    GPU(const GPU&) = delete;
    void operator=(GPU const&) = delete;

    static GPU * getInstance();

    void computeFaceNormals(GLBuffer * positions,
                            GLBuffer * faceIndices,
                            GLBuffer * faceNormals, int numFaces);


    void computeVertexNormals(GLBuffer * normals,
                              GLBuffer * faceNormals,
                              GLBuffer * faceRingSC,
                              GLBuffer * faceRing,
                              int numVertices);

    bool intersectRayMesh(Mesh * mesh, const Ray & ray, PointOnMesh & point);
    void picking(Mesh * mesh, float radius);
    void pickingTopological(Mesh * mesh, float radius);

    void computeArea(GLBuffer * positions,
                     GLBuffer * faceNormals,
                     GLBuffer * falloff,
                     GLBuffer * area,
                     GLBuffer * brush,
                     GLBuffer * faceRing,
                     GLBuffer * faceRingSC,
                     int numVertices);

    void sculpt(Mesh * mesh, float radius, Shader * shader);

    void propagateDeltas(GLBuffer * deltasLow,
                         GLBuffer * deltasHigh,
                         GLBuffer * faces,
                         GLBuffer * VertexRing,
                         GLBuffer * VertexRingSC,
                         GLBuffer * FaceRing,
                         GLBuffer * FaceRingSC,
                         GLBuffer * BorderVertices,
                         GLBuffer * TopologyTags,
                         int numVertices);

    void addDeltas(GLBuffer * points,
                   GLBuffer * backupPoints,
                   GLBuffer * basePoints,
                   GLBuffer * offsetPoints,
                   GLBuffer * face_rings,
                   GLBuffer * face_rings_sc,
                   GLBuffer * faces,
                   int numVertices,
                   float envelope,
                   int iterations,
                   bool tangentSpace);

    void addWeights(GLBuffer * sampleWeights,
                    GLBuffer * meshWeights,
                    int numVertices,
                    float sampleEnvelope);

    void extractDeltas(GLBuffer * positions,
                       GLBuffer * originalPositions,
                       GLBuffer * deltas,
                       int numVertices);

    void initWrinkles(Mesh * mesh, bool asBrush);
    void applyWrinkles(Mesh * mesh,
                       float envelope,
                       float structuralForce,
                       float flexingForce,
                       float shearForc,
                       int iterations,
                       float deltaTime,
                       float normalStrength,
                       bool asBrush);

    void relax(GLBuffer * points_a,
               GLBuffer * points_b,
               GLBuffer * weights,
               GLBuffer * vertex_rings,
               GLBuffer * vertex_rings_sc,
               int iterations,
               float envelope);

    void dummyDeformer(Mesh * mesh, int iterations, float envelope, bool useWeights);

    void mushDeformer(GLBuffer * points,
                      GLBuffer * pointsSmooth,
                      GLBuffer * restPoints,
                      GLBuffer * restPointsSmooth,
                      GLBuffer * weights,
                      GLBuffer * faceIndices,
                      GLBuffer * ringFaces,
                      GLBuffer * ringFacesSC,
                      float envelope,
                      bool tagentSpace);

    void cloneDetails(GLBuffer * source_points,
                      GLBuffer * target_points_A,
                      GLBuffer * target_points_B,
                      GLBuffer * weights,
                      GLBuffer * vertex_rings,
                      GLBuffer * vertex_rings_sc,
                      GLBuffer * face_rings,
                      GLBuffer * face_rings_sc,
                      GLBuffer * faces,
                      GLBuffer * poseA, // for tagent space
                      GLBuffer * poseB, // for tagent space
                      int iterations,
                      float envelope,
                      bool tangentSpace);

    void transform(GLBuffer * points,
                   const mat4 & transform,
                   int numVertices);

    void blendPoints(GLBuffer * pointsA, GLBuffer * pointsB, GLBuffer * mask, float envelope);
};

}

#endif // GPU_H
