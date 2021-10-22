#include "MeshUtils.h"
#include "Utils.h"
#include "SubdivisionLevel.h"

namespace mush3d{

bool MeshUtils::reconstructLevel(SubdivisionLevel * level,
                                 int & numVerticesLow,
                                 uintArray & faceVertices_low,
                                 uintArray & faceCounts_low,
                                 vec2Array & faceUvs_low)
{
    int numVertices_high = level->getNumVertices();
    int numFaces_high = level->getNumFaces();


    uintArray faceVertices_high;
    uintArray faceRingSC_high;
    uintArray faceRing_high;
    uintArray vertexRingSC_high;
    uintArray vertexRing_high;
    uintArray borderMask_high;

    uintArray topologyTags_high(numVertices_high, Utils::INVALID_ID); // topology tag for old level will be defined

    level->faceIndices()->getData(faceVertices_high, 0);
    level->faceRingsSC()->getData(faceRingSC_high, 0);
    level->faceRings()->getData(faceRing_high, 0);
    level->vertexRingsSC()->getData(vertexRingSC_high, 0);
    level->vertexRings()->getData(vertexRing_high, 0);
    level->borders()->getData(borderMask_high, 0);

    // check if all the faces are quads
    for(int i=0; i<numFaces_high; i++)
    {
        if(faceVertices_high[i*4+3] == Utils::INVALID_ID)
        {
            return false;
        }
    }

    // find seed vertex
    int seedVert = -1;

    for(int i=0; i<numVertices_high; i++)
    {
        uint ringSize = vertexRingSC_high[i*2+1];

        if(ringSize > 4)
        {
            seedVert = i;
            break;
        }
    }

    if(seedVert == -1)
    {
        return false;
    }


    // create visited mask
    QVector<bool> visiteds(numVertices_high, false);

    // visit seed vertex and mark as a original vertex
    visiteds[seedVert] = true;
    topologyTags_high[seedVert] = ORIGINAL;

    // mask all the originals
    QVector<int> stack;
    stack.push_back(seedVert);

    while(stack.size() > 0)
    {
        // get last element on stack
        int VERTEX = stack.back();

        // remove the last element from stack
        stack.pop_back();

        // visit verts of faces around extraordinary vertex
        uint start = faceRingSC_high[VERTEX*2];
        uint end = start+faceRingSC_high[VERTEX*2+1];

        for(uint i=start; i<end; i++)
        {
            uint iFace = faceRing_high[i];

            uint iV1 = faceVertices_high[iFace*4];
            uint iV2 = faceVertices_high[iFace*4+1];
            uint iV3 = faceVertices_high[iFace*4+2];
            uint iV4 = faceVertices_high[iFace*4+3];

            visiteds[iV1] = true;
            visiteds[iV2] = true;
            visiteds[iV3] = true;
            visiteds[iV4] = true;
        }

        // visit ring verts of ring verts, if any is not visited by the faces its a new original
        start = vertexRingSC_high[VERTEX*2];
        end = start+vertexRingSC_high[VERTEX*2+1];

        for(uint i=start; i<end; i++)
        {
            uint iVert = vertexRing_high[i];

            uint start2 = vertexRingSC_high[iVert*2];
            uint end2 = start2+vertexRingSC_high[iVert*2+1];

            for(uint j=start2; j<end2; j++)
            {
                uint iVert2 = vertexRing_high[j];

                if(visiteds[iVert2] == false)
                {
                    topologyTags_high[iVert2] = ORIGINAL;
                    visiteds[iVert2] = true;
                    stack.push_back(iVert2);
                }
            }
        }
    }


    // mask all face verts
    int numErros = 0;
    for(int i=0; i<numVertices_high; i++)
    {
        if(topologyTags_high[i] != ORIGINAL)
        {
            uint start = vertexRingSC_high[i*2];
            uint end = start+vertexRingSC_high[i*2+1];

            bool isFaceVertex = true;

            for(uint j=start; j<end; j++)
            {
                uint iRing = vertexRing_high[j];
                if(topologyTags_high[iRing] == ORIGINAL)
                {
                    isFaceVertex = false;
                    break;
                }
            }
            if(isFaceVertex)
            {
                topologyTags_high[i] = FACE;
            }
        }
    }


    // validate face points, they can't be connected to another face point
    for(int i=0; i<numVertices_high; i++)
    {
        if(topologyTags_high[i] == FACE)
        {
            uint start = vertexRingSC_high[i*2];
            uint end = start+vertexRingSC_high[i*2+1];

            for(uint j=start; j<end; j++)
            {
                uint iRing = vertexRing_high[j];

                if(topologyTags_high[iRing] == FACE)
                {
                    return false;
                }

                topologyTags_high[iRing] = EDGE;
            }
        }
    }

    // build new arrays for the low mesh

    // build the faces
    int nbOriginalPoints = 0;
    int maxOriginalID = 0;
    for(int i=0; i<numVertices_high; i++)
    {
        if(topologyTags_high[i] == ORIGINAL)
        {
            nbOriginalPoints++;
            if(i > maxOriginalID)
                maxOriginalID = i;
        }
    }

    // the max original id has to ve smaller than num vertices of new level
    if(maxOriginalID >= nbOriginalPoints)
        return false;

    // build the new faces
    faceVertices_low.reserve(numFaces_high);
    faceCounts_low.reserve(numFaces_high);

    for(int i=0; i<numVertices_high; i++)
    {
        if(topologyTags_high[i] == FACE)
        {
            int start = int(faceRingSC_high[i*2]);
            int end = start+int(faceRingSC_high[i*2+1]);

            uint faceCount = 0;

            // reversed
            for(int j=end-1; j>=start; j--)
            {
                uint iFace = faceRing_high[j];

                // set the parent face
                uint iV1 = faceVertices_high[iFace*4];
                uint iV2 = faceVertices_high[iFace*4+1];
                uint iV3 = faceVertices_high[iFace*4+2];
                uint iV4 = faceVertices_high[iFace*4+3];

                uint ori = Utils::INVALID_ID;

                if(topologyTags_high[iV1] == ORIGINAL) ori = iV1;
                else if(topologyTags_high[iV2] == ORIGINAL) ori = iV2;
                else if(topologyTags_high[iV3] == ORIGINAL) ori = iV3;
                else if(iV4 != Utils::INVALID_ID && topologyTags_high[iV4] == ORIGINAL) ori = iV4;

                if(ori == Utils::INVALID_ID)
                {
                    return false;
                }

                faceVertices_low.append(ori);

                faceCount++;
            }
            faceCounts_low.append(faceCount);
        }
    }

    // uvs not implemented
    faceUvs_low.resize(faceCounts_low.size() * 4);

    // update topology tags of higher level
    level->topologyTags()->setData(topologyTags_high, 0);

    // get num vertices on lower level
    numVerticesLow = nbOriginalPoints;

    return true;
}

}
