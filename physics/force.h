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
    explicit Force(QString name, btRigidBody *body, QObject *parent = 0);

    void apply();

protected:
    virtual void calculate() = 0;

public:
    QString name() const;
    void setName(const QString &name);

    btRigidBody *body() const;

    QVector3D localPosition() const;
    QVector3D worldPosition() const;

    QVector3D value() const;

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(btRigidBody *body READ body)
    Q_PROPERTY(QVector3D localPosition READ localPosition)
    Q_PROPERTY(QVector3D worldPosition READ worldPosition STORED false)
    Q_PROPERTY(QVector3D value READ value)

protected:
    QString m_name;

    btRigidBody *m_body;

    QVector3D m_localPosition;
    QVector3D m_value;
};

class WeightForce : public Force
{
    Q_OBJECT

public:
    explicit WeightForce(btRigidBody *body, QObject *parent = 0);

protected:
    void calculate();
};

} // namespace Physics

#endif // FORCE_H
