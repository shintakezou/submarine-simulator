#ifndef FORCE_H
#define FORCE_H

#include <QObject>
#include <QVector3D>

namespace Physics {

class Body;

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

    Physics::Body *body() const;
    void setBody(Physics::Body *body);

    QVector3D localPosition() const;
    QVector3D worldPosition() const;

    QVector3D force() const;

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Physics::Body *body READ body WRITE setBody)
    Q_PROPERTY(QVector3D localPosition READ localPosition)
    Q_PROPERTY(QVector3D worldPosition READ worldPosition STORED false)
    Q_PROPERTY(QVector3D force READ force)

signals:
    void applied();

protected:
    QString m_name;

    Physics::Body *m_body;

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

class DragForce : public Force
{
    Q_OBJECT

public:
    explicit DragForce(QObject *parent = 0);

protected:
    void calculate();

public:
    double fluidDensity() const;
    void setFluidDensity(double fluidDensity);

    double crossSectionalArea() const;
    void setCrossSectionalArea(double crossSectionalArea);

    double coefficient() const;
    void setCoefficient(double coefficient);

    QVector3D position() const;
    void setPosition(const QVector3D &position);

private:
    double m_fluidDensity;
    double m_crossSectionalArea;
    double m_coefficient;
    QVector3D m_position;
};

class LiftForce : public Force
{
    Q_OBJECT

public:
    explicit LiftForce(QObject *parent = 0);

protected:
    void calculate();

public:
    double fluidDensity() const;
    void setFluidDensity(double fluidDensity);

    double yawCrossSectionalArea() const;
    void setYawCrossSectionalArea(double yawCrossSectionalArea);

    double pitchCrossSectionalArea() const;
    void setPitchCrossSectionalArea(double pitchCrossSectionalArea);

    double coefficientSlope() const;
    void setCoefficientSlope(double coefficientSlope);

    QVector3D position() const;
    void setPosition(const QVector3D &position);

private:
    double m_fluidDensity;
    double m_yawCrossSectionalArea;
    double m_pitchCrossSectionalArea;
    double m_coefficientSlope;
    QVector3D m_position;
};

} // namespace Physics

#endif // FORCE_H
