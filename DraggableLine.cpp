#include <QStyleOption>
#include <QGraphicsScene>
#include <QMessageBox>
#include "PhysicalModel.h"
#include "PotentialScene.h"
#include "DraggableLine.h"

//***************** Base Class *******************************
DraggableLine::DraggableLine(PhysicalModel *_model, qreal len,PotentialScene * p)
:length(len),adj(5),left(0),right(0),parent(p),model(_model)

//DraggableLine::DraggableLine(qreal len,PotentialScene * p): adj(5),left(0),right(0),parent(p)
//DraggableLine::DraggableLine(qreal len,PotentialScene * p):length(len),adj(5),left(0),right(0),parent(p)
{
    //    length = len > 0 ? len : -len;
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
    //	setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
}

QVariant DraggableLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && isSelected() ) 
    {
        if(!scene()->sceneRect().contains(value.toPointF())) return pos();
        return RestrictMotion(value.toPointF());
    }
    return QGraphicsItem::itemChange(change, value);
}

void DraggableLine::SetLeft(DraggableLine * l){left = l;} 
void DraggableLine::SetRight(DraggableLine * r){right = r;}

bool DraggableLine::CheckStart(QPointF point){ return true;}
bool DraggableLine::CheckEnd(QPointF point){return true;}

void DraggableLine::SetLength(qreal len)
{
    prepareGeometryChange(); 
    length = len;
}


//*************** Concrete Classes ****************************
// **************** Horizontal Line ************************
HDraggableLine::HDraggableLine(PhysicalModel *_model, qreal len,PotentialScene * p)
:DraggableLine(model, len,p), model (_model)
{ 
    setCursor(Qt::SizeVerCursor); 
}

QRectF  HDraggableLine::boundingRect() const {return QRectF(-adj,-adj,length+2*adj,2*adj);}	
QPointF HDraggableLine::RestrictMotion(QPointF point) 
{ 
    point.setX(pos().x()); 
    if(left) if(!left->CheckEnd(point)) return pos();
    if(right) if(!right->CheckStart(point+QPointF(length,0))) return pos();
    if(left) left->SetEnd(point);
    if(right) right->SetStart(point+QPointF(length,0));
    parent->calculateEnergies();
    return point;
}
void HDraggableLine::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,QWidget * widget)
{
    if(option->state & QStyle::State_Selected)painter->setPen(Qt::red); else painter->setPen(Qt::black);
    painter->drawLine(QPointF(0,0),QPointF(length,0));
    //    painter->drawLine(0,0,length,0);
}

bool HDraggableLine::CheckStart(QPointF point) { if(length + pos().x() - point.x() < 0) return false; return true;}
bool HDraggableLine::CheckEnd  (QPointF point) { if(point.x() - pos().x() < 0) return false; return true; }

void HDraggableLine::SetStart(QPointF point) { SetLength(length + pos().x() - point.x()); setPos(point); }
void HDraggableLine::SetEnd(QPointF point) { SetLength(point.x()-pos().x()); }

// **************** Vertical Line ************************
VDraggableLine::VDraggableLine( qreal len,PotentialScene * p)
:DraggableLine(model, len,p){ setCursor(Qt::SizeHorCursor); }
QRectF  VDraggableLine::boundingRect() const { return QRectF(-adj,(length>0?-adj:length-adj),2*adj,qAbs(length)+2*adj); }	
QPointF VDraggableLine::RestrictMotion(QPointF point) 
{ 
    point.setY(pos().y()); 
    if(right) if(!right->CheckStart(point+QPointF(0,length))) return pos();
    if(left) if(!left->CheckEnd(point)) return pos();
    if(right) right->SetStart(point+QPointF(0,length));
    if(left)  left ->SetEnd(point);
    parent->calculateEnergies();
    return point;
}
void VDraggableLine::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,QWidget * widget)
{
    //    QPen p;
    if(option->state & QStyle::State_Selected)
    {
        painter->setPen(Qt::red);
    }
    else 
    {
        painter->setPen(Qt::black);
    }
    //    p.setWidth(2);
    //    painter->setPen(p);
    painter->drawLine(QPointF(0,0),QPointF(0,length));
    //    painter->drawLine(0,0,0,length);
}

void VDraggableLine::SetStart(QPointF point) { SetLength( length + pos().y() - point.y()); setPos(point); }
void VDraggableLine::SetEnd(QPointF point) { SetLength( point.y()-pos().y()); }

//***************** Infinite Line ********************************************
InfDraggableLine::InfDraggableLine(PhysicalModel *_model, PotentialScene * p) : model(_model), HDraggableLine(_model,100, p) // было 100
{
    setFlag(QGraphicsItem::ItemIsMovable,false);		
    setFlag(QGraphicsItem::ItemIsSelectable,false);		
}

bool InfDraggableLine::CheckStart(QPointF point) {return true;}
bool InfDraggableLine::CheckEnd  (QPointF point) {return true;}

void InfDraggableLine::SetStart(QPointF point) { setPos(point); }
void InfDraggableLine::SetEnd(QPointF point)   {setPos(point+QPointF(-length,0));}
