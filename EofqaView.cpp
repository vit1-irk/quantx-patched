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

EofqaView::EofqaView(PhysicalModel *m, QWidget *parent)
: QGraphicsView(parent), model(m)
{
    Erase = true; // this must initially be true
    lineWidth = 3;//10;
    tMax = 1.1;
    tMin = -0.1;
    Emin = -15;
    Emax = 20;
    hE = 0.05;

    lineh = NULL;
    linev = NULL;
    lineE = NULL;

    leTmin = NULL;
    leTmax = NULL;
    leEmin = NULL;
    leEmax = NULL;
    leHE   = NULL;
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
    model->set_EmaxEmin(Emax,Emin);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //rubberBandIsShown = false;

//    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(slot_T_of_E));
    connect(this,SIGNAL(signalScaleTEChanged()),this,SLOT(resizePicture()));
    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(resizePicture()));
//    connect(model,SIGNAL(signalPotentialChanged()),this,SLOT(slot_Eofqa()));
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
    if(Emax<=0.) Emax=20;
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
    QPen p;
    p.setStyle(Qt::DashLine);
    p.setColor(Qt::green);
    lineE->setPen(p);
    lineE->setLine(vp.width()*(E-Emin)/(Emax-Emin), 0., vp.height());
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
    double rEmin=0;
    double rEmax=a.width();//npoints-1;
    double rTMin=0;
    double rTMax=a.height();//a.height();
    QRectF b = QRectF(QPointF(rEmin,rTMin),QPointF(rEmax,rTMax));
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
//    QSize s = e->size();
//    QSize o = e->oldSize();
    this->resizePicture();
//    setViewportMapping();
    //QWidget::resizeEvent(e);
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
    double stepY=(tMax-tMin)/5;//numTTicks;
    tMin +=dy*stepY;
    tMax +=dy*stepY;
    double stepX=(Emax-Emin)/5;//numETicks;
    Emin +=dx*stepX;
    Emax +=dx*stepX;
    if(Emin<0) Emin=-0.2;
    if(Emax<0) Emax=1.;
    setScaleTE();
    emit(signalScaleTEChanged());
/*    double stepX=(tMax-tMin)/5;//numTTicks;
    tMin +=dx*stepX;
    tMax +=dx*stepX;
    double stepY=(Emax-Emin)/5;//numETicks;
    Emin +=dy*stepY;
    Emax +=dy*stepY;
    emit(signalScalesChanged());*/
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
    if(type==FINITE) return; 
    QRectF vp = scene()->sceneRect();
    QRectF vp_old=vp;
    QPen p;
    SettingParameters ts;  
    ts=model->getSettingParameters();
    lineWidth=ts.lineWidth;
//    if(lineWidth==0)lineWidth=1;

    p.setWidthF(lineWidth);
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
        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
        scene()->addItem(lineh);
        scene()->addItem(linev);
    }
    else
    {
        linev->setPen(p);
        lineh->setPen(p);
        linev->setLine(vp.width()*(-Emin)/(Emax-Emin), 0, vp.width()*(-Emin)/(Emax-Emin),vp.height() );
        lineh->setLine(0,vp.height()*(-tMin)/(tMax-tMin),vp.width(),vp.height()*(-tMin)/(tMax-tMin));
    }
//--------------------------
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::black, Qt::cyan, Qt::magenta, Qt::yellow
    };
    const int size_colorForIds = sizeof(colorForIds)/sizeof(colorForIds[0]);
    if(type==PERIODIC) 
    {
        model->set_EmaxEmin(Emax,Emin); 
        QVector<double> Ebound = model->getEn();
        int number_of_levels = Ebound.size();
        QPolygonF curveTE;
        curveTE.resize(51);
        double ed,eu;
        for (int i=0; i < number_of_levels; i=i+2)
        {
            ed=Ebound[i];
            if((i+1)<number_of_levels) eu=Ebound[i+1];
            else eu=Emax;
            if(ed>=Emin||eu>=Emin)
            {
            double dE=(eu-ed)/50;
            for(int j=0; j<51;j++)
            {
                double EE=ed+dE*j;
                model->set_E0(EE);
                double qa = model->get_qa()/M_PI;
                double x = vp.width()*(EE-Emin)/(Emax-Emin);
                double y = vp.height()*(qa-tMin)/(tMax-tMin);
                curveTE[j]  = QPointF(x,y);
            }
            setCurve(i/2,curveTE,colorForIds[ i/2 % size_colorForIds]);
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
        connect(this->leHE,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
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
        connect(this->leEmin,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
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
        connect(this->leEmax,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Tmin",this));
        h->addWidget(this->leTmin= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->tMin);
        this->leTmin->setText(x);
        //        this->leTmin->setToolTip("Lower value of E-interval");
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("Tmax",this));
        h->addWidget(this->leTmax= new QLineEdit(this));
        QString x;
        x.sprintf("%lg",this->tMax);
        this->leTmax->setText(x);
        //        this->leTmax->setToolTip("High value of E-interval");
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateScaleTE()));
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
void EofqaView::setScaleTE()
{
    QString x;
    x.sprintf("%lg",this->hE);
    this->leHE->setText(x);
    x.sprintf("%lg",this->Emin);
    this->leEmin->setText(x);
    x.sprintf("%lg",this->Emax);
    this->leEmax->setText(x);
    x.sprintf("%lg",this->tMax);
    this->leTmax->setText(x);
    x.sprintf("%lg",this->tMin);
    this->leTmin->setText(x);
}
void EofqaView::updateScaleTE()
{
    double hENew = this->leHE->text().toDouble();
    double EminNew = this->leEmin->text().toDouble();
    double EmaxNew = this->leEmax->text().toDouble();
    double TminNew = this->leTmin->text().toDouble();
    double TmaxNew = this->leTmax->text().toDouble();
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
    if (tMin != TminNew)
    {
        changed = true;
        tMin=TminNew;
    }
    if (tMax != TmaxNew)
    {
        changed = true;
        tMax=TmaxNew;
    }
    if(changed)
    {
        model->set_EmaxEmin(Emax,Emin);
        emit(signalScaleTEChanged());
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

/*void EofqaView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();
        QRect rect = rubberBandRect.normalized();
        if(rect.width()<4||rect.height()<4)
            return;
        rect.translate(-1, -1);
    }
}
void EofqaView::updateRubberBandRegion()
{
    QRect rect=rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}
void EofqaView::mousePressEvent(QMouseEvent *event)
{
    if (event->button()== Qt::LeftButton)
    {
        rubberBandIsShown = true;
        rubberBandRect.setTopLeft(event->pos());
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
        setCursor(Qt::CrossCursor);
    }
}
void EofqaView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF f = mapToScene(event->pos());
    emit(infoMouseMovedTo(f));
    QGraphicsView::mouseMoveEvent(event);
    if (rubberBandIsShown)
    {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }

}
void EofqaView::enterEvent ( QEvent * event )
{
    setFocus();
    QWidget::enterEvent(event);
}
void EofqaView::paintEvent(QPaintEvent *event)
{
    QVector<QRect> rects = event->region().rects();
    QPainter painter(this);
   for(int i=0; i<(int)rects.size();++i)
        painter.drawImage(rects[i].topLeft(), pixmap.toImage(), rects[i]);
    if(rubberBandIsShown)
    {
        painter.setPen(palette().light().color());//was 2!!!!
        painter.drawRect(rubberBandRect.normalized().adjusted(0,0,-1,-1));
    }
    if (hasFocus()) 
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}
*/
EEDrag::EEDrag(EofqaView *v,QGraphicsItem *parent)
: QGraphicsItem(parent), view(v)
{
    //TODO: how do I control the width of this draggable line?
    setCursor(Qt::SizeVerCursor);
    penHover.setStyle(Qt::DashLine);
    penHover.setWidth(v->lineWidth);
//    penHover.setStyle(Qt::DotLine);
    penHover.setColor(Qt::green);
//    pen.setStyle(Qt::DashLine);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::green);
    penForPainter.setColor(Qt::green);
    pen.setWidth(v->lineWidth);

    setCursor(Qt::SizeHorCursor);
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
            newpos.setY( oldpos.y() );
            return newpos;
        }
    case ItemPositionHasChanged:
        if (isSelected())
        {
            double newE = pos().x();
            QRectF vp = view->sceneRect();//
            newE=view->Emin+(view->Emax-view->Emin)*newE/vp.width();
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

EofqaWidget::EofqaWidget(PhysicalModel *model, QWidget *parent)
: QGroupBox(parent)
{
    setTitle(tr("Dispersion law E(qa)"));
    QVBoxLayout *vl = new QVBoxLayout();
    eofqaView = new EofqaView(model,this);
    vl->addWidget(eofqaView);

    QHBoxLayout *hl = new QHBoxLayout();
    QToolButton *reset = new QToolButton(this);
    reset->setIcon(QIcon("images/player_play.png"));
    reset->adjustSize();//QPushButton(tr("Close"));
//    QPushButton *reset = new QPushButton(tr("&Resize"));	
    connect(reset,SIGNAL(clicked()),eofqaView,SLOT(resizePicture()));

//    bRunTE = new QPushButton(tr("Run "));	
//    connect(bRunTE,SIGNAL(clicked()),this,SLOT(slotRunWP()));

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

//    zoomInButton = new QToolButton(this);
//    zoomInButton->setIcon(QIcon("images/zoomin.png"));
//    zoomInButton->adjustSize();
//    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    QToolButton *buttonClose = new QToolButton(this);
    buttonClose->setIcon(QIcon("images/erase.png"));
    buttonClose->adjustSize();//QPushButton(tr("Close"));
    hl->addStretch();
    hl->addWidget(buttonClose);

    connect(buttonClose,SIGNAL(clicked()),this,SLOT(hide()),Qt::QueuedConnection); //???
    vl->addLayout(hl);
    setLayout(vl);
}
void EofqaWidget::slotRunEqa()
{
 //   bRunTE->setText("STOP");
 //   disconnect(bRunTE, SIGNAL(clicked()), this, SLOT(slotRunEqa()));
 //   breakStatus.onButton(bRunTE);
    eofqaView->resizePicture();
 //   breakStatus.noButton(bRunTE);
 //   bRunTE->setText("RUN ");
 //   connect(bRunTE, SIGNAL(clicked()), this, SLOT(slotRunEqa()));
}
