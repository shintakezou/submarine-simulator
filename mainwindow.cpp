#include <QWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "simulationview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SimulationView *view = new SimulationView();
    view->show();
    QWidget *container = QWidget::createWindowContainer(view, this);
    container->setMinimumSize(320, 320);
    ui->verticalLayout->addWidget(container);
}

MainWindow::~MainWindow()
{
    delete ui;
}
