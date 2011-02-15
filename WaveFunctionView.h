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
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class WaveFunctionView : public QGraphicsView
{
    Q_OBJECT
public:

    WaveFunctionView(PhysicalModel *m, QWidget *parent = 0);
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    CoordinateDistribution *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
     
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
//    void defSP();
//    void defWP();
private:
    QRectF a_old;
    double widthLineV;
    double widthLineH;
    double widthLineE;
    MyParamD psiMax, psiMin; 
    MyParamD xmin,xmax;
//    MyParamD tmin,tmax, time, htime;
//    MyParamI nmaxWP,nminWP,hnWP;  
    MyParamI nMax, nMin, hn;  
//    MyParamD wpE_lo, wpE_hi, wpN;
    QGraphicsLineItem *lineh,*linev;
    MyParamI viewWF;  
    PhysicalModel *model;
    QGroupBox  *gbScaleXY, *gbLevNum, *gbVPsi;
//    QGroupBox  *gbScaleXY, *gbWP, *gbWPl, *gbWPr, *gbLevNum;
//    QGroupBox *grrb;
    QRadioButton *rad1,*rad2,*rad3;
    QButtonGroup *bgR;
//    bool need_build_WavePacket;
    QMap<int,CoordinateDistribution*> curves;


};
