#ifndef FORCEARROW_H
#define FORCEARROW_H

#include <QColor>
#include <QObject>
#include <QVector3D>

namespace Qt3D {
    class QEntity;
    class QRotateTransform;
    class QTranslateTransform;
    class QScaleTransform;
}

class btVector3;

class ForceArrow : public QObject
{
    Q_OBJECT

public:
    explicit ForceArrow(QColor colour, float scale, QObject *parent = 0);
    ~ForceArrow();

    void addToScene(Qt3D::QEntity *scene);
    void update(QVector3D force, QVector3D position);
    void update(btVector3 force, btVector3 position);

private:
    QColor m_colour;
    float m_scale;

    Qt3D::QEntity *m_entity;
    Qt3D::QRotateTransform *m_rotateTransform;
    Qt3D::QTranslateTransform *m_translateTransform;
    Qt3D::QScaleTransform *m_scaleTransform;
};

#endif // FORCEARROW_H
