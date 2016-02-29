#include <bullet/btBulletDynamicsCommon.h>

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
#include <Qt3DRenderer/QViewport>

#include "fluid.h"
#include "submarine.h"
#include "forcearrow.h"

#include "simulation.h"

Simulation::Simulation(QObject *parent) :
    Qt3D::QWindow(),
    m_time(0)
{
    m_fluid = new Fluid(1000, this);
    m_submarine = new Submarine(m_fluid, this);

    m_submarine->setLength(2.9);
    m_submarine->setWidth(0.6);
    m_submarine->setHeight(0.7);
    m_submarine->setMass(140);
    m_submarine->setDragCoefficient(0.04);
    m_submarine->setLiftCoefficientSlope(M_PI / 2.);
    m_submarine->setSpinningDragCoefficient(2);
    m_submarine->setBuoyancyPosition(QVector3D(0, 0.35, 0));
    m_submarine->setWeightPosition(QVector3D());

    m_submarine->setHasHorizontalFins(true);
    m_submarine->setHorizontalFinsLiftCoefficientSlope(M_PI);
    m_submarine->setHorizontalFinsArea(0.025);
    m_submarine->setHorizontalFinsDragCoefficient(0.03);
    m_submarine->setHorizontalFinsPosition(0);
    m_submarine->setHorizontalFinsAspectRatio(3);

    m_submarine->setHasVerticalFins(true);
    m_submarine->setVerticalFinsArea(0.025);
    m_submarine->setVerticalFinsLiftCoefficientSlope(M_PI);
    m_submarine->setVerticalFinsDragCoefficient(0.03);
    m_submarine->setVerticalFinsPosition(0);
    m_submarine->setHorizontalFinsAspectRatio(3);

    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_pairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver;

    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_pairCache,
                                          m_solver, m_collisionConfiguration);

    m_world->setGravity(btVector3(0, 0, 0));

    m_submarine->addToWorld(m_world);

    m_input = new Qt3D::QInputAspect();
    registerAspect(m_input);

    m_rootEntity = new Qt3D::QEntity();

    Qt3D::QCamera *camera = defaultCamera();

    Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
    setFrameGraph(frameGraph);

    Qt3D::QForwardRenderer *forwardRenderer = new Qt3D::QForwardRenderer();
    forwardRenderer->setCamera(defaultCamera());
    forwardRenderer->setClearColor(QColor(26, 164, 154));
    frameGraph->setActiveFrameGraph(forwardRenderer);

    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 8.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    setRootEntity(m_rootEntity);

    m_submarine->addToScene(m_rootEntity);

    m_axisX = new ForceArrow(Qt::red, 1, this);
    m_axisX->addToScene(m_rootEntity);
    m_axisX->update(QVector3D(500, 0, 0), QVector3D());

    m_axisY = new ForceArrow(Qt::green, 1, this);
    m_axisY->addToScene(m_rootEntity);
    m_axisY->update(QVector3D(0, 500, 0), QVector3D());

    m_axisZ = new ForceArrow(Qt::blue, 1, this);
    m_axisZ->addToScene(m_rootEntity);
    m_axisZ->update(QVector3D(0, 0, 500), QVector3D());
}

Simulation::~Simulation() {
    delete m_submarine;
    delete m_fluid;

    delete m_world;

    delete m_solver;
    delete m_pairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Simulation::step() {
    float dt = 1.f / 60.f;

    m_world->stepSimulation(dt, 10);

    m_submarine->update(defaultCamera());

    m_time += dt;
}

Submarine *Simulation::submarine() const
{
    return m_submarine;
}

void Simulation::setSubmarine(Submarine *submarine)
{
    m_submarine = submarine;
}

double Simulation::time() const {
    return m_time;
}
