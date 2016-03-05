#include <bullet/btBulletDynamicsCommon.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QScaleTransform>

#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QMesh>

#include "physics/force.h"

#include "forcearrow.h"

ForceArrow::ForceArrow(QColor colour, float scale, Qt3D::QNode *parent) :
    Qt3D::QEntity(parent),
    m_colour(colour),
    m_scale(scale),
    m_force(0)
{
    auto material = new Qt3D::QPhongMaterial(this);
    material->setAmbient(m_colour);
    addComponent(material);

    auto mesh = new Qt3D::QMesh(this);
    mesh->setSource(QUrl("qrc:/models/arrow.obj"));
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

ForceArrow::~ForceArrow()
{

}

void ForceArrow::update()
{
    QVector3D force = m_force->force();
    QVector3D position = m_force->worldPosition();

    QVector3D up = QVector3D(0, 1, 0);
    QVector3D dir = force.normalized();

    QQuaternion q = QQuaternion::rotationTo(up, dir);

    float angle;
    QVector3D axis;
    q.getAxisAndAngle(&axis, &angle);

    m_rotateTransform->setAxis(axis);
    m_rotateTransform->setAngleDeg(angle);

    m_translateTransform->setTranslation(position);

    m_scaleTransform->setScale((force.length() / 250.) * m_scale);
}

QColor ForceArrow::colour() const
{
    return m_colour;
}

float ForceArrow::scale() const
{
    return m_scale;
}

Physics::Force *ForceArrow::force() const
{
    return m_force;
}

void ForceArrow::setForce(Physics::Force *force)
{
    if (m_force) {
        disconnect(m_force, &Physics::Force::applied, this, &ForceArrow::update);
    }

    m_force = force;

    connect(m_force, &Physics::Force::applied, this, &ForceArrow::update);
}
