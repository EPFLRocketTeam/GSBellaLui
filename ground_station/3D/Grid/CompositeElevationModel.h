
#ifndef GS_MATTERHORN_COMPOSITEELEVATIONMODEL_H
#define GS_MATTERHORN_COMPOSITEELEVATIONMODEL_H


#include "DiscreteElevationModel.h"
#include "IDiscreteElevationModel.h"

class CompositeElevationModel : public IDiscreteElevationModel {
public:
    explicit CompositeElevationModel(std::unique_ptr<const IDiscreteElevationModel> d1,
                                     std::unique_ptr<const IDiscreteElevationModel> d2);

    float elevationAt(int latitudeIndex, int longitudeIndex) const override;

    Interval2D extent() const;

private:
    std::unique_ptr<const IDiscreteElevationModel> dem1_;
    std::unique_ptr<const IDiscreteElevationModel> dem2_;
    Interval2D extent_;
};


#endif //GS_MATTERHORN_COMPOSITEELEVATIONMODEL_H
