#include <bullet/btBulletDynamicsCommon.h>

#include <QtDebug>

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

    btVector3 force = btVector3(m_value.x(), m_value.y(), m_value.z());
    btVector3 localPosition = btVector3(m_localPosition.x(), m_localPosition.y(), m_localPosition.z());

    m_body->applyForce(force, localPosition);
}

QString Force::name() const
{
    return m_name;
}

void Force::setName(const QString &name)
{
    m_name = name;
}

btRigidBody *Force::body() const
{
    return m_body;
}

void Force::setBody(btRigidBody *body)
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

    btTransform transform = m_body->getCenterOfMassTransform();

    btVector3 localPosition = btVector3(m_localPosition.x(), m_localPosition.y(), m_localPosition.z());
    btVector3 worldPosition = localPosition + transform.getOrigin();
    return QVector3D(worldPosition.x(), worldPosition.y(), worldPosition.z());
}

QVector3D Force::value() const
{
    return m_value;
}

WeightForce::WeightForce(QObject *parent) :
    Force("Weight", parent)
{

}

void WeightForce::calculate()
{
    btTransform transform = m_body->getCenterOfMassTransform();

    double mass = 1. / m_body->getInvMass();
    m_value = QVector3D(0, -9.81 * mass, 0);

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
    btTransform transform = m_body->getCenterOfMassTransform();

    double mass = 1. / m_body->getInvMass();
    m_value = QVector3D(0, 9.81 * mass, 0);

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
