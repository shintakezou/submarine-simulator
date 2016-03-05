#include <QMessageBox>
#include <QWidget>
#include <QTimer>

#include "physics/body.h"
#include "simulationpropertiesdialogue.h"
#include "submarine.h"
#include "simulation.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_timer = new QTimer(this);

    m_simulation = new Simulation();
    m_simulation->show();

    connect(m_timer, &QTimer::timeout, m_simulation, &Simulation::step);

    m_simulationWidget = QWidget::createWindowContainer(m_simulation, this);
    m_simulationWidget->setMinimumSize(320, 320);
    ui->horizontalLayout->insertWidget(0, m_simulationWidget);

    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateCharts);

    ui->chartAngle->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngle->addGraph()->setPen(QPen(Qt::green));  // pitch
    ui->chartAngle->addGraph()->setPen(QPen(Qt::blue)); // yaw
    ui->chartAngle->xAxis->setLabel("Time (s)");
    ui->chartAngle->yAxis->setLabel("Angle (°)");
    ui->chartAngle->yAxis->setRange(-30, 30);

    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::green));  // pitch
    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::blue)); // yaw
    ui->chartAngularVelocity->xAxis->setLabel("Time (s)");
    ui->chartAngularVelocity->yAxis->setLabel("Angular Velocity (rad/s)");
    ui->chartAngularVelocity->yAxis->setRange(-M_PI / 4., M_PI / 4.);

    ui->chartLinearVelocity->addGraph()->setPen(QPen(Qt::red));  // X
    ui->chartLinearVelocity->addGraph()->setPen(QPen(Qt::green));  // Y
    ui->chartLinearVelocity->addGraph()->setPen(QPen(Qt::blue)); // Z
    ui->chartLinearVelocity->xAxis->setLabel("Time (s)");
    ui->chartLinearVelocity->yAxis->setLabel("Linear Velocity (m/s)");
    ui->chartLinearVelocity->yAxis->setRange(-5, 5);

    ui->chartPosition->addGraph()->setPen(QPen(Qt::red));
    ui->chartPosition->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->chartPosition->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->chartPosition->addGraph()->setPen(QPen(Qt::green));
    ui->chartPosition->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->chartPosition->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->chartPosition->xAxis->setLabel("X (m)");
    ui->chartPosition->yAxis->setLabel("Y/Z (m)");

    m_timer->start(25);
}

MainWindow::~MainWindow()
{
    delete m_simulation;
    delete ui;
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "Submarine Simulation", "A submarine simulation developed for SUHPS.");
}

void MainWindow::changeSimulationProperties() {
    SimulationPropertiesDialogue d(this);
    d.loadSimulation(m_simulation);
    if (d.exec() == QDialog::Accepted) {
        restartSimulation();
        d.saveSimulation(m_simulation);
    }
}

void limitChartData(QCustomPlot *plot, int length) {
    for (int i = 0; i < plot->graphCount(); i++) {
        QCPGraph *graph = plot->graph(i);
        while (graph->data()->size() > length) {
            graph->data()->remove(graph->data()->firstKey());
        }
    }
}

void clearPlots(QCustomPlot *plot) {
    for (int i = 0; i < plot->graphCount(); i++) {
        QCPGraph *graph = plot->graph(i);
        graph->clearData();
    }
}

void MainWindow::updateCharts() {
    if (m_simulation->paused()) {
        return;
    }

    Submarine *submarine = m_simulation->submarine();
    double time = m_simulation->time();

    ui->chartAngle->graph(0)->addData(time, qRadiansToDegrees(submarine->body()->roll()));
    ui->chartAngle->graph(1)->addData(time, qRadiansToDegrees(submarine->body()->pitch()));
    ui->chartAngle->graph(2)->addData(time, qRadiansToDegrees(submarine->body()->yaw()));
    ui->chartAngle->xAxis->rescale();
    limitChartData(ui->chartAngle, 500);
    ui->chartAngle->replot();

    ui->chartAngularVelocity->graph(0)->addData(time, submarine->body()->angularVelocity().x());
    ui->chartAngularVelocity->graph(1)->addData(time, submarine->body()->angularVelocity().y());
    ui->chartAngularVelocity->graph(2)->addData(time, submarine->body()->angularVelocity().z());
    ui->chartAngularVelocity->xAxis->rescale();
    limitChartData(ui->chartAngularVelocity, 500);
    ui->chartAngularVelocity->replot();

    ui->chartLinearVelocity->graph(0)->addData(time, submarine->body()->linearVelocity().x());
    ui->chartLinearVelocity->graph(1)->addData(time, submarine->body()->linearVelocity().y());
    ui->chartLinearVelocity->graph(2)->addData(time, submarine->body()->linearVelocity().z());
    ui->chartLinearVelocity->xAxis->rescale();
    limitChartData(ui->chartLinearVelocity, 500);
    ui->chartLinearVelocity->replot();

    ui->chartPosition->graph(0)->addData(submarine->body()->position().x(), submarine->body()->position().y());
    ui->chartPosition->graph(1)->addData(submarine->body()->position().x(), submarine->body()->position().z());
    ui->chartPosition->xAxis->rescale();
    ui->chartPosition->yAxis->rescale();
    limitChartData(ui->chartPosition, 2000);
    ui->chartPosition->replot();
}

void MainWindow::playSimulation()
{
    m_simulation->play();
}

void MainWindow::pauseSimulation()
{
    m_simulation->pause();
}

void MainWindow::restartSimulation()
{
    clearPlots(ui->chartAngle);
    clearPlots(ui->chartAngularVelocity);
    clearPlots(ui->chartLinearVelocity);
    clearPlots(ui->chartPosition);

    m_simulation->restart();
}
