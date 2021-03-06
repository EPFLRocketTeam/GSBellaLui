
#include "CameraController.h"
#include "Utils.h"
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DExtras>

CameraController::CameraController(Qt3DCore::QNode *parent)
        : Qt3DExtras::QAbstractCameraController(parent),
          keyboardHandler_{new Qt3DInput::QKeyboardHandler(this)},
          arrowPressed_{false},
          azimuthalAngle_{0},
          polarAngle_{M_PI / 2.4f},
          viewCenter_{0, 0, 0},
          viewCenterOffset_{0, 0, 0},
          viewingDistance_{CameraConstants::VIEWING_DISTANCE_DEFAULT},
          animators_{},
          observables_{},
          observableIt_{observables_.begin()} {
    connect(keyboardHandler_, &Qt3DInput::QKeyboardHandler::pressed, this, &CameraController::handleKeyPress);
    keyboardHandler_->setSourceDevice(keyboardDevice());
    keyboardHandler_->setFocus(true);
}

CameraController::~CameraController() = default;

void
CameraController::registerObservable(IObservable *o) {
    observables_.emplace_back(o);

    if (observables_.size() == 1) {
        switchObservable(); //Look at first-added observable
    }
}

void
CameraController::unregisterObservable(IObservable *o) {
    observables_.remove(o);
}

void
CameraController::switchObservable() {

    if (observables_.empty())
        return;

    animators_.clear();

    if (++observableIt_ == observables_.end()) {
        observableIt_ = observables_.begin();
    }

    QVector3D observablePos = (*observableIt_)->getObjectTransform()->translation();
    QVector3D obsToCam = camera()->position() - observablePos;
    obsToCam = viewingDistance_ * obsToCam.normalized();

    // Recompute azimuthal and polar angle based on the vector going from the tracked object to the camera
    float newPolarAngle = std::acos(obsToCam.y() / viewingDistance_);
    float newAzimuthalAngle = std::atan2(obsToCam.z(), obsToCam.x());

    azimuthalAngle_ = wrapAngle(azimuthalAngle_);
    newAzimuthalAngle = wrapAngle(newAzimuthalAngle);

    /*std::cout << "azimuth: " << azimuthalAngle_ << " new azimuth: " << newAzimuthalAngle << "angular dist "
              << angularDistance(azimuthalAngle_, newAzimuthalAngle) << std::endl;*/

    // Adjust polar angle
    animators_.push_back(std::make_shared<FloatInterpolator>(
            newPolarAngle,
            &polarAngle_));

    // Adjust azimuthal angle
    animators_.push_back(std::make_shared<FloatInterpolator>(
            azimuthalAngle_ + angularDistance(azimuthalAngle_, newAzimuthalAngle),
            &azimuthalAngle_));

    /*// Reset viewing distance
    animators_.push_back(std::make_shared<FloatInterpolator>(
            ,
            &viewingDistance_));*/

    // Reset offset
    animators_.push_back(std::make_shared<QVector3DInterpolator>(
            QVector3D{0, 0, 0},
            &viewCenterOffset_));

}

void
CameraController::handleKeyPress(Qt3DInput::QKeyEvent *event) {

    //std::cout << "Qt3D captured: " << event->key() << "\n";

    switch (event->key()) {
        case Qt::Key_Space:
            switchObservable();
            break;
        case Qt::Key_R:
            emit resetPressed();
        default:
            break;
    }
}


void
CameraController::moveCamera(const Qt3DExtras::QAbstractCameraController::InputState &state, float dt) {


    Qt3DRender::QCamera *cam = camera();

    if (cam == nullptr) {
        return;
    }


    // Mouse input
    if (state.rightMouseButtonActive | state.shiftKeyActive) {
        if (state.leftMouseButtonActive) {
            viewingDistance_ = clampViewingDistance(viewingDistance_ + state.ryAxisValue * 300 * linearSpeed() * dt);
        } else {
            // Translate
            QVector3D rightVector = QVector3D::crossProduct(
                    cam->viewVector().normalized(),
                    cam->upVector().normalized()
            ).normalized();

            QVector3D upVector = QVector3D::crossProduct(
                    rightVector.normalized(),
                    cam->viewVector().normalized()
            ).normalized();

            viewCenterOffset_ -=
                    rightVector * state.rxAxisValue * linearSpeed() + upVector * state.ryAxisValue * linearSpeed();
        }
    } else if (state.leftMouseButtonActive) {
        // Orbit
        azimuthalAngle_ += state.rxAxisValue * lookSpeed() * dt;
        polarAngle_ = clampPolarAngle(polarAngle_ + state.ryAxisValue * lookSpeed() * dt);
    }

    if (state.txAxisValue == 0.0 && state.tyAxisValue == 0.0 && arrowPressed_) {
        arrowPressed_ = false;
    }

    if (!arrowPressed_) {

        if (state.txAxisValue != 0.0 || state.tyAxisValue != 0.0) {
            arrowPressed_ = true;
        }

        if (state.txAxisValue > 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    azimuthalAngle_ - CameraConstants::AZIMUTH_STEP,
                    &azimuthalAngle_)
            );
        } else if (state.txAxisValue < 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    azimuthalAngle_ + CameraConstants::AZIMUTH_STEP,
                    &azimuthalAngle_)
            );
        } else if (state.tyAxisValue > 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    clampPolarAngle(polarAngle_ - CameraConstants::POLAR_STEP),
                    &polarAngle_)
            );

        } else if (state.tyAxisValue < 0.0) {
            animators_.push_back(std::make_shared<FloatInterpolator>(
                    clampPolarAngle(polarAngle_ + CameraConstants::POLAR_STEP),
                    &polarAngle_)
            );
        }
    }

    updateAnimators();
    placeCamera();
}

void
CameraController::placeCamera() {

    if (animators_.empty()) {
        azimuthalAngle_ = wrapAngle(azimuthalAngle_);
    }

    if (observableIt_ != observables_.end()) {
        viewCenter_ += ((*observableIt_)->getObjectTransform()->translation() - viewCenter_) * 0.1;
    }

    float x = viewingDistance_ * std::sin(polarAngle_) * std::cos(azimuthalAngle_);
    float z = viewingDistance_ * std::sin(polarAngle_) * std::sin(azimuthalAngle_);
    float y = viewingDistance_ * std::cos(polarAngle_);

    camera()->setPosition(viewCenter_ + viewCenterOffset_ + QVector3D{x, y, z});
    camera()->setViewCenter(viewCenter_ + viewCenterOffset_);
}


void
CameraController::updateAnimators() {

    for (const auto &animator : animators_)
        animator->updateTarget();

    // Remove animators which have finished
    animators_.erase(std::remove_if(
            animators_.begin(),
            animators_.end(),
            [](std::shared_ptr<Interpolator> i) { return i->animationIsFinished(); }), animators_.end());
}


void
CameraController::setCameraViewCenter(const QVector3D &newViewCenter) {
    viewCenter_ = newViewCenter;

}
