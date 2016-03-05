#include <Qt3DCore/QEntity>
#include <Qt3DCore/QRotateTransform>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QMesh>
#include <QtMath>
#include <QVector2D>

#include <bullet/btBulletDynamicsCommon.h>

#include "fluid.h"
#include "forcearrow.h"
#include "physics/body.h"
#include "physics/force.h"
#include "physics/torque.h"
#include "submarine.h"
#include "torquearrow.h"

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
    m_lift(new Physics::LiftForce(this)),
    m_damping(new Physics::FinDampingTorque(this))
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

    auto liftArrow = new ForceArrow(QColor(0xc0392b), 200., scene);
    liftArrow->setForce(m_lift);

    auto dragArrow = new ForceArrow(QColor(0x27ae60), 150., scene);
    dragArrow->setForce(m_drag);

    auto dampingArrow = new TorqueArrow(QColor(0xf1c40f), 1000., scene);
    dampingArrow->setTorque(m_damping);
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

    m_damping->setRadius(radius);

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

    m_damping->setBody(submarine()->body());
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
}

void Fin::applyDrag(const Fluid *fluid) const
{
    m_drag->setFluidDensity(fluid->density());

    m_drag->apply();
}

void Fin::applyDamping(const Fluid *fluid) const
{
    m_damping->setFluidDensity(fluid->density());

    m_damping->apply();
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

    m_drag->setCrossSectionalArea(m_area);
    m_damping->setCrossSectionalArea(m_area);
}

Physics::DragForce *Fin::drag() const
{
    return m_drag;
}

Physics::LiftForce *Fin::lift() const
{
    return m_lift;
}

Physics::FinDampingTorque *Fin::damping() const
{
    return m_damping;
}
