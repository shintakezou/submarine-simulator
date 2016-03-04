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
    explicit Torque(QString name, btRigidBody *body, QObject *parent = 0);

    void apply();

protected:
    virtual void calculate() = 0;

public:
    QString name() const;
    void setName(const QString &name);

    btRigidBody *body() const;

    QVector3D value() const;

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(btRigidBody *body READ body)
    Q_PROPERTY(QVector3D value READ value)

protected:
    QString m_name;

    btRigidBody *m_body;

    QVector3D m_value;
};

} // namespace Physics

#endif // TORQUE_H
