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
#include "Zview.h"

class EnzView;
class ZDraggable;

class EnzCurve : public QGraphicsPolygonItem
{
    EnzView *view;
public:
    EnzCurve(const QPolygonF& p, EnzView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
        : QGraphicsPolygonItem(p,parent,scene), view(_view)
    {
    }
    void paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
//    void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

class EnzView : public QGraphicsView
{
    Q_OBJECT
public:

    EnzView(PhysicalModel *m, QWidget *parent = 0);
//    virtual ~EnzView();

    void setCurve(int id,const QPolygonF& pcurve,const QPolygonF& curve, const QPen& = QPen());
    void removeCurve(int id);
//    void redrawCurves();
public:
    void showDialogScaleY();
    bool Erase;

public slots:
    void redrawCurves();
    void setViewportMapping();
    void resizePicture();
    void slot_En_of_z();
    void slot_Ec_n();
    void slotZline();
    void updateScaleEnz();
    void slot_drawEc_n();
signals:
    void infoMouseMovedTo(QPointF);
    void signalScaleEnzChanged();
protected:
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);
    void setScalesFromModel();
    void clearAll();

public:
    void showDialogZ();
    QPair<double,double> getEnzMinMax();
    void setEnzMinMax(const QPair<double,double> &s);

//    getScalesEnz();
private:
    void initDialogScaleY();
    QLineEdit *leEnzmin;
    QLineEdit *leEnzmax;
    double Enzmin, Enzmax;
    void setScaleEnz();
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
    double zmax, zmin, hz;
//    double Umin, Umax;
    QGraphicsLineItem *lineh,*linev;
    QGraphicsRectItem *rectEG;
//    QVector<QPointF*> pointEG;

    void contextMenuEvent(QContextMenuEvent *event);
    ZDraggable *lineZ;

    friend class ZDraggable;

    PhysicalModel *model;
    Zview *dialogZ;

    QMap<int,EnzCurve*> curves;
    QMap<int,QPolygonF> pcurves;

};
class EnzWidget : public QGroupBox
{
    Q_OBJECT
public:
    EnzWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
public slots:
    void slotRunEnz();
    void slotErase();
private:
    EnzView *enzView;
    QToolButton *bRunEnz;
};
