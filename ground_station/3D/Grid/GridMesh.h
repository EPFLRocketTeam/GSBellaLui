
#ifndef GS_MATTERHORN_GRIDMESH_H
#define GS_MATTERHORN_GRIDMESH_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QtCore/QSize>
#include "DiscreteElevationModel.h"


class GridMesh : public Qt3DRender::QGeometryRenderer {
Q_OBJECT

public:
    explicit GridMesh(Qt3DCore::QNode *parent, const DiscreteElevationModel *const model);
};


#endif //GS_MATTERHORN_GRIDMESH_H
