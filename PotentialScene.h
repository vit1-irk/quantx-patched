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
	//void clearPotential();
	void calculateEnergies();
	void addNewSegment();
    void modelChanged();

protected:
	void drawBackground(QPainter * painter, const QRectF & rect);

    /* graphical representation */
	int nhlines;
	DraggableLine * first;
	DraggableLine * last;
	QList<QGraphicsLineItem *> lines;

    /* model reference */
    PhysicalModel *model;
//	Elevdata * eld; // energy levels
//	PotData * pdata; // definition of potential
//	void realloc();
};
#endif /* INCLUDED_POTENTIALSCENE_H */
