#ifndef BODY_H
#define BODY_H

#include <QObject>

class btRigidBody;

namespace Physics {

class Body : public QObject
{
    Q_OBJECT

public:
    explicit Body(btRigidBody *body, QObject *parent = 0);
    ~Body();

    btRigidBody *body() const;

    double mass() const;

    Q_PROPERTY(btRigidBody *body READ body)

private:
    btRigidBody *m_body;
};

}

#endif // BODY_H
