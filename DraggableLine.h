#include <QGraphicsLineItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

class PotentialScene;

class DraggableLine : public QGraphicsLineItem
{
public: 
	DraggableLine(QGraphicsScene * p);
	void SetLeft(DraggableLine * l) { left = l; } 
    void SetRight(DraggableLine * r) { right = r; }
	DraggableLine * GetLeft() const { return left; } 
	DraggableLine * GetRight() const { return right; } 

    void paint(QPainter * painter, const QStyleOptionGraphicsItem *option,QWidget *);
protected:
	DraggableLine * left;
	DraggableLine * right;
};

class HDraggableLine: public DraggableLine 
{
public:
    HDraggableLine(QPointF at,qreal other_end_x,QGraphicsScene *p);
    QPointF rightEnd() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
};

class VDraggableLine: public DraggableLine
{
public:
    VDraggableLine(QPointF at,qreal other_end_y,QGraphicsScene *p);
    QPointF lastEnd() const;
};