#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>

class Simulation : public QObject
{
    Q_OBJECT
public:
    explicit Simulation(QObject *parent = 0);

signals:

public slots:
};

#endif // SIMULATION_H