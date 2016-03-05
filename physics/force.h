#ifndef FORCE_H
#define FORCE_H

#include <QObject>
#include <QVector3D>

class btRigidBody;

namespace Physics {

class Force : public QObject
{
    Q_OBJECT

public:
    explicit Force(QString name, QObject *parent = 0);

    void apply();

protected:
    virtual void calculate() = 0;

public:
    QString name() const;
    void setName(const QString &name);

    btRigidBody *body() const;
    void setBody(btRigidBody *body);

    QVector3D localPosition() const;
    QVector3D worldPosition() const;

    QVector3D force() const;

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(btRigidBody *body READ body WRITE setBody)
    Q_PROPERTY(QVector3D localPosition READ localPosition)
    Q_PROPERTY(QVector3D worldPosition READ worldPosition STORED false)
    Q_PROPERTY(QVector3D force READ force)

protected:
    QString m_name;

    btRigidBody *m_body;

    QVector3D m_localPosition;
    QVector3D m_force;
};

class WeightForce : public Force
{
    Q_OBJECT

public:
    explicit WeightForce(QObject *parent = 0);

protected:
    void calculate();

public:
    QVector3D position() const;
    void setPosition(const QVector3D &position);

private:
    QVector3D m_position;
};

class BuoyancyForce : public Force
{
    Q_OBJECT

public:
    explicit BuoyancyForce(QObject *parent = 0);

protected:
    void calculate();

public:
    QVector3D position() const;
    void setPosition(const QVector3D &position);

private:
    QVector3D m_position;
};

class ThrustForce : public Force
{
    Q_OBJECT

public:
    explicit ThrustForce(QObject *parent = 0);

protected:
    void calculate();

public:
    QVector3D value() const;
    void setValue(const QVector3D &value);

    QVector3D position() const;
    void setPosition(const QVector3D &position);

private:
    QVector3D m_value;
    QVector3D m_position;
};

} // namespace Physics

#endif // FORCE_H
