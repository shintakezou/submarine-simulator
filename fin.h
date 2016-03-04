#ifndef FIN_H
#define FIN_H

#include <Qt3DCore/QEntity>

namespace Qt3D {
class QRotateTransform;
class QTranslateTransform;
}

class btVector3;

class Fluid;
class ForceArrow;
class Submarine;

class Fin : public Qt3D::QEntity
{
    Q_OBJECT

public:
    enum Orientation {
        North,
        East,
        South,
        West
    };

    enum Plane {
        Unknown,
        Horizontal,
        Vertical
    };

    explicit Fin(Qt3D::QEntity *scene, Qt3D::QNode *parent = 0);

    void calculatePosition(Orientation orientation, float position);

    void applyForces(const Fluid *fluid) const;
    void applyLift(const Fluid *fluid) const;
    void applyDrag(const Fluid *fluid) const;
    void applyDamping(const Fluid *fluid) const;

    Submarine *submarine() const;
    void setSubmarine(Submarine *submarine);

    Plane plane() const;

    double area() const;
    void setArea(double area);

    double aspectRatio() const;
    void setAspectRatio(double aspectRatio);

    double liftCoefficientSlope() const;
    void setLiftCoefficientSlope(double liftCoefficientSlope);

    double dragCoefficient() const;
    void setDragCoefficient(double dragCoefficient);

    Q_PROPERTY(Submarine *submarine READ submarine WRITE setSubmarine)
    Q_PROPERTY(Plane plane READ plane)
    Q_PROPERTY(double area READ area WRITE setArea)
    Q_PROPERTY(double aspectRatio READ aspectRatio WRITE setAspectRatio)
    Q_PROPERTY(double liftCoefficientSlope READ liftCoefficientSlope WRITE setLiftCoefficientSlope)
    Q_PROPERTY(double dragCoefficient READ dragCoefficient WRITE setDragCoefficient)

private:
    Qt3D::QRotateTransform *m_rotateTransform;
    Qt3D::QTranslateTransform *m_translateTransform;

    ForceArrow *m_forceLift;
    ForceArrow *m_forceDrag;

    Submarine *m_submarine;
    Plane m_plane;
    double m_area;
    double m_aspectRatio;
    double m_liftCoefficientSlope;
    double m_dragCoefficient;

    btVector3 *m_forcePosition;
};

#endif // FIN_H
