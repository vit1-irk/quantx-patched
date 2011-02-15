#pragma once
#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QGraphicsItem>
#include "TimeView.h"
#include "PhysicalModel.h"
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QMap>
#include "myparam.h"

class WavePacketXView;

class CoordinateDistributionCurve : public QGraphicsPolygonItem
{
    WavePacketXView *view;
public:
    CoordinateDistributionCurve(const QPolygonF& p, WavePacketXView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class WavePacketXView : public QGraphicsView
{
    Q_OBJECT
public:

    WavePacketXView(PhysicalModel *m, QWidget *parent = 0);
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
    CoordinateDistributionCurve *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);

public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_WavePacket_of_t();
    void slot_rebuildWavePacket();
//    void slotSetTime();

//    void defSP();
//    void defWP();
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
    void showDialogWavePacket();
    void showDialogViewPsiX();
private:
    double widthLineV;
    double widthLineH;
    double widthLineE;
    MyParamD psiMax, psiMin; 
    MyParamD xmin,xmax;
    MyParamD tmin,tmax, time, htime;
    MyParamI nmaxWP,nminWP,hnWP;  
//    MyParamI nMax, nMin, hn;  
    MyParamD wpE_lo, wpE_hi, wpN;
    QGraphicsLineItem *lineh,*linev;
    MyParamI viewWF;  
    PhysicalModel *model;
    QGroupBox  *gbScaleXY, *gbWP, *gbWPl, *gbWPr, *gbVPsi;//, *gbLevNum;
    QGroupBox *grrb;
    QPushButton *butTime;
    TimeView *dialogTime;
    //    QRadioButton *rad1,*rad2;
    QButtonGroup *bgR,*bgRVF;
    bool need_build_WavePacket;
    QMap<int,CoordinateDistributionCurve*> curves;
};
