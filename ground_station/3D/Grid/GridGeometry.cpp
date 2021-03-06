
#include <Qt3DRender/FunctorType>
#include <3D/3DVisualisationConstants.h>
#include <3D/Utils.h>
#include "GridGeometry.h"


QByteArray
GridGeometry::createPlaneVertexData(
        const std::function<float(int, int)> &heightSampler,
        const std::function<QVector3D(int, int)> &normalSampler) {
    Q_ASSERT(sideLength_ > 0.0f);
    Q_ASSERT(gridResolution_ >= 2);

    const int nVerts = gridResolution_ * gridResolution_;
    float vertSpacing = static_cast<float>(sideLength_) / static_cast<float>(gridResolution_ - 1);

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray bufferBytes;
    bufferBytes.resize(stride * nVerts);
    auto *fptr = reinterpret_cast<float *>(bufferBytes.data());

    const float x0 = 0.0f;
    const float z0 = 0.0f;
    const float dx = vertSpacing;
    const float dz = vertSpacing;
    const float du = 1.0f / (gridResolution_ - 1);
    const float dv = 1.0f / (gridResolution_ - 1);

    // Iterate over z
    for (int j = 0; j < gridResolution_; ++j) {
        const float z = z0 + static_cast<float>(j) * dz;
        const float u = static_cast<float>(j) * du;

        // Iterate over x
        for (int i = 0; i < gridResolution_; ++i) {
            const float x = x0 - static_cast<float>(i) * dx;
            const float v = static_cast<float>(i) * dv;

            // position
            *fptr++ = x;
            *fptr++ = heightSampler(x, z);
            *fptr++ = z;

            // texture coordinates
            *fptr++ = u;
            *fptr++ = 1.0f - v;

            // normal
            QVector3D n = normalSampler(x, z);
            *fptr++ = n.x();
            *fptr++ = n.y();
            *fptr++ = n.z();

            // tangent
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
        }
    }

    return bufferBytes;
}

QByteArray
GridGeometry::createPlaneIndexData() {
    // Create the index data. 2 triangles per rectangular face
    const int faces = 2 * (gridResolution_ - 1) * (gridResolution_ - 1);
    const int indices = 3 * faces;
    Q_ASSERT(indices < std::numeric_limits<quint16>::max());
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint16));
    auto *indexPtr = reinterpret_cast<quint16 *>(indexBytes.data());

    // Iterate over z
    for (int j = 0; j < gridResolution_ - 1; ++j) {
        const int rowStartIndex = j * gridResolution_;
        const int nextRowStartIndex = (j + 1) * gridResolution_;

        // Iterate over x
        for (int i = 0; i < gridResolution_ - 1; ++i) {
            // Split quad into two triangles
            *indexPtr++ = rowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;
            *indexPtr++ = nextRowStartIndex + i;

            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;
            *indexPtr++ = nextRowStartIndex + i + 1;
        }
    }

    return indexBytes;
}


GridGeometry::GridGeometry(Qt3DCore::QNode *parent,
                           const std::function<float(int, int)> &heightSampler,
                           const std::function<QVector3D(int, int)> &normalSampler,
                           int sideLength,
                           int resolution)
        :
        Qt3DRender::QGeometry(parent),
        sideLength_{sideLength},
        gridResolution_{resolution},
        positionAttribute_(new Qt3DRender::QAttribute()),
        normalAttribute_(new Qt3DRender::QAttribute()),
        texCoordAttribute_(new Qt3DRender::QAttribute()),
        tangentAttribute_(new Qt3DRender::QAttribute()),
        indexAttribute_(new Qt3DRender::QAttribute()),
        vertexBuffer_(new Qt3DRender::QBuffer()),
        indexBuffer_(new Qt3DRender::QBuffer()),
        vStride_{(3 + 2 + 3 + 4) * sizeof(float)} {

    const int nVerts = gridResolution_ * gridResolution_;
    const int faces = 2 * (gridResolution_ - 1) * (gridResolution_ - 1);

    vertexBuffer_->setData(createPlaneVertexData(heightSampler, normalSampler));
    indexBuffer_->setData(createPlaneIndexData());

    positionAttribute_->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute_->setVertexSize(3);
    positionAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute_->setBuffer(vertexBuffer_);
    positionAttribute_->setByteStride(vStride_);
    positionAttribute_->setCount(nVerts);

    texCoordAttribute_->setName(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
    texCoordAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    texCoordAttribute_->setVertexSize(2);
    texCoordAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    texCoordAttribute_->setBuffer(vertexBuffer_);
    texCoordAttribute_->setByteStride(vStride_);
    texCoordAttribute_->setByteOffset(3 * sizeof(float));
    texCoordAttribute_->setCount(nVerts);

    normalAttribute_->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
    normalAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    normalAttribute_->setVertexSize(3);
    normalAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute_->setBuffer(vertexBuffer_);
    normalAttribute_->setByteStride(vStride_);
    normalAttribute_->setByteOffset(5 * sizeof(float));
    normalAttribute_->setCount(nVerts);

    tangentAttribute_->setName(Qt3DRender::QAttribute::defaultTangentAttributeName());
    tangentAttribute_->setVertexBaseType(Qt3DRender::QAttribute::Float);
    tangentAttribute_->setVertexSize(4);
    tangentAttribute_->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    tangentAttribute_->setBuffer(vertexBuffer_);
    tangentAttribute_->setByteStride(vStride_);
    tangentAttribute_->setByteOffset(8 * sizeof(float));
    tangentAttribute_->setCount(nVerts);

    indexAttribute_->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute_->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute_->setBuffer(indexBuffer_);

    // Each primitive has 3 vertives
    indexAttribute_->setCount(faces * 3);

    this->addAttribute(positionAttribute_);
    this->addAttribute(texCoordAttribute_);
    this->addAttribute(normalAttribute_);
    this->addAttribute(tangentAttribute_);
    this->addAttribute(indexAttribute_);
}

void
GridGeometry::resampleVertices(const std::function<float(int, int)> &heightSampler,
                               const std::function<QVector3D(int, int)> &normalSampler) {
    vertexBuffer_->setData(createPlaneVertexData(heightSampler, normalSampler));
}


float
GridGeometry::vertexHeightAt(int i, int j) const {
    if (!(
            0 <= i && i < gridResolution_
            &&
            0 <= j && j < gridResolution_
    )) {
        throw std::invalid_argument("Trying to access vertex height outside boudaries of mesh");
    } else {
        float *fptr = reinterpret_cast<float *>(vertexBuffer_->data().data());
        int index = (i * gridResolution_) + j;
        float h = fptr[index * (vStride_ / sizeof(float)) + 1]; // +1 to access the vertical component
        return h;
    }
}


