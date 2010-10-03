#pragma once
#include <QtGui>

class PotentialViewMovable : public QGraphicsView
{
    Q_OBJECT
public:
    PotentialViewMovable(QGraphicsScene *scene, QWidget *parent = 0);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
signals:
    void infoMouseMovedTo(QPointF);
};
