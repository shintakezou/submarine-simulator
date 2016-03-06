#include <QMessageBox>
#include <QWidget>
#include <QTimer>

#ifdef Q_OS_OSX
#include <QMacToolBar>
#include <QMacToolBarItem>
#endif

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

    m_simulationWidget = QWidget::createWindowContainer(m_simulation, this);
    m_simulationWidget->setMinimumSize(320, 320);
    ui->leftLayout->insertWidget(0, m_simulationWidget, 1);

    ui->chartPosition->setMinimumHeight(200);

    ui->chartAngle->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngle->graph(0)->setName("Roll");
    ui->chartAngle->addGraph()->setPen(QPen(Qt::green));  // yaw
    ui->chartAngle->graph(1)->setName("Yaw");
    ui->chartAngle->addGraph()->setPen(QPen(Qt::blue)); // pitch
    ui->chartAngle->graph(2)->setName("Pitch");
    ui->chartAngle->xAxis->setLabel("Time (s)");
    ui->chartAngle->yAxis->setLabel("Angle (°)");
    ui->chartAngle->yAxis->setRange(-20, 20);
    ui->chartAngle->legend->setVisible(true);

    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngularVelocity->graph(0)->setName("Roll");
    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::green));  // yaw
    ui->chartAngularVelocity->graph(1)->setName("Yaw");
    ui->chartAngularVelocity->addGraph()->setPen(QPen(Qt::blue)); // pitch
    ui->chartAngularVelocity->graph(2)->setName("Pitch");
    ui->chartAngularVelocity->xAxis->setLabel("Time (s)");
    ui->chartAngularVelocity->yAxis->setLabel("Angular Velocity (°/s)");
    ui->chartAngularVelocity->yAxis->setRange(-30, 30);
    ui->chartAngularVelocity->legend->setVisible(true);

    ui->chartAngleOfAttack->addGraph()->setPen(QPen(Qt::red));  // roll
    ui->chartAngleOfAttack->graph(0)->setName("Roll");
    ui->chartAngleOfAttack->addGraph()->setPen(QPen(Qt::green));  // yaw
    ui->chartAngleOfAttack->graph(1)->setName("Yaw");
    ui->chartAngleOfAttack->addGraph()->setPen(QPen(Qt::blue)); // pitch
    ui->chartAngleOfAttack->graph(2)->setName("Pitch");
    ui->chartAngleOfAttack->xAxis->setLabel("Time (s)");
    ui->chartAngleOfAttack->yAxis->setLabel("Angle of Attack (°)");
    ui->chartAngleOfAttack->yAxis->setRange(-0.5, 0.5);
    ui->chartAngleOfAttack->legend->setVisible(true);

    ui->chartLinearVelocity->addGraph()->setPen(QPen(Qt::red));  // X
    ui->chartLinearVelocity->graph(0)->setName("X");
    ui->chartLinearVelocity->addGraph()->setPen(QPen(Qt::green));  // Y
    ui->chartLinearVelocity->graph(1)->setName("Y");
    ui->chartLinearVelocity->addGraph()->setPen(QPen(Qt::blue)); // Z
    ui->chartLinearVelocity->graph(2)->setName("Z");
    ui->chartLinearVelocity->xAxis->setLabel("Time (s)");
    ui->chartLinearVelocity->yAxis->setLabel("Linear Velocity (m/s)");
    ui->chartLinearVelocity->yAxis->setRange(-3, 3);
    ui->chartLinearVelocity->legend->setVisible(true);

    ui->chartPosition->addGraph()->setPen(QPen(Qt::red));
    ui->chartPosition->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->chartPosition->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->chartPosition->graph(0)->setName("Y");
    ui->chartPosition->addGraph()->setPen(QPen(Qt::green));
    ui->chartPosition->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->chartPosition->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->chartPosition->graph(1)->setName("Z");
    ui->chartPosition->xAxis->setLabel("X (m)");
    ui->chartPosition->yAxis->setLabel("Y/Z (m)");
    ui->chartPosition->legend->setVisible(true);

    m_timer->start(25);

#ifdef Q_OS_OSX
    initialiseMacToolbar();
#endif

    playSimulation();
}

MainWindow::~MainWindow()
{
    delete m_simulation;
    delete ui;
}

void MainWindow::initialiseMacToolbar()
{
    QMacToolBar *toolBar = new QMacToolBar(this);

    QMacToolBarItem *propertiesItem = toolBar->addItem(QIcon(":/icons/properties.svg"), "Change Properties");
    connect(propertiesItem, &QMacToolBarItem::activated, this, &MainWindow::changeSimulationProperties);

    toolBar->addSeparator();

    m_playPauseItem = toolBar->addItem(QIcon(":/icons/play.svg"), "Play/Pause");

    QMacToolBarItem *restartItem = toolBar->addItem(QIcon(":/icons/restart.svg"), "Restart");
    connect(restartItem, &QMacToolBarItem::activated, this, &MainWindow::restartSimulation);

    QMacToolBarItem *stepItem = toolBar->addItem(QIcon(":/icons/step.svg"), "Step");
    connect(stepItem, &QMacToolBarItem::activated, this, &MainWindow::stepSimulation);

    window()->winId(); // create window->windowhandle()
    toolBar->attachToWindow(window()->windowHandle());

    ui->mainToolBar->hide();
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "Submarine Simulation", "A submarine simulation developed for SUHPS.\n\nIcons from Material Design by Google.");
}

void MainWindow::changeSimulationProperties() {
    SimulationPropertiesDialogue d(this);
    d.loadSimulation(m_simulation);
    if (d.exec() == QDialog::Accepted) {
        restartSimulation();
        d.saveSimulation(m_simulation);

        if (d.shouldStartPaused()) {
            pauseSimulation();
        }
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
    Submarine *submarine = m_simulation->submarine();
    double time = m_simulation->time();

    ui->chartAngle->graph(0)->addData(time, qRadiansToDegrees(submarine->body()->roll()));
    ui->chartAngle->graph(1)->addData(time, qRadiansToDegrees(submarine->body()->yaw()));
    ui->chartAngle->graph(2)->addData(time, qRadiansToDegrees(submarine->body()->pitch()));
    ui->chartAngle->xAxis->rescale();
    limitChartData(ui->chartAngle, 500);
    ui->chartAngle->replot();

    ui->chartAngularVelocity->graph(0)->addData(time, qRadiansToDegrees(submarine->body()->angularVelocity().x()));
    ui->chartAngularVelocity->graph(1)->addData(time, qRadiansToDegrees(submarine->body()->angularVelocity().y()));
    ui->chartAngularVelocity->graph(2)->addData(time, qRadiansToDegrees(submarine->body()->angularVelocity().z()));
    ui->chartAngularVelocity->xAxis->rescale();
    limitChartData(ui->chartAngularVelocity, 500);
    ui->chartAngularVelocity->replot();

    ui->chartAngleOfAttack->graph(0)->addData(time, qRadiansToDegrees(submarine->body()->rollAngleOfAttack()));
    ui->chartAngleOfAttack->graph(1)->addData(time, qRadiansToDegrees(submarine->body()->yawAngleOfAttack()));
    ui->chartAngleOfAttack->graph(2)->addData(time, qRadiansToDegrees(submarine->body()->pitchAngleOfAttack()));
    ui->chartAngleOfAttack->xAxis->rescale();
    limitChartData(ui->chartAngleOfAttack, 500);
    ui->chartAngleOfAttack->replot();

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
    connect(m_timer, &QTimer::timeout, this, &MainWindow::stepSimulation);

    ui->actionPlay->setVisible(false);
    ui->actionPause->setVisible(true);

#ifdef Q_OS_OSX
    m_playPauseItem->setIcon(QIcon(":/icons/pause.svg"));
    m_playPauseItem->setText("Pause");
    disconnect(m_playPauseItem);
    connect(m_playPauseItem, &QMacToolBarItem::activated, this, &MainWindow::pauseSimulation);
#endif
}

void MainWindow::pauseSimulation()
{
    disconnect(m_timer, &QTimer::timeout, this, &MainWindow::stepSimulation);

    ui->actionPlay->setVisible(true);
    ui->actionPause->setVisible(false);

#ifdef Q_OS_OSX
    m_playPauseItem->setIcon(QIcon(":/icons/play.svg"));
    m_playPauseItem->setText("Play");
    disconnect(m_playPauseItem);
    connect(m_playPauseItem, &QMacToolBarItem::activated, this, &MainWindow::playSimulation);
#endif
}

void MainWindow::restartSimulation()
{
    clearPlots(ui->chartAngle);
    clearPlots(ui->chartAngularVelocity);
    clearPlots(ui->chartLinearVelocity);
    clearPlots(ui->chartPosition);

    m_simulation->reset();

    stepSimulation();
}

void MainWindow::stepSimulation()
{
    m_simulation->step();
    updateCharts();
}
