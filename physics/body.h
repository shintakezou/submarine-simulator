#ifndef BODY_H
#define BODY_H

#include <QObject>

class btRigidBody;

namespace Physics {

class Body : public QObject
{
    Q_OBJECT

public:
    explicit Body(btRigidBody *body, QObject *parent = 0);
    ~Body();

    btRigidBody *body() const;

    double mass() const;

    double pitch() const;
    double yaw() const;
    double roll() const;

    QVector2D pitchVelocity() const;
    QVector2D yawVelocity() const;
    QVector2D rollVelocity() const;

    double pitchAngleOfAttack() const;
    double yawAngleOfAttack() const;
    double rollAngleOfAttack() const;

    QVector3D angularVelocity() const;
    QVector3D linearVelocity() const;
    QVector3D position() const;

    Q_PROPERTY(btRigidBody *body READ body)

private:
    btRigidBody *m_body;
};

}

#endif // BODY_H
