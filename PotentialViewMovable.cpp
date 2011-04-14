#include "PotentialViewMovable.h"
#include "MainWindow.h"
#include <QGraphicsView>
#include <QMouseEvent>
#include <QMatrix>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QRectF>
#include <QLineF>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QPainterPath>

// HorDraggableLine places a horizontal line of length width()
// positioned by its left side at pos().
class EnergyDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
    double widthLineE;
    EnergyDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double width)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || width != p2.x())
        {
            prepareGeometryChange();
            p2 = QPointF(width,0);
            this->setPos(x1,y1);
            update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
        update();//repaint();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
        this->setSelected(false);
        update();//repaint();
    }
    double EnergyDraggableLine::getEnergyFromLine()
    {
        QPointF p = pos();
        return p.y(); 
    }
    QRectF boundingRect() const
    {
    QRectF vp = view->sceneRect();
/*    double widthLine= 0.005*vp.height();
        QPoint v1(0,0);
        QPoint v2(0,5);
        QPointF fv1 = view->mapToScene(v1);
        QPointF fv2 = view->mapToScene(v2);
        double widthLine1 = fabs(fv2.y() - fv1.y());
 */   
        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        QRect vpr = view->viewport()->rect();
        QPointF vpr1 = view->mapToScene(vpr.topLeft());
        QPointF vpr2 = view->mapToScene(vpr.bottomRight());
        QMatrix m = view->matrix();
        QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
        return aa;///QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
/*        QPoint va(0,0);
        QPoint vb(0,view->widthLine);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ay = fabs(sa.y() - sb.y());
        double ax = fabs(sa.x() - sb.x());
        view->widthLineE=ay;*/
        penForPainter.setWidthF(view->widthLineE);
        penForPainter.setCapStyle(Qt::FlatCap);

//        penForPainter.setWidthF(ay);
//        penForPainter.setWidthF(view->widthLineE);
        painter->setPen(penForPainter);
        qreal x2 = penForPainter.widthF();
/*        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        */
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
            QMenu m;
//            QAction *scaleX = m.addAction("Scale x");
//            QAction *scaleE = m.addAction(m.tr("Scale E"));
            QAction *what = m.exec(event->screenPos());
//            if (what == scaleX)
//            {
//                view->askScaleX();
//            }
/*            if (what == scaleE)
            {
                view->scaleE();
            }*/
               update();// repaint();
        }
    }

};
/*void  PotentialViewMovable::scaleE()
{   
    if (!gbScaleE) 
    {
        gbScaleE = new QGroupBox(this);
        gbScaleE->setWindowTitle(tr("Energy interval"));
        gbScaleE->setWindowFlags(Qt::Window);
        gbScaleE->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        this->Emin.setDisplay(tr("Emin"),tr("lower limit of energy"),vl);
        this->Emax.setDisplay(tr("Emax"),tr("upper limit of energy"),vl);
        this->hE.setDisplay(tr("hE"),tr("energy step"),vl);
        gbScaleE->setLayout(vl);
    }
    gbScaleE->show(); 
    gbScaleE->raise();//->raactivateWindow();
    gbScaleE->setFocus();
}*/
/*
void  PotentialViewMovable::scaleXY()
{   
    if (!gbScaleXY) 
    {
        gbScaleXY = new QGroupBox(this);
        gbScaleXY->setWindowTitle("Scales for plots Psi(x) and U(x)");
        gbScaleXY->setWindowFlags(Qt::Window);
        gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        this->Umin.setDisplay(("Umin"),("lower bond of potential"),vl);
        this->Umax.setDisplay(("Umax"),("upper bond of potential"),vl);
        this->xmin.setDisplay(("xmin"),("lower bond of x-interval"),vl);
        this->xmax.setDisplay(("xmax"),("upper bond of x-interval"),vl);
        gbScaleXY->setLayout(vl);
    }
    gbScaleXY->show(); 
    gbScaleXY->raise();//->raactivateWindow();
    gbScaleXY->setFocus();
}
*/
//-------------------------
class EnergyLevels : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
    QPen pen;
public:
    double widthLineE;
    EnergyLevels(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { pen = _pen; }
    void setLine(double x1,double y1,double width)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || width != p2.x())
        {
            prepareGeometryChange();
            p2 = QPointF(width,0);
            this->setPos(x1,y1);
             update();//repaint();
        }
    }
    QRectF boundingRect() const
    {
        return QRectF(QPointF(),p2);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        pen.setWidthF(view->widthLineE);
        pen.setCapStyle(Qt::FlatCap);
        painter->setPen(pen);
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

};
EnergyLevels::EnergyLevels(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,false);		
    setFlag(QGraphicsItem::ItemIsSelectable,false);	
}

QVariant EnergyLevels::itemChange(GraphicsItemChange change, const QVariant & value)
{
    return QGraphicsItem::itemChange(change,value);
}

//------------------
class HorDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
    QPen pen,penForPainter;
    QPen penHover;
public:
    double widthLineH;
    HorDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double width)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || width != p2.x())
        {
            prepareGeometryChange();
            p2 = QPointF(width,0);
            this->setPos(x1,y1);
             update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
        update();//repaint();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
        this->setSelected(false);
        update();//repaint();
    }
    QRectF boundingRect() const
    {
        QPoint va(0,0);
        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());

        QRect vpr = view->viewport()->rect();
        QPointF vpr1 = view->mapToScene(vpr.topLeft());
        QPointF vpr2 = view->mapToScene(vpr.bottomRight());
        QMatrix m = view->matrix();
        QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
        return aa;//QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
        if(option->state & QStyle::State_Selected)
        {
//            pen.setStyle(Qt::DashLine);
        }
        else
        {
//            pen.setStyle(Qt::SolidLine);
        }
/*        QPoint va(0,0);
        QPoint vb(0,2);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
        view->widthLineH=ay;*/
//        view->widthLineV=ay;
//        view->widthLineE=ay;
//        penForPainter.setWidthF(ay);
        penForPainter.setWidthF(view->widthLineH);
        penForPainter.setCapStyle(Qt::FlatCap);
        painter->setPen(penForPainter);
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
            QMenu m;
            QPointF p = event->pos();
            double fraction = p.x() / p2.x();
            QAction *splitHere = m.addAction(m.tr("Split here"));
            if (fraction < 0.05 || fraction > 0.95) splitHere->setDisabled(true);
            QAction *removeSegment = m.addAction(m.tr("Remove segment"));
            QAction *what = m.exec(event->screenPos());
            if (what == splitHere)
            {
                view->model->split_d(n, fraction);
            }
            if (what == removeSegment)
            {
                view->model->remove_d(n);
                update();//repaint();
            }
        }
    }
};

class VerDraggableLine : public QGraphicsItem
{
    PotentialViewMovable *view;
    int n;
    QPointF p2;
    QPen pen,penForPainter;
    QPen penHover;
public:
    double widthLineV;
    VerDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent = 0);
    void set_n(int _n) { n = _n; }
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setLine(double x1,double y1,double upDown)
    {
        QPointF p = pos();
        if (x1 != p.x() || y1 != p.y() || upDown != p2.y())
        {
            prepareGeometryChange();
            p2 = QPointF(0,upDown);
            this->setPos(x1,y1);
            update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = penHover;
        penForPainter.setWidthF(widthLineV);
        update();//repaint();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
    {
        penForPainter = pen;
        penForPainter.setWidthF(widthLineV);
        this->setSelected(false);
        update();//repaint();
    }
    QRectF boundingRect() const
    {
        QPoint va(0,0);
        QPoint vb(5,0);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());
        double ay = fabs(sa.y() - sb.y());
    QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    return aa;//QRectF(QPointF(),p2).adjusted(-0.1,-0.1,+0.1,+0.1);
//    return QRectF(QPointF(),p2).adjusted(-0.1,0,+0.1,0);
//    const qreal adj = 0.1; //todo
//    return QRectF( p1.x()-adj, p1.y()-adj, abs(p2.x() - p1.x()) + 2*adj, 2*adj );
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
//        if(option->state & QStyle::State_Selected)
//        {
            //            painter->setPen(Qt::red);
//        }
/*        QPoint va(0,0);
        QPoint vb(2,0);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ax = fabs(sa.x() - sb.x());*/
//        view->widthLineV=ax;
//        penForPainter.setWidthF(ax);
        penForPainter.setWidthF(view->widthLineV);
        penForPainter.setCapStyle(Qt::FlatCap);
        painter->setPen(penForPainter);
        painter->drawLine(QLineF(QPointF(),p2));

    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }
};

PotentialViewMovable::PotentialViewMovable(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m),  lineEnergy(0), 
Umin(-15.), Umax(10.), xmin(-1.), xmax(10.),dialogScalesU(0),widthLineE(0.02),widthLineH(0.02),widthLineV(0.02),
//Emin(0.1), Emax(20.), hE(0.05), 
psiMax(4), psiMin(-1), dx(0.01), whatToDraw(2)
{
    widthLine = 2;
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    if (1)
    {
        setViewportUpdateMode(BoundingRectViewportUpdate);///
        setCacheMode(CacheBackground);
        setRenderHint(QPainter::Antialiasing);///
        setTransformationAnchor(AnchorUnderMouse);///
        setResizeAnchor(AnchorViewCenter);///
    }
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumSize(260, 220);
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotUChanged()));
//    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotPsiofE()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slotEboundChanged()));
    connect(model,SIGNAL(signalScalesUChanged()),this,SLOT(resizePicture()));
//    setScalesFromModel(); 
    resizePicture();
}
#define ID_PSI_ENERGY (200)

PotentialViewMovable::~PotentialViewMovable()
{
    disconnect(this, 0, 0, 0);
    if (dialogScalesU) delete dialogScalesU;
//    if (gbScaleE) delete gbScaleE;
    if (lineEnergy) delete lineEnergy;
//    removeCurve(ID_PSI_ENERGY);
}

void PotentialViewMovable::setScalesFromModel()
{
    QPair<double,double> umin_umax = model->getUminUmax();
    Umin = 1.2*umin_umax.first;
    Umax = umin_umax.second;
    if(Umax<=0.) Umax=-Umin;
    QPair<double,double> xmin_xmax = model->getXminXmax();
    xmin = xmin_xmax.first;
    xmax = xmin_xmax.second;
    ScalesUParameters tp;
    tp.Hx = this->dx;
    tp.Xmin = xmin;
    tp.Xmax = xmax;
    tp.Umin = Umin;
    tp.Umax = Umax;
    tp.Psimin = psiMin;
    tp.Psimax = psiMax;
    model->setScalesUParam(tp);
}
void PotentialViewMovable::resizePicture()
{   
    ScalesUParameters tp = model->getScalesUParam();
    if(this->dx!=tp.Hx||xmin!=tp.Xmin||xmax!=tp.Xmax||
        Umin!=tp.Umin||Umax!=tp.Umax||psiMax!=tp.Psimax||psiMin!=tp.Psimin)
    {
        dx=tp.Hx;
        xmin=tp.Xmin;
        xmax=tp.Xmax;
        Umin=tp.Umin;
        Umax=tp.Umax;
        psiMax=tp.Psimax;
        psiMin=tp.Psimin;
        if(dialogScalesU)dialogScalesU->modelChanged();
        else
        {
            dialogScalesU = new ScalesUx(this);
            dialogScalesU->setModel(model);
        }
    }
    setViewportMapping();
    slotUChanged();
    slotEboundChanged();
    slotEnergyChanged();
}
void PotentialViewMovable::setViewportMapping()
{
//    QRectF vp = model->getGoodViewportAtU();
    QRectF a = QRectF(this->viewport()->rect());
    QRectF vp = QRectF(QPointF(this->xmin,this->Umin),QPointF(this->xmax,this->Umax));

    QRectF sr = scene()->sceneRect();
//    if (vp != sr)
//    {
        scene()->setSceneRect(vp);

        QRectF b = vp; //scene()->sceneRect();
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
//        scene()->invalidate(scene()->sceneRect()); 
        sr = scene()->sceneRect();
//    }
    double ax=fabs(xmax-xmin)*widthLine/a.width();
    double ay=fabs(Umax-Umin)*widthLine/a.height();
    widthLineH= ay;
    widthLineV= ax;//0.05;//0.0025*vp.width();
    widthLineE= ay;
} 
void PotentialViewMovable::resizeEvent(QResizeEvent*) 
{
    setViewportMapping();
}

void PotentialViewMovable::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
/*void PotentialViewMovable::mouseReleaseEvent( QMouseEvent * event)
{
    QRectF vp = model->getGoodViewportAtU();//????????
    this->setSceneRect(vp);//????????????
    if (scene()) setMyTransformMatrix();
    update();
    QGraphicsItem::mouseReleaseEvent(event);
} 
void PotentialViewMovable::mousePressEvent(QMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
*/
void PotentialViewMovable::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void PotentialViewMovable::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void PotentialViewMovable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) 
    {
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Left:
        scrollView(-1, 0);
        break;
    case Qt::Key_Right:
        scrollView(+1, 0);
        break;
    case Qt::Key_Down:
        scrollView(0, -1);
        break;
    case Qt::Key_Up:
        scrollView(0, +1);
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
/*
        foreach (QGraphicsItem *item, scene()->items()) {
            if (qgraphicsitem_cast<Node *>(item))
                item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
        }*/
        break;
    default:
         QGraphicsView::keyPressEvent(event);
    }
}
void PotentialViewMovable::scrollView(int hx, int hy)
{
    ScalesUParameters tp = model->getScalesUParam();
    dx=tp.Hx;
    xmin=tp.Xmin;
    xmax=tp.Xmax;
    Umin=tp.Umin;
    Umax=tp.Umax;
    psiMax=tp.Psimax;
    psiMin=tp.Psimin;
    setViewportMapping();
    double stepX=(xmax-xmin)/5;//numTTicks;
    xmin +=hx*stepX;
    xmax +=hx*stepX;
    double stepY=(Umax-Umin)/5;//numETicks;
    Umin +=hy*stepY;
    Umax +=hy*stepY;
    ScalesUParameters tt;
    tt.Hx = this->dx;
    tt.Xmin = xmin;
    tt.Xmax = xmax;
    tt.Umin = Umin;
    tt.Umax = Umax;
    tt.Psimin = psiMin;
    tt.Psimax = psiMax;
    model->setScalesUParam(tt);
    if(dialogScalesU)dialogScalesU->modelChanged();
    else
    {
        dialogScalesU = new ScalesUx(this);
        dialogScalesU->setModel(model);
    }
}
void PotentialViewMovable::slotUChanged()
{
    QVector<double> d = model->get_d();
//    QVector<double> Ui = model->get_U();
    QVector<double> Ui = model->get_Ui();
    int Nd=d.size();
    int Nu=Ui.size();
    int Nl=linesU.size();
    while (Ui.size() < linesU.size())
    {
        scene()->removeItem(linesU.last()); 
        linesU.pop_back();
    }
    while (Ui.size()-1 < linesV.size())
    {
        scene()->removeItem(linesV.last());
        linesV.pop_back();
    }
    while(Ui.size() > linesU.size())
    {
        HorDraggableLine *line = new HorDraggableLine(this);
        scene()->addItem(line);
        linesU << line;
    }
    while(Ui.size()-1 > linesV.size())
    {
        VerDraggableLine *line = new VerDraggableLine(this);
        scene()->addItem(line);
        linesV << line;
    }
    ///////////////////////////////////////////////////
    // now we have number of graphical lines and segments of physical potential algned
    ///////////////////////////////////////////////////
//    QRectF vp = model->getGoodViewportAtU();
//    scene()->setSceneRect(vp);
    QRectF vp = sceneRect();
//    QRectF vp = scene()->sceneRect();
    double xmin = vp.x();
    double xmax = vp.x() + vp.width();
//    update();
//---------------
    HorDraggableLine *l = linesU[1];
    QLineF ll = l->line();
    QPointF p1 = ll.p1();
    QPointF p2 = ll.p2();
    //if(p1.y()==Ui[1]) return; 
//----------------------
    double xlast = vp.x();
    int nlast=Ui.size()-1;
    for (int n = 0; n < Ui.size(); ++n)
    {
        linesU[n]->setFlag(QGraphicsItem::ItemIsMovable,true);
        linesU[n]->setFlag(QGraphicsItem::ItemIsSelectable,true);
        linesU[n]->setZValue(1000);
        linesU[n]->set_n(n);
        qreal y = Ui[n];
        (void)y;
        double w;
        if (0 < n && n < Ui.size()-1)//!!! =
            w = d[n];
        else if (n==0)
            w = 0 - vp.x();
        else
            w = vp.x() + vp.width() - xlast;

 //       if(xlast+w < xmax)
        {
        linesU[n]->setLine(xlast,Ui[n],w);
        linesU[n]->setZValue(1000);
        xlast += w;
        }
 /*       else
        {
            w = xmax-xlast;
            linesU[n]->setLine(xlast,Ui[n],w);
            nlast=n;
            break;//xlast = xmax;
        }*/
    }
    linesU.last()->setFlag(QGraphicsItem::ItemIsMovable,false);
    linesU.last()->setFlag(QGraphicsItem::ItemIsSelectable,false);
    linesU.last()->setZValue(1000);

    for (int n = 0; n < linesV.size(); ++n)
    {
//        if(n<nlast)
        {
        linesV[n]->setCursor(n == 0 ? QCursor() : Qt::SizeHorCursor);
        linesV[n]->setFlag(QGraphicsItem::ItemIsMovable,n == 0 ? false : true );
        linesV[n]->setFlag(QGraphicsItem::ItemIsSelectable,n == 0 ? false : true );
        linesV[n]->setZValue(1000);
        linesV[n]->set_n(n);
        QLineF left  = linesU[n]->line();
        QLineF right = linesU[n+1]->line();
        qreal y = right.y1();
        (void)y;
        double ud = right.y1()-left.y2();
        linesV[n]->setLine(left.x2(),left.y2(),ud);
        }
//        else break;
    }
    update();
}
void PotentialViewMovable::slotEboundChanged()
{
    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
//    QRectF vp = scene()->sceneRect();
//    QRectF vp = model->getGoodViewportAtU();
//    scene()->setSceneRect(vp);

//    double scalePsi = (Umax-Umin)/(psiMax-psiMin);
    QVector<double> Ebound = model->getEn();
//    int nmax=5;
    while (Ebound.size() > linesEn.size())
    {
        int n = linesEn.size();
        EnergyLevels *line = new EnergyLevels(this);
        double e = Ebound[n];
        line->setLine(xmin,e,xmax-xmin);
        line->setFlag(QGraphicsItem::ItemIsMovable,false);
        line->setFlag(QGraphicsItem::ItemIsSelectable,false);
        line->setCursor(QCursor());
        scene()->addItem(line);
        linesEn.push_back(line);
//        if(n<nmax) setCurve(n,QPolygonF());
    }
    while (Ebound.size() < linesEn.size())
    {
        scene()->removeItem(linesEn.last());
        linesEn.pop_back();
//        if(linesEn.size()<nmax) removeCurve(linesEn.size());
    }
    int npoints=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    double dx = (xmax-xmin)/(npoints-1);
    for (int n = 0; n < linesEn.size(); ++n)
    {
        double modelEn = Ebound[n];
        linesEn[n]->setLine(xmin,modelEn,xmax-xmin);
        linesEn[n]->setPen(colorForIds[n % size_colorForIds]);
    }
    update();
}

//#define ID_PSI_ENERGY (200)

/*void PotentialViewMovable::energyAutoChanged()
{       
//    double Emin=0.1;
//    double he=0.1;
    double scalePsi = (Umax-Umin)/(psiMax-psiMin);
    int npoints=500;
    double dx = (xmax-xmin)/(npoints-1);
    for (double E=Emin; E<=Emax; E+=hE)
    {   
        //        double E=Emin+hE*n;
        if (!lineEnergy) 
        {
            lineEnergy = new EnergyDraggableLine(this);
            lineEnergy->setLine(xmin,E,xmax-xmin);
            scene()->addItem(lineEnergy);
        }
        else     lineEnergy->setLine(xmin,E,xmax-xmin);
        QPolygonF psi;
        psi.resize(npoints);
        QVector<double> waveFunction;
        waveFunction.resize(npoints);

        waveFunction = model->getPsiOfX(E,xmin,xmax,npoints,whatToDraw);
        for (int i=0; i < npoints; i++)
        {
            double x = xmin + dx*i;
            double y = waveFunction[i];
            y = (y-psiMin)*scalePsi+Umin;
            psi[i]  = QPointF(x, y);
        }
        setCurve(ID_PSI_ENERGY, psi, QPen(Qt::darkRed));
        //dima qApp->processEvents(QEventLoop::AllEvents);
    }

}
*/
void PotentialViewMovable::slotEnergyChanged()
{
        QPen p;
        p.setWidthF(this->widthLineE);
        p.setJoinStyle(Qt::BevelJoin);
        p.setCapStyle(Qt::RoundCap);
//      p.setCapStyle(Qt::FlatCap);
        p.setColor(Qt::darkRed);
    double scalePsi = (Umax-Umin)/(psiMax-psiMin);
    double E=model->get_E0(); 
/*    if (lineEnergy) 
    {
        double Eold=lineEnergy->getEnergyFromLine();
        if(Eold!=E) model->set_E0(Eold);
        E=Eold;
    }
*/    if (!lineEnergy) 
    {
        lineEnergy = new EnergyDraggableLine(this);
        lineEnergy->setLine(xmin,E,xmax-xmin);
        scene()->addItem(lineEnergy);
    }
    else     
    {
        lineEnergy->setLine(xmin,E,xmax-xmin);
    }
    model->getTatE();
//    update();
/*    int npoints;//=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
        waveFunction = model->getPsiOfX(E,xmin,xmax,npoints,whatToDraw);
        for (int i=0; i < npoints; i++)
        {
            double x = xmin + dx*i;
            double y = waveFunction[i];
            y = (y-psiMin)*scalePsi+Umin;
            psi[i]  = QPointF(x, y);
        }
    setCurve(ID_PSI_ENERGY, psi, p);
    */
//    setCurve(ID_PSI_ENERGY, psi, QPen(Qt::darkCyan));
}
void PotentialViewMovable::slotPsiofE()
{
    double scalePsi = (Umax-Umin)/(psiMax-psiMin);
    double E=model->get_E0(); 
    /*    if (lineEnergy) 
    {
    double Eold=lineEnergy->getEnergyFromLine();
    if(Eold!=E) model->set_E0(Eold);
    E=Eold;
    }*/
    if (!lineEnergy) 
    {
        lineEnergy = new EnergyDraggableLine(this);
//        lineEnergy->setZValue(0);
        lineEnergy->setLine(xmin,E,xmax-xmin);
        scene()->addItem(lineEnergy);
    }
    else     lineEnergy->setLine(xmin,E,xmax-xmin);
    int npoints=500;
    QVector<double> waveFunction;
    waveFunction.resize(npoints);

    waveFunction = model->getPsiOfX(E,xmin,xmax,npoints,whatToDraw);
    double dx = (xmax-xmin)/(npoints-1);
    QPolygonF psi;
    psi.resize(npoints);
    for (int i=0; i < npoints; i++)
    {
        double x = xmin + dx*i;
        double y = waveFunction[i]; 
        y = (y-psiMin)*scalePsi+Umin;
        psi[i]  = QPointF(x, y);
    }
        QPen p;
        p.setWidth(1);
//        p.setWidthF(this->widthLineE);
        p.setJoinStyle(Qt::RoundJoin);
//        p.setJoinStyle(Qt::BevelJoin);
//        p.setCapStyle(Qt::RoundCap);
        p.setCapStyle(Qt::FlatCap);
         p.setColor(Qt::darkCyan);
    setCurve(ID_PSI_ENERGY, psi, p);
//    setCurve(ID_PSI_ENERGY, psi, QPen(Qt::darkCyan));
    //dima qApp->processEvents(QEventLoop::AllEvents);
}
EnergyDraggableLine::EnergyDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
     setCursor(Qt::SizeHorCursor);
    penHover.setStyle(Qt::DotLine);
    penHover.setWidthF(v->widthLineE);
    penHover.setColor(Qt::green);

    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::green);
    pen.setWidthF(v->widthLineE);

    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);	
    setZValue(999);
    setAcceptHoverEvents ( true );
}

QVariant EnergyDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange: 
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setX( oldpos.x() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newE = pos().y();
            double newx = pos().x();
            view->model->set_Energy(newE);
            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
HorDraggableLine::HorDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    penHover.setStyle(Qt::DashLine);
    penHover.setWidthF(view->widthLineH);
    penHover.setColor(Qt::blue);

    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);	
    setAcceptHoverEvents ( true );
}

QVariant HorDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setX( oldpos.x() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newU = pos().y();
            VerDraggableLine *left  = n-1 >= 0 ? view->linesV[n-1] : NULL;
            VerDraggableLine *right = n < view->linesV.size() ? view->linesV[n] : NULL;
            if (left)
            {
                QLineF l = left->line();
                QPointF p1 = l.p1();
                QPointF p2 = l.p2();
//                left->setLine(p1.x(),p1.y(),newU);
                left->setLine(p1.x(),p1.y(),newU-p1.y());
            }
            if (right)
            {
                QLineF r = right->line();
                QPointF p1 = r.p1();
                QPointF p2 = r.p2();
//                right->setLine(p1.x(),newU,p2.y());
                right->setLine(p1.x(),newU,p2.y()-newU);
            }
            view->model->set_Ui(n,newU);
            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
VerDraggableLine::VerDraggableLine(PotentialViewMovable *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
//    QRectF vp = view->sceneRect();
    setCursor(Qt::SizeHorCursor);
    penHover.setStyle(Qt::DashLine);
    penHover.setWidthF(view->widthLineV);
    penHover.setColor(Qt::blue);

    pen.setWidthF(view->widthLineV);
    pen.setColor(Qt::black);

    setFlag(QGraphicsItem::ItemIsMovable,true);		
    setFlag(QGraphicsItem::ItemIsSelectable,true);		
    setAcceptHoverEvents ( true );
}
QVariant VerDraggableLine::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            double newx = newpos.x();
            QLineF left  = view->linesU[ n ]->line();
            QLineF right = view->linesU[n+1]->line();
            if (newx > right.x2()) newx = right.x2(); 
            if (newx < left.x1())   newx = left.x1(); 
            newpos.setX(newx);
            newpos.setY(oldpos.y());
            return newpos;
        }
    case ItemPositionHasChanged:
       if (isSelected())
        {
            QLineF self  = line();
            QLineF left  = view->linesU[ n ]->line();
            QLineF right = view->linesU[n+1]->line();
    
            double x = self.x1();
            double left_w = x - left.x1();
            double right_w = right.x2() - x;

            view->linesU[ n ]->setLine(left.x1(),left.y1(),left_w);
            view->linesU[n+1]->setLine(x,right.y1(),right_w);
            
            view->model->set_d(n,left_w,n+1,right_w);

            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

void PotentialViewMovable::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves.contains(id))
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MyGraphicsPolylineItem *c = new MyGraphicsPolylineItem(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    repaint();
//    update();
}

void PotentialViewMovable::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves.remove(id);
    repaint();
}

void MyGraphicsPolylineItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    QPen p;
    p.setWidth(3);
//        p.setWidthF(this->widthLineE);
        p.setJoinStyle(Qt::BevelJoin);
        p.setCapStyle(Qt::RoundCap);
//      p.setCapStyle(Qt::FlatCap);
        p.setColor(Qt::darkRed);
    painter->setRenderHint(QPainter::Antialiasing);
//    painter->setPen(QPen(Qt::red, 0.1, Qt::DotLine, Qt::RoundCap, Qt::BevelJoin));
    painter->drawPolyline(polygon().data(),polygon().size());
}

void PotentialViewMovable::showDialogScaleY()
{   
    if (!dialogScalesU) 
    {
        dialogScalesU = new ScalesUx(this);
        dialogScalesU->setModel(model);
    }
    dialogScalesU->show(); 
    dialogScalesU->activateWindow();
    dialogScalesU->setFocus();
}
void PotentialViewMovable::setWhatToDraw(int w)
{   
    if (whatToDraw != w) 
    {
        whatToDraw = w;
        this->slotEnergyChanged();
//        slotPsiofE();
        emit( whatToDrawChanged(w) );
    }
}

void PotentialViewMovable::contextMenuEvent(QContextMenuEvent *event)
{
    {
        QMenu m;
        QAction *scalePsi = m.addAction(tr("Scales"));
        QAction *U1Action = m.addAction(tr("Set U(x) as initial"));
        QAction *U2Action = m.addAction(tr("Set U(x) as final"));
        QPoint poscur = event->globalPos();
        QAction *what = m.exec(event->globalPos());
        if (what == scalePsi)
        {
            this->showDialogScaleY();
            update();
        }
        if (what == U1Action)
        {
            model->slotU1();
            update();
        }
        if (what == U2Action)
        {
            model->slotU2();
            update();
        }
        event->accept();

#if 0
        QPainterPath p = shape();
        QPointF v = event->pos();
        QRectF r = QRectF(v,v).adjusted(-0.1,-0.1,0.1,0.1);

        bool i = p.intersects(r);
        if (i)
        {
            QMenu m;
            QAction *scaleX = m.addAction("Set something");
            QAction *scaleY = m.addAction("Set scales");
            QAction *what = m.exec(event->screenPos());
            if (what == scaleY)
            {
                view->scaleY();
            }
               update();// repaint();
            event->accept();
            return;
        }
#endif
    }
}
class MyLabel : public QLabel
{
public:
    MyLabel(const QString s, QWidget *parent) : QLabel(s,parent) {}
    void resizeEvent( QResizeEvent *e )
    {
        QLabel::resizeEvent(e);
        setMinimumWidth(width());
    }
};

PotentialMovableWidget::PotentialMovableWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
//-------------------
    QString psi=QChar(0x03C8);
    //               QString Psi=QChar(0x03A8);
    QString to2=QChar(0x00B2);
    QString psiofx=psi+"(x)";
    QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
    //            QGroupBox *gb3 = new QGroupBox(tr("Wave function ")+psiofx);
//-------------------
    setTitle(tr("Graphic definition of potential, ")+psi+"(x,E)");

    QVBoxLayout *vl = new QVBoxLayout();
    potentialViewMovable = new PotentialViewMovable(model,this);
    vl->addWidget(potentialViewMovable);


    QHBoxLayout *hl = new QHBoxLayout();
    QRadioButton *rad1= new QRadioButton("Re "+psi);
    QRadioButton *rad2= new QRadioButton("Im "+psi);
    QRadioButton *rad3= new QRadioButton(mod_psiofx);
    hl->addWidget(rad1);
    hl->addWidget(rad2);
    hl->addWidget(rad3);
    bgR = new QButtonGroup(this);
    bgR->setExclusive(true);
    bgR->addButton(rad1,0);
    bgR->addButton(rad2,1);
    bgR->addButton(rad3,2);
    bgR->button(2)->setChecked(true);
    connect(bgR,SIGNAL(buttonClicked(int)),potentialViewMovable,SLOT(setWhatToDraw(int)));

    QLabel *lEtext= new QLabel(this);
    lEtext->setText(tr("E="));
    QLabel *lE = new MyLabel("",this);
    connect(model, SIGNAL(signalEnergyChanged(double)), lE, SLOT(setNum(double)));

    QLabel *lTtext= new QLabel(this);
    lTtext->setText(tr("T="));
    QLabel *lT= new MyLabel("",this);
    lT->setTextFormat(Qt::AutoText);
    connect(model, SIGNAL(signalTransmissionChanged(double)), lT, SLOT(setNum(double)));
    hl->addWidget(lEtext);
    hl->addWidget(lE);
    hl->addWidget(lTtext);
    hl->addWidget(lT);

    hl->addStretch();

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
//    QPushButton *buttonClose = new QPushButton(tr("Close"));
//    hl->addWidget(buttonClose);

    vl->addLayout(hl);
    setLayout(vl);
}
