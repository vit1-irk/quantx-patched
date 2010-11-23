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
#include <QMap>

class EnergyDraggableLine;
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
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    MyGraphicsPolylineItem *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    double widthLineV;
    double widthLineH;
    double widthLineE;

public slots:
    void slotEboundChanged();
    void slotEnergyChanged();
    void slotUChanged();
    void setViewportMapping();
    void resizePicture();

signals:
    void infoMouseMovedTo(QPointF);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);

private:
    PhysicalModel *model;

//    double widthLineV;
//    double widthLineH;
//    double widthLineE;

    QVector<HorDraggableLine*> linesU;
    QVector<VerDraggableLine*> linesV;

    QVector<HorDraggableLine*> linesEn;
    EnergyDraggableLine* lineEnergy;
    double Edraggable;
    QMap<int,MyGraphicsPolylineItem*> curves;

    friend class HorDraggableLine;
    friend class VerDraggableLine;
    friend class EnergyDraggableLine;
};
