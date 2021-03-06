#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <Qt3DCore/QTransform>
#include "3D/Utils.h"
#include "3D/Billboards/Marker.h"
#include "3D/Billboards/Tracker.h"
#include "3D/ForwardRenderer/LayerManager.h"
#include "GroundStation.h"

GroundStation::GroundStation(QVector3D position, const QString &texture, Qt3DRender::QCamera *camera,
                             Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          position_{position},
          transform_{new Qt3DCore::QTransform()} {

    QMatrix4x4 m{};
    m.translate(position_);
    transform_->setMatrix(m);

    this->addComponent(transform_);
    this->addComponent(LayerManager::getInstance().getLayer(LayerType::VISIBLE));

    new Tracker(QVector3D{0, 10, 0}, camera, texture, QStringLiteral("GROUND STATION"), TextType::BOLD, this,
                {0, 0, 0},
                {-8, 1.25, 0});
}

Qt3DCore::QTransform *
GroundStation::getObjectTransform() const {
    return transform_;
}

