#include "Mesh.h"
#include "MeshUtils.h"
#include "GPU.h"
#include "Print.h"

#include <maya/MGlobal.h>

namespace mush3d{

Mesh::Mesh() : _currentLevelIdx(-1)
{

}

Mesh::~Mesh()
{
    clear();
}

GLBuffer * Mesh::positions(){return & _positions;}

GLBuffer * Mesh::normals(){return &_normals;}
GLBuffer * Mesh::backupPositions(){return & _backupPositions;}
GLBuffer * Mesh::morphTarget(){return & _morphTarget;}
GLBuffer * Mesh::basePoints(){return &_basePoints;}
GLBuffer * Mesh::offsetPoints(){return &_offsetPoints;}
GLBuffer * Mesh::weights(){return & _weights;}


QVector<SubdivisionLevel*> &Mesh::getLevels()
{
    return _levels;
}

SubdivisionLevel * Mesh::getCurrentLevel()
{
    if(_currentLevelIdx > -1 && _currentLevelIdx < _levels.size())
    {
        return _levels[_currentLevelIdx];
    }

    return nullptr;
}

SubdivisionLevel * Mesh::addLevel()
{
    _levels.push_back(new SubdivisionLevel());
    _currentLevelIdx++;
    return _levels.back();
}

SubdivisionLevel * Mesh::topLevel() const
{
    if(_levels.size() > 0)
    {
        return _levels[_levels.size()-1];
    }
    return nullptr;
}

int Mesh::getNumVertices()
{
    if(_levels.size() > 0)
    {
        return _levels[_levels.size()-1]->getNumVertices();
    }
    return 0;
}

int Mesh::getNumFaces()
{
    if(_levels.size() > 0)
    {
        return _levels[_levels.size()-1]->getNumFaces();
    }
    return 0;
}

Aabb Mesh::getAabb()
{
    // get positions
    vec4Array points;
    positions()->getData(points, 0);

    Aabb aabb;

    int numPoints = points.size();

    for(int i=0; i<numPoints; i++)
    {
        aabb.expand(points[i]);
    }

    return aabb;
}

int Mesh::getLevelIdx()
{
    return _currentLevelIdx;
}

void Mesh::setLevelIdx(int idx)
{
    if(idx < 0)
    {
        MGlobal::displayError("trying to set a negative index for mesh");
    }
    else if(idx >= _levels.size())
    {
        MGlobal::displayError("trying to index bigger than num levels");
    }
    else
    {
        _currentLevelIdx = idx;
    }
}

void Mesh::propagateDeltas()
{
    int current = _currentLevelIdx;
    int maxLevel = _levels.size()-1;

    if(current == maxLevel)
    {
        return;
    }

    while(current < maxLevel)
    {
        SubdivisionLevel * lower = _levels[current];
        SubdivisionLevel * higher = _levels[current+1];

        GPU::getInstance()->propagateDeltas(lower->deltas(),
                                            higher->deltas(),
                                            higher->faceIndices(),
                                            higher->vertexRings(),
                                            higher->vertexRingsSC(),
                                            higher->faceRings(),
                                            higher->faceRingsSC(),
                                            higher->borders(),
                                            higher->topologyTags(),
                                            higher->getNumVertices());
        current++;
    }
}

void Mesh::build(const vec4Array & positionIn,
                 const uintArray & faceIndicesIn,
                 const uintArray & faceCountsIn)
{
    // clear if its not a new mesh
    clear();

    // init mesh buffers
    int numVertices = positionIn.size();

    _positions.setData(positionIn, 0);
    _normals.setData(vec4Array(numVertices), 0);
    _backupPositions.setData(vec4Array(numVertices), 0);
    _basePoints.setData(vec4Array(numVertices), 0);
    _offsetPoints.setData(vec4Array(numVertices), 0);
    _weights.setData(floatArray(numVertices, 0.0f), 0);

    // save a morph target
    setMorphTarget(positionIn);

    // create a level
    SubdivisionLevel * level = addLevel();

    // initialize level
    level->build(numVertices, faceCountsIn, faceIndicesIn);
}

void Mesh::computeNormals()
{
    auto level = getCurrentLevel();

    if(level)
    {
        GPU::getInstance()->computeFaceNormals(positions(), level->faceIndices(), level->faceNormals(), level->getNumFaces());
        GPU::getInstance()->computeVertexNormals(normals(), level->faceNormals(), level->faceRingsSC(), level->faceRings(), level->getNumVertices());
    }
}

void Mesh::clear()
{
    for(SubdivisionLevel * l: _levels)
        delete l;

    _levels.clear();

    _currentLevelIdx = -1;
}

void Mesh::setMorphTarget(const vec4Array & points)
{
    _morphTarget.setData(points, 0);
}

}
