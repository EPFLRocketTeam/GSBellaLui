#include "HighlightArea.h"
#include <ProgramConstants.h>
#include <Qt3DRender/QShaderProgram>
#include <QUrl>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QMaterial>
#include <Qt3DExtras/QPlaneMesh>


HighlightArea::HighlightArea(Qt3DRender::QParameter *heightParameter, Qt3DCore::QNode *parent) :
        Qt3DCore::QEntity(parent), transform_{new Qt3DCore::QTransform()} {

    // Build effect
    auto *shaderProgram = new Qt3DRender::QShaderProgram();
    shaderProgram->setVertexShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/highlightArea.vert"}));
    shaderProgram->setFragmentShaderCode(shaderProgram->loadSource(QUrl{"qrc:/shaders/highlightArea.frag"}));

    auto *blendEquationArguments = new Qt3DRender::QBlendEquationArguments();
    blendEquationArguments->setSourceAlpha(Qt3DRender::QBlendEquationArguments::One);
    blendEquationArguments->setDestinationAlpha(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    blendEquationArguments->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendEquationArguments->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);

    auto *renderPass = new Qt3DRender::QRenderPass();
    renderPass->setShaderProgram(shaderProgram);
    renderPass->addRenderState(blendEquationArguments);

    auto *filterKey = new Qt3DRender::QFilterKey();
    filterKey->setName("renderingStyle");
    filterKey->setValue("forward");

    auto *technique = new Qt3DRender::QTechnique();
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(OpenGLConstants::VERSION_MAJOR);
    technique->graphicsApiFilter()->setMinorVersion(OpenGLConstants::VERSION_MINOR);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    auto *effect = new Qt3DRender::QEffect();
    effect->addTechnique(technique);

    // Set up material
    auto *material = new Qt3DRender::QMaterial();
    material->setEffect(effect);
    material->addParameter(heightParameter);

    // Set up mesh
    auto *mesh = new Qt3DExtras::QPlaneMesh();
    mesh->setHeight(500);
    mesh->setWidth(500);
    mesh->setMeshResolution(QSize{10, 10});

    // Offset upwards to show above ground
    transform_->setTranslation(QVector3D(0, 50, 0));

    this->addComponent(mesh);
    this->addComponent(material);
    this->addComponent(transform_);
}

void HighlightArea::updatePos(const QVector2D &pos) {
    transform_->setTranslation(QVector3D{pos.x(), 50, pos.y()});
}