#ifndef SIMULATIONPROPERTIESDIALOGUE_H
#define SIMULATIONPROPERTIESDIALOGUE_H

#include <QDialog>

namespace Ui {
class SimulationPropertiesDialogue;
}

class SimulationPropertiesDialogue : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationPropertiesDialogue(QWidget *parent = 0);
    ~SimulationPropertiesDialogue();

private:
    Ui::SimulationPropertiesDialogue *ui;
};

#endif // SIMULATIONPROPERTIESDIALOGUE_H
