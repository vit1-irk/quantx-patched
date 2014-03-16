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

#include "WaveFunctionView.h"
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
//#include "ScalesUx.h"
#include "ScalePsin.h"
#include "LevelNumber.h"
#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QRadioButton>

WaveFunctionView::WaveFunctionView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m),
dialogLevNum(0), lineh(0), linev(0),linez(0),vectorFirst(0),dialogScalePsin(0),//gbWidth(0),//leW(0),
xmin(-1.), xmax(10.),dx(0.01),
nMax(4),nMin(0),hn(1),lineWidth(2),
psiMax(1.), psiMin(-1.), whatToDraw(0)
{
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
//    initDialogWidth();
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(resizePicture()));
//    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_Psi_n_of_x()));
    connect(model,SIGNAL(signalScalePsinChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalScalePsinChanged()),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalEnergyChanged(double)),this,SLOT(slotEnergyChanged()));
//    connect(model,SIGNAL(signalZChanged(double)),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slotEnergyChanged()));

    connect(model,SIGNAL(signalLevelParametersChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));

    setViewportMapping();
    resizePicture();
}
#define ID_PSI_ENERGY (200)
void WaveFunctionView::slotEnergyChanged()
{
    if (! isVisible()) return;
//    clearAll();
    LevelNumberParameters wp = model->getLevelNumberParameters();
    nMin=wp.nmin;
    nMax=wp.nmax;
    hn=wp.hn;
    if(nMin>0&&nMax>0&&hn>0) return;
    QRectF vp = scene()->sceneRect();
    QPen p,pl;
    p.setWidth(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);//Qt::darkRed);
    pl.setColor(Qt::lightGray);
    pl.setWidth(1);
    double xn,yn,cs,sn;
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
    }
    else
    {
        xn= vp.width()/2.;
        yn =2*vp.height()/3.;
        double xk = 0;
        double yk = 0;//vp.height();
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
    p.setColor(Qt::gray);//Qt::darkRed);
    complex Ec;
    double E;
    PotentialType type = model->getPotentialType();
    E=model->get_E0();
    int npoints;//=501;
    QVector<complex> waveFunction;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    if(type==QUASISTATIONARY) return;
    double h=vp.height();
    if(vectorFirst) scene()->removeItem(vectorFirst);
    vectorFirst = new QGraphicsLineItem();
    bool tail=false;
        if(type==PERIODIC) waveFunction = model->getPsiOfX_per(E,xmin,xmax,npoints);
        if(type==FINITE) waveFunction = model->getPsiOfX(E,xmin,xmax,npoints,tail);
        for (int i=0; i < npoints; i++)
        {
            double x,yim,yre,y;
            yre = real(waveFunction[i]);
            yim = imag(waveFunction[i]);
            if(whatToDraw==0) y = yre;
            if(whatToDraw==1) y = yim;
            if(whatToDraw==2) y = yre*yre+yim*yim;
            if(whatToDraw==3)
            {
                double xi = xmin + dx*i;
                yim =vp.height()*yim/psiMax;
                yre =vp.height()*yre/psiMax;
                double xz = xn-(xi-xmin)*cs;
                double yz = yn-(xi-xmin)*sn;
                y = yz+yim;
                x = xz+yre;
                if(y>1e4*psiMax) y=1e4*psiMax;
                if(y<-1e4*psiMax) y=-1e4*psiMax;
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
                x = (i*vp.width())/(npoints-1);
                y =vp.height()*(y-psiMin)/(psiMax-psiMin);
                if(y>1e4*psiMax) y=1e4*psiMax;
                if(y<-1e4*psiMax) y=-1e4*psiMax;
            }
            psi[i]  = QPointF(x, y);
        }
    p.setColor(Qt::darkCyan);//Qt::darkRed);
    setCurve(ID_PSI_ENERGY, psi, p);
}
/*void WaveFunctionView::slotZChanged()
{
    QRectF vp = scene()->sceneRect();
    QRect a = QRect(this->viewport()->rect());
    QPen p;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::darkCyan);//Qt::darkRed);
    double E=model->get_E0();
    int npoints;//=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    waveFunction = model->getPsiOfX(E,xmin,xmax,npoints,whatToDraw,false);
    double h=vp.height();
        for (int i=0; i < npoints; i++)
        {
            double x = (i*vp.width())/(npoints-1);
            double y =h*(waveFunction[i]-psiMin)/(psiMax-psiMin);
            if(y>10*h)
            {
                y=10*h;
            }
            if(y<-10*h)
            {
                y=-10*h;
            }
            psi[i]  = QPointF(x, y);
        }
    setCurve(ID_PSI_ENERGY, psi, p);
}
*/
WaveFunctionView::~WaveFunctionView()
{
    disconnect(this, 0, 0, 0);
    if(!dialogScalePsin) delete dialogScalePsin;
    if(!dialogLevNum) delete dialogLevNum;
    for (QMap<int,CoordinateDistribution*>::iterator i = curves.begin(); i != curves.end(); ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if (!lineh) delete lineh;
    if (!linev) delete linev;
    if (!linez) delete linez;
}
void WaveFunctionView::resizePicture()
{
    ScalePsinParameters tp = model->getScalePsinParam();
    if(this->dx!=tp.Hx||xmin!=tp.Xmin||xmax!=tp.Xmax||psiMax!=tp.Psinmax||psiMin!=tp.Psinmin)
    {
        dx=tp.Hx;
        xmin=tp.Xmin;
        xmax=tp.Xmax;
        psiMax=tp.Psinmax;
        psiMin=tp.Psinmin;
    }
    setViewportMapping();
    LevelNumberParameters wp = model->getLevelNumberParameters();
    nMin=wp.nmin;
    nMax=wp.nmax;
    hn=wp.hn;
    if(nMin<0||nMax<0||hn<0) slotEnergyChanged();
    else  slot_Psi_n_of_x();
}

void WaveFunctionView::setScalesFromModel()
{
    QPair<double,double> xmin_xmax = model->getXminXmax();
    xmin = xmin_xmax.first;
    xmax = xmin_xmax.second;
    ScalePsinParameters tp;
    tp.Hx = this->dx;
    tp.Xmin = xmin;
    tp.Xmax = xmax;
    tp.Psinmin = psiMin;
    tp.Psinmax = psiMax;
    model->setScalePsinParam(tp);
}
void WaveFunctionView::setViewportMapping()
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
}

void WaveFunctionView::resizeEvent(QResizeEvent*)
{
//    setViewportMapping();
    resizePicture();
}

void WaveFunctionView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF f = mapToScene(e->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(e);
}
void WaveFunctionView::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
void WaveFunctionView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.007 || factor > 1000)
        return;

    scale(scaleFactor, scaleFactor);
}

void WaveFunctionView::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_Space:
    case Qt::Key_Enter:
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
    default:
         QGraphicsView::keyPressEvent(event);
    }
}
void WaveFunctionView::clearAll()
{

    for ( QMap<int,CoordinateDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int m = i.key();
        removeCurve(m);
    }
}

void WaveFunctionView::scrollView(int hx, int hy)
{
    ScalePsinParameters tp = model->getScalePsinParam();
    dx=tp.Hx;
    xmin=tp.Xmin;
    xmax=tp.Xmax;
    psiMax=tp.Psinmax;
    psiMin=tp.Psinmin;
    double stepX=(xmax-xmin)/5;//numTTicks;
    xmin +=hx*stepX;
    xmax +=hx*stepX;
    double stepY=(psiMax-psiMin)/5;//numETicks;
    psiMin +=hy*stepY;
    psiMax +=hy*stepY;

    ScalePsinParameters tt;
    tt.Hx = this->dx;
    tt.Xmin = xmin;
    tt.Xmax = xmax;
    tt.Psinmin = psiMin;
    tt.Psinmax = psiMax;
    model->setScalePsinParam(tt);
/*    if(dialogScalePsin) dialogScalePsin->modelChanged();
    else
    {
        dialogScalePsin = new ScalePsin(this);
        dialogScalePsin->setModel(model);
    }*/
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
void WaveFunctionView::slot_Psi_n_of_x()
{
    if (! isVisible()) return;
    LevelNumberParameters wp = model->getLevelNumberParameters();
    nMin=wp.nmin;
    nMax=wp.nmax;
    hn=wp.hn;
    if(nMin<0||nMax<0||hn<0) return;
    QRectF vp = scene()->sceneRect();
    QRect a = QRect(this->viewport()->rect());
    QPen p,pl;
    SettingParameters ts;
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
    p.setWidth(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    pl.setColor(Qt::lightGray);
    if(whatToDraw==3) clearAll();
/*    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta,
        Qt::black,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
  */
    PotentialType type = model->getPotentialType();
    int number_of_levels;
    QVector<double> Ebound;
    QVector<complex> Equasi;
    if(type==QUASISTATIONARY)
    {
    Equasi = model->getEquasi();
    number_of_levels = Equasi.size();
    }
    else
    {
    Ebound = model->getEn();
    number_of_levels = Ebound.size();
    }
    for ( QMap<int,CoordinateDistribution*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        if (n >= number_of_levels)
        {
            removeCurve(n);

        }
        if(n>nMax&&nMax<number_of_levels-1) removeCurve(n);
        if(n<nMin&&nMin<number_of_levels-1) removeCurve(n);
    }
  double xn,yn,cs,sn;
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
  }
  else
  {
      xn= vp.width()/2.;
      yn =2*vp.height()/3.;
      double xk = 0;
      double yk = 0;//vp.height();
      cs = (xn-xk)/(xmax-xmin);
      sn = -(yk-yn)/(xmax-xmin);
      if(!linez)
      {
          linez = new QGraphicsLineItem();
          pl.setWidth(1);
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
    if(vectorFirst) scene()->removeItem(vectorFirst);
    vectorFirst = new QGraphicsLineItem();
    int npoints;//=501;
    QVector<complex> waveFunction;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    p.setWidth(lineWidth);
    bool tail=true;
            for (int n = this->nMin; n <= this->nMax; n+= this->hn)
    {
        if(n>number_of_levels-1) break;
        if(type==PERIODIC) waveFunction = model->getPsiOfX_per(Ebound[n],xmin,xmax,npoints);
        if(type==FINITE) waveFunction = model->getPsiOfX(Ebound[n],xmin,xmax,npoints,tail);
        if(type==QUASISTATIONARY) waveFunction = model->getQuasiPsiOfX(Equasi[n],xmin,xmax,npoints);
        for (int i=0; i < npoints; i++)
        {
            double x,yim,yre,y;
            yre = real(waveFunction[i]);
            yim = imag(waveFunction[i]);
            if(whatToDraw==0) y = yre;
            if(whatToDraw==1) y = yim;
            if(whatToDraw==2) y = yre*yre+yim*yim;
            if(whatToDraw==3)
            {
                double xi = xmin + dx*i;
                yim =vp.height()*yim/psiMax;
                yre =vp.height()*yre/psiMax;
                double xz = xn-(xi-xmin)*cs;
                double yz = yn-(xi-xmin)*sn;
                y = yz+yim;
                x = xz+yre;
                if(y>1e4*psiMax) y=1e4*psiMax;
                if(y<-1e4*psiMax) y=-1e4*psiMax;
/*                if(i==0&&n==nMin)
                {
                    vectorFirst->setLine(xz,yz,x,y);
                    vectorFirst->setPen(pl);
                }
                else
                {
                    QGraphicsLineItem *l = new QGraphicsLineItem(vectorFirst);
                    l->setPen(pl);
                    l->setLine(xz,yz,x,y);
                }*/
            }
            else
            {
                x = (i*vp.width())/(npoints-1);
                y = vp.height()*(y-psiMin)/(psiMax-psiMin);
                if(y>1e4*psiMax) y=1e4*psiMax;
                if(y<-1e4*psiMax) y=-1e4*psiMax;
            }
            psi[i]  = QPointF(x, y);
        }
        if(type==PERIODIC)
        {
            p.setColor(colorForIdsP[n % size_colorForIdsP]);
        }
        else
            p.setColor(colorForIds[n % size_colorForIds]);
        setCurve(n, psi, p);
    }
    update();

}
void WaveFunctionView::setCurve(int id, const QPolygonF & curve, const QPen& pen)
{
    if (curves[id])
    {
        curves[id]->setPolygon(curve);
    }
    else
    {
        CoordinateDistribution *c = new CoordinateDistribution(curve,this);
        scene()->addItem(c);
        curves[id] = c;
    }
    curves[id]->setPen(pen);
    curves[id]->setZValue(id);
    update();
}

void WaveFunctionView::removeCurve(int id)
{
    QGraphicsItem *item = curves[id];
    if (item)
    {
        scene()->removeItem(item);
        delete curves[id];
        curves[id] = 0; //this is dangerous: curves.remove(id);
    }
    update();
}

void CoordinateDistribution::paint(QPainter * painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void WaveFunctionView::showDialogLevNum()
{
    if (!dialogLevNum)
    {
        dialogLevNum = new LevelNumber(this);
        dialogLevNum->setModel(model);
    }
    dialogLevNum->show();
    dialogLevNum->activateWindow();
    dialogLevNum->setFocus();
}

void WaveFunctionView::setWhatToDraw(int w)
{
    if (whatToDraw != w)
    {
        whatToDraw = w;
        slotEnergyChanged();
        slot_Psi_n_of_x();
        emit( whatToDrawChanged(w) );
    }
}
int WaveFunctionView::getWhatToDraw()
{
    return whatToDraw;
}
void WaveFunctionView::showDialogScaleY()
{
    if (!dialogScalePsin)
    {
        dialogScalePsin = new ScalePsin(this);
        dialogScalePsin->setModel(model);
    }
    dialogScalePsin->show();
    dialogScalePsin->activateWindow();
    dialogScalePsin->setFocus();
}

void WaveFunctionView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu m;
    QFont font( "Serif", 10, QFont::DemiBold );
    m.setFont(font);

    QAction *levNum = m.addAction(tr("Level numbers"));
    QAction *scalePsi = m.addAction(tr("Scales"));
    QAction *what = m.exec(event->globalPos());
    if (what == levNum)
    {
        this->showDialogLevNum();
    }
    if (what == scalePsi)
    {
        this->showDialogScaleY();
    }
    event->accept();
}

WaveFunctionWidget::WaveFunctionWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Coordinate distribution"));
    QVBoxLayout *vl = new QVBoxLayout();
    waveFunctionView = new WaveFunctionView(model,this);
    vl->addWidget(waveFunctionView);

    QHBoxLayout *hl = new QHBoxLayout();
    QToolButton *bRun = new QToolButton(this);
    bRun->setIcon(QIcon(":/images/player_play.png"));
    bRun->adjustSize();
    connect(bRun,SIGNAL(clicked()),waveFunctionView,SLOT(resizePicture()));

    QString psi=QChar(0x03C8);
    QString to2=QChar(0x00B2);
    QString psiofx=psi+"(x)";
    QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
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
    bgR->button(0)->setChecked(true);
    connect(bgR,SIGNAL(buttonClicked(int)),waveFunctionView,SLOT(setWhatToDraw(int)));
    connect(waveFunctionView,SIGNAL(whatToDrawChanged(int)),this,SLOT(checkButton(int)));
    hl->addWidget(bRun);

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon(":/images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);

    setLayout(vl);
}
void WaveFunctionWidget::checkButton(int index)
{
    this->bgR->button(index)->setChecked(true);
}
void WaveFunctionWidget::readFromXml(QXmlStreamReader *r)
{
    Q_ASSERT(this);
    Q_ASSERT(r->isStartElement() && r->name() == "Psix");
    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "WhatToDraw")
        {
            QString s = r->readElementText();
            int what = s.toInt();
            waveFunctionView->setWhatToDraw(what);
        }
        else
            skipUnknownElement(r);
    }
        waveFunctionView->clearAll();
}


void WaveFunctionWidget::writeToXml(QXmlStreamWriter *w)
{
    int what = waveFunctionView->getWhatToDraw();
    w->writeStartElement("Psix");
    {
        QString s;
        s.sprintf("%i",what);
        w->writeTextElement("WhatToDraw",s);
    }
    w->writeEndElement();
}
