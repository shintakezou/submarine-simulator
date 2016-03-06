#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Simulation;
class QTimer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showAbout();
    void changeSimulationProperties();
    void updateCharts();

    void playSimulation();
    void pauseSimulation();
    void restartSimulation();
    void stepSimulation();

private:
    Ui::MainWindow *ui;
    Simulation *m_simulation;
    QWidget *m_simulationWidget;
    QTimer *m_timer;
};

#endif // MAINWINDOW_H
