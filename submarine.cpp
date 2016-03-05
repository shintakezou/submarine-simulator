#include <cmath>

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
#include <Qt3DRenderer/QMesh>

#include <Qt3DInput/QInputAspect>

#include <QVector2D>
#include <QPropertyAnimation>

#include <QtMath>
#include <QtDebug>

#include "fin.h"
#include "fluid.h"
#include "forcearrow.h"
#include "physics/body.h"
#include "physics/force.h"
#include "physics/torque.h"
#include "torquearrow.h"

#include "submarine.h"

btVector3 qtVector2btVector(QVector3D v)
{
    return btVector3(v.x(), v.y(), v.z());
}

Submarine::Submarine(QObject *parent) :
    QObject(parent),
    m_shape(0),
    m_body(0),
    m_entity(0),
    m_length(0),
    m_width(0),
    m_height(0),
    m_mass(0),
    m_propellorTorque(new Physics::PropellorTorque(this)),
    m_hasHorizontalFins(false),
    m_horizontalFinsArea(0),
    m_horizontalFinsLiftCoefficientSlope(0),
    m_horizontalFinsDragCoefficient(0),
    m_horizontalFinsPosition(0),
    m_horizontalFinsAspectRatio(0),
    m_hasVerticalFins(false),
    m_verticalFinsArea(0),
    m_verticalFinsLiftCoefficientSlope(0),
    m_verticalFinsDragCoefficient(0),
    m_verticalFinsPosition(0),
    m_verticalFinsAspectRatio(0),
    m_weight(new Physics::WeightForce(this)),
    m_buoyancy(new Physics::BuoyancyForce(this)),
    m_thrust(new Physics::ThrustForce(this)),
    m_drag(new Physics::DragForce(this)),
    m_lift(new Physics::LiftForce(this)),
    m_spinningDrag(new Physics::SpinningDragTorque(this))
{

}

Submarine::~Submarine()
{
    if (m_shape) {
        delete m_shape;
    }
}

Submarine *Submarine::makeDefault(QObject *parent)
{
    Submarine *submarine = new Submarine(parent);

    submarine->setLength(2.9);
    submarine->setWidth(0.6);
    submarine->setHeight(0.7);
    submarine->setMass(140);
    submarine->drag()->setCoefficient(0.04);
    submarine->lift()->setCoefficientSlope(M_PI / 2.);
    submarine->spinningDrag()->setCoefficient(2);
    submarine->buoyancy()->setPosition(QVector3D(0, 0.15, 0));
    submarine->weight()->setPosition(QVector3D());
    submarine->thrust()->setValue(QVector3D(100, 0, 10));
    submarine->propellorTorque()->setValue(QVector3D(20, 0, 0));

    submarine->setHasHorizontalFins(true);
    submarine->setHorizontalFinsLiftCoefficientSlope(M_PI);
    submarine->setHorizontalFinsArea(0.025);
    submarine->setHorizontalFinsDragCoefficient(0.03);
    submarine->setHorizontalFinsPosition(-0.725);
    submarine->setHorizontalFinsAspectRatio(3);

    submarine->setHasVerticalFins(true);
    submarine->setVerticalFinsArea(0.025);
    submarine->setVerticalFinsLiftCoefficientSlope(M_PI);
    submarine->setVerticalFinsDragCoefficient(0.03);
    submarine->setVerticalFinsPosition(-0.725);
    submarine->setVerticalFinsAspectRatio(3);

    return submarine;
}

void Submarine::addToWorld(btDynamicsWorld *world)
{
    if (m_body || m_shape) {
        qFatal("Already added to the world.");
    }

    double radius = (m_width + m_height) / 2.f;
    m_shape = new btCapsuleShapeX(radius, m_length);

    btVector3 localInertia;
    m_shape->calculateLocalInertia(m_mass, localInertia);

    auto info = btRigidBody::btRigidBodyConstructionInfo(m_mass, 0, m_shape,
                                                         localInertia);

    auto body = new btRigidBody(info);

    body->setSleepingThresholds(0, 0);

    auto motionState = new btDefaultMotionState();
    body->setMotionState(motionState);

    world->addRigidBody(body);

    m_body = new Physics::Body(body, this);

    m_propellorTorque->setBody(m_body);
    m_weight->setBody(m_body);
    m_buoyancy->setBody(m_body);
    m_thrust->setBody(m_body);
    m_drag->setBody(m_body);
    m_lift->setBody(m_body);
    m_spinningDrag->setBody(m_body);
}

void Submarine::removeFromWorld(btDynamicsWorld *world)
{
    if (!m_body && !m_shape) {
        qFatal("Not added to the world.");
    }

    world->removeRigidBody(m_body->body());

    delete m_body;
    delete m_shape;

    m_body = 0;
    m_shape = 0;
}

void Submarine::addToScene(Qt3D::QEntity *scene)
{
    if (m_entity) {
        qFatal("Already added to the scene.");
    }

    m_entity = new Qt3D::QEntity(scene);

    auto material = new Qt3D::QPhongMaterial(scene);
    material->setAmbient(QColor(50, 50, 50));

    makeBodyEntity(material);
    makePropellorEntity(material);
    makeFinsEntities(scene, material);

    Qt3D::QTransform *transform = new Qt3D::QTransform(m_entity);

    m_translateTransform = new Qt3D::QTranslateTransform(m_entity);
    m_translateTransform->setTranslation(QVector3D(0, 0, 0));

    m_rotateTransform = new Qt3D::QRotateTransform(m_entity);
    m_rotateTransform->setAxis(QVector3D(0, 1, 0));
    m_rotateTransform->setAngleDeg(0);

    transform->addTransform(m_rotateTransform);
    transform->addTransform(m_translateTransform);

    m_entity->addComponent(transform);

    makeForceArrows(scene);
}

void Submarine::makeBodyEntity(Qt3D::QPhongMaterial *material)
{
    auto bodyEntity = new Qt3D::QEntity(m_entity);

    auto mesh = new Qt3D::QSphereMesh(bodyEntity);
    mesh->setRadius(0.5);
    mesh->setRings(24);
    mesh->setSlices(48);
    bodyEntity->addComponent(mesh);

    bodyEntity->addComponent(material);

    Qt3D::QTransform *bodyTransform = new Qt3D::QTransform(bodyEntity);

    Qt3D::QScaleTransform *bodyScaleTransform = new Qt3D::QScaleTransform(bodyEntity);
    bodyScaleTransform->setScale3D(QVector3D(m_length, m_height, m_width));
    bodyTransform->addTransform(bodyScaleTransform);

    bodyEntity->addComponent(bodyTransform);
}

void Submarine::makePropellorEntity(Qt3D::QPhongMaterial *material)
{
    auto propellorEntity = new Qt3D::QEntity(m_entity);

    auto mesh = new Qt3D::QMesh(propellorEntity);
    mesh->setSource(QUrl("qrc:/models/propellor.obj"));
    propellorEntity->addComponent(mesh);

    propellorEntity->addComponent(material);

    auto transform = new Qt3D::QTransform(propellorEntity);

    auto scaleTransform = new Qt3D::QScaleTransform(propellorEntity);
    scaleTransform->setScale(0.025);
    transform->addTransform(scaleTransform);

    auto rotateTransform = new Qt3D::QRotateTransform(propellorEntity);
    rotateTransform->setAxis(QVector3D(0, 1, 0));
    rotateTransform->setAngleDeg(-90);
    transform->addTransform(rotateTransform);

    auto rotateTransform2 = new Qt3D::QRotateTransform(propellorEntity);
    rotateTransform2->setAxis(QVector3D(1, 0, 0));
    transform->addTransform(rotateTransform2);

    auto translateTransform = new Qt3D::QTranslateTransform(propellorEntity);
    translateTransform->setDx(-m_length / 2);
    transform->addTransform(translateTransform);

    auto rotationAnimation = new QPropertyAnimation(propellorEntity);
    rotationAnimation->setTargetObject(rotateTransform2);
    rotationAnimation->setPropertyName("angle");
    rotationAnimation->setStartValue(QVariant::fromValue(0));
    rotationAnimation->setEndValue(QVariant::fromValue(360));
    rotationAnimation->setDuration(500);
    rotationAnimation->setLoopCount(-1);
    rotationAnimation->start();

    propellorEntity->addComponent(transform);
}

void Submarine::makeFinsEntities(Qt3D::QEntity *scene, Qt3D::QPhongMaterial *material)
{
    if (m_hasHorizontalFins) {
        auto hEntity1 = new Fin(scene, m_entity);
        hEntity1->setSubmarine(this);
        hEntity1->setArea(m_horizontalFinsArea);
        hEntity1->calculatePosition(Fin::North, m_horizontalFinsPosition);
        hEntity1->addComponent(material);
        hEntity1->drag()->setCoefficient(m_horizontalFinsDragCoefficient);
        hEntity1->lift()->setCoefficientSlope(m_horizontalFinsLiftCoefficientSlope);
        m_fins.append(hEntity1);

        auto hEntity2 = new Fin(scene, m_entity);
        hEntity2->setSubmarine(this);
        hEntity2->setArea(m_horizontalFinsArea);
        hEntity2->calculatePosition(Fin::South, m_horizontalFinsPosition);
        hEntity2->addComponent(material);
        hEntity2->drag()->setCoefficient(m_horizontalFinsDragCoefficient);
        hEntity2->lift()->setCoefficientSlope(m_horizontalFinsLiftCoefficientSlope);
        m_fins.append(hEntity2);
    }

    if (m_hasVerticalFins) {
        auto vEntity1 = new Fin(scene, m_entity);
        vEntity1->setSubmarine(this);
        vEntity1->setArea(m_verticalFinsArea);
        vEntity1->calculatePosition(Fin::East, m_verticalFinsPosition);
        vEntity1->addComponent(material);
        vEntity1->drag()->setCoefficient(m_verticalFinsDragCoefficient);
        vEntity1->lift()->setCoefficientSlope(m_verticalFinsLiftCoefficientSlope);
        m_fins.append(vEntity1);

        auto vEntity2 = new Fin(scene, m_entity);
        vEntity2->setSubmarine(this);
        vEntity2->setArea(m_verticalFinsArea);
        vEntity2->calculatePosition(Fin::West, m_verticalFinsPosition);
        vEntity2->addComponent(material);
        vEntity2->drag()->setCoefficient(m_verticalFinsDragCoefficient);
        vEntity2->lift()->setCoefficientSlope(m_verticalFinsLiftCoefficientSlope);
        m_fins.append(vEntity2);
    }
}

void Submarine::makeForceArrows(Qt3D::QEntity *scene)
{
    auto weightArrow = new ForceArrow(QColor(0xe67e22), 0.3, scene);
    weightArrow->setForce(m_weight);

    auto buoyancyArrow = new ForceArrow(QColor(0x9b59b6), 0.3, scene);
    buoyancyArrow->setForce(m_buoyancy);

    auto thrustArrow = new ForceArrow(QColor(0x2c3e50), 5., scene);
    thrustArrow->setForce(m_thrust);

    auto dragArrow = new ForceArrow(QColor(0x2ecc71), 5., scene);
    dragArrow->setForce(m_drag);

    auto liftArrow = new ForceArrow(QColor(0xe74c3c), 3.5, scene);
    liftArrow->setForce(m_lift);

    auto propellorTorqueArrow = new TorqueArrow(Qt::red, 0.25, scene);
    propellorTorqueArrow->setTorque(m_propellorTorque);

    auto spinningDragArrow = new TorqueArrow(Qt::blue, 1., scene);
    spinningDragArrow->setTorque(m_spinningDrag);
}

void Submarine::update(const Fluid *fluid, Qt3D::QCamera *camera)
{
    updateTransformation();
    updateCamera(camera);
    updateForces(fluid);
}

void Submarine::updateTransformation()
{
    btMotionState *motionState = m_body->body()->getMotionState();

    btTransform transform;
    motionState->getWorldTransform(transform);

    btVector3 t = transform.getOrigin();
    QVector3D position(t.x(), t.y(), t.z());
    m_translateTransform->setTranslation(position);

    btQuaternion q = transform.getRotation();
    btVector3 a = q.getAxis();
    m_rotateTransform->setAxis(QVector3D(a.x(), a.y(), a.z()));
    m_rotateTransform->setAngleRad(q.getAngle());
}

void Submarine::updateCamera(Qt3D::QCamera *camera)
{
    QVector3D position = m_translateTransform->translation();
    camera->lookAt()->setPosition(position + QVector3D(2, 4, 10));
    camera->lookAt()->setViewCenter(position);
}

void Submarine::updateForces(const Fluid *fluid)
{
    applyPropellorTorque();
    applyWeight();
    applyBuoyancy();
    applyThrust();
    applyDrag(fluid);
    applyLift(fluid);
    applySpinningDrag(fluid);

    for (Fin *fin : m_fins) {
        fin->applyForces(fluid);
    }
}

void Submarine::applyPropellorTorque()
{
    m_propellorTorque->apply();
}

void Submarine::applyWeight()
{
    m_weight->apply();
}

void Submarine::applyBuoyancy()
{
    m_buoyancy->apply();
}

void Submarine::applyThrust()
{
    m_thrust->apply();
}

void Submarine::applyDrag(const Fluid *fluid)
{
    m_drag->setCrossSectionalArea(crossSectionalArea());
    m_drag->setFluidDensity(fluid->density());

    m_drag->apply();
}

void Submarine::applyLift(const Fluid *fluid)
{
    m_lift->setPitchCrossSectionalArea(M_PI * m_width * m_length);
    m_lift->setYawCrossSectionalArea(M_PI * m_height * m_length);
    m_lift->setFluidDensity(fluid->density());

    m_lift->apply();
}

void Submarine::applySpinningDrag(const Fluid *fluid)
{
    m_spinningDrag->setFluidDensity(fluid->density());
    m_spinningDrag->setPitchCrossSectionalArea(M_PI * m_width * m_length);
    m_spinningDrag->setYawCrossSectionalArea(M_PI * m_height * m_length);

    m_spinningDrag->apply();
}

Physics::Body *Submarine::body() const
{
    return m_body;
}

double Submarine::crossSectionalArea() const
{
    return M_PI * m_width * m_height;
}

double Submarine::length() const
{
    return m_length;
}

void Submarine::setLength(double length)
{
    m_length = length;

    m_lift->setPosition(QVector3D(length / 4., 0, 0));
    m_thrust->setPosition(QVector3D(-length / 2., 0, 0));
    m_spinningDrag->setBodyLength(length);
}

double Submarine::width() const
{
    return m_width;
}

void Submarine::setWidth(double width)
{
    m_width = width;
}

double Submarine::height() const
{
    return m_height;
}

void Submarine::setHeight(double height)
{
    m_height = height;
}

double Submarine::mass() const
{
    return m_mass;
}

void Submarine::setMass(double mass)
{
    m_mass = mass;
}

double Submarine::hasHorizontalFins() const
{
    return m_hasHorizontalFins;
}

void Submarine::setHasHorizontalFins(double hasHorizontalFins)
{
    m_hasHorizontalFins = hasHorizontalFins;
}

double Submarine::horizontalFinsArea() const
{
    return m_horizontalFinsArea;
}

void Submarine::setHorizontalFinsArea(double horizontalFinsArea)
{
    m_horizontalFinsArea = horizontalFinsArea;
}

double Submarine::horizontalFinsLiftCoefficientSlope() const
{
    return m_horizontalFinsLiftCoefficientSlope;
}

void Submarine::setHorizontalFinsLiftCoefficientSlope(double horizontalFinsLiftCoefficientSlope)
{
    m_horizontalFinsLiftCoefficientSlope = horizontalFinsLiftCoefficientSlope;
}

double Submarine::horizontalFinsDragCoefficient() const
{
    return m_horizontalFinsDragCoefficient;
}

void Submarine::setHorizontalFinsDragCoefficient(double horizontalFinsDragCoefficient)
{
    m_horizontalFinsDragCoefficient = horizontalFinsDragCoefficient;
}

double Submarine::horizontalFinsPosition() const
{
    return m_horizontalFinsPosition;
}

void Submarine::setHorizontalFinsPosition(double horizontalFinsPosition)
{
    m_horizontalFinsPosition = horizontalFinsPosition;
}

double Submarine::horizontalFinsAspectRatio() const
{
    return m_horizontalFinsAspectRatio;
}

void Submarine::setHorizontalFinsAspectRatio(double horizontalFinsAspectRatio)
{
    m_horizontalFinsAspectRatio = horizontalFinsAspectRatio;
}

double Submarine::hasVerticalFins() const
{
    return m_hasVerticalFins;
}

void Submarine::setHasVerticalFins(double hasVerticalFins)
{
    m_hasVerticalFins = hasVerticalFins;
}

double Submarine::verticalFinsArea() const
{
    return m_verticalFinsArea;
}

void Submarine::setVerticalFinsArea(double verticalFinsArea)
{
    m_verticalFinsArea = verticalFinsArea;
}

double Submarine::verticalFinsLiftCoefficientSlope() const
{
    return m_verticalFinsLiftCoefficientSlope;
}

void Submarine::setVerticalFinsLiftCoefficientSlope(double verticalFinsLiftCoefficientSlope)
{
    m_verticalFinsLiftCoefficientSlope = verticalFinsLiftCoefficientSlope;
}

double Submarine::verticalFinsDragCoefficient() const
{
    return m_verticalFinsDragCoefficient;
}

void Submarine::setVerticalFinsDragCoefficient(double verticalFinsDragCoefficient)
{
    m_verticalFinsDragCoefficient = verticalFinsDragCoefficient;
}

double Submarine::verticalFinsPosition() const
{
    return m_verticalFinsPosition;
}

void Submarine::setVerticalFinsPosition(double verticalFinsPosition)
{
    m_verticalFinsPosition = verticalFinsPosition;
}

double Submarine::verticalFinsAspectRatio() const
{
    return m_verticalFinsAspectRatio;
}

void Submarine::setVerticalFinsAspectRatio(double verticalFinsAspectRatio)
{
    m_verticalFinsAspectRatio = verticalFinsAspectRatio;
}

Physics::PropellorTorque *Submarine::propellorTorque() const
{
    return m_propellorTorque;
}

Physics::WeightForce *Submarine::weight() const
{
    return m_weight;
}

Physics::BuoyancyForce *Submarine::buoyancy() const
{
    return m_buoyancy;
}

Physics::ThrustForce *Submarine::thrust() const
{
    return m_thrust;
}

Physics::DragForce *Submarine::drag() const
{
    return m_drag;
}

Physics::LiftForce *Submarine::lift() const
{
    return m_lift;
}

Physics::SpinningDragTorque *Submarine::spinningDrag() const
{
    return m_spinningDrag;
}
