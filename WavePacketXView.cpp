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

WavePacketXView::WavePacketXView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m), 
dialogTime(0),// gbDefWP(0),
lineh(0), linev(0),widthLineWP(3),
dialogWPEp(0),dialogWPEm(0),dialogScaleWPX(0),
whatToDraw(2)
{
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    initDialogWidth();
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
    connect(model,SIGNAL(signalScalesUChanged()),this,SLOT(reDraw()));
//    connect(model,SIGNAL(signalScaleWPXChanged()),this,SLOT(resizePicture()));

}
WavePacketXView::~WavePacketXView()
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
    reDraw();
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
    QRectF vp = scene()->sceneRect();
    QPen p;
    p.setWidthF(widthLineWP);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
//    if(linev)
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
        lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        p.setColor(Qt::darkCyan);
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
        lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
        int npoints;
        QPolygonF psi;
        npoints=1+(xmax-xmin)/this->dx;
        psi.resize(npoints);
        waveFunction.resize(npoints);
        for (int i=0; i < npoints; i++)
        {
            double x = (i*vp.width())/(npoints-1);
            double y =vp.height()*(waveFunction[i]-psiMin)/(psiMax-psiMin);
            psi[i]  = QPointF(x, y);
        }
        setCurve(0, psi, p);
    }
}

void WavePacketXView::slot_WavePacket_of_t()
{
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    QRect a = QRect(this->viewport()->rect());
    QPen p;
    p.setWidthF(widthLineWP);
    p.setJoinStyle(Qt::BevelJoin);
    p.setCapStyle(Qt::RoundCap);
    p.setColor(Qt::black);
    for ( QMap<int,CoordinateDistributionCurve*>::iterator i = curves.begin();  i != curves.end();   ++i)
    {
        int n = i.key();
        removeCurve(n);
    }
    if(!linev)
    {
        lineh = new QGraphicsLineItem();
        linev = new QGraphicsLineItem();
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
        lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
        lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
    }
    int npoints;
    QPolygonF psi;
    npoints=1+(xmax-xmin)/this->dx;
    psi.resize(npoints);
    waveFunction.resize(npoints);
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
    p.setColor(Qt::darkCyan);
    p.setWidthF(widthLineWP);

    for (double t=this->time; t>=tp.tmin&&t<=tp.tmax; t+=htime)
    {
        ScaleWPXParameters sc = model->getScaleWPXParam();
        vp = scene()->sceneRect();
        if(vp!=vp_old||dx!=sc.Hx||xmin!=sc.Xmin||xmax!=sc.Xmax||psiMax!=sc.WPXmax||psiMin!=sc.WPXmin)
        {
            dx=sc.Hx;
            xmin=sc.Xmin;
            xmax=sc.Xmax;
            psiMax=sc.WPXmax;
            psiMin=sc.WPXmin;
            npoints=1+(xmax-xmin)/this->dx;
            psi.resize(npoints);
            waveFunction.resize(npoints);
            setViewportMapping();
            //            vp = scene()->sceneRect();
            QRect a = QRect(this->viewport()->rect());
            p.setColor(Qt::black);
            linev->setLine(vp.width()*(-xmin)/(xmax-xmin), 0, vp.width()*(-xmin)/(xmax-xmin),vp.height() );
            lineh->setLine(0,vp.height()*(-psiMin)/(psiMax-psiMin),vp.width(),vp.height()*(-psiMin)/(psiMax-psiMin));
            p.setColor(Qt::darkCyan);
            p.setWidthF(widthLineWP);
            vp_old=vp;
        }
        TimeParameters tt=model->getTimeParam(); 
        if(t!=tt.time||htime!=tt.ht)
        {
            this->time=tt.time;
            htime=tt.ht;
        }
        waveFunction = model->getPsiOfXT(t, xmin, xmax, npoints, whatToDraw);
        for (int i=0; i < npoints; i++)
        {
            double x = (i*vp.width())/(npoints-1);
            double y =vp.height()*(waveFunction[i]-psiMin)/(psiMax-psiMin);
            psi[i]  = QPointF(x, y);
        }
        setCurve(0, psi, p);
        tp=model->getTimeParam();
        if (getBreakStatus(0)) 
        {
            return;
        }
    }
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
//    double width=pen.widthF();
//    int width=pen.width();
    update();
}

void WavePacketXView::removeCurve(int id)
{
    scene()->removeItem(curves[id]);
    delete curves[id];
    curves[id] = 0; //this is dangerous: curves.remove(id);
    update();
}

void CoordinateDistributionCurve::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget *)
{
    (void)option;
    painter->setPen( pen() );
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(polygon().data(),polygon().size());
}
void WavePacketXView::initDialogWidth()
{   
    gbWidth = new QGroupBox(this);
    gbWidth->setWindowTitle("Width of lines");
    gbWidth->setWindowFlags(Qt::Window);
    gbWidth->setFont(QFont("Serif", 12, QFont::Bold )); 

    QVBoxLayout *vl = new QVBoxLayout;
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("width",this));
        h->addWidget(this->leW = new QLineEdit(this));
        this->leW->setToolTip("width: 1-10");
        QString x;
        x.sprintf("%lg",this->widthLineWP);
        this->leW->setText(x);
        connect(this->leW,SIGNAL(editingFinished()),this,SLOT(updateWidth()));
        vl->addWidget(line);
    }
    gbWidth->setLayout(vl);
}
void WavePacketXView::showDialogWidth()
{
    gbWidth->show();
    gbWidth->raise();
    gbWidth->setFocus();
}
void WavePacketXView::setWidth()
{
    QString buf;
    buf.sprintf("%lg",this->widthLineWP);
    this->leW->setText(buf);
}
void WavePacketXView::updateWidth()
{
    double a = this->leW->text().toDouble();
    bool changed = false;
    if (widthLineWP != a)
    {
        widthLineWP=a;
        changed = true;
    }
    if(changed)
    {
        emit(signalWidthChanged());
    }
}
//---------------
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
        QAction *width = m.addAction(m.tr("width of line"));
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
        if (what == width)
        {
            this->showDialogWidth();
            update();
        }
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
        whatToDraw = w;
//        slot_WavePacket_of_t();
        emit( whatToDrawChanged(w) );
    }
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
    bRunPsiXT->setIcon(QIcon("images/player_play.png"));
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
    hl->addWidget(rad1);
    hl->addWidget(rad2);
    hl->addWidget(rad3);
    bgR = new QButtonGroup(this);
    bgR->setExclusive(true);
    bgR->addButton(rad1,0);
    bgR->addButton(rad2,1);
    bgR->addButton(rad3,2);
    bgR->button(2)->setChecked(true);
    connect(bgR,SIGNAL(buttonClicked(int)),wavePacketXView,SLOT(setWhatToDraw(int)));
    //---------------
    QLabel *ltext= new QLabel(this);
    ltext->setText(tr("time:"));
    QLabel *ltime= new QLabel(this);
    connect(model, SIGNAL(signalTimeChanged(double)), ltime, SLOT(setNum(double)));

    hl->addWidget(bRunPsiXT);		
    hl->addWidget(ltext);
    hl->addWidget(ltime);

    //    QPushButton *buttonClose = new QPushButton(tr("Close"));
    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);
    setLayout(vl);
}

void WavePacketXWidget::slotRunWP()
{
    bRunPsiXT->setIcon(QIcon("images/player_pause.png"));
    bRunPsiXT->adjustSize();
//    bRunPsiXT->setText(tr("STOP"));
    disconnect(bRunPsiXT, SIGNAL(clicked()), this, SLOT(slotRunWP()));
    breakStatus.onButton(bRunPsiXT);
    wavePacketXView->resizePicture();
    breakStatus.noButton(bRunPsiXT);
    bRunPsiXT->setIcon(QIcon("images/player_play.png"));
    bRunPsiXT->adjustSize();
//    bRunPsiXT->setText(tr("RUN "));
    connect(bRunPsiXT, SIGNAL(clicked()), this, SLOT(slotRunWP()));
}

