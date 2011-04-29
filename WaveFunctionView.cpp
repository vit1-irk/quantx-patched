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

WaveFunctionView::WaveFunctionView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), 
dialogLevNum(0), lineh(0), linev(0),dialogScalePsin(0),//gbWidth(0),//leW(0),
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
    connect(model,SIGNAL(signalZChanged(double)),this,SLOT(slotEnergyChanged()));
    connect(model,SIGNAL(signalLevelParametersChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalWidthChanged()),this,SLOT(resizePicture()));

    setViewportMapping();
    resizePicture();
}
#define ID_PSI_ENERGY (200)
void WaveFunctionView::slotEnergyChanged()
{
        if (! isVisible()) return;
    LevelNumberParameters wp = model->getLevelNumberParameters();
    nMin=wp.nmin;
    nMax=wp.nmax;
    hn=wp.hn;  
    if(nMin>0&&nMax>0&&hn>0) return;
    QRectF vp = scene()->sceneRect();
    QPen p;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);//Qt::darkRed);
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
    lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
    linev->setPen(p);
    lineh->setPen(p);
    linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
    lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));

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
//            double x = xmin + dx*i;
//            double y = waveFunction[i];
//            y = (y-psiMin)*scalePsi+Umin;
//            psi[i]  = QPointF(x, y);
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
/*WaveFunctionView::~WaveFunctionView()
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
}
*/
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

/*        if(dialogScalePsin) dialogScalePsin->modelChanged();
        else
        {
            dialogScalePsin = new ScalePsin(this);
            dialogScalePsin->setModel(model);
        }*/
    }
    setViewportMapping();
    slot_Psi_n_of_x();
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

/*void WaveFunctionView::setViewportMapping()
{
    QRectF vp = QRectF(QPointF(this->xmin,this->psiMin),QPointF(this->xmax,this->psiMax));

    QRectF sr = scene()->sceneRect();
    
    QRectF a = QRectF(this->viewport()->rect());
    if (vp != sr||a != a_old)
    {
        scene()->setSceneRect(vp);

        QRectF b = vp; //scene()->sceneRect();
 //       QRectF a = QRectF(this->viewport()->rect());
        qreal m11 = a.width() / b.width();
        qreal m22 = - a.height() / b.height(); 
        qreal dx = - m11 * a.x();
        qreal dy = - m22 * (a.y() + a.height());
        QMatrix m(m11,0,0,m22,dx,dy);
        this->setMatrix(m);
        scene()->update(scene()->sceneRect());
        sr = scene()->sceneRect();
        a_old=a; 
     double ax=fabs(xmax-xmin)*lineWidth/a.width();
//     double ay=a.height()/20000.;
//      double ay=a.height()/fabs(psiMax-psiMin);
//     double ay=fabs(psiMax-psiMin)*lineWidth/a.height();
     double ay=fabs(psiMax-psiMin)*lineWidth/a.height();
     widthLineEn= ay;
   }
    update();
} */
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

void WaveFunctionView::slot_Psi_n_of_x()
{
    if (! isVisible()) return;
    QRectF vp = scene()->sceneRect();
    QRect a = QRect(this->viewport()->rect());
    QPen p;
    SettingParameters ts;  
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
//    if(lineWidth==0)lineWidth=1;
    p.setWidthF(lineWidth);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    static const QColor colorForIds[12] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow,
        Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);

    QVector<double> Ebound = model->getEn();
    int number_of_levels = Ebound.size();
    LevelNumberParameters wp = model->getLevelNumberParameters();
    nMin=wp.nmin;
    nMax=wp.nmax;
    hn=wp.hn;  
    if(nMin<0||nMax<0||hn<0) return;
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

    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
    lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
    linev->setPen(p);
    lineh->setPen(p);
    linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
    lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
    int npoints;//=501;
    QVector<double> waveFunction;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
    p.setWidthF(lineWidth);
    bool tail=true;
    for (int n = this->nMin; n <= this->nMax; n+= this->hn)
    {
        if(n>number_of_levels-1) break;    
        waveFunction = model->getPsiOfX(Ebound[n],xmin,xmax,npoints,whatToDraw,tail);
        for (int i=0; i < npoints; i++)
        {
 //           double x = xmin + dx*i;
 //           double y = waveFunction[i];
 //           psi[i]  = QPointF(x, y);
/*            double x = i;//(i*vp.width())/(npoints-1);
            double y =vp.height()*(waveFunction[i]-psiMin)/(psiMax-psiMin);
            psi[i]  = QPointF(x, y);*/
            double x = (i*vp.width())/(npoints-1);
            double y =vp.height()*(waveFunction[i]-psiMin)/(psiMax-psiMin);
            if(y>1e4*psiMax) y=1e4*psiMax;
            if(y<-1e4*psiMax) y=-1e4*psiMax;
            psi[i]  = QPointF(x, y);
             }
        p.setColor(colorForIds[n % size_colorForIds]);
        setCurve(n, psi, p);
    }
    update();
}
//void WaveFunctionView::setCurve(int id,const QPolygon & curve, const QPen& pen)
void WaveFunctionView::setCurve(int id,const QPolygonF & curve, const QPen& pen)
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
//    repaint();
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
//    QAction *width = m.addAction(m.tr("width of line"));
/*    if (what == width)
    {
        this->showDialogWidth();
        update();
    }*/
    if (what == levNum)
    {
        this->showDialogLevNum();
        //        update();
    }
    if (what == scalePsi)
    {
        this->showDialogScaleY();
        //        update();
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
    bRun->setIcon(QIcon("images/player_play.png"));
    bRun->adjustSize();
    connect(bRun,SIGNAL(clicked()),waveFunctionView,SLOT(resizePicture()));

    QString psi=QChar(0x03C8);
    QString to2=QChar(0x00B2);
    QString psiofx=psi+"(x)";
    QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
    QRadioButton *rad1= new QRadioButton("Re "+psi);
    QRadioButton *rad2= new QRadioButton("Im "+psi);
    QRadioButton *rad3= new QRadioButton(mod_psiofx);
    hl->addWidget(rad1);
    hl->addWidget(rad2);
    hl->addWidget(rad3);
    QButtonGroup *bgR = new QButtonGroup(this);
    bgR->setExclusive(true);
    bgR->addButton(rad1,0);
    bgR->addButton(rad2,1);
    bgR->addButton(rad3,2);
    bgR->button(0)->setChecked(true);
    connect(bgR,SIGNAL(buttonClicked(int)),waveFunctionView,SLOT(setWhatToDraw(int)));
//    QPushButton *buttonClose = new QPushButton(tr("Close"));
    hl->addWidget(bRun);		

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);

    setLayout(vl);
}
