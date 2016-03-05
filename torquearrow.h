#ifndef TORQUEARROW_H
#define TORQUEARROW_H

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
class Torque;
}

class TorqueArrow : public Qt3D::QEntity
{
    Q_OBJECT

public:
    explicit TorqueArrow(QColor colour, float scale, Qt3D::QNode *parent = 0);
    ~TorqueArrow();

private slots:
    void update();

public:
    QColor colour() const;

    float scale() const;

    Physics::Torque *torque() const;
    void setTorque(Physics::Torque *torque);

    Q_PROPERTY(QColor colour READ colour)
    Q_PROPERTY(float scale READ scale)
    Q_PROPERTY(Physics::Torque *torque READ torque WRITE setTorque)

private:
    QColor m_colour;
    float m_scale;
    Physics::Torque *m_torque;

    Qt3D::QRotateTransform *m_rotateTransform;
    Qt3D::QTranslateTransform *m_translateTransform;
    Qt3D::QScaleTransform *m_scaleTransform;
};

#endif // TORQUEARROW_H
