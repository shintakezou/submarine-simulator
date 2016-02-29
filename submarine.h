#ifndef SUBMARINE_H
#define SUBMARINE_H

#include <QObject>

class Submarine : public QObject
{
    Q_OBJECT
public:
    explicit Submarine(QObject *parent = 0);

signals:

public slots:
};

#endif // SUBMARINE_H