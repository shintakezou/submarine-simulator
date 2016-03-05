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

protected:
    void calculate();

public:
    double fluidDensity() const;
    void setFluidDensity(double fluidDensity);

    double yawCrossSectionalArea() const;
    void setYawCrossSectionalArea(double yawCrossSectionalArea);

    double pitchCrossSectionalArea() const;
    void setPitchCrossSectionalArea(double pitchCrossSectionalArea);

    double coefficient() const;
    void setCoefficient(double coefficient);

    double bodyLength() const;
    void setBodyLength(double bodyLength);

private:
    double m_fluidDensity;
    double m_yawCrossSectionalArea;
    double m_pitchCrossSectionalArea;
    double m_coefficient;
    double m_bodyLength;
};

class FinDampingTorque : public Torque
{
    Q_OBJECT

public:
    explicit FinDampingTorque(QObject *parent = 0);

protected:
    void calculate();

public:
    double fluidDensity() const;
    void setFluidDensity(double fluidDensity);

    double crossSectionalArea() const;
    void setCrossSectionalArea(double crossSectionalArea);

    double aspectRatio() const;
    void setAspectRatio(double aspectRatio);

    double radius() const;
    void setRadius(double radius);

private:
    double m_fluidDensity;
    double m_crossSectionalArea;
    double m_aspectRatio;
    double m_radius;

};

} // namespace Physics

#endif // TORQUE_H
