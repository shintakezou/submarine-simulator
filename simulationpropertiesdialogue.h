#ifndef SIMULATIONPROPERTIESDIALOGUE_H
#define SIMULATIONPROPERTIESDIALOGUE_H

#include <QDialog>

namespace Ui {
class SimulationPropertiesDialogue;
}

class Simulation;
class Fluid;
class Submarine;

class SimulationPropertiesDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationPropertiesDialogue(QWidget *parent = 0);
    ~SimulationPropertiesDialogue();

    void loadSimulation(const Simulation *simulation);
    void saveSimulation(Simulation *simulation) const;

private:
    void loadFluid(const Fluid *fluid);
    void saveFluid(Fluid *fluid) const;

    void loadSubmarine(const Submarine *submarine);
    void saveSubmarine(Submarine *submarine) const;

public:
    bool shouldStartPaused() const;

    Q_PROPERTY(bool shouldStartPaused READ shouldStartPaused STORED false)

private:
    Ui::SimulationPropertiesDialogue *ui;
};

#endif // SIMULATIONPROPERTIESDIALOGUE_H
