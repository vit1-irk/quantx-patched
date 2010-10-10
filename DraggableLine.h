#pragma once
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

class PotentialScene;

class DraggableLine : public QGraphicsItem
{
public: 
	DraggableLine(QGraphicsItem *parent = 0);
	void SetLeft(DraggableLine * l) { left = l; } 
    void SetRight(DraggableLine * r) { right = r; }
    void SetP1(QPointF p1) { prepareGeometryChange(); this->p1 = p1; }
    void SetP2(QPointF p2) { prepareGeometryChange(); this->p2 = p2; }

    void paint(QPainter * painter, const QStyleOptionGraphicsItem *option,QWidget *);
    QPointF p1, p2;

protected:
	DraggableLine * left;
	DraggableLine * right;
};

class HDraggableLine: public DraggableLine 
{
public:
    HDraggableLine(QPointF at,qreal other_end_x,QGraphicsItem *parent = 0);
    QPointF rightEnd() const { return p2; }


    //reimplementations
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    QRectF boundingRect() const;
};

class VDraggableLine: public DraggableLine
{
public:
    VDraggableLine(QPointF at,qreal other_end_y,QGraphicsItem *parent = 0);
    QPointF lastEnd() const { return p2; }

    //reimplementations
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    QRectF boundingRect() const;
};