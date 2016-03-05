#ifndef FORCEARROW_H
#define FORCEARROW_H

#include <QColor>
#include <Qt3DCore/QEntity>
#include <QVector3D>

namespace Qt3D {
class QEntity;
class QNode;
class QRotateTransform;
class QTranslateTransform;
class QScaleTransform;
}

class btVector3;

namespace Physics {
class Force;
}

class ForceArrow : public Qt3D::QEntity
{
    Q_OBJECT

public:
    explicit ForceArrow(QColor colour, float scale, Qt3D::QNode *parent = 0);
    ~ForceArrow();

private slots:
    void update();

public:
    QColor colour() const;

    float scale() const;

    Physics::Force *force() const;
    void setForce(Physics::Force *force);

    Q_PROPERTY(QColor colour READ colour)
    Q_PROPERTY(float scale READ scale)
    Q_PROPERTY(Physics::Force *force READ force WRITE setForce)

private:
    QColor m_colour;
    float m_scale;
    Physics::Force *m_force;

    Qt3D::QRotateTransform *m_rotateTransform;
    Qt3D::QTranslateTransform *m_translateTransform;
    Qt3D::QScaleTransform *m_scaleTransform;
};

#endif // FORCEARROW_H
