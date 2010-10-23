/*#ifndef POTENTIALWIDGET_H
#define POTENTIALWIDGET_H

#include <QtGui/QGraphicsView>

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "PhysicalModel.h"

class DraggableLine;

class PotentialWidget : public QGraphicsView
{
    Q_OBJECT
class PotentialScene: public QGraphicsScene
{
	Q_OBJECT
public:
    PotentialScene(PhysicalModel *, QObject * parent = 0);

public slots:
	void addNewSegment();

    void updatePhysicalModel();
    void drawU();
    void redrawU();
    void redrawEn();
//--
public:
    GraphWidget();

    void itemMoved();

protected:
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);
    void wheelEvent(QWheelEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void scaleView(qreal scaleFactor);

private:
    int timerId;
    Node *centerNode;
};

#endif
*/
///-----------------------------------
#ifndef INCLUDED_POTENTIALSCENE_H
#define INCLUDED_POTENTIALSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "PhysicalModel.h"

class DraggableLine;

class PotentialScene: public QGraphicsScene
{
	Q_OBJECT
public:
    PotentialScene(PhysicalModel *, QObject * parent = 0);

public slots:
	void addNewSegment();

    void updatePhysicalModel();
    void drawU();
    void redrawU();
    void redrawEn();

protected:
	void drawBackground(QPainter * painter, const QRectF & rect);

    /* graphical representation */
	QVector<DraggableLine *> linesU;
    QVector<DraggableLine *> linesV;
	QVector<QGraphicsLineItem *> linesEn;

    /* model reference */
    PhysicalModel *model;

};
#endif /* INCLUDED_POTENTIALSCENE_H */
