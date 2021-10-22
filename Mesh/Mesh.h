#ifndef MESH_H
#define MESH_H

#include "Types.h"
#include "Aabb.h"
#include "GLBuffer.h"
#include "SubdivisionLevel.h"

namespace mush3d{

class Mesh
{
public:
    Mesh();
    ~Mesh();

    QVector<SubdivisionLevel*> & getLevels();

    SubdivisionLevel * getCurrentLevel();
    SubdivisionLevel * addLevel();
    SubdivisionLevel * topLevel() const;

    int getNumVertices();
    int getNumFaces();

    // level switch
    int getLevelIdx();
    void setLevelIdx(int idx);

    // propagate
    void propagateDeltas();

    Aabb getAabb();

    GLBuffer * positions();
    GLBuffer * normals();
    GLBuffer * backupPositions();
    GLBuffer * morphTarget();
    GLBuffer * basePoints();
    GLBuffer * offsetPoints();
    GLBuffer * weights();

    void build(const vec4Array & positionIn,
               const uintArray & faceIndicesIn,
               const uintArray & faceCountsIn);

    void setMorphTarget(const vec4Array &points);

    void computeNormals();

    void clear();

private:
    int _currentLevelIdx;
    QVector<SubdivisionLevel*> _levels;

    GLBuffer _positions;
    GLBuffer _normals;
    GLBuffer _morphTarget;
    GLBuffer _backupPositions;
    GLBuffer _basePoints;
    GLBuffer _offsetPoints;
    GLBuffer _weights;
};

}


#endif // MESH_H
