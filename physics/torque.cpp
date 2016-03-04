#include <bullet/btBulletDynamicsCommon.h>

#include "physics/torque.h"

using namespace Physics;

Torque::Torque(QString name, btRigidBody *body, QObject *parent) :
    QObject(parent),
    m_name(name),
    m_body(body)
{

}

void Torque::apply()
{
    calculate();

    btVector3 torque = btVector3(m_value.x(), m_value.y(), m_value.z());
    m_body->applyTorque(torque);
}

QString Torque::name() const
{
    return m_name;
}

void Torque::setName(const QString &name)
{
    m_name = name;
}

btRigidBody *Torque::body() const
{
    return m_body;
}

QVector3D Torque::value() const
{
    return m_value;
}
