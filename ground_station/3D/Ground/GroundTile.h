
#ifndef GS_MATTERHORN_GROUNDTILETMP_H
#define GS_MATTERHORN_GROUNDTILETMP_H

#include <Qt3DCore/QNode>
#include <3D/Scene/WorldReference.h>
#include "HighlightArea.h"
#include "3D/CoordinateUtils.h"
#include "3D/Grid/ContinuousElevationModel.h"

class GroundTile : public Qt3DCore::QEntity {
Q_OBJECT
public:
    explicit GroundTile(Qt3DCore::QNode *parent,
                        const QVector2D &offset,
                        const LatLon &topLeftLatLon,
                        std::shared_ptr<const ContinuousElevationModel> model,
                        std::shared_ptr<const WorldReference> worldRef,
                        const QString texturePath = "");

    float vertexHeightAt(int i, int j) const;

public slots:

private:
    Qt3DCore::QTransform *transform_;
    GridMesh *mesh_;
};


#endif //GS_MATTERHORN_GROUNDTILETMP_H
