#ifndef TORQUE_H
#define TORQUE_H

#include <QObject>
#include <QVector3D>

class btRigidBody;

namespace Physics {

class Body;

class Torque : public QObject
{
    Q_OBJECT

public:
    explicit Torque(QString name, QObject *parent = 0);

    void apply();

protected:
    virtual void calculate() = 0;

public:
    QString name() const;
    void setName(const QString &name);

    Physics::Body *body() const;
    void setBody(Physics::Body *body);

    QVector3D value() const;

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Physics::Body *body READ body)
    Q_PROPERTY(QVector3D value READ value)

protected:
    QString m_name;

    Physics::Body *m_body;

    QVector3D m_value;
};

class PropellorTorque : public Torque
{
    Q_OBJECT

public:
    explicit PropellorTorque(QObject *parent = 0);

protected:
    void calculate();

public:
    void setValue(const QVector3D &value);
};

class SpinningDragTorque : public Torque
{
    Q_OBJECT

public:
    explicit SpinningDragTorque(QObject *parent = 0);
};

} // namespace Physics

#endif // TORQUE_H
