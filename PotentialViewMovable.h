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
#include "myparam.h"

class EnergyDraggableLine;
class HorDraggableLine;
class VerDraggableLine; 
class EnergyLevels;
class PotentialViewMovable;

class MyGraphicsPolylineItem : public QGraphicsPolygonItem
{
    PotentialViewMovable *view;
public:
    MyGraphicsPolylineItem(const QPolygonF& p, PotentialViewMovable *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class PotentialViewMovable : public QGraphicsView
{
    Q_OBJECT
public:

    PotentialViewMovable(PhysicalModel *m, QWidget *parent = 0);
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    MyGraphicsPolylineItem *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
     
public slots:
    void slotEboundChanged();
    void slotEnergyChanged();
    void slotPsiofE(); 
    void slotUChanged();
    void setViewportMapping();
    void resizePicture();
    void setScalesFromModel();
//    void scaleXY();
    void scaleE();
    void energyAutoChanged();

signals:
    void infoMouseMovedTo(QPointF);

protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);

public:
    void showDialogScaleY();
    void showDialogViewPsiX();

private:
    double widthLineV;
    double widthLineH;
    double widthLineE;
    MyParamD Umin,Umax, xmin, xmax, psiMax, psiMin; 
    MyParamD Emin,Emax,hE;
    MyParamI viewWF;

    PhysicalModel *model;
    QGroupBox  *gbScaleXY, *gbScaleE, *gbScaleY,*gbVPsi;
    QRadioButton *rad1,*rad2,*rad3;
    QButtonGroup *bgR;


    QVector<HorDraggableLine*> linesU;
    QVector<VerDraggableLine*> linesV;

    QVector<EnergyLevels*> linesEn;
    EnergyDraggableLine* lineEnergy;
    double Edraggable;
    QMap<int,MyGraphicsPolylineItem*> curves;

    friend class HorDraggableLine;
    friend class EnergyLevels;
    friend class VerDraggableLine;
    friend class EnergyDraggableLine;
};
