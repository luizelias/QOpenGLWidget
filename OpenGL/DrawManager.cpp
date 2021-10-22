#include "DrawManager.h"
#include "Global.h"
#include "DrawManager.h"
#include "SubdivisionLevel.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Print.h"

#include <maya/MGlobal.h>

#include <QtGui/QOpenGLTexture>

//#include "Brush.h"
//#include "Node.h"
//#include "Camera.h"
//#include "FnCamera.h"

namespace mush3d{

DrawManager::DrawManager()
{

}

DrawManager * DrawManager::getInstance()
{
    static DrawManager g;

    // initialized opengl
    if(g.isInitialized() == false)
    {
        g.initialized();
    }

    return & g;
}

void DrawManager::initialized()
{
    if(_initialized)
        return;

    // load matcap texture
    _matcapTex = new QOpenGLTexture(QOpenGLTexture::Target2D);
    QImage matcapImg(":/Images/MatCap_Gray.tga");
    _matcapTex->setData(matcapImg.mirrored(), QOpenGLTexture::DontGenerateMipMaps);

    _initialized = true;

    MGlobal::displayInfo("initializing opengl for DrawManager");
}

void DrawManager::drawTriangles(const vec4Array & triangles, const mat4 modelView, const mat4 mvp)
{
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    Shader * shader = shaderManager->getShader(Shader::kNormals);

    _points.setData(triangles, 0);

    shader->bind();


    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("ModelView", modelView);
    shader->setUniformValue("Offset", -0.1f);

    _points.bind(0);

    size_t numPoints = triangles.size();

    gl->glDrawArrays(GL_TRIANGLES, 0, numPoints);

    shader->release();
}

void DrawManager::drawPoint(const vec3 p, const vec3 &color, float offset, int size, float depth, const mat4 mvp)
{
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    vec4Array points;
    points.append(vec4(p, 1.0f));

    _points.setData(points, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("depth", depth);

    _points.unbind();

    gl->glPointSize(size);

    gl->glDrawArrays(GL_POINTS, 0, 1);

    shader->release();
}

void DrawManager::drawPoints(const vec4Array & points, vec3 color, int size, const mat4 mvp)
{
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    _points.setData(points, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("depth", 1.0f);

    _points.bind(0);

    size_t numPoints = points.size();

    gl->glPointSize(size);

    gl->glDrawArrays(GL_POINTS, 0, numPoints);

    shader->release();
}

void DrawManager::drawLine(const vec3 p1, const vec3 p2, const vec3 &color, float depth, int width, const mat4 mvp)
{
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    vec4Array points;
    points.append(vec4(p1, 1.0f));
    points.append(vec4(p2, 1.0f));

    _points.setData(points, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("Depth", depth);

    _points.bind(0);

    gl->glLineWidth(width);

    gl->glDrawArrays(GL_LINES, 0, 2);

    shader->release();
}

void DrawManager::drawLines(const vec4Array &points,
                            const vec3 &color,
                            int width,
                            float depth,
                            const mat4 & mvp)
{
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    _points.setData(points, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("Depth", depth);

    _points.bind(0);

    gl->glLineWidth(width);

    gl->glDrawArrays(GL_LINES, 0, points.size());

    shader->release();
}

void DrawManager::drawCone(const vec3 &position,
                           const vec3 &direction,
                           float height,
                           float radius,
                           const vec3 & color,
                           float depth,
                           const mat4 &mvp)
{
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    int segments = 64;

    vec3 apex = position + direction * height;

    vec3 c = apex + (-direction * height);
    vec3 e0 = perp(direction);
    vec3 e1 = QVector3D::crossProduct(e0, direction);
    float angInc = 360.0f / segments * M_PI_DIV180;

    // calculate points around center
    vec3Array pts;
    for (int i = 0; i < segments; ++i)
    {
        float rad = angInc * i;
        vec3 p = c + (((e0 * float(cos(rad))) + (e1 * float(sin(rad)))) * radius);
        pts.push_back(p);
    }

    // cone triangles
    vec4Array drawPoints;

    for (int i = 0; i < segments; ++i)
    {
        drawPoints.push_back(vec4(apex,1.0f));
        drawPoints.push_back(vec4(pts[i],1.0f));
        if(i > 0)
        {
            drawPoints.push_back(vec4(pts[i-1],1.0f));
        }
        else
        {
            drawPoints.push_back(vec4(pts[segments-1],1.0f));
        }
    }

    // botton triangles
    for (int i = 0; i < segments; ++i)
    {
        drawPoints.push_back(vec4(c,1.0f));
        drawPoints.push_back(vec4(pts[i],1.0f));
        if(i < segments-1)
        {
            drawPoints.push_back(vec4(pts[i+1],1.0f));
        }
        else
        {
            drawPoints.push_back(vec4(pts[0],1.0f));
        }
    }

    _points.setData(drawPoints, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("depth", depth);

    _points.bind(0);

    gl->glDrawArrays(GL_TRIANGLES, 0, drawPoints.size());

    shader->release();
}

void DrawManager::drawCube(const mat4 &transform,
                           const vec3 &color,
                           float radius,
                           float depth,
                           const mat4 &mvp)
{
    vec4Array points;

    points.push_back(vec4(-radius, -radius, radius, 1.0));
    points.push_back(vec4(radius, -radius, radius, 1.0));
    points.push_back(vec4(-radius, radius, radius, 1.0));
    points.push_back(vec4(radius, -radius, -radius, 1.0));
    points.push_back(vec4(radius, radius, -radius, 1.0));
    points.push_back(vec4(radius, radius, radius, 1.0));
    points.push_back(vec4(-radius, radius, radius, 1.0));
    points.push_back(vec4(-radius, radius, -radius, 1.0));
    points.push_back(vec4(-radius, -radius, radius, 1.0));
    points.push_back(vec4(-radius, -radius, radius, 1.0));
    points.push_back(vec4(radius, -radius, -radius, 1.0));
    points.push_back(vec4(radius, -radius, radius, 1.0));
    points.push_back(vec4(radius, radius, -radius, 1.0));
    points.push_back(vec4(-radius, -radius, -radius, 1.0));
    points.push_back(vec4(-radius, radius, -radius, 1.0));
    points.push_back(vec4(-radius, -radius, -radius, 1.0));
    points.push_back(vec4(-radius, -radius, radius, 1.0));
    points.push_back(vec4(-radius, radius, -radius, 1.0));
    points.push_back(vec4(radius, -radius, radius, 1.0));
    points.push_back(vec4(radius, -radius, -radius, 1.0));
    points.push_back(vec4(radius, radius, radius, 1.0));
    points.push_back(vec4(radius, -radius, -radius, 1.0));
    points.push_back(vec4(-radius, -radius, radius, 1.0));
    points.push_back(vec4(-radius, -radius, -radius, 1.0));
    points.push_back(vec4(radius, -radius, radius, 1.0));
    points.push_back(vec4(radius, radius, radius, 1.0));
    points.push_back(vec4(-radius, radius, radius, 1.0));
    points.push_back(vec4(radius, radius, -radius, 1.0));
    points.push_back(vec4(radius, -radius, -radius, 1.0));
    points.push_back(vec4(-radius, -radius, -radius, 1.0));
    points.push_back(vec4(radius, radius, radius, 1.0));
    points.push_back(vec4(-radius, radius, -radius, 1.0));
    points.push_back(vec4(-radius, radius, radius, 1.0));
    points.push_back(vec4(radius, radius, radius, 1.0));
    points.push_back(vec4(radius, radius, -radius, 1.0));
    points.push_back(vec4(-radius, radius, -radius, 1.0));

    for(int i=0; i<points.size(); i++)
    {
        points[i] = transform * points[i];
    }

    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    _points.setData(points, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("Depth", depth);

    _points.bind(0);

    gl->glDrawArrays(GL_TRIANGLES, 0, points.size());

    shader->release();
}

void DrawManager::drawMatrix(const mat4 &matrix, const mat4 &mvp)
{
    // transform the points
    vec4 x1 = matrix * vec4(0,0,0,1);
    vec4 x2 = matrix * vec4(1,0,0,1);

    vec4 y1 = matrix * vec4(0,0,0,1);
    vec4 y2 = matrix * vec4(0,1,0,1);

    vec4 z1 = matrix * vec4(0,0,0,1);
    vec4 z2 = matrix * vec4(0,0,1,1);

    drawLine(x1.toVector3D(), x2.toVector3D(), vec3(1,0,0), 0.0, 4, mvp);
    drawLine(y1.toVector3D(), y2.toVector3D(), vec3(0,1,0), 0.0, 4, mvp);
    drawLine(z1.toVector3D(), z2.toVector3D(), vec3(0,0,1), 0.0, 4, mvp);
}

void DrawManager::drawSphere(float radius,
                             const vec3 & center,
                             const vec3 & color,
                             const mat4 & mvp,
                             float depth,
                             int resolution)
{
    // get the points
    vec4Array points;

    points.reserve(resolution * resolution * 6);

    float PI = 3.14159265359f;
    float startU=0.0f;
    float startV=0.0f;
    float endU=PI*2.0f;
    float endV=PI;
    float stepU=(endU-startU)/resolution;
    float stepV=(endV-startV)/resolution;

    vec4 p0, p1, p2, p3;

    vec4 offset(center, 0.0f);

    for(int i=0; i<resolution; i++)
    {
        for(int j=0; j<resolution; j++)
        {
            float u=i*stepU+startU;
            float v=j*stepV+startV;
            float un=  (i+1==resolution) ? endU : (i+1)*stepU+startU;
            float vn=  (j+1==resolution) ? endV : (j+1)*stepV+startV;

            p0 = vec4(cos(u)*sin(v)*radius, cos(v)*radius, sin(u)*sin(v)*radius, 1.0f ) + offset;
            p1 = vec4(cos(u)*sin(vn)*radius, cos(vn)*radius, sin(u)*sin(vn)*radius, 1.0f )+ offset;
            p2 = vec4(cos(un)*sin(v)*radius, cos(v)*radius, sin(un)*sin(v)*radius, 1.0f )+ offset;
            p3 = vec4(cos(un)*sin(vn)*radius, cos(vn)*radius, sin(un)*sin(vn)*radius, 1.0f )+ offset;

            points.push_back(p2);
            points.push_back(p1);
            points.push_back(p0);
            points.push_back(p1);
            points.push_back(p2);
            points.push_back(p3);
        }
    }

    // drawing
    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    _points.setData(points, 0);

    Shader * shader = shaderManager->getShader(Shader::kConstant);

    shader->bind();

    shader->setUniformValue("color", color);
    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("Depth", depth);

    _points.bind(0);

    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    gl->glDrawArrays(GL_TRIANGLES, 0, points.size());

    gl->glDisable(GL_BLEND);

    shader->release();
}


void DrawManager::drawMesh(Mesh * mesh,
                           const mat4 & modelView,
                           const mat3 & normal,
                           const mat4 & mvp)
{
    if(!mesh) return;

    SubdivisionLevel * level = mesh->getCurrentLevel();

    if(!level) return;

    auto gl = GLFunctions::getInstance();
    auto shaderManager = ShaderManager::getInstance();

    Shader * shader = shaderManager->getShader(Shader::kMatcap);

    float wireframeIntensity = Global::getInstance()->wireframeEnbaled() ?
                Global::getInstance()->wireframeIntensity() : 0.0f;

    float hardNormalsIntensity = Global::getInstance()->hardNormalIntensity();

    int numFaces = level->getNumFaces();

    shader->bind();

    shader->setUniformValue("MVP", mvp);
    shader->setUniformValue("ModelViewMatrix", modelView);
    shader->setUniformValue("NormalMatrix", normal);
    shader->setUniformValue("WireframeIntensity", 1.0f-wireframeIntensity);
    shader->setUniformValue("HardNormalsIntensity", hardNormalsIntensity);
    shader->setUniformValue("NumFaces", numFaces);

    mesh->positions()->bind(0);
    level->faceIndices()->bind(1);
    level->faceNormals()->bind(2);
    level->faceRings()->bind(3);
    level->faceRingsSC()->bind(4);
    level->mask()->bind(5);

    gl->glActiveTexture(GL_TEXTURE0);

    _matcapTex->bind();

    gl->glDrawArrays(GL_POINTS, 0, numFaces);

    _matcapTex->release();

    shader->release();

}

vec3 DrawManager::perp(const vec3 & vec)
{
    float vmin = fabs(vec.x());
    vec3 cardinalAxis(1, 0, 0);

    if (fabs(vec.y()) < vmin) {
        vmin = fabs(vec.y());
        cardinalAxis = vec3(0, 1, 0);
    }

    if (fabs(vec.z()) < vmin) {
        cardinalAxis = vec3(0, 0, 1);
    }

    return QVector3D::crossProduct(vec, cardinalAxis);
}

}
