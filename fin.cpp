#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QMesh>
#include <QtMath>
#include <QVector2D>

#include <bullet/btBulletDynamicsCommon.h>

#include "physics/body.h"
#include "physics/force.h"

#include "fluid.h"
#include "forcearrow.h"
#include "submarine.h"

#include "fin.h"

float calculateEllipseProportion(float proportion)
{
    return qSqrt(1.f - (proportion * proportion));
}

Fin::Fin(Qt3D::QEntity *scene, Qt3D::QNode *parent) :
    Qt3D::QEntity(parent),
    m_plane(Unknown),
    m_forcePosition(new btVector3()),
    m_drag(new Physics::DragForce(this)),
    m_lift(new Physics::LiftForce(this))
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

    m_forceLift = new ForceArrow(Qt::red, 150., scene);
    m_forceLift->addToScene(scene);

    m_forceDrag = new ForceArrow(Qt::green, 150., scene);
    m_forceDrag->addToScene(scene);
}

void Fin::calculatePosition(Orientation orientation, float position)
{
    m_translateTransform->setDx(position);

    float p = qAbs(position) / (submarine()->length() / 2.);
    float radius = calculateEllipseProportion(p) * (submarine()->width() / 2.f);

    m_translateTransform->setDy(radius);

    m_forcePosition->setX(position);

    switch (orientation) {
    case North:
        m_forcePosition->setY(radius);
        break;

    case East:
        m_forcePosition->setZ(radius);
        break;

    case South:
        m_forcePosition->setY(-radius);
        break;

    case West:
        m_forcePosition->setZ(-radius);
        break;
    }

    // FIXME this is required :/
    qDebug() << "p:" << position;

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
        m_lift->setYawCrossSectionalArea(m_area);
        m_lift->setPitchCrossSectionalArea(0);
        break;

    case East:
    case West:
        m_plane = Vertical;
        m_lift->setPitchCrossSectionalArea(m_area);
        m_lift->setYawCrossSectionalArea(0);
        break;
    }

    m_drag->setBody(submarine()->body());
    m_drag->setPosition(QVector3D(m_forcePosition->x(), m_forcePosition->y(), m_forcePosition->z()));

    m_lift->setBody(submarine()->body());
    m_lift->setPosition(QVector3D(m_forcePosition->x(), m_forcePosition->y(), m_forcePosition->z()));
}

void Fin::applyForces(const Fluid *fluid) const
{
    applyLift(fluid);
    applyDrag(fluid);
    applyDamping(fluid);
}

void Fin::applyLift(const Fluid *fluid) const
{
    m_lift->setFluidDensity(fluid->density());

    m_lift->apply();
    m_forceLift->update(m_lift);
}

void Fin::applyDrag(const Fluid *fluid) const
{
    m_drag->setCrossSectionalArea(m_area);
    m_drag->setFluidDensity(fluid->density());

    m_drag->apply();
    m_forceDrag->update(m_drag);
}

void Fin::applyDamping(const Fluid *fluid) const
{
    float span = qSqrt(m_aspectRatio * m_area);

    float radius = 0;
    switch (m_plane) {
    case Horizontal:
        radius = m_forcePosition->z();
        break;

    case Vertical:
        radius = m_forcePosition->y();
        break;

    default:
        qFatal("Unknown fin plane.");
        return;
    }

    float v2 = submarine()->body()->body()->getAngularVelocity().x() * submarine()->body()->body()->getAngularVelocity().x();
    float torque = -2.f * fluid->density() * m_area * v2 * (radius + span) * (radius + span) * (radius + span / 2.f);

    submarine()->body()->body()->applyTorque(btVector3(torque, 0, 0));
}

Fin::Plane Fin::plane() const
{
    return m_plane;
}

Submarine *Fin::submarine() const
{
    return m_submarine;
}

void Fin::setSubmarine(Submarine *submarine)
{
    m_submarine = submarine;
}

double Fin::area() const
{
    return m_area;
}

void Fin::setArea(double area)
{
    m_area = area;
}

double Fin::aspectRatio() const
{
    return m_aspectRatio;
}

void Fin::setAspectRatio(double aspectRatio)
{
    m_aspectRatio = aspectRatio;
}

Physics::DragForce *Fin::drag() const
{
    return m_drag;
}

Physics::LiftForce *Fin::lift() const
{
    return m_lift;
}
