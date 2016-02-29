#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <Qt3DRenderer/QWindow>

namespace Qt3D {
    class QInputAspect;
    class QEntity;
    class QCamera;
}

class SimulationView : public Qt3D::QWindow
{
public:
    SimulationView();

private:
    Qt3D::QInputAspect *m_input;
    Qt3D::QEntity *m_rootEntity;
};

#endif // SIMULATIONVIEW_H
