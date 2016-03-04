#ifndef FIN_H
#define FIN_H

#include <Qt3DCore/QEntity>

namespace Qt3D {
class QRotateTransform;
class QTranslateTransform;
}

class Submarine;

class Fin : public Qt3D::QEntity
{
    Q_OBJECT

public:
    enum Orientation {
        North,
        East,
        South,
        West
    };

    enum Plane {
        Unknown,
        Horizontal,
        Vertical
    };

    explicit Fin(Qt3D::QNode *parent = 0);

    void calculatePosition(Submarine *submarine, Orientation orientation, float position);

    Plane plane() const;

    Q_PROPERTY(Plane plane READ plane)

private:
    Qt3D::QRotateTransform *m_rotateTransform;
    Qt3D::QTranslateTransform *m_translateTransform;

    Plane m_plane;
};

#endif // FIN_H
