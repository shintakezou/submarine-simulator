#include <QtDebug>
#include <QtMath>

#include <bullet/btBulletDynamicsCommon.h>

#include "physics/body.h"

#include "physics/torque.h"

using namespace Physics;

Torque::Torque(QString name, QObject *parent) :
    QObject(parent),
    m_name(name),
    m_body(0)
{

}

void Torque::apply()
{
    if (!m_body) {
        qCritical() << "Body not set on torque:" << m_name;
        return;
    }

    calculate();

    btVector3 torque = btVector3(m_value.x(), m_value.y(), m_value.z());
    m_body->body()->applyTorque(torque);

    emit applied();
}

QString Torque::name() const
{
    return m_name;
}

void Torque::setName(const QString &name)
{
    m_name = name;
}

Physics::Body *Torque::body() const
{
    return m_body;
}

void Torque::setBody(Physics::Body *body)
{
    m_body = body;
}

QVector3D Torque::value() const
{
    return m_value;
}

PropellorTorque::PropellorTorque(QObject *parent) :
    Torque("Propellor", parent)
{

}

void PropellorTorque::calculate()
{
    // intentionally do nothing
}

void PropellorTorque::setValue(const QVector3D &value)
{
    m_value = value;
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

SpinningDragTorque::SpinningDragTorque(QObject *parent) :
    Torque("Spinning Drag", parent)
{

}

void SpinningDragTorque::calculate()
{
    float pitchAngularVelocity = m_body->angularVelocity().z();
    float pitchDrag = calculateSpinningDrag(m_fluidDensity, m_pitchCrossSectionalArea, m_coefficient, pitchAngularVelocity, m_bodyLength);

    float yawAngularVelocity = m_body->angularVelocity().y();
    float yawDrag = calculateSpinningDrag(m_fluidDensity, m_yawCrossSectionalArea, m_coefficient, yawAngularVelocity, m_bodyLength);

    m_value = QVector3D(0, yawDrag, pitchDrag);
}

double SpinningDragTorque::fluidDensity() const
{
    return m_fluidDensity;
}

void SpinningDragTorque::setFluidDensity(double fluidDensity)
{
    m_fluidDensity = fluidDensity;
}

double SpinningDragTorque::yawCrossSectionalArea() const
{
    return m_yawCrossSectionalArea;
}

void SpinningDragTorque::setYawCrossSectionalArea(double yawCrossSectionalArea)
{
    m_yawCrossSectionalArea = yawCrossSectionalArea;
}

double SpinningDragTorque::pitchCrossSectionalArea() const
{
    return m_pitchCrossSectionalArea;
}

void SpinningDragTorque::setPitchCrossSectionalArea(double pitchCrossSectionalArea)
{
    m_pitchCrossSectionalArea = pitchCrossSectionalArea;
}

double SpinningDragTorque::coefficient() const
{
    return m_coefficient;
}

void SpinningDragTorque::setCoefficient(double coefficient)
{
    m_coefficient = coefficient;
}

double SpinningDragTorque::bodyLength() const
{
    return m_bodyLength;
}

void SpinningDragTorque::setBodyLength(double bodyLength)
{
    m_bodyLength = bodyLength;
}

FinDampingTorque::FinDampingTorque(QObject *parent) :
    Torque("Fin Damping", parent)
{

}

void FinDampingTorque::calculate()
{
    float span = qSqrt(m_aspectRatio * m_crossSectionalArea);

    float v2 = m_body->angularVelocity().x() * m_body->angularVelocity().x();
    float torque = 2.f * m_fluidDensity * m_crossSectionalArea * v2 * (m_radius + span) * (m_radius + span) * (m_radius + span / 2.f);

    if (m_body->angularVelocity().x() > 0) {
        torque = -torque;
    }

    m_value = QVector3D(torque, 0, 0);
}

double FinDampingTorque::fluidDensity() const
{
    return m_fluidDensity;
}

void FinDampingTorque::setFluidDensity(double fluidDensity)
{
    m_fluidDensity = fluidDensity;
}

double FinDampingTorque::crossSectionalArea() const
{
    return m_crossSectionalArea;
}

void FinDampingTorque::setCrossSectionalArea(double crossSectionalArea)
{
    m_crossSectionalArea = crossSectionalArea;
}

double FinDampingTorque::aspectRatio() const
{
    return m_aspectRatio;
}

void FinDampingTorque::setAspectRatio(double aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

double FinDampingTorque::radius() const
{
    return m_radius;
}

void FinDampingTorque::setRadius(double radius)
{
    m_radius = radius;
}
