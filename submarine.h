#ifndef SUBMARINE_H
#define SUBMARINE_H

#include <QObject>
#include <QVector3D>
#include <QVector>

namespace Qt3D {
class QEntity;
class QTranslateTransform;
class QRotateTransform;
class QCamera;
class QPhongMaterial;
}

class btCapsuleShape;
class btRigidBody;
class btDynamicsWorld;
class btVector3;

namespace Physics {
class Body;
class BuoyancyForce;
class DragForce;
class LiftForce;
class PropellorTorque;
class SpinningDragTorque;
class ThrustForce;
class WeightForce;
}

class Fin;
class Fluid;
class ForceArrow;

class Submarine : public QObject
{
    Q_OBJECT

public:
    explicit Submarine(QObject *parent = 0);
    ~Submarine();

    static Submarine *makeDefault(QObject *parent = 0);

    void addToWorld(btDynamicsWorld *world);
    void removeFromWorld(btDynamicsWorld *world);

    void addToScene(Qt3D::QEntity *scene);

private:
    void makeBodyEntity(Qt3D::QPhongMaterial *material);
    void makePropellorEntity(Qt3D::QPhongMaterial *material);
    void makeFinsEntities(Qt3D::QEntity *scene, Qt3D::QPhongMaterial *material);
    void makeForceArrows(Qt3D::QEntity *scene);

public:
    void update(const Fluid *fluid, Qt3D::QCamera *camera);

private:
    void updateTransformation();
    void updateCamera(Qt3D::QCamera *camera);
    void updateForces(const Fluid *fluid);

    void applyPropellorTorque();
    void applyWeight();
    void applyBuoyancy();
    void applyThrust();
    void applyDrag(const Fluid *fluid);
    void applyLift(const Fluid *fluid);
    void applyPitchSpinningDrag(const Fluid *fluid);
    void applyYawSpinningDrag(const Fluid *fluid);
    void applySpinningDrag(const Fluid *fluid);

public:
    Physics::Body *body() const;

    double crossSectionalArea() const;

    double length() const;
    void setLength(double length);

    double width() const;
    void setWidth(double width);

    double height() const;
    void setHeight(double height);

    double mass() const;
    void setMass(double mass);

    double hasHorizontalFins() const;
    void setHasHorizontalFins(double hasHorizontalFins);

    double horizontalFinsArea() const;
    void setHorizontalFinsArea(double horizontalFinsArea);

    double horizontalFinsLiftCoefficientSlope() const;
    void setHorizontalFinsLiftCoefficientSlope(double horizontalFinsLiftCoefficientSlope);

    double horizontalFinsDragCoefficient() const;
    void setHorizontalFinsDragCoefficient(double horizontalFinsDragCoefficient);

    double horizontalFinsPosition() const;
    void setHorizontalFinsPosition(double horizontalFinsPosition);

    double horizontalFinsAspectRatio() const;
    void setHorizontalFinsAspectRatio(double horizontalFinsAspectRatio);

    double hasVerticalFins() const;
    void setHasVerticalFins(double hasVerticalFins);

    double verticalFinsArea() const;
    void setVerticalFinsArea(double verticalFinsArea);

    double verticalFinsLiftCoefficientSlope() const;
    void setVerticalFinsLiftCoefficientSlope(double verticalFinsLiftCoefficientSlope);

    double verticalFinsDragCoefficient() const;
    void setVerticalFinsDragCoefficient(double verticalFinsDragCoefficient);

    double verticalFinsPosition() const;
    void setVerticalFinsPosition(double verticalFinsPosition);

    double verticalFinsAspectRatio() const;
    void setVerticalFinsAspectRatio(double verticalFinsAspectRatio);

    Physics::PropellorTorque *propellorTorque() const;

    Q_PROPERTY(double length READ length WRITE setLength)
    Q_PROPERTY(double width READ width WRITE setWidth)
    Q_PROPERTY(double height READ height WRITE setHeight)
    Q_PROPERTY(double mass READ mass WRITE setMass)
    Q_PROPERTY(double crossSectionalArea READ crossSectionalArea STORED false)
    Q_PROPERTY(bool hasHorizontalFins READ hasHorizontalFins WRITE setHasHorizontalFins)
    Q_PROPERTY(double horizontalFinsArea READ horizontalFinsArea WRITE setHorizontalFinsArea)
    Q_PROPERTY(double horizontalFinsLiftCoefficientSlope READ horizontalFinsLiftCoefficientSlope WRITE setHorizontalFinsLiftCoefficientSlope)
    Q_PROPERTY(double horizontalFinsDragCoefficient READ horizontalFinsDragCoefficient WRITE setHorizontalFinsDragCoefficient)
    Q_PROPERTY(double horizontalFinsPosition READ horizontalFinsPosition WRITE setHorizontalFinsPosition)
    Q_PROPERTY(double horizontalFinsAspectRatio READ horizontalFinsAspectRatio WRITE setHorizontalFinsAspectRatio)
    Q_PROPERTY(bool hasVerticalFins READ hasVerticalFins WRITE setHasVerticalFins)
    Q_PROPERTY(double verticalFinsArea READ verticalFinsArea WRITE setVerticalFinsArea)
    Q_PROPERTY(double verticalFinsLiftCoefficientSlope READ verticalFinsLiftCoefficientSlope WRITE setVerticalFinsLiftCoefficientSlope)
    Q_PROPERTY(double verticalFinsDragCoefficient READ verticalFinsDragCoefficient WRITE setVerticalFinsDragCoefficient)
    Q_PROPERTY(double verticalFinsPosition READ verticalFinsPosition WRITE setVerticalFinsPosition)
    Q_PROPERTY(double verticalFinsAspectRatio READ verticalFinsAspectRatio WRITE setVerticalFinsAspectRatio)

    Physics::WeightForce *weight() const;
    Physics::BuoyancyForce *buoyancy() const;
    Physics::ThrustForce *thrust() const;
    Physics::DragForce *drag() const;
    Physics::LiftForce *lift() const;
    Physics::SpinningDragTorque *spinningDrag() const;

private:
    btCapsuleShape *m_shape;
    Physics::Body *m_body;

    Qt3D::QEntity *m_entity;
    Qt3D::QTranslateTransform *m_translateTransform;
    Qt3D::QRotateTransform *m_rotateTransform;

    QVector<Fin *> m_fins;

    ForceArrow *m_forceNoise;
    ForceArrow *m_forceWeight;
    ForceArrow *m_forceBuoyancy;
    ForceArrow *m_forceThrust;
    ForceArrow *m_forceDrag;
    ForceArrow *m_forceLift;

    double m_length;
    double m_width;
    double m_height;
    double m_mass;
    Physics::PropellorTorque *m_propellorTorque;

    double m_hasHorizontalFins;
    double m_horizontalFinsArea;
    double m_horizontalFinsLiftCoefficientSlope;
    double m_horizontalFinsDragCoefficient;
    double m_horizontalFinsPosition;
    double m_horizontalFinsAspectRatio;

    double m_hasVerticalFins;
    double m_verticalFinsArea;
    double m_verticalFinsLiftCoefficientSlope;
    double m_verticalFinsDragCoefficient;
    double m_verticalFinsPosition;
    double m_verticalFinsAspectRatio;

    Physics::WeightForce *m_weight;
    Physics::BuoyancyForce *m_buoyancy;
    Physics::ThrustForce *m_thrust;
    Physics::DragForce *m_drag;
    Physics::LiftForce *m_lift;
    Physics::SpinningDragTorque *m_spinningDrag;
};

#endif // SUBMARINE_H
