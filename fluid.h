#ifndef FLUID_H
#define FLUID_H

#include <QObject>

class Fluid : public QObject
{
    Q_OBJECT

public:
    explicit Fluid(double density, QObject *parent = 0);

    Q_PROPERTY(double density READ density WRITE setDensity)

    double density() const;
    void setDensity(double density);

signals:

public slots:

private:
    double m_density;
};

#endif // FLUID_H
