#include "physics/force.h"
#include "physics/torque.h"
#include "simulation.h"
#include "fluid.h"
#include "submarine.h"

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

void SimulationPropertiesDialogue::loadSimulation(const Simulation *simulation)
{
    loadFluid(simulation->fluid());
    loadSubmarine(simulation->submarine());
}

void SimulationPropertiesDialogue::saveSimulation(Simulation *simulation) const
{
    saveFluid(simulation->fluid());
    saveSubmarine(simulation->submarine());
}

void SimulationPropertiesDialogue::loadFluid(const Fluid *fluid)
{
    ui->spinFluidDensity->setValue(fluid->density());
}

void SimulationPropertiesDialogue::saveFluid(Fluid *fluid) const
{
    fluid->setDensity(ui->spinFluidDensity->value());
}

void SimulationPropertiesDialogue::loadSubmarine(const Submarine *submarine)
{
    ui->spinSubWidth->setValue(submarine->width());
    ui->spinSubHeight->setValue(submarine->height());
    ui->spinSubLength->setValue(submarine->length());
    ui->spinSubMass->setValue(submarine->mass());
    ui->spinSubDragCoefficient->setValue(submarine->dragCoefficient());
    ui->spinSubLiftCoefficientSlope->setValue(submarine->liftCoefficientSlope());
    ui->spinSubSpinningDragCoefficient->setValue(submarine->spinningDragCoefficient());
    ui->spinSubBuoyancyPositionX->setValue(submarine->buoyancy()->position().x());
    ui->spinSubBuoyancyPositionY->setValue(submarine->buoyancy()->position().y());
    ui->spinSubBuoyancyPositionZ->setValue(submarine->buoyancy()->position().z());
    ui->spinSubWeightPositionX->setValue(submarine->weight()->position().x());
    ui->spinSubWeightPositionY->setValue(submarine->weight()->position().y());
    ui->spinSubWeightPositionZ->setValue(submarine->weight()->position().z());
    ui->spinSubThrustX->setValue(submarine->thrust()->value().x());
    ui->spinSubThrustY->setValue(submarine->thrust()->value().y());
    ui->spinSubThrustZ->setValue(submarine->thrust()->value().z());
    ui->spinSubPropellorTorque->setValue(submarine->propellorTorque()->value().x());

    ui->checkHorizontalFinsEnabled->setChecked(submarine->hasHorizontalFins());
    ui->spinHorizontalFinsArea->setValue(submarine->horizontalFinsArea());
    ui->spinHorizontalFinsDragCoefficient->setValue(submarine->horizontalFinsDragCoefficient());
    ui->spinHorizontalFinsLiftCoefficientSlope->setValue(submarine->horizontalFinsLiftCoefficientSlope());
    ui->spinHorizontalFinsPosition->setValue(submarine->horizontalFinsPosition());
    ui->spinHorizontalFinsAspectRatio->setValue(submarine->horizontalFinsAspectRatio());

    ui->checkVerticalFinsEnabled->setChecked(submarine->hasVerticalFins());
    ui->spinVerticalFinsArea->setValue(submarine->verticalFinsArea());
    ui->spinVerticalFinsDragCoefficient->setValue(submarine->verticalFinsDragCoefficient());
    ui->spinVerticalFinsLiftCoefficientSlope->setValue(submarine->verticalFinsLiftCoefficientSlope());
    ui->spinVerticalFinsPosition->setValue(submarine->verticalFinsPosition());
    ui->spinVerticalFinsAspectRatio->setValue(submarine->verticalFinsAspectRatio());
}

void SimulationPropertiesDialogue::saveSubmarine(Submarine *submarine) const
{
    submarine->setWidth(ui->spinSubWidth->value());
    submarine->setHeight(ui->spinSubHeight->value());
    submarine->setLength(ui->spinSubLength->value());
    submarine->setMass(ui->spinSubMass->value());
    submarine->setDragCoefficient(ui->spinSubDragCoefficient->value());
    submarine->setLiftCoefficientSlope(ui->spinSubLiftCoefficientSlope->value());
    submarine->setSpinningDragCoefficient(ui->spinSubSpinningDragCoefficient->value());
    submarine->buoyancy()->setPosition(QVector3D(ui->spinSubBuoyancyPositionX->value(),
                                                 ui->spinSubBuoyancyPositionY->value(),
                                                 ui->spinSubBuoyancyPositionZ->value()));
    submarine->weight()->setPosition(QVector3D(ui->spinSubWeightPositionX->value(),
                                               ui->spinSubWeightPositionY->value(),
                                               ui->spinSubWeightPositionZ->value()));
    submarine->thrust()->setValue(QVector3D(ui->spinSubThrustX->value(),
                                            ui->spinSubThrustY->value(),
                                            ui->spinSubThrustZ->value()));
    submarine->propellorTorque()->setValue(QVector3D(ui->spinSubPropellorTorque->value(), 0, 0));

    submarine->setHasHorizontalFins(ui->checkHorizontalFinsEnabled->isChecked());
    submarine->setHorizontalFinsArea(ui->spinHorizontalFinsArea->value());
    submarine->setHorizontalFinsDragCoefficient(ui->spinHorizontalFinsDragCoefficient->value());
    submarine->setHorizontalFinsLiftCoefficientSlope(ui->spinHorizontalFinsLiftCoefficientSlope->value());
    submarine->setHorizontalFinsPosition(ui->spinHorizontalFinsPosition->value());
    submarine->setHorizontalFinsAspectRatio(ui->spinHorizontalFinsAspectRatio->value());

    submarine->setHasVerticalFins(ui->checkVerticalFinsEnabled->isChecked());
    submarine->setVerticalFinsArea(ui->spinVerticalFinsArea->value());
    submarine->setVerticalFinsDragCoefficient(ui->spinVerticalFinsDragCoefficient->value());
    submarine->setVerticalFinsLiftCoefficientSlope(ui->spinVerticalFinsLiftCoefficientSlope->value());
    submarine->setVerticalFinsPosition(ui->spinVerticalFinsPosition->value());
    submarine->setVerticalFinsAspectRatio(ui->spinVerticalFinsAspectRatio->value());
}
