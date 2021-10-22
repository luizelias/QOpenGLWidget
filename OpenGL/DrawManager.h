#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include "GLBuffer.h"
#include "Shader.h"
#include "Types.h"

#define M_PI_DIV180 0.01745329251

class QOpenGLTexture;

namespace mush3d{

class Mesh;
class Camera;

// singleton
class DrawManager
{
private:
    DrawManager();

public:
    // remove copy and assign operator
    DrawManager(const DrawManager&) = delete;
    void operator=(DrawManager const&) = delete;

    static DrawManager * getInstance();

    bool isInitialized(){return _initialized;}
    void initialized();

    void drawTriangles(const vec4Array &triangles,
                       const mat4 modelView,
                       const mat4 mvp);

    void drawPoint(const vec3 p,
                   const vec3 & color,
                   float offset,
                   int size,
                   float depth,
                   const mat4 mvp);

    void drawPoints(const vec4Array &points, vec3 color,int size,
                    const mat4 mvp);

    void drawLine(const vec3 p1,
                  const vec3 p2,
                  const vec3 & color,
                  float depth,
                  int width,
                  const mat4 mvp);

    void drawLines(const vec4Array & points,
                   const vec3 & color,
                   int width, float depth,
                   const mat4 & mvp);

    void drawCone(const vec3 & position,
                  const vec3 & direction,
                  float height,
                  float radius,
                  const vec3 &color,
                  float depth,
                  const mat4 & mvp);

    void drawCube(const mat4 &transform,
                  const vec3 &color,
                  float radius,
                  float depth,
                  const mat4 &mvp);

    void drawMatrix(const mat4 & matrix,
                    const mat4 & mvp);

    void drawSphere(float radius,
                    const vec3 &center,
                    const vec3 &color,
                    const mat4 &mvp,
                    float depth,
                    int resolution);

    vec3 perp(const vec3 & vec);

    void drawMesh(Mesh * mesh,
                  const mat4 & modelView,
                  const mat3 & normal,
                  const mat4 & mvp);

    void drawMoveTransform(const mat4 & transform, const mat4 & MVP);

private:
    GLBuffer _points; // for simple drawings, points, lines

    bool _initialized = false;

    QOpenGLTexture * _matcapTex=nullptr;
};

}

#endif // DRAWMANAGER_H
