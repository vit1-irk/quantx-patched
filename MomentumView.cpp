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

#include "MomentumView.h"
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
#include "ScalePhin.h"

MomentumView::MomentumView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), dialogScalePhin(0),lineh(0),linev(0)//,
{
    lineWidth = 3;;
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
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_Phi_n_of_k()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalScalePhinChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalLevelParametersChanged()),this,SLOT(resizePicture()));
    resizePicture();
}

/*MomentumView::~MomentumView()
{
    disconnect(this, 0, 0, 0);
    if (!dialogScalePhin) delete dialogScalePhin;
    for (QMap<int,MomentumDistribution*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
void MomentumView::resizePicture()
{
    ScalePhinParameters tp = model->getScalePhinParam();
    dk=tp.Hk;
    kmin=tp.Kmin;
    kmax=tp.Kmax;
    phiMax=tp.Phinmax;
    phiMin=tp.Phinmin;
    setViewportMapping();
    PotentialType type = model->getPotentialType();
    slot_Phi_n_of_k();
    if(type==PERIODIC)  slotEnergyChanged();
}

void MomentumView::setViewportMapping()
{
    QRectF a = QRectF(this->viewport()->rect());
    double rxmin=0;
    double rxmax=a.width();//npoints-1;
    double rpsiMin=0;
    double rpsiMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rxmin,rpsiMin),QPointF(rxmax,rpsiMax));
    scene()->setSceneRect(b);
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
     QRectF   sr = scene()->sceneRect();
    update();
/*
    QRectF vp = QRectF(QPointF(this->kmin,this->phiMin),QPointF(this->kmax,this->phiMax));

    QRectF sr = scene()->sceneRect();
//    if (vp != sr)
    {
        scene()->setSceneRect(vp);

        QRectF b = vp; //scene()->sceneRect();
        QRectF a = QRectF(this->viewport()->rect());
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height();
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
        sr = scene()->sceneRect();
    }
    update();*/
}
void MomentumView::resizeEvent(QResizeEvent *e)
{
        resizePicture();
//    setViewportMapping();
}

void MomentumView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void MomentumView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
    event->accept();
}
void MomentumView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}
void MomentumView::clearAll()
{

        for ( QMap<int,MomentumDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
}
void MomentumView::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_Enter:
        break;
    }
    QGraphicsView::keyPressEvent(event);
}
#define ID_PSI_ENERGY (200)
void MomentumView::slotEnergyChanged()
{
    if (! isVisible()) return;
    LevelNumberParameters wp = model->getLevelNumberParameters();
    int nMin=wp.nmin;
    int nMax=wp.nmax;
    int hn=wp.hn;
    if(nMin>0&&nMax>0&&hn>0) return;
    PotentialType type = model->getPotentialType();
    if(type!=PERIODIC) return;
    QRectF vp = scene()->sceneRect();
    //    QRect a = QRect(this->viewport()->rect());
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    p.setWidth(lineWidth);
    p.setColor(Qt::magenta);//Qt::darkRed);
    double E=model->get_E0();
    model->set_E0(E);
    double qa = model->get_qa();
    if(model->discrim>0) return;
    double     ax=0;
    for(int n=1; n<=model->getN(); n++)
    {
        ax += model->get_d(n);
    }
    double bk = 2*M_PI/ax;//this is bb in my old programm
//    double qa = model->get_qa();
    double qx = qa/ax;
    int nqmin = (kmin-qx)/bk;
    int nqmax = (kmax-qx)/bk;
    int np = nqmax-nqmin+1;
    QVector<double> waveFunction(np);
    QPolygonF phi;
    phi.resize(3*np+2);
    waveFunction = model->getPhiOfkPer(kmin,kmax);
    double x = 0;
    double y0 = vp.height()*(-phiMin)/(phiMax-phiMin);
    phi[0]=QPointF(0., y0);
    for (int n = 0; n < np; n++)
    {
        double q = (nqmin+n)*bk+qx;
        x = (q-kmin)*vp.width()/(kmax-kmin);
        double y = vp.height()*(waveFunction[n]-phiMin)/(phiMax-phiMin);

        phi[3*(n+1)-2]=QPointF(x, y0);
        phi[3*(n+1)-1]=QPointF(x, y);
        phi[3*(n+1)]=QPointF(x, y0);
    }
    x = vp.width();
    phi[3*np+1]=QPointF(x, y0);
    setCurve(ID_PSI_ENERGY, phi, p);
    update();
}
static const QColor colorForIds[12] = {
    Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::darkYellow,
    Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::black
};
const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
static const QColor colorForIdsP[12] = {
    Qt::darkRed, Qt::red, Qt::darkGreen, Qt::green, Qt::darkBlue, Qt::blue,
    Qt::darkCyan,Qt::cyan, Qt::darkMagenta,Qt::magenta,
    Qt::black,Qt::gray
};
const int size_colorForIdsP = sizeof(colorForIdsP)/sizeof(colorForIdsP[0]);

void MomentumView::slot_Phi_n_of_k()
{
    if (! isVisible()) return;
    PotentialType type = model->getPotentialType();
    if(type==QUASISTATIONARY) return;
    QRectF vp = scene()->sceneRect();
    QRect a = QRect(this->viewport()->rect());
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidth(2);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::gray);
    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    if(number_of_levels==0) return;
    LevelNumberParameters wp = model->getLevelNumberParameters();
    int nMin=wp.nmin;
    int nMax=wp.nmax;
    int hn=wp.hn;
    if(nMin<0||nMax<0||hn<0||nMin>number_of_levels-1||nMax<nMin) return;
    for ( QMap<int,MomentumDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        if (n >= number_of_levels)
        {
            removeCurve(n);
        }
        if(n>nMax&&nMax<number_of_levels-1) removeCurve(n);
        if(n<nMin&&nMin<number_of_levels-1) removeCurve(n);
    }
        if(!linev)
        {
            lineh = new QGraphicsLineItem();
            linev = new QGraphicsLineItem();
            linev->setZValue(0);
            lineh->setZValue(0);
            scene()->addItem(lineh);
            scene()->addItem(linev);
        }
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-kmin)/(kmax-kmin), 0, vp.width()*(-kmin)/(kmax-kmin),vp.height() );
        lineh->setLine(0,vp.height()*(-phiMin)/(phiMax-phiMin),vp.width(),vp.height()*(-phiMin)/(phiMax-phiMin));
    if(type==PERIODIC)
    {
        p.setWidth(lineWidth);
        double     ax=0;
        for(int n=1; n<=model->getN(); n++)
        {
            ax += model->get_d(n);
        }
        double bk = 2*M_PI/ax;//this is bb in my old programm
        int np,nqmin,nqmax;
        double qx,e;
        QVector<double> waveFunction;
        for (int j = nMin; j <= nMax; j+= hn)
        {
            if(j>number_of_levels-1) break;
            double ee=Ebound[j];
            model->set_E0(ee);
            if(model->discrim>0) break;
            qx = model->get_qa()/ax;
            nqmin = (kmin-qx)/bk;
            nqmax = (kmax-qx)/bk;
            np = nqmax-nqmin+1;
            waveFunction.resize(np);
            waveFunction = model->getPhiOfkPer(kmin,kmax);
            QPolygonF phi;
            phi.resize(3*np+2);
            double x = 0;
            double y0 = vp.height()*(2*j-phiMin)/(phiMax-phiMin);
            phi[0]=QPointF(0., y0);
            for (int n = 0; n < np; n++)
            {
                double q = (nqmin+n)*bk+qx;
                x = (q-kmin)*vp.width()/(kmax-kmin);
                double y = vp.height()*(2*j+waveFunction[n]-phiMin)/(phiMax-phiMin);

                phi[3*(n+1)-2]=QPointF(x, y0);
                phi[3*(n+1)-1]=QPointF(x, y);
                phi[3*(n+1)]=QPointF(x, y0);
            }
            x = vp.width();
            phi[3*np+1]=QPointF(x, y0);
            p.setColor(colorForIdsP[j % size_colorForIdsP]);
            setCurve(j, phi, p);
        }
        update();
    }
    else
    {
        int npoints;
        QVector<double> waveFunction;
        QPolygonF phi;
        npoints=1+(kmax-kmin)/this->dk;
        phi.resize(npoints);
        waveFunction.resize(npoints);
        p.setWidth(lineWidth);
        for (int n = nMin; n <= nMax; n+= hn)
        {
            if(n>number_of_levels-1) break;
            waveFunction = model->getPhiOfk(Ebound[n],kmin,kmax,npoints);
            for (int i=0; i < npoints; i++)
            {
                double x = (i*vp.width())/(npoints-1);
                double y =vp.height()*(waveFunction[i]-phiMin)/(phiMax-phiMin);
                phi[i]  = QPointF(x, y);
            }
            p.setColor(colorForIds[n % size_colorForIds]);
            setCurve(n, phi, p);
        }
        update();
    }
 }

void MomentumView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        MomentumDistribution *c = new MomentumDistribution(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    curves[id]->setZValue(1);
//    repaint();
    update();
}

void MomentumView::removeCurve(int id)
{
    QGraphicsItem *item = curves[id];
    if (item)
    {
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0;
    }//this is dangerous: curves.remove(id);
    update();
}

void MomentumDistribution::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void MomentumView::showDialogScale()
{
    if (!dialogScalePhin)
    {
        dialogScalePhin = new ScalePhin(this);
        dialogScalePhin->setModel(model);
    }
    dialogScalePhin->show();
    dialogScalePhin->activateWindow();
    dialogScalePhin->setFocus();
}

void MomentumView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *scalePsi = m.addAction("Set scales");
        QAction *what = m.exec(event->globalPos());
        if (what == scalePsi)
        {
            this->showDialogScale();
            update();
        }
}

MomentumDistibutionWidget::MomentumDistibutionWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Momentum distribution: bound states"));

    QVBoxLayout *vl = new QVBoxLayout();
    MomentumView *momentumView = new MomentumView(model);
    vl->addWidget(momentumView);

    QHBoxLayout *hl = new QHBoxLayout();
//    QPushButton * reset = new QPushButton("&Resize");
//    QPushButton *buttonClose = new QPushButton(tr("Close"));
    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

//    hl->addWidget(reset);

//    connect(reset,SIGNAL(clicked()),momentumView,SLOT(resizePicture()));
    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);

    setLayout(vl);
}
