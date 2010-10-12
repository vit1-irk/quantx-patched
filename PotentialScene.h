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
