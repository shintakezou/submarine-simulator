#include <bullet/btBulletDynamicsCommon.h>

#include "physics/body.h"

using namespace Physics;

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
