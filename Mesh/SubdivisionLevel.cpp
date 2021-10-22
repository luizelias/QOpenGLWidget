#include "SubdivisionLevel.h"
#include "Utils.h"

namespace mush3d{

SubdivisionLevel::SubdivisionLevel() : _numVertices(0), _numFaces(0), _numTriangles(0)
{

}

GLBuffer * SubdivisionLevel::borders(){return & _border;}
GLBuffer * SubdivisionLevel::topologyTags(){return & _topologyTags;}
GLBuffer * SubdivisionLevel::faceIndices(){return & _faceIndices;}
GLBuffer * SubdivisionLevel::faceNormals(){return & _faceNormals;}
GLBuffer * SubdivisionLevel::faceRingsSC(){return & _faceRingsSC;}
GLBuffer * SubdivisionLevel::faceRings(){return & _faceRings;}
GLBuffer * SubdivisionLevel::vertexRingsSC(){return & _vertexRingsSC;}
GLBuffer * SubdivisionLevel::vertexRings(){return & _vertexRings;}
GLBuffer * SubdivisionLevel::particlesIn(){return & _particlesIn;}
GLBuffer * SubdivisionLevel::particlesOut(){return & _particlesOut;}
GLBuffer * SubdivisionLevel::deltas(){return & _deltas;}
GLBuffer * SubdivisionLevel::mask(){return & _mask;}
GLBuffer * SubdivisionLevel::triangles(){return & _triangles;}
GLBuffer * SubdivisionLevel::visibilityMask(){return & _visibilityMask;}

GLBuffer * SubdivisionLevel::falloff1(){return &_falloff1;}
GLBuffer * SubdivisionLevel::falloff2(){return &_falloff2;}
GLBuffer * SubdivisionLevel::brush(){return &_brush;}
GLBuffer * SubdivisionLevel::faceHits(){return &_faceHits;}
GLBuffer * SubdivisionLevel::area(){return &_area;}
GLBuffer * SubdivisionLevel::waveIndices(){return &_waveIndices;}

void SubdivisionLevel::build(int numVertices,
                             const uintArray & faceCountsIn,
                             const uintArray & faceVertsIn)
{
    _numVertices = numVertices;
    _numFaces = faceCountsIn.size();

    // triangulate faces with more than 4 vertices
    uintArray faceVertices;
    uintArray triangleIndices;

    faceVertices.reserve(faceVertsIn.size());
    triangleIndices.reserve(faceCountsIn.size() * 2);

    uint acc = 0;
    uint v1_in, v2_in, v3_in, v4_in;
    uint v1,v2,v3,v4;
    for(int i=0; i<_numFaces; i++)
    {
        uint numFaceVerts = faceCountsIn[i];

        v1_in = faceVertsIn[acc++];
        v2_in = faceVertsIn[acc++];
        v3_in = faceVertsIn[acc++];
        v4_in = (numFaceVerts > 3) ? faceVertsIn[acc++] : Utils::INVALID_ID;

        v1 = v1_in;
        v2 = v2_in;
        v3 = v3_in;
        v4 = v4_in;

        faceVertices.append(v1);
        faceVertices.append(v2);
        faceVertices.append(v3);
        faceVertices.append(v4);

        triangleIndices.append(v1);
        triangleIndices.append(v2);
        triangleIndices.append(v3);

        if(v4 != Utils::INVALID_ID)
        {
            triangleIndices.append(v1);
            triangleIndices.append(v3);
            triangleIndices.append(v4);
        }

        for(uint j=4; j<numFaceVerts; j++)
        {
            v1 = faceVertsIn[acc-j];
            v2 = faceVertsIn[acc-1];
            v3 = faceVertsIn[acc++];

            faceVertices.append(v1);
            faceVertices.append(v2);
            faceVertices.append(v3);
            faceVertices.append(Utils::INVALID_ID);

            triangleIndices.append(v1);
            triangleIndices.append(v2);
            triangleIndices.append(v3);
        }
    }

    _numFaces = faceVertices.size() / 4;

    _numTriangles = triangleIndices.size() / 3;

    // get num faces around each vertex
    uintArray countRing(_numVertices,0);

    uint numFaceRings = 0;
    for(int i=0; i<_numFaces; i++)
    {
        countRing[faceVertices[i*4+0]]++; numFaceRings++;
        countRing[faceVertices[i*4+1]]++; numFaceRings++;
        countRing[faceVertices[i*4+2]]++; numFaceRings++;

        if(faceVertices[i*4+3] != Utils::INVALID_ID)
        {
            countRing[faceVertices[i*4+3]]++; numFaceRings++;
        }
    }

    // get face rings start count
    uintArray faceRingStartCount(_numVertices * 2);

    acc = 0;
    for(uint i=0; i<_numVertices; i++)
    {
        uint count = countRing[i];
        faceRingStartCount[i * 2] = acc;
        faceRingStartCount[i * 2 + 1] = count;
        acc += count;
    }

    // get face rings
    uintArray faceRing(numFaceRings);

    for(uint i=0; i<_numFaces; i++)
    {
        uint iv1 = faceVertices[i*4];
        uint iv2 = faceVertices[i*4+1];
        uint iv3 = faceVertices[i*4+2];
        uint iv4 = faceVertices[i*4+3];

        faceRing[faceRingStartCount[iv1 * 2] + (--countRing[iv1])] = i;
        faceRing[faceRingStartCount[iv2 * 2] + (--countRing[iv2])] = i;
        faceRing[faceRingStartCount[iv3 * 2] + (--countRing[iv3])] = i;

        if(iv4 != Utils::INVALID_ID)
        {
            faceRing[faceRingStartCount[iv4 * 2] + (--countRing[iv4])] = i;
        }
    }

    // reorder the face rings
    uint prev, next;
    uint start, count, end;
    uint iFace0, iFace1;
    for(uint i=0; i<_numVertices; i++)
    {
        start = faceRingStartCount[i*2];
        count = faceRingStartCount[i*2+1];
        end = start+count;

        // loop in the first face
        for(uint j=start; j<end; j++)
        {
            iFace0 = faceRing[j];

            v1 = faceVertices[iFace0*4+0];
            v2 = faceVertices[iFace0*4+1];
            v3 = faceVertices[iFace0*4+2];
            v4 = faceVertices[iFace0*4+3];

            bool isQuad = (v4 != Utils::INVALID_ID);

            // get the next vertex in this face
            if     (v1 == i) next = v2;
            else if(v2 == i) next = v3;
            else if(v3 == i) next = isQuad ? v4 : v1;
            else if(v4 == i) next = v1;

            // loop from next face
            for(uint k=j+1; k<end; k++)
            {
                iFace1 = faceRing[k];

                v1 = faceVertices[iFace1*4];
                v2 = faceVertices[iFace1*4+1];
                v3 = faceVertices[iFace1*4+2];
                v4 = faceVertices[iFace1*4+3];

                bool isQuad = (v4 != Utils::INVALID_ID);

                // get the previews vertex in the face
                if     (v1 == i) prev = isQuad ? v4 : v3;
                else if(v2 == i) prev = v1;
                else if(v3 == i) prev = v2;
                else if(v4 == i) prev = v3;

                if(next == prev)
                {
                    uint temp = faceRing[j+1];
                    faceRing[j+1] = faceRing[k];
                    faceRing[k] = temp;
                    break;
                }
            }
        }
    }

    // get ring vertices
    uintArray vertexRingStartCount(_numVertices*2);
    uintArray vertexRing;
    vertexRing.reserve(_numVertices*6);

    uintArray tags(_numVertices, 0);
    uint flag = 0;

    acc=0;
    for(int i=0; i<_numVertices; i++)
    {
        uint start = faceRingStartCount[i*2];
        uint end = start+faceRingStartCount[i*2+1];

        uint numFoundVerts = 0;
        flag++;
        for(uint j=start; j<end; j++)
        {
            uint iFace = faceRing[j];

            uint v1 = faceVertices[iFace*4];
            uint v2 = faceVertices[iFace*4+1];
            uint v3 = faceVertices[iFace*4+2];
            uint v4 = faceVertices[iFace*4+3];

            bool isQuad = (v4 != Utils::INVALID_ID);

            if(v1 == i)
            {
                prev = isQuad ? v4 : v3;
                next = v2;
            }
            else if(v2 == i)
            {
                prev = v1;
                next = v3;
            }
            else if(v3 == i)
            {
                prev = v2;
                next = isQuad ? v4 : v1;

            }
            else if(v4 == i)
            {
                prev = v3;
                next = v1;
            }

            // check if prev and next have been visited for this ring
            if(tags[prev] != flag)
            {
                vertexRing.push_back(prev);
                tags[prev] = flag;
                numFoundVerts++;
            }

            if(tags[next] != flag)
            {
                vertexRing.push_back(next);
                tags[next] = flag;
                numFoundVerts++;
            }
        }

        vertexRingStartCount[i*2] = acc;
        vertexRingStartCount[i*2+1] = numFoundVerts;
        acc += numFoundVerts;

    }

    // tag border vertices
    uintArray borderVertices(_numVertices);

    for(int i=0; i<_numVertices; i++)
    {
        uint v = vertexRingStartCount[i*2+1];
        uint f = faceRingStartCount[i*2+1];

        borderVertices[i] = (v != f);
    }

    // BUILD SPRINGS
    //    QVector<uintArray> springs(_numVertices);
    //    QVector<uintArray> springTypes(_numVertices);

    QVector<Particle> particles(_numVertices);

    uint SHEAR = 0;
    uint STRUCTURAL = 1;
    uint FLEXION = 2;


    // get the shear springs
    for(int i=0; i<_numFaces; i++)
    {
        uint iv1 = faceVertices[i*4+0];
        uint iv2 = faceVertices[i*4+1];
        uint iv3 = faceVertices[i*4+2];
        uint iv4 = faceVertices[i*4+3];

        if(iv4!=Utils::INVALID_ID)
        {
            particles[iv1].addSpring(iv3, SHEAR);
            particles[iv3].addSpring(iv1, SHEAR);
            particles[iv2].addSpring(iv4, SHEAR);
            particles[iv4].addSpring(iv2, SHEAR);
        }
    }

    // get structural springs
    for(int i=0; i<_numVertices; i++)
    {
        uint start = vertexRingStartCount[i*2];
        uint count = vertexRingStartCount[i*2+1];
        uint end = start+count;

        for(uint j=start; j<end; j++)
        {
            particles[i].addSpring(vertexRing[j], STRUCTURAL);
        }
    }

    // get flexion springs
    for(int i=0; i<_numVertices; i++)
    {
        uint start = vertexRingStartCount[i*2];
        uint count = vertexRingStartCount[i*2+1];

        if(count == 4)
        {
            uint iv1 = vertexRing[start];
            uint iv2 = vertexRing[start+1];
            uint iv3 = vertexRing[start+2];
            uint iv4 = vertexRing[start+3];

            particles[iv1].addSpring(iv3, FLEXION);
            particles[iv3].addSpring(iv1, FLEXION);
            particles[iv2].addSpring(iv4, FLEXION);
            particles[iv4].addSpring(iv2, FLEXION);
        }
        else if(count == 3 && borderVertices[i])
        {
            uint iv1 = vertexRing[start];
            uint iv2 = vertexRing[start+1];
            uint iv3 = vertexRing[start+2];

            if(borderVertices[i] && borderVertices[i])
            {
                if(borderVertices[iv1] && borderVertices[iv2])
                {
                    particles[iv1].addSpring(iv2, FLEXION);
                    particles[iv2].addSpring(iv1, FLEXION);
                }
                else if(borderVertices[iv1] && borderVertices[iv3])
                {
                    particles[iv1].addSpring(iv3, FLEXION);
                    particles[iv3].addSpring(iv1, FLEXION);
                }
                else if(borderVertices[iv2] && borderVertices[iv3])
                {
                    particles[iv2].addSpring(iv3, FLEXION);
                    particles[iv3].addSpring(iv2, FLEXION);
                }
            }
        }
    }

    // upload buffers to gpu

    // border tags 0 1
    _border.setData(borderVertices, 0);

    // topology tags, initialize all to 0 and update when reconstruct subd
    uintArray topologyTags(_numVertices, Utils::INVALID_ID);
    _topologyTags.setData(topologyTags, 0);

    // face indices
    _faceIndices.setData(faceVertices, 0);

    // triangle indices
    _triangles.setData(triangleIndices, 0);

    // face normals, init to zero, compute normals per frame
    vec4Array faceNormals(_numFaces);
    _faceNormals.setData(faceNormals, 0);

    // face rings
    _faceRingsSC.setData(faceRingStartCount, 0);
    _faceRings.setData(faceRing, 0);

    // vertex ring
    _vertexRingsSC.setData(vertexRingStartCount, 0);
    _vertexRings.setData(vertexRing, 0);

    // particles
    _particlesIn.setData(particles, 0);
    _particlesOut.setData(particles, 0);

    // deltas
    _deltas.setData(vec4Array(_numVertices), 0);

    // mask
    _mask.setData(QVector<float>(_numVertices, 0.0f), 0);

    _falloff1.setData(floatArray(_numVertices), 0);
    _falloff2.setData(floatArray(_numVertices), 0);
    _brush.setData(QVector<BrushData>(2), 0);
    _faceHits.setData(QVector<FaceHit>(_numFaces), 0);
    _area.setData(vec4Array(_numVertices*2), 0);
    _waveIndices.setData(uintArray(_numVertices), 0);
    _visibilityMask.setData(uintArray(_numVertices, 1), 0);
}

int SubdivisionLevel::getNumVertices()
{
    return _numVertices;
}

int SubdivisionLevel::getNumFaces()
{
    return _numFaces;
}

int SubdivisionLevel::getNumTriangles()
{
    return _numTriangles;
}

}
