#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QMesh>
#include <QtMath>
#include <QVector2D>

#include <bullet/btBulletDynamicsCommon.h>

#include "physics/body.h"

#include "fluid.h"
#include "forcearrow.h"
#include "submarine.h"

#include "fin.h"

float calculateEllipseProportion(float proportion)
{
    return qSqrt(1.f - (proportion * proportion));
}

Fin::Fin(Qt3D::QEntity *scene, Qt3D::QNode *parent) :
    Qt3D::QEntity(parent),
    m_plane(Unknown),
    m_forcePosition(new btVector3())
{
    auto mesh = new Qt3D::QMesh(this);
    mesh->setSource(QUrl("qrc:/models/fin.obj"));
    addComponent(mesh);

    m_rotateTransform = new Qt3D::QRotateTransform(this);
    m_rotateTransform->setAxis(QVector3D(1, 0, 0));

    m_translateTransform = new Qt3D::QTranslateTransform(this);

    auto transform = new Qt3D::QTransform(this);
    transform->addTransform(m_translateTransform);
    transform->addTransform(m_rotateTransform);
    addComponent(transform);

    m_forceLift = new ForceArrow(Qt::red, 50., scene);
    m_forceLift->addToScene(scene);

    m_forceDrag = new ForceArrow(Qt::green, 50., scene);
    m_forceDrag->addToScene(scene);
}

void Fin::calculatePosition(Orientation orientation, float position)
{
    m_translateTransform->setDx(position);

    float p = qAbs(position) / (submarine()->length() / 2.);
    float radius = calculateEllipseProportion(p) * (submarine()->width() / 2.f);

    m_translateTransform->setDy(radius);

    m_forcePosition->setX(position);

    switch (orientation) {
    case North:
        m_forcePosition->setY(radius);
        break;

    case East:
        m_forcePosition->setZ(radius);
        break;

    case South:
        m_forcePosition->setY(-radius);
        break;

    case West:
        m_forcePosition->setZ(-radius);
        break;
    }

    // FIXME this is required :/
    qDebug() << "p:" << position;

    // TODO rotate fin

    switch (orientation) {
    case North:
        m_rotateTransform->setAngleDeg(0);
        break;

    case East:
        m_rotateTransform->setAngleDeg(90);
        break;

    case South:
        m_rotateTransform->setAngleDeg(180);
        break;

    case West:
        m_rotateTransform->setAngleDeg(270);
        break;
    }

    switch (orientation) {
    case North:
    case South:
        m_plane = Horizontal;
        break;

    case East:
    case West:
        m_plane = Vertical;
        break;
    }
}

void Fin::applyForces(const Fluid *fluid) const
{
    applyLift(fluid);
    applyDrag(fluid);
    applyDamping(fluid);
}

QVector2D calcLift(float fluidDensity, float angleOfAttack, float liftCoefficientSlope, float area, QVector2D velocity)
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

void Fin::applyLift(const Fluid *fluid) const
{
    btTransform transform = submarine()->body()->body()->getCenterOfMassTransform();

    QVector2D velocity;
    float angleOfAttack;

    switch (m_plane) {
    case Horizontal:
        velocity = submarine()->pitchVelocity();
        angleOfAttack = submarine()->pitchAngleOfAttack();
        break;

    case Vertical:
        velocity = submarine()->yawVelocity();
        angleOfAttack = submarine()->yawAngleOfAttack();
        break;

    default:
        qFatal("Unknown fin plane.");
        return;
    }

    if (qAbs(angleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D lift = calcLift(fluid->density(), angleOfAttack, m_liftCoefficientSlope, m_area, velocity);

        btVector3 position = btVector3(m_forcePosition->x(), m_forcePosition->y(), m_forcePosition->z());
        position = (transform * position) - transform.getOrigin();

        btVector3 force;

        switch (m_plane) {
        case Horizontal:
            force = btVector3(lift.x(), lift.y(), 0);
            break;

        case Vertical:
            force = btVector3(lift.x(), 0, lift.y());
            break;

        default:
            qFatal("Unknown fin plane.");
            return;
        }

        submarine()->body()->body()->applyForce(force, position);

        position += transform.getOrigin();
        m_forceLift->update(force, position);
    }
}

void Fin::applyDrag(const Fluid *fluid) const
{
    btTransform transform = submarine()->body()->body()->getCenterOfMassTransform();

    QVector2D velocity;

    switch (m_plane) {
    case Horizontal:
        velocity = submarine()->pitchVelocity();
        break;

    case Vertical:
        velocity = submarine()->yawVelocity();
        break;

    default:
        qFatal("Unknown fin plane.");
        return;
    }

    float v2 = velocity.lengthSquared();
    float value = 0.5f * fluid->density() * m_area * m_dragCoefficient * v2;

    QVector2D drag = velocity.normalized() * -value;

    btVector3 position = btVector3(m_forcePosition->x(), m_forcePosition->y(), m_forcePosition->z());
    position = (transform * position) - transform.getOrigin();

    btVector3 force;

    switch (m_plane) {
    case Horizontal:
        force = btVector3(drag.x(), drag.y(), 0);
        break;

    case Vertical:
        force = btVector3(drag.x(), 0, drag.y());
        break;

    default:
        qFatal("Unknown fin plane.");
        return;
    }

    submarine()->body()->body()->applyForce(force, position);

    position += transform.getOrigin();
    m_forceDrag->update(force, position);
}

void Fin::applyDamping(const Fluid *fluid) const
{
    float span = qSqrt(m_aspectRatio * m_area);

    float radius = 0;
    switch (m_plane) {
    case Horizontal:
        radius = m_forcePosition->z();
        break;

    case Vertical:
        radius = m_forcePosition->y();
        break;

    default:
        qFatal("Unknown fin plane.");
        return;
    }

    float v2 = submarine()->body()->body()->getAngularVelocity().x() * submarine()->body()->body()->getAngularVelocity().x();
    float torque = -2.f * fluid->density() * m_area * v2 * (radius + span) * (radius + span) * (radius + span / 2.f);

    submarine()->body()->body()->applyTorque(btVector3(torque, 0, 0));
}

Fin::Plane Fin::plane() const
{
    return m_plane;
}

Submarine *Fin::submarine() const
{
    return m_submarine;
}

void Fin::setSubmarine(Submarine *submarine)
{
    m_submarine = submarine;
}

double Fin::area() const
{
    return m_area;
}

void Fin::setArea(double area)
{
    m_area = area;
}

double Fin::aspectRatio() const
{
    return m_aspectRatio;
}

void Fin::setAspectRatio(double aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

double Fin::liftCoefficientSlope() const
{
    return m_liftCoefficientSlope;
}

void Fin::setLiftCoefficientSlope(double liftCoefficientSlope)
{
    m_liftCoefficientSlope = liftCoefficientSlope;
}

double Fin::dragCoefficient() const
{
    return m_dragCoefficient;
}

void Fin::setDragCoefficient(double dragCoefficient)
{
    m_dragCoefficient = dragCoefficient;
}
