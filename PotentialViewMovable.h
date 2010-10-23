#pragma once
#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>

class PotentialViewMovable : public QGraphicsView
{
    Q_OBJECT
public:
    PotentialViewMovable(QGraphicsScene *scene, QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
signals:
    void infoMouseMovedTo(QPointF);
};
