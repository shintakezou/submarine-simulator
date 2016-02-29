#include <QMessageBox>
#include <QWidget>
#include <QTimer>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "simulation.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_simulation = new Simulation();
    m_simulation->show();

    QWidget *container = QWidget::createWindowContainer(m_simulation, this);
    container->setMinimumSize(320, 320);
    ui->verticalLayout->addWidget(container);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, m_simulation, &Simulation::step);
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
