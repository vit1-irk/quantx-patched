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

//#include <qpainter.h>
//#include <qstyle.h>
//#include <qtoolbutton.h>
//Added by qt3to4:
//#include <Q3TextStream>
//#include <Q3PointArray>
#include "plotter.h"
#include <cmath>
#include <map>
using namespace std;
//#include <QColorGroup>
#include <QToolButton>
#include <QPainter>
#include <QStyleOption>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

void Plotter::moveButtons()
{
    const int sep = 1;
    int lx = sep;

    zoomAllButton->move(lx, sep);
    lx += zoomAllButton->width() + sep;

    zoomInButton->move(lx, sep);
    lx += zoomInButton->width() + sep;

    zoomOutButton->move(lx, sep);
    lx += zoomOutButton->width() + sep;

/*    scaleXButton->move(lx, sep);
    lx += scaleXButton->width() + sep;
*/
    scaleYButton->move(lx, sep);
    lx += scaleYButton->width() + sep;

}

Plotter::Plotter(QWidget *parent, Qt::WindowFlags flags)
:   QWidget(parent, flags),
    scaleFixed(true),
    eraseButton(0)
{
//    setBackgroundRole(QPalette::Dark);
    setBackgroundRole(QPalette::Text); //was!!!1.
    setAutoFillBackground ( true );
//    setBackGround(Qt::TransparentMode);
//    setBackGroundMode(Qt::BGMode Qt::TransparentMode);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    rubberBandIsShown = false;

/*    scaleXButton = new QToolButton(this);
    scaleXButton->setIcon(QIcon("images/sizeh.png"));
    scaleXButton->adjustSize();
    scaleXButton->show();
    connect(scaleXButton, SIGNAL(clicked()), this, SLOT(scaleX()));
*/

    scaleYButton = new QToolButton(this);
    scaleYButton->setIcon(QIcon("images/sizev.png"));
    scaleYButton->adjustSize();
    scaleYButton->show();
    connect(scaleYButton, SIGNAL(clicked()), this, SLOT(scaleY()));

    zoomInButton = new QToolButton(this);
    zoomInButton->setIcon(QIcon("images/zoomin.png"));
    zoomInButton->adjustSize();
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    zoomOutButton = new QToolButton(this);
    zoomOutButton->setIcon(QIcon("images/zoomout.png"));
    zoomOutButton->adjustSize();
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));

    zoomAllButton = new QToolButton(this);
    zoomAllButton->setIcon(QIcon("images/zoomall.png"));
    zoomAllButton->adjustSize();
    zoomAllButton->show();
    connect(zoomAllButton, SIGNAL(clicked()), this, SLOT(zoomAll()));

    setPlotSettings(PlotSettings());
    setMouseTracking(true);
}

/*void Plotter::initStatusBar()
{
//    createStatusBar();
    this->statusBar()->setFont(QFont("Serif", 10, QFont::DemiBold ));
    this->mouseAtX = new QLabel("x: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->mouseAtX);

    this->mouseAtY = new QLabel("y: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->mouseAtY);
    connect(this,SIGNAL(infoMouseMovedTo(QPoint)),
            this->statusBar(),SLOT(updateMouseMovedTo(QPoint)));
}
*/
void Plotter::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.resize(1);
    zoomStack[0] = settings;
    curZoom = 0;
    yScale=0;
    xScale=0;
    zoomInButton->hide();
    zoomOutButton->hide();
    refreshPixmap();
}

void Plotter::zoomAll()
{
    this->captureBoundsToSettings();
    refreshPixmap();
}

void Plotter::zoomOut()
{
    if(curZoom>0) {
        --curZoom;
        zoomOutButton->setEnabled(curZoom);
        zoomInButton->setEnabled(true);
        zoomInButton->show();
        refreshPixmap();
    }
}

void Plotter::scaleX()
{
        xScale++;
        if(xScale==3) xScale=0;
        this->captureBoundsToSettings();
        refreshPixmap();
}

void Plotter::scaleY()
{
        yScale++;
        if(yScale==2) yScale=0;
        this->captureBoundsToSettings();
        refreshPixmap();

}
void Plotter::zoomIn()
{
    if(curZoom<(int)zoomStack.size()-1) {
        ++curZoom;
        zoomInButton->setEnabled(curZoom<(int)zoomStack.size()-1);
        zoomOutButton->setEnabled(true);
        zoomOutButton->show();
        refreshPixmap();
    }
}

void Plotter::clearAll()
{
    curveMap.erase(curveMap.begin(),curveMap.end());
    this->refreshPixmap();
}

void Plotter::captureBoundsToSettings()
{
    double xmin=1e308,ymin=1e308,xmax=-1e308,ymax=-1e308;
    int updated = 0;
    map<int, CurveData>::const_iterator it;
    for (it = curveMap.begin(); it != curveMap.end(); ++it)
    {
        //int id = (*it).first;
        const CurveData &data = (*it).second;
        int maxPoints = data.size()/2;
        for (int i=0; i<maxPoints; ++i)
        {
            double xd=data[2*i];
            double yd=data[2*i+1];
            if(yScale==1) yd=log10(yd);
//            if(xScale==1) xd=1./xd;
//           if(xScale==2) xd=log10(xd);

            if (xd < xmin) xmin = xd;
            if (xd > xmax) xmax = xd;
            if (yd < ymin) ymin = yd;
            if (yd > ymax) ymax = yd;
            updated = 1;
        }

    }
    if (updated)
    {
        double dx = xmax - xmin;
        double dy = ymax - ymin;
        this->zoomStack[this->curZoom].minX = xmin - 0.05*dx;
        this->zoomStack[this->curZoom].maxX = xmax + 0.05*dx;
        this->zoomStack[this->curZoom].minY = ymin - 0.05*dy;
        this->zoomStack[this->curZoom].maxY = ymax + 0.05*dy;
    }
}

void Plotter::setCurveData(int id, const CurveData &data)
{
    this->curveMap[id] = data;
    if(!this->scaleFixed) this->captureBoundsToSettings();
    refreshPixmap();
}
void Plotter::clearCurve(int id)
{
    curveMap.erase(id);
//    this->captureBoundsToSettings();
    refreshPixmap();
}
QSize Plotter::minimumSizeHint() const
{
    return QSize(6 * Margin, 4 * Margin);
}
QSize Plotter::sizeHint() const
{
    return QSize(12 * Margin, 8 * Margin);
}
void Plotter::paintEvent(QPaintEvent *event)
{
	QRegion::const_iterator rects = event->region().begin();
    QPainter painter(this);
//    PlotSettings settings = zoomStack[curZoom];
   for(int i=0; i<event->region().rectCount();++i)
        painter.drawImage(rects[i].topLeft(), pixmap.toImage(), rects[i]);
    if(rubberBandIsShown)
    {
        painter.setPen(palette().light().color());//was 2!!!!
//        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawRect(rubberBandRect.normalized().adjusted(0,0,-1,-1));
    }
    if (hasFocus())
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
    }
}

void Plotter::resizeEvent(QResizeEvent *)
{
    moveButtons();
    refreshPixmap();
}

void Plotter::mousePressEvent(QMouseEvent *event)
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
void Plotter::mouseMoveEvent(QMouseEvent *event)
{
    QRect rect(Margin, Margin,
        width() - 2*Margin,height() - 2*Margin);
        double xx,yy;
    PlotSettings settings = zoomStack[curZoom];
    QPoint f = event->pos();
    xx=((f.x()-rect.left())*settings.spanX())/(rect.width()-1)+settings.minX;
    yy=-((f.y()-rect.bottom())*settings.spanY())/(rect.height()-1)+settings.minY;
    QPointF ff;
    ff=QPointF(xx,yy);
     emit(infoMouseMovedTo(ff));
    if (event->buttons() & Qt::LeftButton)
    {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void Plotter::enterEvent ( QEvent * event )
{
    setFocus();
    QWidget::enterEvent(event);
}

void Plotter::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();
        QRect rect = rubberBandRect.normalized();
        if(rect.width()<4||rect.height()<4)
            return;
        rect.translate(-Margin, -Margin);

        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings settings;
        double dx=prevSettings.spanX()/(width()-2*Margin);
        double dy=prevSettings.spanY()/(height()-2*Margin);
        settings.minX=prevSettings.minX+dx*rect.left();
        settings.maxX=prevSettings.minX+dx*rect.right();
        settings.minY=prevSettings.maxY-dy*rect.bottom();
        settings.maxY=prevSettings.maxY-dy*rect.top();
        settings.adjust();
        zoomStack.resize(curZoom+1);
        zoomStack.push_back(settings);
        zoomIn();
    }
}
void Plotter::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()){
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Delete:
        clearAll();
        break;
//    case Qt::Key_Home:
//        yScale++;
//        if(yScale==2) yScale=0;
//        refreshPixmap();
//        drawCurves();
        break;
    case Qt::Key_Insert:
        savePlotAs();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Left:
        zoomStack[curZoom].scroll(-1,0);
        refreshPixmap();
        break;
    case Qt::Key_Right:
        zoomStack[curZoom].scroll(+1,0);
        refreshPixmap();
        break;
    case Qt::Key_Down:
        zoomStack[curZoom].scroll(0, -1);
        refreshPixmap();
        break;
    case Qt::Key_Up:
        zoomStack[curZoom].scroll(0, +1);
        refreshPixmap();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}
void Plotter::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta()/8;
    int numTicks = numDegrees / 15;
    if(event->orientation() == Qt::Horizontal)
        zoomStack[curZoom].scroll(numTicks,0);
    else
        zoomStack[curZoom].scroll(0, numTicks);
    refreshPixmap();
}
void Plotter::updateRubberBandRegion()
{
    QRect rect=rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}
void Plotter::refreshPixmap()
{
    pixmap = QPixmap(size());
//    pixmap.setMask ( const QBitmap & mask )
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    drawGrid(&painter);
    drawCurves(&painter);
    repaint();
//    update();
}
void Plotter::drawGrid(QPainter *painter)
{
    QRect rect(Margin, Margin,
        width() - 2*Margin,height() - 2*Margin);
    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = palette().color(QPalette::Dark);
    QPen light = palette().color(QPalette::Light);
//    settings.adjust();
    for(int i = 0; i<=settings.numXTicks; ++i)
    {
        int x = rect.left() + (i*(rect.width()-1)/settings.numXTicks);
        double label = settings.minX + (i*settings.spanX()/settings.numXTicks);
        painter->setPen(quiteDark);
//        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(light);
//        painter->drawLine(x, rect.bottom(), x, rect.bottom()+5);
        painter->drawText(x-50, rect.bottom()+5, 100, 15,
            Qt::AlignHCenter | Qt::AlignTop,
            QString::number(label));
    }
    for(int j = 0; j<=settings.numYTicks; ++j) {
        int y = rect.bottom() - (j*(rect.height()-1)/settings.numYTicks);
        double label = settings.minY + (j*settings.spanY()/settings.numYTicks);
        painter->setPen(quiteDark);
//        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(light);
//        painter->drawLine(rect.left()-5, y, rect.left(),y);
        painter->drawText(rect.left()-Margin, y-10, Margin-5, 20,
            Qt::AlignRight | Qt::AlignVCenter,
            QString::number(label));
    }
    painter->drawRect(rect);
}

void Plotter::drawCurves(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::white, Qt::cyan, Qt::magenta, Qt::yellow
    };
//    painter->setBackGroundMode(Qt::TransparentMode);
    QRect rect(Margin, Margin,
        width() - 2*Margin,height() - 2*Margin);
//    painter->backgroundMode(Qt::TransparentMode);
    PlotSettings settings = zoomStack[curZoom];
//----------------------
//    QRect rect(Margin, Margin,
//        width()-2*Margin, height()-2*Margin);
    painter->setClipRect(rect.x()+1, rect.y()+1, rect.width()-2, rect.height()-2);
    map<int, CurveData>::const_iterator it = curveMap.begin();
    while (it !=curveMap.end())
    {
        int id = it->first;
        const CurveData &data = it->second;
        int numPoints=0;
        int maxPoints = data.size()/2;
        QPolygon points(maxPoints);


        for (int i=0; i<maxPoints; ++i){
            double xd=data[2*i];
            double yd=data[2*i+1];
            if(yScale==1) yd=log10(yd);
 //           if(xScale==1) xd=1./xd;
//            if(xScale==2) xd=log10(xd);
            double dx = xd-settings.minX;
            double dy = yd-settings.minY;
            double x = rect.left()+(dx*(rect.width()-1)/settings.spanX());
            double y = rect.bottom()-(dy*(rect.height()-1)/settings.spanY());
            if(fabs(x)<32768&&fabs(y)<32768){
                points[numPoints]= QPoint((int)x, (int)y);
                ++numPoints;
            }
        }
        points.erase(points.begin()+numPoints, points.end()); //points.truncate(numPoints);
        painter->setPen(colorForIds[(uint)id%6]);
        painter->drawPolyline(points);
        ++it;
    }
}

//==========================================================================
// PlotSettings
//==========================================================================

PlotSettings::PlotSettings()
{
    minX = 0.0;
    maxX = 10.0;
    numXTicks = 5;
    minY = 0.0;
    maxY = 1.2;
    numYTicks = 5;
}
void PlotSettings::scroll(int dx, int dy)
{
    double stepX=spanX()/numXTicks;
    minX +=dx*stepX;
    maxX +=dx*stepX;
    double stepY=spanY()/numYTicks;
    minY +=dy*stepY;
    maxY +=dy*stepY;
}
void PlotSettings::adjust()
{
    adjustAxis(minX,maxX,numXTicks);
    adjustAxis(minY,maxY,numYTicks);
}
void PlotSettings::adjustAxis(double &min, double &max, int &numTicks)
{
    const int MinTicks = 4;
    double range = max-min;
///    double step = range/(MinTicks-1);
//    double range = nicenum(max-min,0);
//    double step = nicenum(range/(MinTicks-1),1);
    double grossStep = (max-min)/MinTicks;
    double step = pow(10.0, floor(log10(grossStep)));
    if(5*step<grossStep)
        step *=5;
    else if (2*step<grossStep)
        step *=2;

    numTicks = (int)(ceil(max/step)-floor(min/step));
    min = floor(min/step)*step;
    max = ceil(max/step)*step;
}
double PlotSettings::nicenum(double x, int round)
{
    int expv;				/* exponent of x */
    double f;				/* fractional part of x */
    double nf;				/* nice, rounded fraction */

    expv = floor(log10(x));
    f = x/expt(10., expv);		/* between 1 and 10 */
    if (round)
	if (f<1.5) nf = 1.;
	else if (f<3.) nf = 2.;
	else if (f<7.) nf = 5.;
	else nf = 10.;
    else
	if (f<=1.) nf = 1.;
	else if (f<=2.) nf = 2.;
	else if (f<=5.) nf = 5.;
	else nf = 10.;
    double unswer= nf*expt(10., expv);
    return unswer;
//    return nf*expt(10., expv);
}
double PlotSettings::expt(double a, int n)
{
    double x;

    x = 1.;
    if (n>0) for (; n>0; n--) x *= a;
    else for (; n<0; n++) x /= a;
    return x;
}



bool Plotter::savePlotAs()
{
    //QString fn = Q3FileDialog::getSaveFileName( QString::null, QString::null, this );
    QString fn = QFileDialog::getSaveFileName(this);
    if ( !fn.isEmpty() )
    {
        curFileCurve = fn;
        return this->savePlot();
    }
    return false;
}

bool Plotter::savePlot()
{
    if (this->curFileCurve.isEmpty())    {
        return this->savePlotAs();
    }
//    curFileCurve = "C:\\conductivity.dat";
    QFile f(this->curFileCurve);
    if(f.exists()){
        int n=QMessageBox::warning(0,
            tr("Warning"),
            "File with this name has already existed,"
            "\n Do you want to rewrite it?",
            "Yes",
            "No",
            QString(),
            0,
            1
            );
        if(n){
           return this->savePlotAs(); //выбираем новое имя
        }
    }
    //Saving the file!
    f.open(QIODevice::WriteOnly|QIODevice::Truncate);

    QTextStream o(&f);
    o << "Sigma(T)\n";

    map<int, CurveData>::const_iterator it = curveMap.begin();
    while (it !=curveMap.end())
    {
        int id = it->first;
//      o<<"Curve%i"<<id<<'\n';
        QString s;
        s.sprintf("Curve %i\n",id);
        o << s;
        const CurveData &data = it->second;
//        int numPoints=0;
        int maxPoints = data.size()/2;
        QPolygon points(maxPoints);
//        Q3PointArray points(maxPoints);
        for (int i=0; i<maxPoints; ++i){
            double x = data[2*i];
            double y = data[2*i+1];
            QString s;
            s.sprintf("%-14.7lg %lg\n",x,y);
            o << s;
            }

         s.sprintf("\n");
         o<< s;
            ++it;
    }
     f.close();
    return true;
}
