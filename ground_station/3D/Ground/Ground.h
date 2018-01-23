
#ifndef GS_MATTERHORN_GROUND_H
#define GS_MATTERHORN_GROUND_H


#include <3D/CoordinateUtils.h>
#include <3D/Scene/WorldReference.h>
#include <3D/Grid/ContinuousElevationModel.h>
#include "GroundTile.h"

class Ground {

public:

    explicit Ground(Qt3DCore::QNode *parent,
                    const ContinuousElevationModel *model,
                    const WorldReference *const worldRef,
                    int sideLength);

    float groundElevationAt(int worldX, int worldZ) const;

private:

    const int sideLength_;
    const int halfSideLength_;
    const int extentFromOrigin_;
    std::vector<std::unique_ptr<GroundTile>> tiles_;

};


#endif //GS_MATTERHORN_GROUND_H
