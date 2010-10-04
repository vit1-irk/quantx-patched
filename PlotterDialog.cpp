#include "PlotterDialog.h"
#include <QVBoxLayout>

PlotterDialog::PlotterDialog(QWidget *parent, Qt::WindowFlags flags)
: QDialog(parent,flags)
{
    QVBoxLayout *vb = new QVBoxLayout(this);
    vb->addWidget( _plotter = new Plotter(this) );
    vb->addWidget( label = new QLabel(this) );
    connect(_plotter,SIGNAL(infoMouseMovedTo(QPoint)),
        this,SLOT(processInfoMouseMovedTo(QPoint)));
}

void PlotterDialog::processInfoMouseMovedTo(QPoint p)
{
    QString s;
    s.sprintf("x=%i y=%i",p.x(),p.y());
    label->setText(s);
}
