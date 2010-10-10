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
/*
    {
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton * bsx = new QPushButton("&x scale");	
        connect(bsx,SIGNAL(clicked()),this,SLOT(slotScaleX()));
        hl->addWidget(bsx);		

        QPushButton * bsz = new QPushButton("&z scale");	
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);		
        vb->addLayout(hl);

    }
    {
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton * bsy = new QPushButton("&y scales");	
        connect(bsy,SIGNAL(clicked()),this,SLOT(slotScaleY()));
        hl->addWidget(bsy);		

        QCheckBox *flgScale= new QCheckBox("Fixed y-scales",this);
        flgScale->setChecked(true);
        hl->addWidget(flgScale);
        vb->addLayout(hl);
    }

    {
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton *bRun = new QPushButton(tr("RUN")); 
        connect(bRun, SIGNAL(clicked()), this, SLOT(slotRun()));
        hl->addWidget(bRun);

        QPushButton *stopB = new QPushButton(tr("STOP")); 
        stopB->setShortcut(tr("Alt+S"));
        connect(stopB, SIGNAL(clicked()), this, SLOT(stopCalc()));
        hl->addWidget(stopB);
        vb->addLayout(hl);
    }
*/
}

void PlotterDialog::processInfoMouseMovedTo(QPointF p)
{
    QString s;
    s.sprintf("x=%lg y=%lg",p.x(),p.y());
    label->setText(s);
}
