#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <3D/Utils.h>
#include <3D/Billboards/Marker.h>
#include <3D/Billboards/Tracker.h>
#include "GroundStation.h"

GroundStation::GroundStation(QVector3D position, Qt3DRender::QTexture2D *texture, Qt3DRender::QCamera *camera,
                             Qt3DCore::QNode *parent)
        : Qt3DCore::QEntity(parent),
          position_{position},
          transform_{new Qt3DCore::QTransform()} {

    QMatrix4x4 m{};
    m.translate(position_);
    transform_->setMatrix(m);

    this->addComponent(transform_);

    new Tracker(QVector3D{0, 100, 0}, camera, texture, QStringLiteral("GROUND STATION"), this, {0, 0, 0},
                OpenGLConstants::ABOVE_RIGHT);
}

