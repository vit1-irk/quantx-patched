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

#include "EGView.h"
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
#include "BreakStatus.h"

class gDraggable : public QGraphicsItem
{
    EGView *view;
    QPointF p2;
    QPen penForPainter;
    QPen penHover;
    QPen pen;
public:
    //    double widthLineE;
    gDraggable(EGView *v,QGraphicsItem *parent = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    void setPen(QPen _pen) { penForPainter = pen = _pen; }
    void setLine(double x1,double y1,double height)
    {
        QPointF p = pos();
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
    double getGFromLine()
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
        // QPoint v2(0,3);
        QPointF fv1 = view->mapToScene(v1);
        QPointF fv2 = view->mapToScene(v2);
        //double widthLine1 = fabs(fv2.x() - fv1.x());
        // double widthLine1 = fabs(fv2.y() - fv1.y());

        QPoint va(0,0);
        QPoint vb(5,0);
        // QPoint vb(0,5);
        QPointF sa = view->mapToScene(va);
        QPointF sb = view->mapToScene(vb);
        double ay = fabs(sa.y() - sb.y());
        // double ax = fabs(sa.x() - sb.x());
        double ax= fabs(sa.x() - sb.x());
        // double ay = fabs(sa.y() - sb.y());
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
        double ax = fabs(sa.x() - sb.x());
        //        double ay = fabs(sa.y() - sb.y());
        //        view->widthLineE=0;
        penForPainter.setWidthF(view->lineWidth);
        //        penForPainter.setWidthF(ax);
        //        penForPainter.setWidthF(ay);
        painter->setPen(penForPainter);
        //qreal x2 = penForPainter.widthF();
        painter->drawLine(QLineF(QPointF(),p2));
    }
    QLineF line() const { return QLineF(pos(),pos() + p2); }

    void mousePressEvent(QGraphicsSceneMouseEvent * event)
    {
        if (event->buttons() & Qt::RightButton)
        {
/*            QMenu m;
            QAction *scaleE = m.addAction("Scale z");
            QAction *what = m.exec(event->screenPos());
            if (what == scaleE)
            {
                //                view->scaleE();
            }
            update();// repaint();
*/        }
    }

};

gDraggable::gDraggable(EGView *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    setCursor(Qt::SizeVerCursor);
    penHover.setStyle(Qt::DashLine);//(Qt:: DotLine);
    penHover.setColor(Qt::blue);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::blue);

    setCursor(Qt::SizeHorCursor);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setZValue(999);
    setAcceptHoverEvents ( true );
}

QVariant gDraggable::itemChange(GraphicsItemChange change, const QVariant & value)
{
    switch (change)
    {
    case ItemPositionChange:
        if (isSelected())
        {
            QPointF newpos = value.toPointF();
            QPointF oldpos = pos();
            newpos.setY( oldpos.y() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newG = pos().x();
            QRectF vp = view->sceneRect();//
            newG=view->Gmin+(view->Gmax-view->Gmin)*newG/vp.width();
            view->model->set_G(newG);
            return QVariant();

//            double newG = pos().x();
//            view->model->set_G(newG);
//            return QVariant();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}
EGView::EGView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m),lineh(0),linev(0),lineG(0), rectEG(0), rectFirst(0),
Emin(0.001), Emax(20), hE(0.05), Gmax(0.1), Gmin(-3.), dG(0.05), dialogG(0),gbScaleXY(0),numberOfCurves(0)
{
    curve_number=-1;
    Erase = true; // this must initially be true
    lineWidth = 3;
    leEmin = NULL;
    leEmax = NULL;
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
    setMinimumSize(260, 100);
//    setScalesFromModel();
    initDialogScaleY();
    setViewportMapping();
    connect(model,SIGNAL(signalGChanged(double)),this,SLOT(slotGLine()));
    // connect(this,SIGNAL(signalScaleEGChanged()),this,SLOT(redrawCurves()));
    connect(this,SIGNAL(signalScaleEGChanged()),this,SLOT(resizePicture()));
    // connect(this,SIGNAL(signalScaleEGChanged()),this,SLOT(slot_Ec_n()));
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_drawEc_n()));
}
/*EGView::~EGView()
{
    disconnect(this, 0, 0, 0);
    if (!dialogG) delete dialogG;
    if (!gbScaleXY) delete gbScaleXY;
    for (QMap<int,EGCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
QPair<double,double> EGView::getEGScale()
{
    return QPair<double,double>(Emax,Emin);
}
void EGView::setEGScale(const QPair<double,double> &s)
{
    Emax = s.first;
    Emin = s.second;
}
void EGView::slotGLine()
{
    PotentialType type = model->getPotentialType();
    if(type!=QUASISTATIONARY) return;
    gParameters tp = model->getGParam();
    double g=tp.g;
    if(g>tp.gmax) g=tp.gmax;
    if(g<tp.gmin) g=tp.gmin;
    tp.g=g;
    model->setGParam(tp);
    QRectF vp = scene()->sceneRect();
    if (!lineG)
    {
        lineG = new gDraggable(this);
        scene()->addItem(lineG);
    }
    lineG->setLine(vp.width()*(g-Gmin)/(Gmax-Gmin), 0., vp.height());
}

void EGView::setScalesFromModel()
{
    QPair<double,double> Emax_Emin = this->getEGScale();
    Emax = Emax_Emin.first;
    Emin = Emax_Emin.second;
}

void EGView::resizePicture()
{
    PotentialType type = model->getPotentialType();
    if(type!=QUASISTATIONARY) return;
    gParameters tp = model->getGParam();
    if(tp.gmax!=Gmax||tp.gmin!=Gmin||dG!=tp.hg)
    {
        Gmin=tp.gmin;
        Gmax=tp.gmax;
        dG=tp.hg;
        GG=tp.g;
    }
    setViewportMapping();
    slotGLine();
    slot_Ec_n();
}
void EGView::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
    double rGmin=0;
    double rGmax=a.width();//npoints-1;
    double rEMin=0;
    double rEMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rGmin,rEMin),QPointF(rGmax,rEMax));
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

void EGView::resizeEvent(QResizeEvent*)
{
    PotentialType type = model->getPotentialType();
    setViewportMapping();
    slot_drawEc_n();
    slotGLine();
}

void EGView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void EGView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void EGView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void EGView::clearAll()
{
    if(rectFirst)
    {
        scene()->removeItem(rectFirst);
    }
    /*
    for ( QMap<int,EGCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
    int m = i.key();
    removeCurve(m);
    }
    numberOfCurves=0;*/
}

void EGView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
        clearAll();
        break;
    case Qt::Key_Plus:
        scaleView(qreal(1.2));
        break;
    case Qt::Key_Minus:
        scaleView(1 / qreal(1.2));
        break;
    case Qt::Key_Space:
        setBreakStatus(1);
        break;
    case Qt::Key_Enter:
        setBreakStatus(1);
        break;
    case Qt::Key_Escape:
        setBreakStatus(1);
        break;
    case Qt::Key_Pause :
        setBreakStatus(1);
        break;
    }
    QGraphicsView::keyPressEvent(event);
}
static const QColor colorForIds[12] = {
    Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::black,
    Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
};
const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

//zeros on the complex plane

void EGView::slot_drawEc_n()
{
    PotentialType type = model->getPotentialType();
    if(type!=QUASISTATIONARY) return;
    if (! isVisible()) return;
    // scene()->clear();
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    QRectF vp = scene()->sceneRect();
    QPen p;
    if(rectFirst) scene()->removeItem(rectFirst);
    rectFirst = new QGraphicsRectItem(NULL,scene());
    QVector<complex> Equasi;
    Equasi = model->getEnquasi();
    gParameters tp = model->getGParam();
    double Gmin=tp.gmin;//-5;//model->get_Gmin();
    double Gmax=tp.gmax;//-0.0001;
    double GG=tp.g;//-0.0001;
    if(Equasi.size()==0) return;
    QBrush b(Qt::SolidPattern);
/*    for (int i=0; i < Equasi.size(); i++)
    {
        double gn = imag(Equasi[i]);
        if(gn<Gmin) Gmin=gn;
    }
   Gmin=-5.;//1.2*Gmin;
   */
    int a=10;
    for (int i=0; i < Equasi.size(); i++)
    {
        double gg = imag(Equasi[i]);
        double Er = real(Equasi[i]);
        b.setColor(Qt::red);
        QGraphicsRectItem *r = new QGraphicsRectItem(rectFirst);
        // QGraphicsRectItem *r = new QGraphicsRectItem(NULL,scene());
        r->setBrush(b);
        r->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
        // r->setRect(vp.width()*(gg-a/2-Gmin)/(Gmax-Gmin),vp.height()*(Er-a/2-Emin)/(Emax-Emin), 5, 5 );
    }
    update();

}
void EGView::slot_Ec_n()
{
    if (! isVisible()) return;
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    gParameters tp = model->getGParam();
    double Gmin=tp.gmin;//-5;//model->get_Gmin();
    double Gmax=tp.gmax;//-0.0001;
    double dG=tp.hg;//0.01;
    double g0=Gmax;
    GG=tp.g;
    int npoints=1+(g0-Gmin)/dG;
    int nmxold=-1;
    double Er=Emin;
    /* if (0)
    {
    scene()->clear();
    }
    else
    {
    QList<QGraphicsItem *> list = scene()->items();
    QList<QGraphicsItem *>::Iterator it = list.begin()+1;
    // QList<QGraphicsItem *>::Iterator it = list.begin();
    for ( ; it != list.end(); ++it )
    {
    if ( *it )
    {
    scene()->removeItem(*it);
    delete *it;
    }
    }
    }*/

    QVector<complex> Equasi;
    QBrush b(Qt::SolidPattern);
//    model->set_EmaxEmin(Emax,Emin);
    model->getColors(Emin, Emax, hE, Gmin, Gmax, dG);
    Equasi = model->getEnquasi();
    int nR=model->redBoundary.size();
    double a=2;
    QPointF pp=model->redBoundary[0];
    double gg=pp.x();
    Er=pp.y();
    if(rectFirst) scene()->removeItem(rectFirst);
    rectFirst = new QGraphicsRectItem(NULL,scene());
    b.setColor(Qt::red);//colorForIds[1 % size_colorForIds]);
    p.setColor(Qt::red);//colorForIds[1 % size_colorForIds]);
    rectFirst->setBrush(b);
    rectFirst->setPen(p);
    rectFirst->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
    for (int i=1; i < nR; i++)
    {
        QPointF pp=model->redBoundary[i];
        double gg=pp.x();
        Er=pp.y();
        QGraphicsRectItem *r = new QGraphicsRectItem(rectFirst);
        b.setColor(Qt::red);//colorForIds[1 % size_colorForIds]);
        p.setColor(Qt::red);//colorForIds[1 % size_colorForIds]);
        r->setBrush(b);
        r->setPen(p);
        r->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
    }
    for (int i=0; i < model->yellowBoundary.size(); i++)
    {
        QPointF pp=model->yellowBoundary[i];
        double gg=pp.x();
        Er=pp.y();
        QGraphicsRectItem *r = new QGraphicsRectItem(rectFirst);
        b.setColor(Qt::darkYellow);//colorForIds[1 % size_colorForIds]);
        p.setColor(Qt::darkYellow);//colorForIds[1 % size_colorForIds]);
        r->setBrush(b);
        r->setPen(p);
        r->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
    }
    for (int i=0; i < model->greenBoundary.size(); i++)
    {
        QPointF pp=model->greenBoundary[i];
        double gg=pp.x();
        Er=pp.y();
        QGraphicsRectItem *r = new QGraphicsRectItem(rectFirst);
        b.setColor(Qt::green);//colorForIds[1 % size_colorForIds]);
        p.setColor(Qt::green);//colorForIds[1 % size_colorForIds]);
        r->setBrush(b);
        r->setPen(p);
        r->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
    }
    for (int i=0; i < model->blueBoundary.size(); i++)
    {
        QPointF pp=model->blueBoundary[i];
        double gg=pp.x();
        Er=pp.y();
        QGraphicsRectItem *r = new QGraphicsRectItem(rectFirst);
        b.setColor(Qt::blue);//colorForIds[1 % size_colorForIds]);
        p.setColor(Qt::blue);//colorForIds[1 % size_colorForIds]);
        r->setBrush(b);
        r->setPen(p);
        r->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
    }
    a=10;
    for (int i=0; i < Equasi.size(); i++)
    {
        double gg = imag(Equasi[i]);
        Er = real(Equasi[i]);
        b.setColor(Qt::darkRed);
        QGraphicsRectItem *r = new QGraphicsRectItem(rectFirst);
        p.setColor(Qt::black);
        r->setPen(p);
        r->setRect(vp.width()*(gg-Gmin)/(Gmax-Gmin)-a/2,vp.height()*(Er-Emin)/(Emax-Emin)-a/2, a,a );
    }

        update();
}
/*
void EGView::redrawCurves()
{
    PotentialType type = model->getPotentialType();
    if(type==QUASISTATIONARY) return;
    if(type==FINITE&&curves.size()==0) return;
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
//    if(lineWidth==0)lineWidth=1;

    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);

    QRectF vp = scene()->sceneRect();
    zParameters tp = model->getzParam();
    double zt=tp.z;
    double hz=tp.hz;
    double zmin=tp.zmin;
    double zmax=tp.zmax;

    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    linev->setPen(p);
    lineh->setPen(p);
    linev->setLine(vp.width()*(-zmin)/(zmax-zmin), 0, vp.width()*(-zmin)/(zmax-zmin),vp.height() );
    lineh->setLine(0,vp.height()*(-Emin)/(Emax-Emin),vp.width(),vp.height()*(-Emin)/(Emax-Emin));
    for ( QMap<int,EGCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int id = i.key();
        QPolygonF ipg = curves[id]->polygon();
         QPolygonF ppg = pcurves[id];
        for (int k = 0; k < ipg.size(); ++k)
        {
            qreal newx = vp.width()*(-zmin)/(zmax-zmin) + ppg[k].x()*vp.width()/(zmax-zmin);
            qreal newy = vp.height()*(ppg[k].y()-Emin)/(Emax-Emin);
            ipg[k] = QPointF(newx,newy);
        }
        p.setColor(colorForIds[id % size_colorForIds]);
        if(ipg.size()>=1)setCurve(id,ppg,ipg,p);
    }
    update();
}
*/

void EGView::setCurve(int id,const QPolygonF & pcurve, const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
        pcurves[id] = pcurve;
    }
    else
    {
        EGCurve *c = new EGCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    update();
}

void EGView::removeCurve(int id)
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

void EGCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void EGView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu m;
    QFont font( "Serif", 10, QFont::DemiBold );
    m.setFont(font);
    QAction *scaleG = m.addAction(tr("G-scale"));
    QAction *scaleY = m.addAction(tr("E-scale"));
    QAction *what = m.exec(event->globalPos());
    if (what == scaleG)
    {
        this->showDialogG();
        update();
    }
    if (what == scaleY)
    {
        this->showDialogScaleY();
        update();
    }
}
void EGView::showDialogG()
{
    if (!dialogG)
    {
        dialogG = new GView(this);
        dialogG->setModel(model);
    }
    dialogG->show();
    dialogG->activateWindow();
    dialogG->setFocus();
}

EGWidget::EGWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("B_{N+1}(E+iG)"));

    QVBoxLayout *vl = new QVBoxLayout();
    egView = new EGView(model,this);
    vl->addWidget(egView);
    QHBoxLayout *hl = new QHBoxLayout();
    bRunEG = new QToolButton(this);
    bRunEG->setIcon(QIcon(":/images/player_play.png"));
    bRunEG->adjustSize();
    hl->addWidget(bRunEG);

    QLabel *ltext= new QLabel(this);
    ltext->setText(tr("G="));
    hl->addWidget(ltext);
    QLabel *lg= new QLabel(this);
    hl->addWidget(lg);
    connect(model, SIGNAL(signalGChanged(double)), lg, SLOT(setNum(double)));
    hl->addStretch();

    QCheckBox *ch=new QCheckBox(tr("Erase"),this);
    ch->setCheckState(Qt::Checked);//Qt::Unchecked);
    connect(ch, SIGNAL(stateChanged(int)), this, SLOT(slotErase())); //???
    hl->addWidget(ch);

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    hl->addWidget(buttonClose);
    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???

    connect(bRunEG,SIGNAL(clicked()),this,SLOT(slotRunEG()));
    // hl->addStretch();


    vl->addLayout(hl);

    setLayout(vl);

}
void EGWidget::slotErase()
{
    if(egView->Erase==true) egView->Erase=false;
    else egView->Erase=true;
}

void EGWidget::slotRunEG()
{
    bRunEG->setIcon(QIcon(":/images/player_stop.png"));//->setText(tr("STOP"));
    bRunEG->adjustSize();
    disconnect(bRunEG, SIGNAL(clicked()), this, SLOT(slotRunEG()));
    breakStatus.onButton(bRunEG);
    egView->resizePicture();
    breakStatus.noButton(bRunEG);
    // breakStatus.disconnect();
    bRunEG->setIcon(QIcon(":/images/player_play.png"));//->setText(tr("RUN "));
    bRunEG->adjustSize();
    connect(bRunEG, SIGNAL(clicked()), this, SLOT(slotRunEG()));
}

void EGView::initDialogScaleY()
{
    gbScaleXY = new QGroupBox(this);
    gbScaleXY->setWindowTitle(tr("Scales for real part E"));
    gbScaleXY->setWindowFlags(Qt::Window);
    gbScaleXY->setFont(QFont("Serif", 12, QFont::Bold ));

    QVBoxLayout *vl = new QVBoxLayout;
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("E_min"),this));
        h->addWidget(this->leEmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Emin);
        this->leEmin->setText(x);
        connect(this->leEmin,SIGNAL(editingFinished()),this,SLOT(updateScaleEG()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("E_max"),this));
        h->addWidget(this->leEmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->Emax);
        this->leEmax->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leEmax,SIGNAL(editingFinished()),this,SLOT(updateScaleEG()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hE"),this));
        h->addWidget(this->leHE= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->hE);
        this->leHE->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leHE,SIGNAL(editingFinished()),this,SLOT(updateScaleEG()));
        vl->addWidget(line);
    }
    gbScaleXY->setLayout(vl);
}

void EGView::showDialogScaleY()
{
    gbScaleXY->show();
    gbScaleXY->raise();
    gbScaleXY->setFocus();
}
void EGView::setScaleEG()
{
    QString x;
    this->leEmax->setText(x);
    x.sprintf("%lg",this->Emax);
    this->leEmax->setText(x);
    x.sprintf("%lg",this->Emin);
    this->leEmin->setText(x);
    this->leHE->setText(x);
    x.sprintf("%lg",this->hE);
}

void EGView::updateScaleEG()
{
    double EGminNew = this->leEmin->text().toDouble();
    double EGmaxNew = this->leEmax->text().toDouble();
    double heNew = this->leHE->text().toDouble();
    if (Emin != EGminNew||Emax != EGmaxNew||hE!=heNew)
    {
        Emin=EGminNew;
        Emax=EGmaxNew;
        hE=heNew;
        emit(signalScaleEGChanged());
    }
}
void EGWidget::readFromXml(QXmlStreamReader *r)
{
    Q_ASSERT(this);
    Q_ASSERT(r->isStartElement() && r->name() == "EGplane");
    double Emin = 0, Emax = 0, he=0;
    double Gmin = 0, Gmax = 0, hG=0, G=0;
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
            Emin = s.toDouble();
        }
        else if (r->name() == "Emax")
        {
            QString s = r->readElementText();
            Emax = s.toDouble();
        }
        else if (r->name() == "hE")
        {
            QString s = r->readElementText();
            he = s.toDouble();
        }
        else
            skipUnknownElement(r);
    }
    QPair<double,double> t;
    t.first=Emax;
    t.second=Emin;
    egView->Emax=Emax;
    egView->Emin=Emin;
    egView->hE=he;
    egView->setEGScale(t);
//    egView->model->set_EmaxEmin(Emax,Emin);

}


void EGWidget::writeToXml(QXmlStreamWriter *w)
{
    w->writeStartElement("EGplane");
    {
        QPair<double,double> t = egView->getEGScale();
        QString s;
        s.sprintf("%lg",t.first);
        w->writeTextElement("Emax",s);
        s.sprintf("%lg",t.second);
        w->writeTextElement("Emin",s);
        s.sprintf("%lg",egView->hE);
        w->writeTextElement("hE",s);
    }
    w->writeEndElement();
}
