#include <bullet/btBulletDynamicsCommon.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QScaleTransform>

#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QMesh>

#include "physics/body.h"
#include "physics/torque.h"

#include "torquearrow.h"

TorqueArrow::TorqueArrow(QColor colour, float scale, Qt3D::QNode *parent) :
    Qt3D::QEntity(parent),
    m_colour(colour),
    m_scale(scale),
    m_torque(0)
{
    auto material = new Qt3D::QPhongMaterial(this);
    material->setAmbient(m_colour);
    addComponent(material);

    auto mesh = new Qt3D::QMesh(this);
    mesh->setSource(QUrl("qrc:/models/torque-arrow.obj"));
    addComponent(mesh);

    auto transform = new Qt3D::QTransform(this);

    m_rotateTransform = new Qt3D::QRotateTransform(this);
    transform->addTransform(m_rotateTransform);

    m_scaleTransform = new Qt3D::QScaleTransform(this);
    transform->addTransform(m_scaleTransform);

    m_translateTransform = new Qt3D::QTranslateTransform(this);
    transform->addTransform(m_translateTransform);

    addComponent(transform);

}

TorqueArrow::~TorqueArrow()
{

}

void TorqueArrow::update()
{
    QVector3D torque = m_torque->value();
    QVector3D position = m_torque->body()->position();

    QVector3D up = QVector3D(0, 1, 0);
    QVector3D dir = torque.normalized();

    QQuaternion q = QQuaternion::rotationTo(up, dir);

    float angle;
    QVector3D axis;
    q.getAxisAndAngle(&axis, &angle);

    m_rotateTransform->setAxis(axis);
    m_rotateTransform->setAngleDeg(angle);

    m_translateTransform->setTranslation(position);

    m_scaleTransform->setScale(torque.length() * 0.1 * m_scale);
}

QColor TorqueArrow::colour() const
{
    return m_colour;
}

float TorqueArrow::scale() const
{
    return m_scale;
}

Physics::Torque *TorqueArrow::torque() const
{
    return m_torque;
}

void TorqueArrow::setTorque(Physics::Torque *torque)
{
    if (m_torque) {
        disconnect(m_torque, &Physics::Torque::applied, this, &TorqueArrow::update);
    }

    m_torque = torque;

    connect(m_torque, &Physics::Torque::applied, this, &TorqueArrow::update);
}
