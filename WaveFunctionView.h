#pragma once
#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QGraphicsItem>
#include "PhysicalModel.h"
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QMap>
#include "myparam.h"
#include "ScalePsin.h"

class WaveFunctionView;

class CoordinateDistribution : public QGraphicsPolygonItem
{
    WaveFunctionView *view;
public:
    CoordinateDistribution(const QPolygonF& p, WaveFunctionView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class WaveFunctionView : public QGraphicsView
{
    Q_OBJECT
public:

    WaveFunctionView(PhysicalModel *m, QWidget *parent = 0);
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    CoordinateDistribution *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void contextMenuEvent(QContextMenuEvent *event);

     
public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_Psi_n_of_x();
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
    void showDialogLevNum();
private:
    ScalePsin  *dialogScalePsin;
    void setScalesFromModel();
    QRectF a_old;
    double widthLineV;
    double widthLineH;
    double widthLineE;
    double psiMax, psiMin,xmin,xmax,dx; 
//    MyParamD psiMax, psiMin; 
//    MyParamD xmin,xmax;
    MyParamI   nMax, nMin, hn;  
//    int   nMax, nMin, hn;  
    QGraphicsLineItem *lineh,*linev;
    int  viewWF;  
    PhysicalModel *model;
    QGroupBox  *gbScaleXY, *gbLevNum, *gbVPsi;
    QRadioButton *rad1,*rad2,*rad3;
    QButtonGroup *bgR;
    QMap<int,CoordinateDistribution*> curves;


};
