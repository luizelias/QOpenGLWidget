#ifndef TYPES_H
#define TYPES_H

#include <math.h>

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPoint>
#include <QDataStream>
#include <QQuaternion>

#include <QMatrix4x4>
#include <QMatrix3x3>

#define MAX_FLOAT 3.402823466e+38

// to convert radians to degrees multiply by that
#define RADIANS2DEGRESS 57.29577951308232

typedef QVector2D   vec2;
typedef QVector3D   vec3;
typedef QVector4D   vec4;

typedef QMatrix4x4  mat4;
typedef QMatrix3x3  mat3;

typedef QVector<QVector4D>    vec4Array;
typedef QVector<QVector3D>    vec3Array;
typedef QVector<QVector2D>    vec2Array;
typedef QVector<QMatrix4x4>   mat4Array;
typedef QVector<float>        floatArray;
typedef QVector<unsigned int> uintArray;
typedef QVector<int>          intArray;
typedef QVector<bool>         boolArray;
typedef QVector<double>       doubleArray;

typedef unsigned int uint;

namespace mush3d{

enum MinedataType
{
    kCamera
};

enum ToolType{
    kClayTool,
    kMoveTool,
    kStandardTool,
    kCreaseTool,
    kInflateTool,
    kEraseTool,
    kCloneTool,
    kMaskingTool,
    kWrinkleTool,
    kClothTool,
    kTransformMove,
    kTransformRotate,
    kTransformScale,
    kWeightsTool,
    kSelectMuscleAnchorFaces,

    kCameraTool,
    kCompareTool,
    kLightingTool,
    kHideMeshTool,
    kSelectionTool,
    kRelaxTool};

struct BrushData
{
    BrushData();

    vec4 pressPosition;
    vec4 pressNormal;
    vec4 intersection;
    vec4 delta;

    vec4 aPosition;
    vec4 aNormal;

    bool pressHit;
    int  hitTriangle;
    bool enoughtDistance;
    uint numStrokes;

    uint count;
    uint numVerticesInSphere;
    uint closestVertex;
    int  padding[1];

    mat4 brushMatrix;
    mat4 brushMatrixInverse;
};

struct FaceHit
{
    FaceHit() : id(-1), distance(MAX_FLOAT)
    {}
    int id;
    float distance;
};

struct Ray
{
    Ray(){}
    Ray(const vec3 & n, const vec3 & f): _near(n), _far(f){}

    vec3 getNear() const {return _near;}
    vec3 getFar() const {return _far;}
    vec3 direction() const {return (_far-_near).normalized();}

    Ray flipped()
    {
        vec3 n_flipped(_near.x() * -1.0f, _near.y(), _near.z());
        vec3 f_flipped(_far.x() * -1.0f, _far.y(), _far.z());

        return Ray(n_flipped, f_flipped);
    }

    vec3 _near;
    vec3 _far;
};

class Plane
{
public:
    Plane(const vec3 & center=vec3(), const vec3 & normal=vec3());
    vec3 intersect(const Ray & ray);
    void setCenter(const vec3 & point);
    void setNormal(const vec3 & norm);
    vec3 getNormal();

private:
    vec3 _center;
    vec3 _normal;
};

struct ShaderSettings
{
    ShaderSettings() : shininess(10.0f), wireframe(0.0f), hardness(0.0f)
    {
        kd = QVector3D(0.33, 0.33, 0.33);
        ka = QVector3D(0.05, 0.05, 0.05);
        ks = QVector3D(0.13, 0.13, 0.13);
    }
    QVector3D kd;
    QVector3D ka;
    QVector3D ks;
    float shininess;
    float wireframe;
    float hardness;
};

struct PointOnMesh
{
    PointOnMesh() : triangle(-1){}
    vec3 position;
    vec3 normal;
    int triangle;
    mat4 tangentSpace;
};

struct Line
{
    Line(){}
    Line(const vec3 a, const vec3 b) : P0(a), P1(b){}
    vec3 P0;
    vec3 P1;
};

// PARTICLE //

class Particle
{
public:
    Particle();

    void addSpring(uint idx, uint type);
    int  getNumSprings();
    uint getSpring(int idx);

    int vertex_id;
    bool locked;
    float weight;
    float inverseWeight;

    uint springs[24];
    uint springType[24];

    vec4 position;
    vec4 previousPosition;
    vec4 restPosition;
    vec4 force;
};


}

#endif // TYPES_H
