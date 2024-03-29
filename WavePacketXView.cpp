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

#include "WavePacketXView.h"
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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLabel>


WavePacketXView::WavePacketXView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), dialogTime(0),
lineh(0), linev(0),linez(0),vectorFirst(0),lineWidth(2),
dialogWPEp(0),dialogWPEm(0),dialogScaleWPX(0),
whatToDraw(2)
{
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
//    initDialogWidth();
    if (1)
    {
        setViewportUpdateMode(BoundingRectViewportUpdate);///
        setCacheMode(CacheBackground);
        setRenderHint(QPainter::Antialiasing);///
        setTransformationAnchor(AnchorUnderMouse);///
        setResizeAnchor(AnchorViewCenter);///
    }
    setMinimumSize(260, 100);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(model,SIGNAL(signalScalesUChanged()),this,SLOT(reDrawAxes()));
//    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(reDraw()));
//    connect(model,SIGNAL(signalScaleWPXChanged()),this,SLOT(resizePicture()));

}
/*WavePacketXView::~WavePacketXView()
{
    disconnect(this, 0, 0, 0);
//    if (gbDefWP) delete gbDefWP;
    if (!dialogTime) delete dialogTime;
    if (!dialogWPEp) delete dialogWPEp;
    if (!dialogWPEm) delete dialogWPEm;
    if (!dialogScaleWPX) delete dialogScaleWPX;
    for (QMap<int,CoordinateDistributionCurve*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
}
*/
void WavePacketXView::resizePicture()
{
    ScaleWPXParameters tp = model->getScaleWPXParam();
    dx=tp.Hx;
    xmin=tp.Xmin;
    xmax=tp.Xmax;
    psiMax=tp.WPXmax;
    psiMin=tp.WPXmin;
    setViewportMapping();
    slot_WavePacket_of_t();
}
void WavePacketXView::setViewportMapping()
{
        ScaleWPXParameters sc = model->getScaleWPXParam();
        if(dx!=sc.Hx||xmin!=sc.Xmin||xmax!=sc.Xmax||psiMax!=sc.WPXmax||psiMin!=sc.WPXmin)
        {
            dx=sc.Hx;
            xmin=sc.Xmin;
            xmax=sc.Xmax;
            psiMax=sc.WPXmax;
            psiMin=sc.WPXmin;
        }
    QRectF a = QRectF(this->viewport()->rect());
    double rxmin=0;
    double rxmax=a.width();//npoints-1;
    double rpsiMin=0;
    double rpsiMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rxmin,rpsiMin),QPointF(rxmax,rpsiMax));
    scene()->setSceneRect(b);
//    QRectF sr = scene()->sceneRect();
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
void WavePacketXView::resizeEvent(QResizeEvent *)
{
    setViewportMapping();
    reDrawAxes();
//    reDraw();
//    clearAll();
}

void WavePacketXView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void WavePacketXView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void WavePacketXView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void WavePacketXView::keyPressEvent(QKeyEvent *event)
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
void WavePacketXView::clearAll()
{

        for ( QMap<int,CoordinateDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
            removeCurve(m);
    }
}
void WavePacketXView::reDraw()
{
    if (! isVisible()) return;
    QRectF vp = scene()->sceneRect();
    QPen p;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    //    if(lineWidth==0)lineWidth=1;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    linev->setPen(p);
    lineh->setPen(p);
    linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
    lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
    int npoints;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    p.setColor(Qt::darkCyan);
    waveFunction.resize(npoints);
    for (int i=0; i < npoints; i++)
    {
        double x = (i*vp.width())/(npoints-1);
        double y =vp.height()*(waveFunction[i]-psiMin)/(psiMax-psiMin);
        psi[i]  = QPointF(x, y);
    }
    setCurve(0, psi, p);
}
void WavePacketXView::reDrawAxes()
{
    QRectF vp = scene()->sceneRect();
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidthF(lineWidth);
    pl.setWidthF(0);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    pl.setColor(Qt::lightGray);
    for ( QMap<int,CoordinateDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if(vectorFirst) scene()->removeItem(vectorFirst);
    if(whatToDraw<3)
    {
        if(linez)
        {
            scene()->removeItem(linez);
            linez=0;
        }
        if(!linev)
        {
            lineh = new QGraphicsLineItem();
            linev = new QGraphicsLineItem();
            scene()->addItem(lineh);
            scene()->addItem(linev);
        }
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
        lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
        p.setColor(Qt::darkCyan);
    }
    else
    {
        xn= vp.width()/2.;
        yn =2*vp.height()/3.;
        xk = 0;
        yk = 0;
        cs = (xn-xk)/(xmax-xmin);
        sn = -(yk-yn)/(xmax-xmin);
        if(!linez)
        {
            linez = new QGraphicsLineItem();
            scene()->addItem(linez);
            linez->setPen(p);
        }
        if(!linev)
        {
            lineh = new QGraphicsLineItem();
            linev = new QGraphicsLineItem();
            scene()->addItem(lineh);
            scene()->addItem(linev);
        }
        linev->setLine(xn, yn, xn, vp.height());
        lineh->setLine(xn, yn, vp.width(), yn);
        linez->setLine(xn, yn, xk, yk);
        linev->setPen(p);
        lineh->setPen(p);
    }
}
void WavePacketXView::setInitialTime()
{
    TimeParameters tp=model->getTimeParam();
    this->time=tp.time;
    this->htime=tp.ht;
    this->tmin=tp.tmin;
    this->tmax=tp.tmax;
    double tt=this->time+this->htime;
    if(tt>=this->tmax)
    {
        tt=this->tmin;
        this->time=tt;
    }
    if(tt<this->tmin)
    {
        tt=this->tmax;
        this->time=tt;
    }
}
QPointF WavePacketXView::getPoint(bool first, double xi, complex yi)

{
    double x,y;
    QRectF vp = scene()->sceneRect();
    if(whatToDraw==3)
            {
                double yim =vp.height()*imag(yi)/psiMax;
                double yre =vp.height()*real(yi)/psiMax;
                double xz = xn-(xi-xmin)*cs;
                double yz = yn-(xi-xmin)*sn;
                y = yz+yim;
                x = xz+yre;
                if(first)
                {
                    vectorFirst->setLine(xz,yz,x,y);
                    vectorFirst->setPen(pl);
                }
                else
                {
                    QGraphicsLineItem *l = new QGraphicsLineItem(vectorFirst);
                    l->setPen(pl);
                    l->setLine(xz,yz,x,y);
                }
            }
            else
            {
                switch(whatToDraw)
                {
                case 2:
                    y=squaremod(yi);
                    break;
                case 0:
                    y=real(yi);
                    break;
                case 1:
                    y=imag(yi);
                    break;
                default:
                    break;
                }
                x = (xi-xmin)*vp.width()/(xmax-xmin);
                y =vp.height()*(y-psiMin)/(psiMax-psiMin);
            }
return QPointF(x,y);
}
void WavePacketXView::slot_WavePacket_of_t()
{
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    if (! isVisible()) return;
    for ( QMap<int,CoordinateDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    reDrawAxes();
    int npoints;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    waveFunctionC.resize(npoints);
    setInitialTime();
    PotentialType type = model->getPotentialType();
    if(vectorFirst) scene()->removeItem(vectorFirst);
    vectorFirst = new QGraphicsLineItem();
    TimeParameters tp=model->getTimeParam();
    SettingParameters ts;
    for (double t=this->time; t>=tp.tmin&&t<=tp.tmax; t+=htime)
    {
        tp.time=t;
        tp.ht=htime;
        model->setTimeParam(tp);
        ScaleWPXParameters sc = model->getScaleWPXParam();
        vp = scene()->sceneRect();
        ts=model->getSettingParameters();
        if(vp!=vp_old||dx!=sc.Hx||xmin!=sc.Xmin||xmax!=sc.Xmax||psiMax!=sc.WPXmax||psiMin!=sc.WPXmin||lineWidth!=ts.lineWidth)
        {
            lineWidth=ts.lineWidth;
            p.setWidthF(lineWidth);
            dx=sc.Hx;
            xmin=sc.Xmin;
            xmax=sc.Xmax;
            psiMax=sc.WPXmax;
            psiMin=sc.WPXmin;
            npoints=1+(xmax-xmin)/this->dx;
            psi.resize(npoints);
            waveFunction.resize(npoints);
            waveFunctionC.resize(npoints);
            setViewportMapping();
            reDrawAxes();
            vp_old=vp;
        }
            waveFunctionC = model->getPsi3DOfXT(t,xmin,xmax,npoints);
 //       if(whatToDraw==3)
        {
            if(vectorFirst) scene()->removeItem(vectorFirst);
            vectorFirst = new QGraphicsLineItem();
        }
            bool first=true;
        for (int i=0; i < npoints; i++)
        {
            psi[i]  = getPoint(first, xmin + dx*i,waveFunctionC[i]);
            first=false;

/*            complex yi = waveFunctionC[i];
            if(whatToDraw==3)
            {
                double xi = xmin + dx*i;//(i*vp.width())/(npoints-1);
                double yim =vp.height()*(imag(waveFunctionC[i]))/psiMax;
                double yre =vp.height()*(real(waveFunctionC[i]))/psiMax;
                double xz = xn-(xi-xmin)*cs;
                double yz = yn-(xi-xmin)*sn;
                y = yz+yim;
                x = xz+yre;
                if(i==0)
                {
                    vectorFirst->setLine(xz,yz,x,y);
                    vectorFirst->setPen(pl);
                }
                else
                {
                    QGraphicsLineItem *l = new QGraphicsLineItem(vectorFirst);
                    l->setPen(pl);
                    l->setLine(xz,yz,x,y);
                }
            }
            else
            {
                switch(whatToDraw)
                {
                case 2:
                    y=squaremod(yi);
                    break;
                case 0:
                    y=real(yi);
                    break;
                case 1:
                    y=imag(yi);
                    break;
                default:
                    break;
                }
                x = (i*vp.width())/(npoints-1);
                y =h*(y-psiMin)/(psiMax-psiMin);
            }
            waveFunction[i]=y;
            psi[i]  = QPointF(x, y);*/
        }
        p.setColor(Qt::red);//Qt::darkRed);
        setCurve(0, psi, p);
        if (getBreakStatus(0))
        {
            return;
        }
/*        tp=model->getTimeParam();
        if(t!=tp.time||htime!=tp.ht)
        {
            this->time=tp.time;
            htime=tp.ht;
        }*/
    }
    model->setTimeParam(tp);
}

void WavePacketXView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
{

    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        CoordinateDistributionCurve *c = new CoordinateDistributionCurve(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    update();
}

void WavePacketXView::removeCurve(int id)
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

void CoordinateDistributionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget *)
{
    (void)option;
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void WavePacketXView::showDialogScaleY()
{
    if (!dialogScaleWPX)
    {
        dialogScaleWPX = new ScaleWPX(this);
        dialogScaleWPX->setModel(model);
    }
    dialogScaleWPX->show();
    dialogScaleWPX->activateWindow();
    dialogScaleWPX->setFocus();
}
void WavePacketXView::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu m;
        QFont font( "Serif", 10, QFont::DemiBold );
        m.setFont(font);
        QAction *wpdefEm = m.addAction(m.tr("Wave packet definition:E_n<0"));
        QAction *wpdefEp = m.addAction(m.tr("Wave packet definition:E_j>0"));
        QAction *time = m.addAction(m.tr("Time parameters"));
        QAction *scalePsi = m.addAction(m.tr("Scales"));
//        QAction *width = m.addAction(m.tr("width of line"));
        QAction *what = m.exec(event->globalPos());
        if (what == wpdefEm)
        {
            this->slotSetWPEm();//showDialogDefWPEm();
            update();
        }
        if (what == wpdefEp)
        {
            this->slotSetWPEp();//showDialogDefWPEp();
            update();
        }
        if (what == time)
        {
            this->slotSetTime();
            update();
        }
        if (what == scalePsi)
        {
            this->showDialogScaleY();
            update();
        }
/*        if (what == width)
        {
            this->showDialogWidth();
            update();
        }*/
        event->accept();
}
void  WavePacketXView::slotSetWPEm()
{
    if (!dialogWPEm)
    {
        dialogWPEm = new WPparametersM(this);
        dialogWPEm->setModel(model);
    }
    dialogWPEm->show();
    dialogWPEm->activateWindow();
    dialogWPEm->setFocus();
}
void  WavePacketXView::slotSetWPEp()
{
    if (!dialogWPEp)
    {
        dialogWPEp = new WPparametersP(this);
        dialogWPEp->setModel(model);
    }
    dialogWPEp->show();
    dialogWPEp->activateWindow();
    dialogWPEp->setFocus();
}

void WavePacketXView::slotSetTime()
{
    if (!dialogTime)
    {
        dialogTime = new TimeView(this);
        dialogTime->setModel(model);
    }
    dialogTime->show();
    dialogTime->activateWindow();
    dialogTime->setFocus();
}

void WavePacketXView::setWhatToDraw(int w)
{
    if (whatToDraw != w)
    {
        if(w==3&&whatToDraw<3||w<3&&whatToDraw==3)
        {
            whatToDraw = w;
            reDrawAxes();
        }
        else whatToDraw = w;
        emit( whatToDrawChanged(w) );
    }
}
int WavePacketXView::getWhatToDraw()
{
    return whatToDraw;
}

WavePacketXWidget::WavePacketXWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Coordinate distribution: time development"));
    QVBoxLayout *vl = new QVBoxLayout();
    wavePacketXView = new WavePacketXView(model,this);
    vl->addWidget(wavePacketXView);

    QHBoxLayout *hl = new QHBoxLayout();
    bRunPsiXT = new QToolButton(this);
    bRunPsiXT->setIcon(QIcon(":/images/player_play.png"));
    bRunPsiXT->adjustSize();
//    bRunPsiXT = new QPushButton(tr("Run "));
    connect(bRunPsiXT,SIGNAL(clicked()),this,SLOT(slotRunWP()));
    //------------
    QString psi=QChar(0x03C8);
    //               QString Psi=QChar(0x03A8);
    QString to2=QChar(0x00B2);
    QString psiofx=psi+"(x)";
    QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
    //            QGroupBox *gb3 = new QGroupBox(tr("Wave function ")+psiofx);
    QRadioButton *rad1= new QRadioButton("Re "+psi);
    QRadioButton *rad2= new QRadioButton("Im "+psi);
    QRadioButton *rad3= new QRadioButton(mod_psiofx);
    QRadioButton *rad4= new QRadioButton("3D");
    hl->addWidget(rad1);
    hl->addWidget(rad2);
    hl->addWidget(rad3);
    hl->addWidget(rad4);
    bgR = new QButtonGroup(this);
    bgR->setExclusive(true);
    bgR->addButton(rad1,0);
    bgR->addButton(rad2,1);
    bgR->addButton(rad3,2);
    bgR->addButton(rad4,3);
    bgR->button(2)->setChecked(true);
    connect(bgR,SIGNAL(buttonClicked(int)),wavePacketXView,SLOT(setWhatToDraw(int)));
    connect(wavePacketXView,SIGNAL(whatToDrawChanged(int)),this,SLOT(checkButton(int)));
    QLabel *ltext= new QLabel(this);
    ltext->setText(tr("time:"));
    QLabel *ltime= new QLabel(this);
    connect(model, SIGNAL(signalTimeChanged(double)), ltime, SLOT(setNum(double)));

    hl->addWidget(bRunPsiXT);
    hl->addWidget(ltext);
    hl->addWidget(ltime);

    //    QPushButton *buttonClose = new QPushButton(tr("Close"));
    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);
    setLayout(vl);
}
void WavePacketXWidget::checkButton(int index)
{
    this->bgR->button(index)->setChecked(true);
}

void WavePacketXWidget::slotRunWP()
{
    bRunPsiXT->setIcon(QIcon(":/images/player_pause.png"));
    bRunPsiXT->adjustSize();
//    bRunPsiXT->setText(tr("STOP"));
    disconnect(bRunPsiXT, SIGNAL(clicked()), this, SLOT(slotRunWP()));
    breakStatus.onButton(bRunPsiXT);
    wavePacketXView->resizePicture();
    breakStatus.noButton(bRunPsiXT);
    bRunPsiXT->setIcon(QIcon(":/images/player_play.png"));
    bRunPsiXT->adjustSize();
//    bRunPsiXT->setText(tr("RUN "));
    connect(bRunPsiXT, SIGNAL(clicked()), this, SLOT(slotRunWP()));
}

