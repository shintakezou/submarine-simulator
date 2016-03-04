#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QMesh>
#include <QtMath>

#include "submarine.h"

#include "fin.h"

float calculateEllipseProportion(float proportion)
{
    return qSqrt(1.f - (proportion * proportion));
}

Fin::Fin(Qt3D::QNode *parent) :
    Qt3D::QEntity(parent),
    m_plane(Unknown)
{
    auto mesh = new Qt3D::QMesh(this);
    mesh->setSource(QUrl("qrc:/models/fin.obj"));
    addComponent(mesh);

    m_rotateTransform = new Qt3D::QRotateTransform(this);
    m_rotateTransform->setAxis(QVector3D(1, 0, 0));

    m_translateTransform = new Qt3D::QTranslateTransform(this);

    auto transform = new Qt3D::QTransform(this);
    transform->addTransform(m_translateTransform);
    transform->addTransform(m_rotateTransform);
    addComponent(transform);
}

void Fin::calculatePosition(Submarine *submarine, Orientation orientation, float position) {
    m_translateTransform->setDx(position);

    float p = qAbs(position) / (submarine->length() / 2.);
    float radius = calculateEllipseProportion(p) * (submarine->width() / 2.f);

    m_translateTransform->setDy(radius);

    // TODO rotate fin

    switch (orientation) {
    case North:
        m_rotateTransform->setAngleDeg(0);
        break;

    case East:
        m_rotateTransform->setAngleDeg(90);
        break;

    case South:
        m_rotateTransform->setAngleDeg(180);
        break;

    case West:
        m_rotateTransform->setAngleDeg(270);
        break;
    }

    switch (orientation) {
    case North:
    case South:
        m_plane = Horizontal;
        break;

    case East:
    case West:
        m_plane = Vertical;
        break;
    }
}

Fin::Plane Fin::plane() const
{
    return m_plane;
}
