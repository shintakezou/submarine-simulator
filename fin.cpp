#include <Qt3DCore/QScaleTransform>
#include <Qt3DRenderer/QMesh>

#include "fin.h"

Fin::Fin(Qt3D::QNode *parent) : Qt3D::QEntity(parent)
{
    auto mesh = new Qt3D::QMesh(this);
    mesh->setSource(QUrl("qrc:/models/fin.obj"));
    addComponent(mesh);
}
