#pragma once
#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
//#include "DraggableLine.h"
#include <QPointF>
#include <QGraphicsItem>
#include "PhysicalModel.h"

class HorDraggableLine;
class VerDraggableLine;

class PotentialViewMovable : public QGraphicsView
{
    Q_OBJECT
public:
    PotentialViewMovable(PhysicalModel *m, QWidget *parent = 0);

public slots:
    void slotEboundChanged();
    void slotUChanged();

signals:
    void infoMouseMovedTo(QPointF);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void setMyTransformMatrix();

private:
    PhysicalModel *model;
    QVector<HorDraggableLine*> linesEn;
    QVector<HorDraggableLine*> linesU;
    QVector<VerDraggableLine*> linesV;

    friend class HorDraggableLine;
    friend class VerDraggableLine;
};
