#include <Qt3DCore/QEntity>
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QTorusMesh>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRenderer/QRenderAspect>
#include <Qt3DRenderer/QFrameGraph>
#include <Qt3DRenderer/QForwardRenderer>
#include <Qt3DRenderer/QPhongMaterial>

#include <Qt3DRenderer/QCylinderMesh>
#include <Qt3DRenderer/QSphereMesh>
#include <Qt3DRenderer/QTorusMesh>
#include <Qt3DRenderer/QWindow>

#include <QPropertyAnimation>

#include "simulationview.h"

SimulationView::SimulationView()
{
    m_input = new Qt3D::QInputAspect();
    registerAspect(m_input);

    m_rootEntity = new Qt3D::QEntity();

    Qt3D::QCamera *camera = defaultCamera();

    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, -40.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));
    m_input->setCamera(camera);

    // from code

    // Material
        Qt3D::QMaterial *material = new Qt3D::QPhongMaterial(m_rootEntity);

        // Torus
        Qt3D::QEntity *torusEntity = new Qt3D::QEntity(m_rootEntity);
        Qt3D::QTorusMesh *torusMesh = new Qt3D::QTorusMesh;
        torusMesh->setRadius(5);
        torusMesh->setMinorRadius(1);
        torusMesh->setRings(100);
        torusMesh->setSlices(20);

        Qt3D::QTransform *torusTransform = new Qt3D::QTransform;
        Qt3D::QScaleTransform *torusScaleTransform = new Qt3D::QScaleTransform;
        torusScaleTransform->setScale3D(QVector3D(1.5, 1, 0.5));

        Qt3D::QRotateTransform *torusRotateTransform = new Qt3D::QRotateTransform;
        torusRotateTransform->setAxis(QVector3D(1, 0, 0));
        torusRotateTransform->setAngleDeg(45);

        torusTransform->addTransform(torusScaleTransform);
        torusTransform->addTransform(torusRotateTransform);

        torusEntity->addComponent(torusMesh);
        torusEntity->addComponent(torusTransform);
        torusEntity->addComponent(material);

        // Sphere
        Qt3D::QEntity *sphereEntity = new Qt3D::QEntity(m_rootEntity);
        Qt3D::QSphereMesh *sphereMesh = new Qt3D::QSphereMesh;
        sphereMesh->setRadius(3);

        Qt3D::QTransform *sphereTransform = new Qt3D::QTransform;
        Qt3D::QTranslateTransform *sphereTranslateTransform = new Qt3D::QTranslateTransform;
        sphereTranslateTransform->setTranslation(QVector3D(20, 0, 0));

        Qt3D::QRotateTransform *sphereRotateTransform = new Qt3D::QRotateTransform;
        QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereRotateTransform);
        sphereRotateTransformAnimation->setTargetObject(sphereRotateTransform);
        sphereRotateTransformAnimation->setPropertyName("angle");
        sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
        sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
        sphereRotateTransformAnimation->setDuration(10000);
        sphereRotateTransformAnimation->setLoopCount(-1);
        sphereRotateTransformAnimation->start();

        sphereRotateTransform->setAxis(QVector3D(0, 1, 0));
        sphereRotateTransform->setAngleDeg(0);

        sphereTransform->addTransform(sphereTranslateTransform);
        sphereTransform->addTransform(sphereRotateTransform);

        sphereEntity->addComponent(sphereMesh);
        sphereEntity->addComponent(sphereTransform);
        sphereEntity->addComponent(material);

    // end example

    setRootEntity(m_rootEntity);
}
