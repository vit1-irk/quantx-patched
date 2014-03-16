/*
 *  Copyright (C) 2011 Olga Tkachenko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

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
#include <QLabel>
#include "BreakStatus.h"
#include "EofqaView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

class EEDrag : public QGraphicsItem
{
    EofqaView *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
    EEDrag(EofqaView *v,QGraphicsItem *parent = 0);
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
    double getEnergyFromLine()
    {
        QPointF p = pos();
        return p.y();
    }
    QRectF boundingRect() const
    {
        QRectF vp = view->sceneRect();
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
        return aa;

    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *)
    {
//        penForPainter.setWidthF(view->lineWidth);
//        penForPainter.setCapStyle(Qt::FlatCap);
        painter->setPen(penForPainter);
//        qreal x2 = penForPainter.widthF();
        painter->drawLine(QLineF(QPointF(),p2));

/*        painter->setPen(penForPainter);
        QPainter::RenderHints saved_hints = painter->renderHints();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawLine(QLineF(QPointF(),p2));
        painter->setRenderHints(saved_hints);*/
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
               update();// repaint();
        }
    }
};

/*
class EEDrag : public QGraphicsItem
{
    EofqaView *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
//    double widthLineE;
    EEDrag(EofqaView *v,QGraphicsItem *parent = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double height)
    {
        QPointF p = pos();
        QRectF vp = view->sceneRect();//
        if (x1 != p.x() || y1 != p.y() || height != p2.y())
        {
            prepareGeometryChange();
            p2 = QPointF(0,height);
//            p2 = QPointF(width,0);
            this->setPos(x1,y1);
//            this->setPos(x1,y1);
            update();//repaint();
        }
    }
    void hoverEnterEvent ( QGraphicsSceneHoverEvent *  )
    {
        penForPainter = penHover;
        update();//repaint();
    }
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent *  )
    {
        penForPainter = pen;
        this->setSelected(false);
        update();//repaint();
    }
    double EEDrag::getEnergyFromLine()
    {
        QPointF p = pos();
        return p.x();
    }
    QRectF boundingRect() const
    {
    QRectF vp = view->sceneRect();
    //double lineWidth= 0.005*vp.height();
        QPoint v1(0,0);
        QPoint v2(3,0);
//        QPoint v2(0,3);
        QPointF fv1 = view->mapToScene(v1);
        QPointF fv2 = view->mapToScene(v2);
        //double widthLine1 = fabs(fv2.x() - fv1.x());
//        double widthLine1 = fabs(fv2.y() - fv1.y());

        QPoint va(0,0);
        QPoint vb(5,0);
//        QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ay = fabs(sa.y() - sb.y());
//        double ax = fabs(sa.x() - sb.x());
        double ax= fabs(sa.x() - sb.x());
//      double ay = fabs(sa.y() - sb.y());
        QRect vpr = view->viewport()->rect();
        QPointF vpr1 = view->mapToScene(vpr.topLeft());
        QPointF vpr2 = view->mapToScene(vpr.bottomRight());
        QMatrix m = view->matrix();
        QRectF aa=QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
        return aa;///QRectF(QPointF(),p2).adjusted(-ax,-ay,ax,ay);
    }
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
    {
        QPoint va(0,0);
        QPoint vb(1,0);
//        QPoint vb(0,1);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        //double ax = fabs(sa.x() - sb.x());

        //qreal ax1 = view->mapFromGlobal(QPoint(1,1)).x();
        //qreal ax2 = view->mapFromScene(QPoint(1,1)).x();
        //qreal ax3 = view->width();
        penForPainter.setWidthF(view->lineWidth);
        painter->setPen(penForPainter);
        QPainter::RenderHints saved_hints = painter->renderHints();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawLine(QLineF(QPointF(),p2));
        painter->setRenderHints(saved_hints);
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
               update();// repaint();
        }
    }

};
*/
EofqaView::EofqaView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m)
{
    Erase = true; // this must initially be true
    lineWidth = 3;//10;
    qaMax = 1.01;
    qaMin = -1.01;
    Emin = -15;
    Emax = 2.20;
    hE = 0.05;

    lineh = NULL;
    linev = NULL;
    lineE = NULL;

    leTmin = NULL;
    leTmax = NULL;
    leEmin = NULL;
    leEmax = NULL;
//    leHE   = NULL;
    initDialogScaleY();

    model = m;
    curve_number = 0;

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
    setMinimumSize(300, 150);
//    setScalesFromModel();
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this,SIGNAL(signalScaleQAChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEline()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));
    resizePicture();
}
/*EofqaView::~EofqaView()
{
    disconnect(this, 0, 0, 0);
    for (QMap<int,EofqaCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!gbScaleXY) delete gbScaleXY;
    if (!lineE) delete lineE;
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
void EofqaView::setScalesFromModel()
{
    QPair<double,double> umin_umax = model->getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;
    Emin = Umin + 1e-7;
    if (Emax == Emin) Emax = 2*(Emin + 1);
    if (Emax < Emin)
    {
        double t = Emax;
        Emax = Emin;
        Emin = t;
    }
}
void EofqaView::slotEline()
{
    QRectF vp = scene()->sceneRect();
    double E=model->get_E0();
    if (!lineE)
    {
        lineE = new EEDrag(this);
        scene()->addItem(lineE);
    }
/*    QPen p;
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setStyle(Qt::DashLine);
    p.setColor(Qt::green);
    lineE->setPen(p);*/
    lineE->setLine(0, vp.height()*(E-Emin)/(Emax-Emin), vp.width());
//    lineE->setLine(vp.width()*(E-Emin)/(Emax-Emin), 0., vp.height());
}
void EofqaView::resizePicture()
{
    setViewportMapping();
    slot_Eofqa();
    slotEline();
}
void EofqaView::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
//    double rEmin=0;
//    double rEmax=a.width();//npoints-1;
//    double rTMin=0;
//    double rTMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(0,0),QPointF(a.width(),a.height()));
//    QRectF b = QRectF(QPointF(rEmin,rTMin),QPointF(rEmax,rTMax));
    scene()->setSceneRect(b);
    {
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
    }
    update();
}
void EofqaView::resizeEvent(QResizeEvent *)
{
    this->resizePicture();
}

void EofqaView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void EofqaView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}
void EofqaView::clearAll()
{
    for ( QMap<int,EofqaCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
        removeCurve(m);
    }
}
void EofqaView::scrollView(int dx, int dy)
{
/*    double stepY=(qaMax-qaMin)/5;//numTTicks;
    qaMin +=dy*stepY;
    qaMax +=dy*stepY;
    double stepX=(Emax-Emin)/5;//numETicks;
    Emin +=dx*stepX;
    Emax +=dx*stepX;
    if(Emin<0) Emin=-0.2;
    if(Emax<0) Emax=1.;
    setScaleQE();
    emit(signalScaleQAChanged());*/
    double stepX=(qaMax-qaMin)/5;//numTTicks;
    qaMin +=dx*stepX;
    qaMax +=dx*stepX;
    double stepY=(Emax-Emin)/5;
    Emin +=dy*stepY;
    Emax +=dy*stepY;
    emit(signalScaleQAChanged());
}

void EofqaView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        clearAll();
        break;
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
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void EofqaView::slot_Eofqa()
{
    if (! isVisible()) return;
    PotentialType type = model->getPotentialType();
    if(type==FINITE||type==QUASISTATIONARY) return;
    EParameters sE;
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    if(lineWidth==0)lineWidth=1;

    p.setWidth(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    if(Emin>Emax) Emax=1.5*Emin;
    Erase = true;
    if(Erase) curve_number=0;
    else curve_number=curve_number+1;
    int n=curve_number;
    if(Erase)
    {
        for ( QMap<int,EofqaCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
        {
            int m = i.key();
            removeCurve(m);
        }
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        lineh->setZValue(1000);
        linev->setZValue(1000);
//        linev->setLine(vp.width()*(-qaMin)/(qaMax-qaMin), 0., vp.width()*(-qaMin)/(qaMax-qaMin), vp.height());
//        lineh->setLine(0,vp.width()*(-Emin)/(Emax-Emin), vp.width(),vp.width()*(-Emin)/(Emax-Emin));
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
//    else
//    {
//        linev->setPen(p);
//        lineh->setPen(p);
        linev->setLine(vp.width()*(-qaMin)/(qaMax-qaMin), 0., vp.width()*(-qaMin)/(qaMax-qaMin), vp.height());
        lineh->setLine(0,vp.height()*(-Emin)/(Emax-Emin), width(),vp.height()*(-Emin)/(Emax-Emin));
//        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
//        lineh->setLine(0,vp.height()*(-qaMin)/(qaMax-qaMin),vp.width(),vp.height()*(-qaMin)/(qaMax-qaMin));
//    }
//--------------------------
    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta,
        Qt::black,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    if(type==PERIODIC)
    {
        sE.Emin = this->Emin;
        sE.Emax = this->Emax;
        model->setEParameters(sE);
        QVector<double> Ebound = model->getEn();
        int number_of_levels = Ebound.size();
//        QPolygonF curve;
        QPolygonF curvem;
        QPolygonF curvep;
        QPolygonF curveEQ;
        int npoint=101;
        double ed,eu;
        int i=0;
        int jc=0;
        while(i < number_of_levels)
        {
            ed=Ebound[i];
            int numberOfBand=(2+i)/2-1;
            curveEQ.resize(npoint);
//            curveEQ.resize(2*npoint);
            if((i+1)<number_of_levels) eu=Ebound[i+1];
            else eu=Emax;
            model->set_E0(0.5*(ed+eu));
            double qa = model->get_qa();
            if(qa>M_PI)
            {
                i++;
            }
            else
            {
                    curvem.resize(0);
                    curvep.resize(0);
                    double dE=(eu-ed)/(npoint-1);
                    for(int j=0; j<npoint;j++)
                    {
                        double EE=ed+dE*j;
                        model->set_E0(EE);
                        qa = model->get_qa()/M_PI;
                        if(qa<=1)
                        {
                            double y = vp.height()*(EE-Emin)/(Emax-Emin);
                            double x = vp.width()*(qa-qaMin)/(qaMax-qaMin);
                            double xm = vp.width()*(-qa-qaMin)/(qaMax-qaMin);
                            curvep.push_back(QPointF(x,y));
                            curvem.push_back(QPointF(xm,y));
//                            curve.push_back(QPointF(qa,EE));
                        }
                    }
/*                    int ns=curve.size()-1;
                    int dqde_nc = (numberOfBand%2) ? -1 : +1;
                    for (int l=0; l <= ns; l++)
                    {
                       if(dqde_nc>0)
                        {
                        QPointF p=curve[ns-l];
                        double y = vp.height()*(p.y()-Emin)/(Emax-Emin);
                        double xp = vp.width()*(p.x()-qaMin)/(qaMax-qaMin);
                        double xm = vp.width()*(-p.x()-qaMin)/(qaMax-qaMin);
                        curveEQ[l]=QPointF(xm,y);//curve[curve.size()-1-l];
//                        curveEQ[2*ns+1-l]=QPointF(xp,y);
                       }
                       else
                       {
                        QPointF p=curve[l];
                        double y = vp.height()*(p.y()-Emin)/(Emax-Emin);
                        double xp = vp.width()*(p.x()-qaMin)/(qaMax-qaMin);
                        double xm = vp.width()*(-p.x()-qaMin)/(qaMax-qaMin);
                        curveEQ[l]=QPointF(xm,y);//curve[curve.size()-1-l];
//                        curveEQ[2*ns+1-l]=QPointF(xp,y);
                       }
                    }*/
                    p.setColor(colorForIds[ i % size_colorForIds]);
                    setCurve(jc,curvep,p);
                    jc=jc+1;
                    setCurve(jc,curvem,p);
//                    setCurve(i,curveEQ,p);
                    jc=jc+1;
                    i++;
            }
        }
    }
}



void EofqaView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        EofqaCurve *c = new EofqaCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
        c->setZValue(1);
    }
    curves[id]->setPen(pen);
    update();
}

void EofqaView::removeCurve(int id)
{
    QGraphicsItem *item = curves[id];
    if (item)
    {
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0;
    }//this is dangerous: curves.remove(id);
    update();
//    repaint();
}

void EofqaCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * , QWidget * )
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());

}

void EofqaView::initDialogScaleY()
{
    gbScaleXY = new QGroupBox(this);
    gbScaleXY->setWindowTitle("Scales for qa(E)");
    gbScaleXY->setWindowFlags(Qt::Window);
    gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout;
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("hE",this));
        h->addWidget(this->leHE = new QLineEdit(this));
        this->leHE->setToolTip("E-increment");
        QString x;
        x.sprintf("%lg",this->hE);
        this->leHE->setText(x);
        connect(this->leHE,SIGNAL(editingFinished()),this,SLOT(updateScaleQE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Emin",this));
        h->addWidget(this->leEmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Emin);
        this->leEmin->setText(x);
        this->leEmin->setToolTip("Lower value of E-interval");
        connect(this->leEmin,SIGNAL(editingFinished()),this,SLOT(updateScaleQE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Emax",this));
        h->addWidget(this->leEmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Emax);
        this->leEmax->setText(x);
        this->leEmax->setToolTip("High value of E-interval");
        connect(this->leEmax,SIGNAL(editingFinished()),this,SLOT(updateScaleQE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("qa_min/pi",this));
        h->addWidget(this->leTmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->qaMin);
        this->leTmin->setText(x);
        //        this->leTmin->setToolTip("Lower value of E-interval");
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateScaleQE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("qa_max/pi",this));
        h->addWidget(this->leTmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->qaMax);
        this->leTmax->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateScaleQE()));
        vl->addWidget(line);
    }
    gbScaleXY->setLayout(vl);
}

void EofqaView::showDialogScaleY()
{
    gbScaleXY->show();
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
void EofqaView::setScaleQE()
{
    QString x;
    x.sprintf("%lg",this->hE);
    this->leHE->setText(x);
    x.sprintf("%lg",this->Emin);
    this->leEmin->setText(x);
    x.sprintf("%lg",this->Emax);
    this->leEmax->setText(x);
    x.sprintf("%lg",this->qaMax);
    this->leTmax->setText(x);
    x.sprintf("%lg",this->qaMin);
    this->leTmin->setText(x);
}
void EofqaView::updateScaleQE()
{
    double hENew = this->leHE->text().toDouble();
    double EminNew = this->leEmin->text().toDouble();
    double EmaxNew = this->leEmax->text().toDouble();
    double qaMinNew = this->leTmin->text().toDouble();
    double qaMaxNew = this->leTmax->text().toDouble();
    bool changed = false;
    if (hE != hENew)
    {
        hE=hENew;
        changed = true;
    }
    if (Emin != EminNew)
    {
        changed = true;
        Emin=EminNew;
    }
    if (Emax != EmaxNew)
    {
        changed = true;
        Emax=EmaxNew;
    }
    if (qaMin != qaMinNew)
    {
        changed = true;
        qaMin=qaMinNew;
    }
    if (qaMax != qaMaxNew)
    {
        changed = true;
        qaMax=qaMaxNew;
    }
    if(changed)
    {
//        setScaleQE();
        emit(signalScaleQAChanged());
    }
}
void EofqaView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scaleT = m.addAction("Set scales");
        QAction *what = m.exec(event->globalPos());
        if (what == scaleT)
        {
            this->showDialogScaleY();
            update();
        }
}
ScalesEQ EofqaView::getScalesEQ() const
{
ScalesEQ tp;
tp.hE=this->hE;
tp.Emin=this->Emin;
tp.Emax=this->Emax;
tp.qaMin=this->qaMin;
tp.qaMax=this->qaMax;
return tp;
}
void  EofqaView::setScalesEQ(const ScalesEQ& u)
{
 bool changed_x = false;
 bool changed_y = false;
 double v=u.Emax;
 if(v!=this->Emax)
 {
     this->Emax=v;
     changed_x = true;
 }
 v=u.Emin;
 if(v!=this->Emin)
 {
     changed_x = true;
     this->Emin=v;
 }
 v=u.qaMax;
 if(v!=this->qaMax)
 {
     this->qaMax=v;
     changed_y = true;
 }

 v=u.qaMin;
 if(v!=this->qaMin)
 {
     changed_y = true;
     this->qaMin=v;
 }

 if(this->hE!=u.hE)
 {
 changed_x = true;
 this->hE=u.hE;
 }
    if (changed_x||changed_y)
    {
    setScaleQE();
    emit(signalScaleQAChanged());
    }
}

EEDrag::EEDrag(EofqaView *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    //TODO: how do I control the width of this draggable line?
//    setCursor(Qt::SizeHorCursor);
    penHover.setCapStyle(Qt::FlatCap);
    penHover.setStyle(Qt::DashLine);
    penHover.setWidth(v->lineWidth);
//    penHover.setStyle(Qt::DotLine);
    penHover.setColor(Qt::green);
//    pen.setStyle(Qt::DashLine);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::green);
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidth(v->lineWidth);
    penForPainter.setColor(Qt::green);
    penForPainter.setWidth(view->lineWidth);
    penForPainter.setCapStyle(Qt::FlatCap);


    setCursor(Qt::SizeVerCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setZValue(999);
    setAcceptHoverEvents ( true );
}

QVariant EEDrag::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setX( oldpos.x() );
//            newpos.setY( oldpos.y() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newE = pos().y();
//            double newE = pos().x();
            QRectF vp = view->sceneRect();//
            newE=view->Emin+(view->Emax-view->Emin)*newE/vp.height();
//            newE=view->Emin+(view->Emax-view->Emin)*newE/vp.width();
            view->model->set_Energy(newE);
            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
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

QEWidget::QEWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Dispersion law E(qa)"));
    QVBoxLayout *vl = new QVBoxLayout();
    eofqaView = new EofqaView(model,this);
    vl->addWidget(eofqaView);

    QHBoxLayout *hl = new QHBoxLayout();
    QToolButton *reset = new QToolButton(this);
    reset->setIcon(QIcon(":/images/player_play.png"));
    reset->adjustSize();//QPushButton(tr("Close"));
    connect(reset,SIGNAL(clicked()),eofqaView,SLOT(resizePicture()));

    QLabel *lTtext= new QLabel(this);
    lTtext->setText(tr("qa="));
    QLabel *lT= new MyLabel("",this);
    lT->setTextFormat(Qt::AutoText);
    connect(model, SIGNAL(signalQaChanged(double)), lT, SLOT(setNum(double)));

    QLabel *lEtext= new QLabel(this);
    lEtext->setText(tr("E="));

    QLabel *lE = new MyLabel("",this);

//    hl->addWidget(bRunTE);
    hl->addWidget(reset);
    hl->addWidget(lEtext);
    hl->addWidget(lE);
    hl->addWidget(lTtext);
    hl->addWidget(lT);

    connect(model, SIGNAL(signalEnergyChanged(double)), lE, SLOT(setNum(double)));

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);
    setLayout(vl);
}
void QEWidget::slotRunEqa()
{
    eofqaView->resizePicture();
}
void QEWidget::readFromXml(QXmlStreamReader *r)
{
    Q_ASSERT(this);
    Q_ASSERT(r->isStartElement() && r->name() == "QE");
    double Emin = 0, Emax = 0, he=0;
    ScalesEQ u;
    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "Emin")
        {
            QString s = r->readElementText();
            u.Emin = s.toDouble();
        }
        else if (r->name() == "Emax")
        {
            QString s = r->readElementText();
            u.Emax = s.toDouble();
        }
        else if (r->name() == "hE")
        {
            QString s = r->readElementText();
            u.hE = s.toDouble();
        }
        else if (r->name() == "qa_max")
        {
            QString s = r->readElementText();
            u.qaMax = s.toDouble();
        }
        else if (r->name() == "qa_min")
        {
            QString s = r->readElementText();
            u.qaMin = s.toDouble();
        }
        else
            skipUnknownElement(r);
    }
    eofqaView->setScalesEQ(u);
}


void QEWidget::writeToXml(QXmlStreamWriter *w)
{
ScalesEQ u=eofqaView->getScalesEQ();
w->writeStartElement("QE");
    {
        QString s;
        s.sprintf("%lg",u.Emin);
        w->writeTextElement("Emin",s);
        s.sprintf("%lg",u.Emax);
        w->writeTextElement("Emax",s);
        s.sprintf("%lg",u.hE);
        w->writeTextElement("hE",s);
        s.sprintf("%lg",u.qaMin);
        w->writeTextElement("qa_min",s);
        s.sprintf("%lg",u.qaMax);
        w->writeTextElement("qa_max",s);
    }
    w->writeEndElement();
}
