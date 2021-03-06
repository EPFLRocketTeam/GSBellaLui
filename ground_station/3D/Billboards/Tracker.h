
#ifndef GS_MATTERHORN_TRACKER_H
#define GS_MATTERHORN_TRACKER_H

#include <Qt3DRender/QTexture>
#include <3D/Billboards/Marker.h>
#include <3D/Billboards/Text3D.h>
#include <Qt3DRender/QCamera>
#include <3D/Interfaces/IObservable.h>


class Tracker : public Qt3DCore::QEntity, public IObservable {
Q_OBJECT
public:
    explicit Tracker(const QVector3D &position, const Qt3DRender::QCamera *const camera, const QString &iconTextureName, const QString &text,
                     TextType textType, Qt3DCore::QNode *const parent, const QVector3D &markerOffset,
                     const QVector3D &textOffset);

public slots:

    void updatePosition(QVector3D pos);

    QVector3D getPosition();

    Qt3DCore::QTransform *getObjectTransform() const override;

private:
    Marker *marker_;
    Text3D *text_;

    Qt3DCore::QTransform *transform_;
    QVector3D textOffset_;
    QVector3D markerOffset_;
};


#endif //GS_MATTERHORN_TRACKER_H
