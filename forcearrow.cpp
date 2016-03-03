#include <bullet/btBulletDynamicsCommon.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QLookAtTransform>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DCore/QScaleTransform>

#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QMesh>

#include "forcearrow.h"

ForceArrow::ForceArrow(QColor colour, float scale, QObject *parent) :
    QObject(parent),
    m_colour(colour),
    m_scale(scale)
{

}

ForceArrow::~ForceArrow()
{

}

void ForceArrow::addToScene(Qt3D::QEntity *scene)
{
    auto entity = new Qt3D::QEntity(scene);

    auto material = new Qt3D::QPhongMaterial(entity);
    material->setAmbient(m_colour);
    entity->addComponent(material);

    auto mesh = new Qt3D::QMesh(entity);
    mesh->setSource(QUrl("qrc:/models/arrow.obj"));
    entity->addComponent(mesh);

    auto transform = new Qt3D::QTransform(entity);

    m_rotateTransform = new Qt3D::QRotateTransform(entity);
    transform->addTransform(m_rotateTransform);

    m_scaleTransform = new Qt3D::QScaleTransform(entity);
    transform->addTransform(m_scaleTransform);

    m_translateTransform = new Qt3D::QTranslateTransform(entity);
    transform->addTransform(m_translateTransform);

    entity->addComponent(transform);
}

void ForceArrow::update(QVector3D force, QVector3D position)
{
    QVector3D up = QVector3D(0, 1, 0);
    QVector3D dir = force.normalized();

    QQuaternion q = QQuaternion::rotationTo(up, dir);

    float angle;
    QVector3D axis;
    q.getAxisAndAngle(&axis, &angle);

    m_rotateTransform->setAxis(axis);
    m_rotateTransform->setAngleDeg(angle);

    m_translateTransform->setTranslation(position);

    m_scaleTransform->setScale((force.length() / 500.) * m_scale);
}

void ForceArrow::update(btVector3 force, btVector3 position)
{
    QVector3D forceVector(force.x(), force.y(), force.z());
    QVector3D positionVector(position.x(), position.y(), position.z());
    update(forceVector, positionVector);
}

QColor ForceArrow::colour() const
{
    return m_colour;
}

void ForceArrow::setColour(const QColor &colour)
{
    m_colour = colour;
}

float ForceArrow::scale() const
{
    return m_scale;
}

void ForceArrow::setScale(float scale)
{
    m_scale = scale;
}
