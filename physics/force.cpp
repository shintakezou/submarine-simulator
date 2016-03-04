#include <bullet/btBulletDynamicsCommon.h>

#include "physics/force.h"

using namespace Physics;

Force::Force(QString name, btRigidBody *body, QObject *parent) :
    QObject(parent),
    m_name(name),
    m_body(body)
{

}

void Force::apply()
{
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

QVector3D Force::localPosition() const
{
    return m_localPosition;
}

QVector3D Force::worldPosition() const
{
    btVector3 localPosition = btVector3(m_localPosition.x(), m_localPosition.y(), m_localPosition.z());
    btVector3 worldPosition = localPosition + m_body->getCenterOfMassPosition();
    return QVector3D(worldPosition.x(), worldPosition.y(), worldPosition.z());
}

QVector3D Force::value() const
{
    return m_value;
}

WeightForce::WeightForce(btRigidBody *body, QObject *parent) :
    Force("Weight", body, parent)
{

}

void WeightForce::calculate()
{
    double mass = 1. / m_body->getInvMass();
    m_value = QVector3D(0, -9.81 * mass, 0);
    m_localPosition = QVector3D(0, 0, 0);
}
