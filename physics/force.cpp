#include <QtDebug>
#include <QtMath>
#include <QVector2D>

#include <bullet/btBulletDynamicsCommon.h>

#include "physics/body.h"

#include "physics/force.h"

using namespace Physics;

Force::Force(QString name, QObject *parent) :
    QObject(parent),
    m_name(name),
    m_body(0)
{

}

void Force::apply()
{
    if (!m_body) {
        qCritical() << "Body not set on force:" << m_name;
        return;
    }

    calculate();

    btVector3 force = btVector3(m_force.x(), m_force.y(), m_force.z());
    btVector3 localPosition = btVector3(m_localPosition.x(), m_localPosition.y(), m_localPosition.z());

    m_body->body()->applyForce(force, localPosition);

    emit applied();
}

QString Force::name() const
{
    return m_name;
}

void Force::setName(const QString &name)
{
    m_name = name;
}

Physics::Body *Force::body() const
{
    return m_body;
}

void Force::setBody(Physics::Body *body)
{
    m_body = body;
}

QVector3D Force::localPosition() const
{
    return m_localPosition;
}

QVector3D Force::worldPosition() const
{
    if (!m_body) {
        qCritical() << "Body not set on force:" << m_name;
        return QVector3D();
    }

    btTransform transform = m_body->body()->getCenterOfMassTransform();

    btVector3 localPosition = btVector3(m_localPosition.x(), m_localPosition.y(), m_localPosition.z());
    btVector3 worldPosition = localPosition + transform.getOrigin();
    return QVector3D(worldPosition.x(), worldPosition.y(), worldPosition.z());
}

QVector3D Force::force() const
{
    return m_force;
}

WeightForce::WeightForce(QObject *parent) :
    Force("Weight", parent)
{

}

void WeightForce::calculate()
{
    btTransform transform = m_body->body()->getCenterOfMassTransform();
;
    m_force = QVector3D(0, -9.81 * m_body->mass(), 0);

    btVector3 position(m_position.x(), m_position.y(), m_position.z());
    btVector3 localPosition = (transform * position) - transform.getOrigin();
    m_localPosition = QVector3D(localPosition.x(), localPosition.y(), localPosition.z());
}

QVector3D WeightForce::position() const
{
    return m_position;
}

void WeightForce::setPosition(const QVector3D &position)
{
    m_position = position;
}

BuoyancyForce::BuoyancyForce(QObject *parent) :
    Force("Buoyancy", parent)
{

}

void BuoyancyForce::calculate()
{
    btTransform transform = m_body->body()->getCenterOfMassTransform();

    m_force = QVector3D(0, 9.81 * m_body->mass(), 0);

    btVector3 position(m_position.x(), m_position.y(), m_position.z());
    btVector3 localPosition = (transform * position) - transform.getOrigin();
    m_localPosition = QVector3D(localPosition.x(), localPosition.y(), localPosition.z());
}

QVector3D BuoyancyForce::position() const
{
    return m_position;
}

void BuoyancyForce::setPosition(const QVector3D &position)
{
    m_position = position;
}

ThrustForce::ThrustForce(QObject *parent) :
    Force("Thrust", parent)
{

}

void ThrustForce::calculate()
{
    btTransform transform = m_body->body()->getCenterOfMassTransform();

    btVector3 value(m_value.x(), m_value.y(), m_value.z());
    btVector3 localValue = (transform * value) - transform.getOrigin();

    btVector3 position(m_position.x(), m_position.y(), m_position.z());
    btVector3 localPosition = (transform * position) - transform.getOrigin();

    m_force = QVector3D(localValue.x(), localValue.y(), localValue.z());
    m_localPosition = QVector3D(localPosition.x(), localPosition.y(), localPosition.z());
}

QVector3D ThrustForce::value() const
{
    return m_value;
}

void ThrustForce::setValue(const QVector3D &value)
{
    m_value = value;
}

QVector3D ThrustForce::position() const
{
    return m_position;
}

void ThrustForce::setPosition(const QVector3D &position)
{
    m_position = position;
}

DragForce::DragForce(QObject *parent) :
    Force("Drag", parent)
{

}

void DragForce::calculate()
{
    btTransform transform = m_body->body()->getCenterOfMassTransform();

    btVector3 velocity = m_body->body()->getLinearVelocity();
    if (velocity.length() == 0) {
        return;  // can't be normalised
    }

    float value = 0.5f * m_fluidDensity * m_crossSectionalArea * m_coefficient * velocity.length2();
    btVector3 force = velocity.normalized() * -value;

    m_force = QVector3D(force.x(), force.y(), force.z());

    btVector3 position(m_position.x(), m_position.y(), m_position.z());
    btVector3 localPosition = (transform * position) - transform.getOrigin();
    m_localPosition = QVector3D(localPosition.x(), localPosition.y(), localPosition.z());
}

double DragForce::fluidDensity() const
{
    return m_fluidDensity;
}

void DragForce::setFluidDensity(double fluidDensity)
{
    m_fluidDensity = fluidDensity;
}

double DragForce::crossSectionalArea() const
{
    return m_crossSectionalArea;
}

void DragForce::setCrossSectionalArea(double crossSectionalArea)
{
    m_crossSectionalArea = crossSectionalArea;
}

double DragForce::coefficient() const
{
    return m_coefficient;
}

void DragForce::setCoefficient(double coefficient)
{
    m_coefficient = coefficient;
}

QVector3D DragForce::position() const
{
    return m_position;
}

void DragForce::setPosition(const QVector3D &position)
{
    m_position = position;
}

LiftForce::LiftForce(QObject *parent) :
    Force("Lift", parent)
{

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

void LiftForce::calculate()
{
    m_force = QVector3D();

    // pitch
    QVector2D pitchVelocity = m_body->pitchVelocity();
    float pitchAngleOfAttack = m_body->pitchAngleOfAttack();

    if (qAbs(pitchAngleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D pitchLift = calculateLift(m_fluidDensity, pitchAngleOfAttack, m_coefficientSlope, m_pitchCrossSectionalArea, pitchVelocity);
        m_force += QVector3D(pitchLift.x(), pitchLift.y(), 0);
    }

    // yaw
    QVector2D yawVelocity = m_body->yawVelocity();
    float yawAngleOfAttack = m_body->yawAngleOfAttack();

    if (qAbs(yawAngleOfAttack) < qDegreesToRadians(15.)) {
        QVector2D yawLift = calculateLift(m_fluidDensity, yawAngleOfAttack, m_coefficientSlope, m_yawCrossSectionalArea, yawVelocity);
        m_force += QVector3D(yawLift.x(), 0, yawLift.y());
    }

    btTransform transform = m_body->body()->getCenterOfMassTransform();
    btVector3 position(m_position.x(), m_position.y(), m_position.z());
    btVector3 localPosition = (transform * position) - transform.getOrigin();
    m_localPosition = QVector3D(localPosition.x(), localPosition.y(), localPosition.z());
}

double LiftForce::fluidDensity() const
{
    return m_fluidDensity;
}

void LiftForce::setFluidDensity(double fluidDensity)
{
    m_fluidDensity = fluidDensity;
}

double LiftForce::yawCrossSectionalArea() const
{
    return m_yawCrossSectionalArea;
}

void LiftForce::setYawCrossSectionalArea(double yawCrossSectionalArea)
{
    m_yawCrossSectionalArea = yawCrossSectionalArea;
}

double LiftForce::pitchCrossSectionalArea() const
{
    return m_pitchCrossSectionalArea;
}

void LiftForce::setPitchCrossSectionalArea(double pitchCrossSectionalArea)
{
    m_pitchCrossSectionalArea = pitchCrossSectionalArea;
}

double LiftForce::coefficientSlope() const
{
    return m_coefficientSlope;
}

void LiftForce::setCoefficientSlope(double coefficientSlope)
{
    m_coefficientSlope = coefficientSlope;
}

QVector3D LiftForce::position() const
{
    return m_position;
}

void LiftForce::setPosition(const QVector3D &position)
{
    m_position = position;
}
