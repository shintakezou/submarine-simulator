#include <QVector2D>
#include <QVector3D>

#include <bullet/btBulletDynamicsCommon.h>

#include "physics/body.h"

using namespace Physics;

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

Body::Body(btRigidBody *body, QObject *parent) :
    QObject(parent),
    m_body(body)
{

}

Body::~Body()
{
    delete m_body;
}

btRigidBody *Body::body() const
{
    return m_body;
}

double Body::mass() const
{
    return 1. / m_body->getInvMass();
}

double Body::pitch() const
{
    btTransform transform = m_body->getCenterOfMassTransform();

    btMatrix3x3 basis = transform.getBasis();

    float yaw, pitch, roll;
    // yes, those do appear to be in the wrong order, but it is correct
    basis.getEulerYPR(pitch, yaw, roll);

    return wrapAngle(pitch);
}

double Body::yaw() const
{
    btTransform transform = m_body->getCenterOfMassTransform();

    btMatrix3x3 basis = transform.getBasis();

    float yaw, pitch, roll;
    // yes, those do appear to be in the wrong order, but it is correct
    basis.getEulerYPR(pitch, yaw, roll);

    return wrapAngle(yaw);
}

double Body::roll() const
{
    btTransform transform = m_body->getCenterOfMassTransform();

    btMatrix3x3 basis = transform.getBasis();

    float yaw, pitch, roll;

    // yes, those do appear to be in the wrong order, but I'm sure it is correct
    basis.getEulerYPR(pitch, yaw, roll);

    return wrapAngle(roll);
}

QVector2D Body::pitchVelocity() const
{
    btVector3 velocity = m_body->getLinearVelocity();
    return QVector2D(velocity.x(), velocity.y());
}

QVector2D Body::yawVelocity() const
{
    btVector3 velocity = m_body->getLinearVelocity();
    return QVector2D(velocity.x(), velocity.z());
}

QVector2D Body::rollVelocity() const
{
    btVector3 velocity = m_body->getLinearVelocity();
    return QVector2D(velocity.y(), velocity.z());
}

double Body::pitchAngleOfAttack() const
{
    QVector2D velocity = pitchVelocity();
    float velocityAngle = wrapAngle(atan2(velocity.y(), velocity.x()));
    return wrapAngle(pitch() - velocityAngle);
}

double Body::yawAngleOfAttack() const
{
    QVector2D velocity = yawVelocity();
    float velocityAngle = wrapAngle(atan2(velocity.y(), velocity.x()));
    return wrapAngle(yaw() - velocityAngle);
}

double Body::rollAngleOfAttack() const
{
    QVector2D velocity = rollVelocity();
    float velocityAngle = wrapAngle(atan2(velocity.y(), velocity.x()));
    return wrapAngle(roll() - velocityAngle);
}

QVector3D Body::angularVelocity() const
{
    btVector3 v = m_body->getAngularVelocity();
    return QVector3D(v.x(), v.y(), v.z());
}

QVector3D Body::linearVelocity() const
{
    btVector3 v = m_body->getLinearVelocity();
    return QVector3D(v.x(), v.y(), v.z());
}

QVector3D Body::position() const
{
    btVector3 p = m_body->getCenterOfMassPosition();
    return QVector3D(p.x(), p.y(), p.z());
}
