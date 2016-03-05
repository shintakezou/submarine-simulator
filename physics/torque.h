#ifndef TORQUE_H
#define TORQUE_H

#include <QObject>
#include <QVector3D>

class btRigidBody;

namespace Physics {

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

    btRigidBody *body() const;
    void setBody(btRigidBody *body);

    QVector3D value() const;

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(btRigidBody *body READ body)
    Q_PROPERTY(QVector3D value READ value)

protected:
    QString m_name;

    btRigidBody *m_body;

    QVector3D m_value;
};

class FixedTorque : public Torque
{
    Q_OBJECT

public:
    explicit FixedTorque(QString name, QObject *parent = 0);

protected:
    void calculate();

public:
    void setValue(const QVector3D &value);
};

class PropellorTorque : public FixedTorque
{
    Q_OBJECT

public:
    explicit PropellorTorque(QObject *parent = 0);
};

} // namespace Physics

#endif // TORQUE_H
