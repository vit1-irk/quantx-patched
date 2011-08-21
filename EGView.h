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
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "myparam.h"
#include "BreakStatus.h"
#include "GView.h"

class EGView;
class gDraggable;

class EGCurve : public QGraphicsPolygonItem
{
    EGView *view;
public:
    EGCurve(const QPolygonF& p, EGView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class EGView : public QGraphicsView
{
    Q_OBJECT
public:

    EGView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~EGView();

    void setCurve(int id,const QPolygonF& pcurve,const QPolygonF& curve, const QPen& = QPen());
    void removeCurve(int id);
public:
    void showDialogScaleY();
    bool Erase;

public slots:
//    void redrawCurves();
    void setViewportMapping();
    void resizePicture();
//    void slot_En_of_z();
    void slot_Ec_n();
    void slotGLine();
    void updateScaleEG();
    void slot_drawEc_n();
signals:
    void infoMouseMovedTo(QPointF);
    void signalScaleEGChanged();
protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    void clearAll();

public:
    void showDialogG();
    QPair<double,double> getEGScale();
    void setEGScale(const QPair<double,double> &s);
    PhysicalModel *model;
    double Emin, Emax, hE;

private:
    void initDialogScaleY();
    QLineEdit *leEmin;
    QLineEdit *leEmax;
    QLineEdit *leHE;
    QGraphicsRectItem *rectFirst;
//    double Emin, Emax, hE;
    void setScaleEG();
    QGroupBox  *gbScaleXY;
    double lineWidth;
    int numberOfCurves;
    QVector<QPolygonF> adjCurves;
    QVector<QPolygonF> physCurves;
    QVector<QPolygonF> adjCurvesI;
    QVector<QPolygonF> physCurvesI;
    QVector<QPolygonF> adjCurvesR;
    QVector<QPolygonF> physCurvesR;

    int curve_number;
    double Gmax, Gmin, dG, GG;
    QGraphicsLineItem *lineh,*linev;
    QGraphicsRectItem *rectEG;

    void contextMenuEvent(QContextMenuEvent *event);
    gDraggable *lineG;

    friend class gDraggable;

//    PhysicalModel *model;
    GView *dialogG;

    QMap<int,EGCurve*> curves;
    QMap<int,QPolygonF> pcurves;

};
class EGWidget : public QGroupBox
{
    Q_OBJECT
public:
    EGWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
public slots:
    void slotRunEG();
    void slotErase();
private:
    EGView *egView;
    QToolButton *bRunEG;
};
