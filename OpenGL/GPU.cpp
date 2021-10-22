#include "GPU.h"
#include "Global.h"
#include "GLFunctions.h"
#include "ShaderManager.h"
#include "GLBuffer.h"
#include "SubdivisionLevel.h"
#include "Mesh.h"

namespace mush3d{

GPU::GPU()
{

}

GPU * GPU::getInstance()
{
    static GPU g;
    return & g;
}

void GPU::computeFaceNormals(GLBuffer * positions,
                             GLBuffer * faceIndices,
                             GLBuffer * faceNormals,
                             int numFaces)
{
    auto * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kFaceNormals);

    shader->bind();

    positions->bind(0);
    faceIndices->bind(1);
    faceNormals->bind(2);

    int numWorkGroups = numFaces / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
}

void GPU::computeVertexNormals(GLBuffer * normals,
                               GLBuffer * faceNormals,
                               GLBuffer * faceRingSC,
                               GLBuffer * faceRing,
                               int numVertices)
{
    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kVertexNormals);

    shader->bind();

    normals->bind(0);
    faceNormals->bind(1);
    faceRingSC->bind(2);
    faceRing->bind(3);

    shader->setUniformValue("NumVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
}


bool GPU::intersectRayMesh(Mesh * mesh, const Ray & ray, PointOnMesh & point)
{
    SubdivisionLevel * level = mesh->getCurrentLevel();

    if(!level)
        return false;

    GLFunctions * gl = GLFunctions::getInstance();

    mesh->positions()->bind(0);
    level->triangles()->bind(1);
    level->faceHits()->bind(2);
    level->brush()->bind(3);
    level->faceNormals()->bind(4);

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kIntersectRayMesh);

    shader->bind();

    int numTriangles = level->getNumTriangles();

    shader->setUniformValue("nearP", ray._near);
    shader->setUniformValue("farP", ray._far);
    shader->setUniformValue("ignoreBackface", 1);
    shader->setUniformValue("numTriangles", numTriangles);

    // PASS 0
    shader->setUniformValue("pass", 0);

    int numWorkGroups = numTriangles / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    // PASS 1
    shader->setUniformValue("pass", 1);

    int numGroups = int(ceil(float(numTriangles) / 1024.0f / 1024.0f));

    gl->glDispatchCompute(numGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    // PASS 2
    shader->setUniformValue("pass", 2);

    gl->glDispatchCompute(1, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();

    QVector<BrushData> bdata;
    level->brush()->getData(bdata, 0);

    if(bdata.size() > 0)
    {
        if(bdata[0].hitTriangle > -1)
        {
            point.position = bdata[0].intersection.toVector3D();
            point.normal = bdata[0].pressNormal.toVector3D();
            point.triangle = bdata[0].hitTriangle;
            point.tangentSpace = bdata[0].brushMatrix;

            return true;
        }
    }

    return false;
}

void GPU::picking(Mesh * mesh, float radius)
{
    SubdivisionLevel * level = mesh->getCurrentLevel();

    if(!level)
        return;

    int numVertices = level->getNumVertices();

    mesh->positions()->bind(0);
    level->brush()->bind(1);
    level->falloff1()->bind(2);
    level->mask()->bind(3);


    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kPicking);

    shader->bind();

    shader->setUniformValue("Radius", radius);
    shader->setUniformValue("Radius2", radius * radius);
    shader->setUniformValue("numVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::pickingTopological(Mesh * mesh, float radius)
{
    SubdivisionLevel * level = mesh->getCurrentLevel();
    if(!level) return;

    int numVertices = level->getNumVertices();
    int numTriangles = level->getNumTriangles();

    // position not working at index
    mesh->positions()->bind(0);
    level->brush()->bind(1);
    level->triangles()->bind(2);
    level->falloff1()->bind(3);
    level->falloff2()->bind(4);
    level->waveIndices()->bind(5);
    level->vertexRings()->bind(6);
    level->vertexRingsSC()->bind(7);

    GLFunctions * gl = GLFunctions::getInstance();
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kPickingTopological);

    shader->bind();

    shader->setUniformValue("mode", 0);
    shader->setUniformValue("radius", radius);
    shader->setUniformValue("radius2", radius*radius);
    shader->setUniformValue("numElements", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    // wave propagation

    numWorkGroups = numTriangles / WORKGROUPSIZE + 1;

    shader->setUniformValue("numElements", numTriangles);

    QVector<BrushData> bData;
    int count = 0;
    int numIterations = 0;
    for(int i=0; i<200; i++)
    {
        // add close vrtices
        shader->setUniformValue("mode", 1);
        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

        // compute distance
        shader->setUniformValue("mode", 2);
        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );


        // check if we should continue
        QVector<BrushData> bData;
        level->brush()->getData(bData, 0);

        // rebind points to 0
        mesh->positions()->bind(0);

        if(bData.length() > 0)
        {
            if(count == bData[0].count) break;

            count = bData[0].count;
            numIterations++;
        }
        else
        {
            break;
        }
    }

    ///////////////////////////////////
    /////// calculate fallof //////////
    ///////////////////////////////////
    numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    shader->setUniformValue("numElements", numVertices);

    shader->setUniformValue("mode", 3);


    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    //////////////////////////////////
    ////// smooth falloff ////////////
    //////////////////////////////////

    level->falloff2()->copyFrom(level->falloff1());

    int smoothIterations = 3;

    shader->setUniformValue("mode", 4);
    for(int i=0; i<smoothIterations; i++)
    {
        int x = i % 2;

        if(x == 0)
        {
            level->falloff1()->bind(3);
            level->falloff2()->bind(4);
        }
        else
        {
            level->falloff2()->bind(3);
            level->falloff1()->bind(4);
        }

        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    }

    // make sure it copys back to falloff1
    if(smoothIterations % 2 == 1)
    {
        level->falloff1()->copyFrom(level->falloff2());
    }

    shader->release();
}

void GPU::computeArea(GLBuffer * positions,
                      GLBuffer * faceNormals,
                      GLBuffer * falloff,
                      GLBuffer * area,
                      GLBuffer * brush,
                      GLBuffer * faceRing,
                      GLBuffer * faceRingSC,
                      int numVertices)
{
    positions->bind(0);
    faceNormals->bind(1);
    falloff->bind(2);
    area->bind(3);
    brush->bind(4);
    faceRing->bind(5);
    faceRingSC->bind(6);

    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kArea);

    shader->bind();

    shader->setUniformValue("numVertices", numVertices);

    // PASS 0
    shader->setUniformValue("pass", 0);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    // PASS 1
    shader->setUniformValue("pass", 1);

    int numGroups = int(ceil(float(numVertices) / 1024.0f / 1024.0f));

    gl->glDispatchCompute(numGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    // PASS 2
    shader->setUniformValue("pass", 2);

    gl->glDispatchCompute(1, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}


void GPU::sculpt(Mesh * mesh, float radius, Shader * shader)
{
    /*
    SubdivisionLevel * level = mesh->getCurrentLevel();

    if(!level) return;

    int numVertices = level->getNumVertices();

    mesh->positions()->bind(0);
    level->falloff1()->bind(1);
    level->deltas()->bind(2);
    level->mask()->bind(3);
    level->brush()->bind(4);
    level->faceRings()->bind(5);
    level->faceRingsSC()->bind(6);

    GLFunctions * gl = GLFunctions::getInstance();

    shader->bind();

    shader->setUniformValue("radius", radius);
    shader->setUniformValue("intensity", Brush::intensity);
    shader->setUniformValue("pressure", Brush::pressure);
    shader->setUniformValue("control", false);
    shader->setUniformValue("numVertices", numVertices);
    shader->setUniformValue("moveDelta", Brush::moveDelta);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
    */
}

void GPU::propagateDeltas(GLBuffer * deltasLow,
                          GLBuffer * deltasHigh,
                          GLBuffer * faces,
                          GLBuffer * VertexRing,
                          GLBuffer * VertexRingSC,
                          GLBuffer * FaceRing,
                          GLBuffer * FaceRingSC,
                          GLBuffer * BorderVertices,
                          GLBuffer * TopologyTags,
                          int numVertices)
{
    deltasLow->bind(0);
    deltasHigh->bind(1);
    faces->bind(2);
    VertexRing->bind(3);
    VertexRingSC->bind(4);
    FaceRing->bind(5);
    FaceRingSC->bind(6);
    BorderVertices->bind(7);
    TopologyTags->bind(8);

    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kPropagateDeltas);

    shader->bind();

    shader->setUniformValue("numVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::addDeltas(GLBuffer * points,
                    GLBuffer * backupPoints,
                    GLBuffer * basePoints,
                    GLBuffer * offsetPoints,
                    GLBuffer * face_rings,
                    GLBuffer * face_rings_sc,
                    GLBuffer * faces,
                    int numVertices,
                    float envelope,
                    int iterations,
                    bool tangentSpace)
{
    // resize buffer b if necessary
    if(tangentSpace == true)
    {
        backupPoints->copyFrom(points);
    }

    // bind buffers
    points->bind(0);
    backupPoints->bind(1);
    basePoints->bind(2);
    offsetPoints->bind(3);
    face_rings->bind(4);
    face_rings_sc->bind(5);
    faces->bind(6);

    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kaddDeltas);

    shader->bind();

    shader->setUniformValue("NumVertices", numVertices);
    shader->setUniformValue("Envelope", envelope);
    shader->setUniformValue("TangentSpace", tangentSpace);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::addWeights(GLBuffer * sampleWeights, GLBuffer * meshWeights, int numVertices, float sampleEnvelope)
{
    sampleWeights->bind(0);
    meshWeights->bind(1);

    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kAddWeights);

    shader->bind();

    shader->setUniformValue("NumVertices", numVertices);
    shader->setUniformValue("Envelope", sampleEnvelope);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::extractDeltas(GLBuffer *positions, GLBuffer *originalPositions, GLBuffer *deltas, int numVertices)
{
    positions->bind(0);
    originalPositions->bind(1);
    deltas->bind(2);

    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kExtractDeltas);

    shader->bind();

    shader->setUniformValue("numVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::initWrinkles(Mesh * mesh, bool asBrush)
{
    SubdivisionLevel * level = mesh->getCurrentLevel();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kWrinkle);

    int numVertices = level->getNumVertices();

    // upload weights
    mesh->positions()->bind(0);

    if(asBrush)
    {
        level->falloff1()->bind(1);
    }
    else
    {
        mesh->weights()->bind(1);
    }

    mesh->morphTarget()->bind(2);
    level->particlesIn()->bind(3);
    level->particlesOut()->bind(4);
    level->deltas()->bind(5);
    level->mask()->bind(6);

    GLFunctions * gl = GLFunctions::getInstance();

    shader->bind();

    shader->setUniformValue("NumVertices", numVertices);

    shader->setUniformValue("Pass", 0);
    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::applyWrinkles(Mesh * mesh, float envelope, float structuralForce, float flexingForce, float shearForce, int iterations, float deltaTime, float normalStrength, bool asBrush)
{
    SubdivisionLevel * level = mesh->getCurrentLevel();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kWrinkle);

    int numVertices = level->getNumVertices();

    // upload weights
    mesh->positions()->bind(0);

    if(asBrush)
    {
        level->falloff1()->bind(1);
    }
    else
    {
        mesh->weights()->bind(1);
    }

    mesh->morphTarget()->bind(2);
    level->particlesIn()->bind(3);
    level->particlesOut()->bind(4);
    mesh->normals()->bind(5);

    GLFunctions * gl = GLFunctions::getInstance();

    shader->bind();

    shader->setUniformValue("Envelope", envelope);

    shader->setUniformValue("NumVertices", numVertices);
    shader->setUniformValue("StructuralForce", structuralForce);
    shader->setUniformValue("ShearForce", flexingForce);
    shader->setUniformValue("FlexionForce", shearForce);
    shader->setUniformValue("DeltaTime", deltaTime);
    shader->setUniformValue("NormalStrength", 1.0f - normalStrength);

    //    shader->setUniformValue("AsBrush", asBrush);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    // deform
    int count = 0;
    for(int i=0; i<iterations; i++)
    {
        // solve springs
        shader->setUniformValue("Pass", 1);

        if(count == 0)
        {
            level->particlesIn()->bind(3);
            level->particlesOut()->bind(4);
        }
        else
        {
            level->particlesIn()->bind(4);
            level->particlesOut()->bind(3);
        }

        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );


        if(i != iterations-1)
            count = 1-count;
    }


    // transfer result to points
    shader->setUniformValue("Pass", 2);

    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::relax(GLBuffer * points_a,
                GLBuffer * points_b,
                GLBuffer * weights,
                GLBuffer * vertex_rings,
                GLBuffer * vertex_rings_sc,
                int iterations,
                float envelope)
{
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kRelaxDeformer);

    int numVertices = points_a->numElements();

    // upload weights
    GLFunctions * gl = GLFunctions::getInstance();

    points_a->bind(0);
    points_b->bind(1);
    weights->bind(2);
    vertex_rings->bind(3);
    vertex_rings_sc->bind(4);

    shader->bind();

    shader->setUniformValue("Envelope", envelope);
    shader->setUniformValue("NumVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    for(int i=0; i<iterations; i++)
    {
        if( (i % 2) == 0)
        {
            points_a->bind(0);
            points_b->bind(1);
        }
        else
        {
            points_b->bind(0);
            points_a->bind(1);
        }

        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    }

    // if number of iterations is odd we need to copy back to positions buffer
    if((iterations > 0) && ( (iterations % 2) == 1))
    {
        points_a->copyFrom(points_b);
    }

    shader->release();
}

void GPU::dummyDeformer(Mesh *mesh, int iterations, float envelope, bool useWeights)
{
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kDummyDeformer);

    int numVertices = mesh->getNumVertices();

    mesh->positions()->bind(0);

    GLFunctions * gl = GLFunctions::getInstance();

    shader->bind();

    shader->setUniformValue("NumVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    gl->glDispatchCompute(numWorkGroups, 1, 1);

    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::mushDeformer(GLBuffer * points,
                       GLBuffer * pointsSmooth,
                       GLBuffer * restPoints,
                       GLBuffer * restPointsSmooth,
                       GLBuffer * weights,
                       GLBuffer * faceIndices,
                       GLBuffer * ringFaces,
                       GLBuffer * ringFacesSC,
                       float envelope,
                       bool tagentSpace)
{
    // add deltas to mesh
    GLFunctions * gl = GLFunctions::getInstance();

    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kMushDeformer);

    int numVertices = points->numElements();

    points->bind(0);
    pointsSmooth->bind(1);
    restPoints->bind(2);
    restPointsSmooth->bind(3);
    weights->bind(4);
    faceIndices->bind(5);
    ringFaces->bind(6);
    ringFacesSC->bind(7);

    shader->bind();

    shader->setUniformValue("Envelope", envelope);
    shader->setUniformValue("NumVertices", numVertices);
    shader->setUniformValue("TangentSpace", tagentSpace);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::cloneDetails(GLBuffer *source_points,
                       GLBuffer *target_points_A,
                       GLBuffer *target_points_B,
                       GLBuffer *weights,
                       GLBuffer *vertex_rings,
                       GLBuffer *vertex_rings_sc,
                       GLBuffer *face_rings,
                       GLBuffer *face_rings_sc,
                       GLBuffer *faces,
                       GLBuffer *poseA,
                       GLBuffer *poseB,
                       int iterations,
                       float envelope,
                       bool tangentSpace)
{
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kCloneDeformer);

    int numVertices = source_points->numElements();

    GLFunctions * gl = GLFunctions::getInstance();

    if(target_points_B->numElements() != target_points_A->numElements())
    {
        target_points_B->copyFrom(target_points_A);
    }

    source_points->bind(0);
    target_points_A->bind(1);
    target_points_B->bind(2);
    weights->bind(3);
    vertex_rings->bind(4);
    vertex_rings_sc->bind(5);
    face_rings->bind(6);
    face_rings_sc->bind(7);
    faces->bind(8);
    poseA->bind(9);
    poseB->bind(10);

    shader->bind();

    shader->setUniformValue("Envelope", envelope);
    shader->setUniformValue("NumVertices", numVertices);
    shader->setUniformValue("TangentSpace", tangentSpace);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    for(int i=0; i<iterations; i++)
    {
        if( (i % 2) == 0)
        {
            target_points_A->bind(1);
            target_points_B->bind(2);
        }
        else
        {
            target_points_B->bind(1);
            target_points_A->bind(2);
        }

        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    }

    // if number of iterations is odd we need to copy back to positions buffer
    if((iterations > 0) && ( (iterations % 2) == 1))
    {
        target_points_A->copyFrom(target_points_B);
    }

    shader->release();
}

void GPU::transform(GLBuffer * points,
               const mat4 & transform,
               int numVertices)
{
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kTransform);

    GLFunctions * gl = GLFunctions::getInstance();

    points->bind(0);

    shader->bind();

    shader->setUniformValue("Transform", transform);
    shader->setUniformValue("NumVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

void GPU::blendPoints(GLBuffer *pointsA, GLBuffer *pointsB, GLBuffer *mask, float envelope)
{
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kBlendPoints);

    int numVertices = pointsA->numElements();

    GLFunctions * gl = GLFunctions::getInstance();

    pointsA->bind(0);
    pointsB->bind(1);
    mask->bind(2);

    shader->bind();

    shader->setUniformValue("Envelope", envelope);
    shader->setUniformValue("NumVertices", numVertices);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;

    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();
}

}
