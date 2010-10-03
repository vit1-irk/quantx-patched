#include "PotentialViewMovable.h"
#include <QGraphicsView>
#include <QMouseEvent>
#include <QMatrix>

PotentialViewMovable::PotentialViewMovable(QGraphicsScene *scene, QWidget *parent)
: QGraphicsView(scene, parent)
{
}

void PotentialViewMovable::resizeEvent(QResizeEvent*)
{
    if (!scene()) return;
    QRectF b = scene()->sceneRect();
    QRectF a = QRectF(this->viewport()->rect());
    qreal m11 = a.width() / b.width();
    qreal m22 = - a.height() / b.height();
    qreal dx = - m11 * a.x();
    qreal dy = - m22 * (a.y() + a.height());
    QMatrix m(m11,0,0,m22,dx,dy);
    this->setMatrix(m);
}

void PotentialViewMovable::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
