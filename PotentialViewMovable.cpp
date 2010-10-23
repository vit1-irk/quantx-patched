#include "PotentialViewMovable.h"
#include <QGraphicsView>
#include <QMouseEvent>
#include <QMatrix>

PotentialViewMovable::PotentialViewMovable(QGraphicsScene *scene, QWidget *parent)
: QGraphicsView(scene, parent)
{
//    setViewportUpdateMode(FullViewportUpdate);
    setViewportUpdateMode(BoundingRectViewportUpdate);///
    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);///
    setTransformationAnchor(AnchorUnderMouse);///
    setResizeAnchor(AnchorViewCenter);///
       //scale(qreal(0.8), qreal(0.8));
    setWindowTitle(tr("Qu-Qu!!!!"));

    resetMatrix ();
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

void PotentialViewMovable::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void PotentialViewMovable::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void PotentialViewMovable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
 /*   case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;
        */
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
/*
        foreach (QGraphicsItem *item, scene()->items()) {
            if (qgraphicsitem_cast<Node *>(item))
                item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
        }*/
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}