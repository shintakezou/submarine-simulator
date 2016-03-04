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

#include "submarine.h"

btVector3 qtVector2btVector(QVector3D v)
{
    return btVector3(v.x(), v.y(), v.z());
}

float wrapAngle(float angle)
{
    while (angle > M_PI) {
        angle -= M_PI * 2;
    }

    while (angle < -M_PI) {
        angle += M_PI * 2;
    }

    return angle;
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
    m_dragCoefficient(0),
    m_liftCoefficientSlope(0),
    m_spinningDragCoefficient(0),
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
    m_verticalFinsAspectRatio(0)
{

}

Submarine::~Submarine()
{
    if (m_body) {
        delete m_body;
    }

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
    submarine->setDragCoefficient(0.04);
    submarine->setLiftCoefficientSlope(M_PI / 2.);
    submarine->setSpinningDragCoefficient(2);
    submarine->setBuoyancyPosition(QVector3D(0, 0.15, 0));
    submarine->setWeightPosition(QVector3D());
    submarine->setThrust(QVector3D(100, 0, 10));
    submarine->setPropellorTorque(20);

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

    m_body = new btRigidBody(info);

    m_body->setSleepingThresholds(0, 0);

    auto motionState = new btDefaultMotionState();
    m_body->setMotionState(motionState);

    world->addRigidBody(m_body);
}

void Submarine::removeFromWorld(btDynamicsWorld *world)
{
    if (!m_body && !m_shape) {
        qFatal("Not added to the world.");
    }

    world->removeRigidBody(m_body);

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
    makeFinsEntities(material);

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

void Submarine::makeFinsEntities(Qt3D::QPhongMaterial *material)
{
    auto scaleTransform = new Qt3D::QScaleTransform(m_entity);
    scaleTransform->setScale(0.7);

    if (m_hasHorizontalFins) {
        auto hTranslateTransform = new Qt3D::QTranslateTransform(m_entity);
        hTranslateTransform->setDx(m_horizontalFinsPosition);

        auto hEntity1 = new Fin(m_entity);

        hEntity1->addComponent(material);

        Qt3D::QTransform *hTransform1 = new Qt3D::QTransform(m_entity);
        hTransform1->addTransform(scaleTransform);
        hTransform1->addTransform(hTranslateTransform);

        hEntity1->addComponent(hTransform1);

        auto hEntity2 = new Fin(m_entity);

        hEntity2->addComponent(material);

        Qt3D::QTransform *hTransform2 = new Qt3D::QTransform(m_entity);
        hTransform2->addTransform(scaleTransform);

        auto hRotateTransform2 = new Qt3D::QRotateTransform(m_entity);
        hRotateTransform2->setAxis(QVector3D(1, 0, 0));
        hRotateTransform2->setAngleDeg(180);
        hTransform2->addTransform(hRotateTransform2);

        hTransform2->addTransform(hTranslateTransform);

        hEntity2->addComponent(hTransform2);
    }

    if (m_hasVerticalFins) {
        auto vTranslateTransform = new Qt3D::QTranslateTransform(m_entity);
        vTranslateTransform->setDx(m_verticalFinsPosition);

        auto vEntity1 = new Fin(m_entity);

        vEntity1->addComponent(material);

        Qt3D::QTransform *vTransform1 = new Qt3D::QTransform(m_entity);
        vTransform1->addTransform(scaleTransform);

        auto vRotateTransform1 = new Qt3D::QRotateTransform(m_entity);
        vRotateTransform1->setAxis(QVector3D(1, 0, 0));
        vRotateTransform1->setAngleDeg(90);
        vTransform1->addTransform(vRotateTransform1);

        vTransform1->addTransform(vTranslateTransform);

        vEntity1->addComponent(vTransform1);

        auto vEntity2 = new Fin(m_entity);

        vEntity2->addComponent(material);

        Qt3D::QTransform *vTransform2 = new Qt3D::QTransform(m_entity);
        vTransform2->addTransform(scaleTransform);

        auto vRotateTransform2 = new Qt3D::QRotateTransform(m_entity);
        vRotateTransform2->setAxis(QVector3D(1, 0, 0));
        vRotateTransform2->setAngleDeg(270);
        vTransform2->addTransform(vRotateTransform2);

        vTransform2->addTransform(vTranslateTransform);

        vEntity2->addComponent(vTransform2);
    }
}

void Submarine::makeForceArrows(Qt3D::QEntity *scene)
{
    m_forceNoise = new ForceArrow(Qt::red, 1.f);
    m_forceNoise->addToScene(scene);

    m_forceWeight = new ForceArrow(Qt::green, 0.5f);
    m_forceWeight->addToScene(scene);

    m_forceBuoyancy = new ForceArrow(Qt::blue, 0.5f);
    m_forceBuoyancy->addToScene(scene);

    m_forceThrust = new ForceArrow(Qt::black, 5.f);
    m_forceThrust->addToScene(scene);

    m_forceDrag = new ForceArrow(Qt::white, 5.f);
    m_forceDrag->addToScene(scene);

    m_forcePitchLift = new ForceArrow(Qt::magenta, 1.f);
    m_forcePitchLift->addToScene(scene);

    m_forceYawLift = new ForceArrow(Qt::yellow, 1.f);
    m_forceYawLift->addToScene(scene);

    m_forceHorizontalFinsLift = new ForceArrow(Qt::darkCyan, 1.f);
    m_forceHorizontalFinsLift->addToScene(scene);

    m_forceVerticalFinsLift = new ForceArrow(Qt::darkCyan, 1.f);
    m_forceVerticalFinsLift->addToScene(scene);

    m_forceHorizontalFinsDrag = new ForceArrow(Qt::cyan, 100.f);
    m_forceHorizontalFinsDrag->addToScene(scene);

    m_forceVerticalFinsDrag = new ForceArrow(Qt::cyan, 100.f);
    m_forceVerticalFinsDrag->addToScene(scene);
}

void Submarine::update(const Fluid *fluid, Qt3D::QCamera *camera)
{
    updateTransformation();
    updateCamera(camera);
    updateForces(fluid);

    qDebug() << roll();
}

void Submarine::updateTransformation()
{
    btMotionState *motionState = m_body->getMotionState();

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
    applyFinsLift(fluid);
    applyFinsDrag(fluid);
    applyFinsDamping(fluid);
}

void Submarine::applyPropellorTorque()
{
    m_body->applyTorque(btVector3(m_propellorTorque, 0, 0));
}

void Submarine::applyWeight()
{
    btVector3 force(0, -9.81 * m_mass, 0);

    btVector3 position = m_body->getCenterOfMassTransform() * qtVector2btVector(m_weightPosition);

    m_body->applyForce(force, position);
    m_forceWeight->update(force, position);
}

void Submarine::applyBuoyancy()
{
    btVector3 force(0, 9.81 * m_mass, 0);

    btVector3 position = m_body->getCenterOfMassTransform() * qtVector2btVector(m_buoyancyPosition);

    m_body->applyForce(force, position);
    m_forceBuoyancy->update(force, position);
}

void Submarine::applyThrust()
{
    btTransform transform = m_body->getCenterOfMassTransform();

    btVector3 force(m_thrust.x(), m_thrust.y(), m_thrust.z());
    force = (transform * force) - transform.getOrigin();

    btVector3 position(-m_length / 2., 0, 0);
    position = (transform * position) - transform.getOrigin();

    m_body->applyForce(force, position);

    position += transform.getOrigin();
    m_forceThrust->update(force, position);
}

void Submarine::applyDrag(const Fluid *fluid)
{
    btVector3 velocity = m_body->getLinearVelocity();
    if (velocity.length() == 0) {
        return;  // can't be normalised
    }

    float value = 0.5f * fluid->density() * crossSectionalArea() * m_dragCoefficient * velocity.length2();
    btVector3 force = velocity.normalized() * -value;

    m_body->applyCentralForce(force);

    btVector3 position = m_body->getCenterOfMassPosition();
    m_forceDrag->update(force, position);
}

QVector2D calculateLift(float fluidDensity, float angleOfAttack, float liftCoefficientSlope, float area, QVector2D velocity)
{
    float liftCoefficient = angleOfAttack * liftCoefficientSlope;

    float value = 0.5f * fluidDensity * area * liftCoefficient * velocity.lengthSquared();

    QVector2D result = velocity.normalized();

    // rotate90(1)
    float x = result.x();
    result.setX(-result.y());
    result.setY(x);

    return result * value;
}

void Submarine::applyPitchLift(const Fluid *fluid)
{
    btTransform transform = m_body->getCenterOfMassTransform();

    QVector2D velocity = pitchVelocity();
    float angleOfAttack = pitchAngleOfAttack();

    if (qAbs(angleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D lift = calculateLift(fluid->density(), angleOfAttack, m_liftCoefficientSlope, crossSectionalArea(), velocity);

        btVector3 position = qtVector2btVector(m_liftPosition);
        position = (transform * position) - transform.getOrigin();

        btVector3 force(lift.x(), lift.y(), 0);

        m_body->applyForce(force, position);

        position += transform.getOrigin();
        m_forcePitchLift->update(force, position);
    }
}

void Submarine::applyYawLift(const Fluid *fluid)
{
    btTransform transform = m_body->getCenterOfMassTransform();

    QVector2D velocity = yawVelocity();
    float angleOfAttack = yawAngleOfAttack();

    if (qAbs(angleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D lift = calculateLift(fluid->density(), angleOfAttack, m_liftCoefficientSlope, crossSectionalArea(), velocity);

        btVector3 position = qtVector2btVector(m_liftPosition);
        position = (transform * position) - transform.getOrigin();

        btVector3 force(lift.x(), 0, lift.y());

        m_body->applyForce(force, position);

        position += transform.getOrigin();
        m_forceYawLift->update(force, position);
    }
}

void Submarine::applyLift(const Fluid *fluid)
{
    applyPitchLift(fluid);
    applyYawLift(fluid);
}

float calculateSpinningDrag(float fluidDensity, float area, float spinningDragCoefficient, float angularVelocity, float length)
{
    float v2 = angularVelocity * angularVelocity;
    float value = (0.5f * fluidDensity * area * spinningDragCoefficient * v2) / length;

    float drag = -value;
    if (angularVelocity < 0) {
        drag = value;
    }

    return drag;
}

void Submarine::applyPitchSpinningDrag(const Fluid *fluid)
{
    float angularVelocity = m_body->getAngularVelocity().z();
    float drag = calculateSpinningDrag(fluid->density(), crossSectionalArea(), m_spinningDragCoefficient, angularVelocity, m_length);
    m_body->applyTorque(btVector3(0, 0, drag));
}

void Submarine::applyYawSpinningDrag(const Fluid *fluid)
{
    float angularVelocity = m_body->getAngularVelocity().y();
    float drag = calculateSpinningDrag(fluid->density(), crossSectionalArea(), m_spinningDragCoefficient, angularVelocity, m_length);
    m_body->applyTorque(btVector3(0, 0, drag));
}

void Submarine::applySpinningDrag(const Fluid *fluid)
{
    applyPitchSpinningDrag(fluid);
    applyYawSpinningDrag(fluid);
}

float getEllipseProportion(float proportion)
{
    return qSqrt(1.f - (proportion / 0.5f) * (proportion / 0.5f));
}

void Submarine::applyHorizontalFinsLift(const Fluid *fluid)
{
    btTransform transform = m_body->getCenterOfMassTransform();

    QVector2D velocity = pitchVelocity();
    float angleOfAttack = pitchAngleOfAttack();

    if (qAbs(angleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D lift = calculateLift(fluid->density(), angleOfAttack, m_horizontalFinsLiftCoefficientSlope, m_horizontalFinsArea, velocity);

        btVector3 position = btVector3(m_horizontalFinsPosition, 0, 0);
        position = (transform * position) - transform.getOrigin();

        btVector3 force(lift.x(), lift.y(), 0);

        m_body->applyForce(force, position);

        position += transform.getOrigin();
        m_forceHorizontalFinsLift->update(force, position);
    }
}

void Submarine::applyVerticalFinsLift(const Fluid *fluid)
{
    btTransform transform = m_body->getCenterOfMassTransform();

    QVector2D velocity = yawVelocity();
    float angleOfAttack = yawAngleOfAttack();

    if (qAbs(angleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D lift = calculateLift(fluid->density(), angleOfAttack, m_verticalFinsLiftCoefficientSlope, m_verticalFinsArea, velocity);

        btVector3 position = btVector3(m_verticalFinsPosition, 0, 0);
        position = (transform * position) - transform.getOrigin();

        btVector3 force(lift.x(), lift.y(), 0);

        m_body->applyForce(force, position);

        position += transform.getOrigin();
        m_forceVerticalFinsLift->update(force, position);
    }
}

void Submarine::applyFinsLift(const Fluid *fluid)
{
    if (m_hasHorizontalFins) {
        applyHorizontalFinsLift(fluid);
    }

    if (m_hasVerticalFins) {
        applyVerticalFinsLift(fluid);
    }
}

void Submarine::applyHorizontalFinsDrag(const Fluid *fluid)
{
    btTransform transform = m_body->getCenterOfMassTransform();

    QVector2D velocity = pitchVelocity();

    float v2 = velocity.lengthSquared();
    float value = 0.5f * fluid->density() * m_horizontalFinsArea * m_horizontalFinsDragCoefficient * v2;

    QVector2D drag = velocity.normalized() * -value;

    btVector3 position = btVector3(m_horizontalFinsPosition, 0, 0);
    position = (transform * position) - transform.getOrigin();

    btVector3 force(drag.x(), drag.y(), 0);

    m_body->applyForce(force, position);

    position += transform.getOrigin();
    m_forceHorizontalFinsDrag->update(force, position);
}

void Submarine::applyVerticalFinsDrag(const Fluid *fluid)
{
    btTransform transform = m_body->getCenterOfMassTransform();

    QVector2D velocity = yawVelocity();

    float v2 = velocity.lengthSquared();
    float value = 0.5f * fluid->density() * m_verticalFinsArea * m_verticalFinsDragCoefficient * v2;

    QVector2D drag = velocity.normalized() * -value;

    btVector3 position = btVector3(m_verticalFinsPosition, 0, 0);
    position = (transform * position) - transform.getOrigin();

    btVector3 force(drag.x(), 0, drag.y());

    m_body->applyForce(force, position);

    position += transform.getOrigin();
    m_forceVerticalFinsDrag->update(force, position);
}

void Submarine::applyFinsDrag(const Fluid *fluid)
{
    if (m_hasHorizontalFins) {
        applyHorizontalFinsDrag(fluid);
    }

    if (m_hasVerticalFins) {
        applyVerticalFinsDrag(fluid);
    }
}

void Submarine::applyHorizontalFinsDamping(const Fluid *fluid)
{
    float span = qSqrt(m_horizontalFinsAspectRatio * m_horizontalFinsArea);

    float p = qAbs(m_horizontalFinsArea) / (m_length / 2.);
    float radius = getEllipseProportion(p) * m_width / 2.f;
    if (m_horizontalFinsArea < 0) {
        radius -= radius;
    }

    float v2 = m_body->getAngularVelocity().x() * m_body->getAngularVelocity().x();
    float torque = -2.f * fluid->density() * m_horizontalFinsArea * v2 * (radius + span) * (radius + span) * (radius + span / 2.f);

    m_body->applyTorque(btVector3(torque, 0, 0));
}

void Submarine::applyVerticalFinsDamping(const Fluid *fluid)
{
    float span = qSqrt(m_verticalFinsAspectRatio * m_verticalFinsArea);

    float p = qAbs(m_verticalFinsArea) / (m_length / 2.);
    float radius = getEllipseProportion(p) * m_width / 2.f;
    if (m_horizontalFinsArea < 0) {
        radius -= radius;
    }

    float v2 = m_body->getAngularVelocity().x() * m_body->getAngularVelocity().x();
    float torque = -2.f * fluid->density() * m_verticalFinsArea * v2 * (radius + span) * (radius + span) * (radius + span / 2.f);

    m_body->applyTorque(btVector3(torque, 0, 0));
}

void Submarine::applyFinsDamping(const Fluid *fluid)
{
    if (m_hasHorizontalFins) {
        applyHorizontalFinsDamping(fluid);
    }

    if (m_hasVerticalFins) {
        applyVerticalFinsDamping(fluid);
    }
}

double Submarine::crossSectionalArea() const
{
    return M_PI * m_width * m_height;
}

double Submarine::pitch() const
{
    //return m_body->getOrientation().z();

    btTransform transform;
    m_body->getMotionState()->getWorldTransform(transform);

    btMatrix3x3 basis = transform.getBasis();

    float yaw, pitch, roll;
    // yes, those do appear to be in the wrong order, but it is correct
    basis.getEulerYPR(pitch, yaw, roll);

    return wrapAngle(pitch);
}

double Submarine::yaw() const
{
    //return m_body->getOrientation().y();

    btTransform transform;
    m_body->getMotionState()->getWorldTransform(transform);

    btMatrix3x3 basis = transform.getBasis();

    float yaw, pitch, roll;
    // yes, those do appear to be in the wrong order, but it is correct
    basis.getEulerYPR(pitch, yaw, roll);

    return wrapAngle(yaw);
}

double Submarine::roll() const
{
    //return m_body->getOrientation().x();

    btTransform transform;
    m_body->getMotionState()->getWorldTransform(transform);

    btMatrix3x3 basis = transform.getBasis();

    float yaw, pitch, roll;
    // yes, those do appear to be in the wrong order, but it is correct
    basis.getEulerYPR(pitch, yaw, roll);

    return wrapAngle(roll);
}

QVector2D Submarine::pitchVelocity() const
{
    btVector3 velocity = m_body->getLinearVelocity();
    return QVector2D(velocity.x(), velocity.y());
}

QVector2D Submarine::yawVelocity() const
{
    btVector3 velocity = m_body->getLinearVelocity();
    return QVector2D(velocity.x(), velocity.z());
}

double Submarine::pitchAngleOfAttack() const
{
    QVector2D velocity = pitchVelocity();
    float velocityAngle = wrapAngle(atan2(velocity.y(), velocity.x()));
    return wrapAngle(pitch() - velocityAngle);
}

double Submarine::yawAngleOfAttack() const
{
    QVector2D velocity = yawVelocity();
    float velocityAngle = wrapAngle(atan2(velocity.y(), velocity.x()));
    return wrapAngle(yaw() - velocityAngle);
}

QVector3D Submarine::angularVelocity() const
{
    btVector3 v = m_body->getAngularVelocity();
    return QVector3D(v.x(), v.y(), v.z());
}

QVector3D Submarine::linearVelocity() const
{
    btVector3 v = m_body->getLinearVelocity();
    return QVector3D(v.x(), v.y(), v.z());
}

QVector3D Submarine::position() const
{
    btVector3 p = m_body->getCenterOfMassPosition();
    return QVector3D(p.x(), p.y(), p.z());
}

double Submarine::length() const
{
    return m_length;
}

void Submarine::setLength(double length)
{
    m_length = length;
    m_liftPosition = QVector3D(length / 4., 0, 0);
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

double Submarine::dragCoefficient() const
{
    return m_dragCoefficient;
}

void Submarine::setDragCoefficient(double dragCoefficient)
{
    m_dragCoefficient = dragCoefficient;
}

double Submarine::liftCoefficientSlope() const
{
    return m_liftCoefficientSlope;
}

void Submarine::setLiftCoefficientSlope(double liftCoefficientSlope)
{
    m_liftCoefficientSlope = liftCoefficientSlope;
}

double Submarine::spinningDragCoefficient() const
{
    return m_spinningDragCoefficient;
}

void Submarine::setSpinningDragCoefficient(double spinningDragCoefficient)
{
    m_spinningDragCoefficient = spinningDragCoefficient;
}

QVector3D Submarine::buoyancyPosition() const
{
    return m_buoyancyPosition;
}

void Submarine::setBuoyancyPosition(const QVector3D &buoyancyPosition)
{
    m_buoyancyPosition = buoyancyPosition;
}

QVector3D Submarine::weightPosition() const
{
    return m_weightPosition;
}

void Submarine::setWeightPosition(const QVector3D &weightPosition)
{
    m_weightPosition = weightPosition;
}

QVector3D Submarine::thrust() const
{
    return m_thrust;
}

void Submarine::setThrust(const QVector3D &thrust)
{
    m_thrust = thrust;
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

double Submarine::propellorTorque() const
{
    return m_propellorTorque;
}

void Submarine::setPropellorTorque(double propellorTorque)
{
    m_propellorTorque = propellorTorque;
}
