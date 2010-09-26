#include <QGraphicsItem>
#include <QPainter>

class PotentialScene;

class DraggableLine: public QGraphicsItem
{
public: 
	DraggableLine(Model *,qreal len,PotentialScene * p);
	virtual void SetStart(QPointF point) = 0;
	virtual void SetEnd(QPointF point) = 0;
	virtual bool CheckStart(QPointF point);
	virtual bool CheckEnd(QPointF point);
protected:
	virtual QPointF RestrictMotion(QPointF point) = 0;

	QVariant itemChange(GraphicsItemChange change, const QVariant &value);	

    /* model reference */
    Model *model;

	qreal length;
	qreal adj;
	DraggableLine * left;
	DraggableLine * right;
	PotentialScene * parent;
public:
	virtual void SetLeft(DraggableLine * l); 
	virtual void SetRight(DraggableLine * r); 
	DraggableLine * GetLeft() {return left;} 
	DraggableLine * GetRight() {return right;} 
	qreal GetLength(){return length;}
	void SetLength(qreal len);
};

class HDraggableLine: public DraggableLine 
{
public:
	HDraggableLine(Model *,qreal len,PotentialScene * p);
	QRectF boundingRect() const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,QWidget * widget);
protected:
    /* model reference */
    Model *model;

    QPointF RestrictMotion(QPointF point);
	virtual void SetStart(QPointF point);
	virtual void SetEnd(QPointF point);
	virtual bool CheckStart(QPointF point);
	virtual bool CheckEnd(QPointF point);
};

class VDraggableLine: public DraggableLine
{
public:
	VDraggableLine(qreal len,PotentialScene * p);
	QRectF boundingRect() const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,QWidget * widget);
protected:
	QPointF RestrictMotion(QPointF point);
	virtual void SetStart(QPointF point);
	virtual void SetEnd(QPointF point);
};

class InfDraggableLine: public HDraggableLine 
{
public:
	InfDraggableLine(Model *,PotentialScene * p);
protected:
    Model *model;
	virtual void SetStart(QPointF point);
	virtual void SetEnd(QPointF point);
	virtual bool CheckStart(QPointF point);
	virtual bool CheckEnd(QPointF point);
};

