#include "fluid.h"

Fluid::Fluid(double density, QObject *parent) : QObject(parent)
{
    m_density = density;
}

double Fluid::density() const
{
    return m_density;
}

void Fluid::setDensity(double density)
{
    m_density = density;
}
