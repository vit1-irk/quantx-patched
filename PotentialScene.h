#ifndef INCLUDED_POTENTIALSCENE_H
#define INCLUDED_POTENTIALSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "model.h"

class DraggableLine;
class PotentialScene: public QGraphicsScene
{
	Q_OBJECT
public:

    PotentialScene(Model *, QObject * parent = 0);

    void setModel(Model*);

public slots:
	void clearPotential();
	void calculateEnergies();
	void addNewSegment();
    void modelChanged();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void drawBackground(QPainter * painter, const QRectF & rect);

    /* graphical representation */
	int nhlines;
	DraggableLine * first;
	DraggableLine * last;
	QList<QGraphicsLineItem *> lines;

    /* model reference */
    Model *model;
//	Elevdata * eld; // energy levels
//	PotData * pdata; // definition of potential
//	void realloc();
};
#endif /* INCLUDED_POTENTIALSCENE_H */
