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
#include "LevelNumber.h"

class WaveFunctionView;

class CoordinateDistribution : public QGraphicsPolygonItem
{
    WaveFunctionView *view;
public:
    CoordinateDistribution(const QPolygonF& p, WaveFunctionView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
//    CoordinateDistribution(const QPolygon& p, WaveFunctionView *_view, QGraphicsItem *parent=0,QGraphicsScene *scene=0)
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
    virtual ~WaveFunctionView();

//    void setCurve(int id,const QPolygon&, const QPen& = QPen());
    void setCurve(int id,const QPolygonF&, const QPen& = QPen());
private:
    CoordinateDistribution *getCurve(int id) const { return curves[id]; }
    void removeCurve(int id);
    void contextMenuEvent(QContextMenuEvent *event);


public slots:
    void setViewportMapping();
    void resizePicture();
    void slot_Psi_n_of_x();
    void clearAll();
    void slotEnergyChanged();
//    void updateWidth();
signals:
    void infoMouseMovedTo(QPointF);
    void signalViewChanged();
    void signalWidthChanged();
//    void signalScalesChanged();
protected:
    void scrollView(int dx, int dy);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void scaleView(qreal scaleFactor);

public:
    void showDialogScaleY();
    void showDialogLevNum();
private:
//    void initDialogWidth();
//    void showDialogWidth();
//    void setWidth();
//    QLineEdit *leW;
//    QGroupBox  *gbWidth;
    ScalePsin  *dialogScalePsin;
    LevelNumber *dialogLevNum;
    void setScalesFromModel();
    double lineWidth;
    double psiMax, psiMin,xmin,xmax,dx;
    int   nMax, nMin, hn;
    QGraphicsLineItem *lineh,*linev,*vectorFirst,*linez;
    PhysicalModel *model;
    QMap<int,CoordinateDistribution*> curves;

    int whatToDraw;
public slots:
    void setWhatToDraw(int);
signals:
    void whatToDrawChanged(int);
public:
    int getWhatToDraw();
};

class WaveFunctionWidget : public QGroupBox
{
    Q_OBJECT
public:
    WaveFunctionWidget(PhysicalModel *model, QWidget * parent = 0);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
private:
    QButtonGroup *bgR;
public slots:
    void checkButton(int);
private:
    WaveFunctionView *waveFunctionView;
};
