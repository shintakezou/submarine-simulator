#include "fluid.h"

Fluid::Fluid(QObject *parent) : QObject(parent)
{

}

Fluid *Fluid::makeDefault(QObject *parent) {
    Fluid *fluid = new Fluid(parent);
    fluid->setDensity(1000);  // water
    return fluid;
}

double Fluid::density() const
{
    return m_density;
}

void Fluid::setDensity(double density)
{
    m_density = density;
}
