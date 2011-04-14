 #include "PlotterDialog.h"
#include <QVBoxLayout>
#include "MainWindow.h"
PlotterDialog::PlotterDialog(QWidget *parent, Qt::WindowFlags flags)
: QWidget(parent,flags)
//: QDialog(parent,flags)
{
    QVBoxLayout *vb = new QVBoxLayout(this);
    vb->addWidget( _plotter = new Plotter(this) );
    vb->addWidget( label = new QLabel(this) );
    connect(_plotter,SIGNAL(infoMouseMovedTo(QPointF)),
        this,SLOT(processInfoMouseMovedTo(QPointF)));
}

void PlotterDialog::processInfoMouseMovedTo(QPointF p)
{
    QString s;
    s.sprintf("x=%lg y=%lg",p.x(),p.y());
    label->setText(s);
}
