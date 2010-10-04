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
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
signals:
    void infoMouseMovedTo(QPointF);
};
