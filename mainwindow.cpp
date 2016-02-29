#include <QMessageBox>
#include <QWidget>
#include <QTimer>

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

    m_simulation = new Simulation(this);
    m_simulation->show();

    QWidget *container = QWidget::createWindowContainer(m_simulation, this);
    container->setMinimumSize(320, 320);
    ui->horizontalLayout->insertWidget(0, container);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, m_simulation, &Simulation::step);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateCharts);
    m_timer->start(25);

    ui->chartAngle->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngle->addGraph()->setPen(QPen(Qt::green));  // pitch
    ui->chartAngle->addGraph()->setPen(QPen(Qt::blue)); // yaw
    ui->chartAngle->xAxis->setLabel("Time (s)");
    ui->chartAngle->yAxis->setLabel("Angle (rad)");
    ui->chartAngle->yAxis->setRange(-M_PI / 2., M_PI / 2.);

    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::green));  // pitch
    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::blue)); // yaw
    ui->chartAngularVelocity->xAxis->setLabel("Time (s)");
    ui->chartAngularVelocity->yAxis->setLabel("Angular Velocity (rad/s)");
    ui->chartAngularVelocity->yAxis->setRange(-5, 5);

    ui->chartPosition->addGraph()->setPen(QPen(Qt::black));
    ui->chartPosition->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->chartPosition->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->chartPosition->xAxis->setLabel("X (m)");
    ui->chartPosition->yAxis->setLabel("Y (m)");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "Submarine Simulation", "A submarine simulation developed for SUHPS.");
}

void MainWindow::changeSimulationProperties() {
    SimulationPropertiesDialogue d(this);
    d.exec();
}

void limitChartData(QCustomPlot *plot, int length) {
    for (int i = 0; i < plot->graphCount(); i++) {
        QCPGraph *graph = plot->graph(i);
        while (graph->data()->size() > length) {
            graph->data()->remove(graph->data()->firstKey());
        }
    }
}

void MainWindow::updateCharts() {
    Submarine *submarine = m_simulation->submarine();
    double time = m_simulation->time();

    ui->chartAngle->graph(0)->addData(time, submarine->roll());
    ui->chartAngle->graph(1)->addData(time, submarine->pitch());
    ui->chartAngle->graph(2)->addData(time, submarine->yaw());
    ui->chartAngle->xAxis->rescale();
    limitChartData(ui->chartAngle, 500);
    ui->chartAngle->replot();

    ui->chartAngularVelocity->graph(0)->addData(time, submarine->angularVelocity().x());
    ui->chartAngularVelocity->graph(1)->addData(time, submarine->angularVelocity().y());
    ui->chartAngularVelocity->graph(2)->addData(time, submarine->angularVelocity().z());
    ui->chartAngularVelocity->xAxis->rescale();
    limitChartData(ui->chartAngularVelocity, 500);
    ui->chartAngularVelocity->replot();

    ui->chartPosition->graph(0)->addData(submarine->position().x(), submarine->position().y());
    ui->chartPosition->xAxis->rescale();
    ui->chartPosition->yAxis->rescale();
    limitChartData(ui->chartPosition, 2000);
    ui->chartPosition->replot();
}
