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
#include <QGraphicsPolygonItem>
#include <QPolygonF>

class HorDraggableLine;
class VerDraggableLine;

class MyGraphicsPolylineItem : public QGraphicsPolygonItem
{
public:
    MyGraphicsPolylineItem(const QPolygonF& p, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
};

class PotentialViewMovable : public QGraphicsView
{
    Q_OBJECT
public:
    PotentialViewMovable(PhysicalModel *m, QWidget *parent = 0);
    int addCurve(const QPolygonF& );
    void replaceCurve(int id,const QPolygonF&);
    void removeCurve(int id);

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
    QVector<MyGraphicsPolylineItem*> curves;

    friend class HorDraggableLine;
    friend class VerDraggableLine;
};
