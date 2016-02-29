#include "simulationpropertiesdialogue.h"
#include "ui_simulationpropertiesdialogue.h"

SimulationPropertiesDialogue::SimulationPropertiesDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationPropertiesDialogue)
{
    ui->setupUi(this);
}

SimulationPropertiesDialogue::~SimulationPropertiesDialogue()
{
    delete ui;
}
