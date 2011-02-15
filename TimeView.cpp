#include "TimeView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


TimeView::TimeView(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle("Time parameters:");
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("time",this));
        h->addWidget(this->leTime = new QLineEdit(this));
        this->leTime->setEnabled(true);
        this->leTime->setToolTip("Current value of time");
        connect(this->leTime,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("tmin",this));
        h->addWidget(this->leTmin= new QLineEdit(this));
        this->leTmin->setToolTip("Lower bound of time interval");
        connect(this->leTmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("tmax",this));
        h->addWidget(this->leTmax= new QLineEdit(this));
        this->leTmax->setToolTip("High bound of time interval");
        connect(this->leTmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("ht",this));
        h->addWidget(this->leHTime= new QLineEdit(this));
        this->leHTime->setToolTip("time increment");
        connect(this->leHTime,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

TimeView::~TimeView(void)
{
}

void TimeView::setCurrentTime(double d)
{
    QString s;
    s.sprintf("%lg",d);
    this->leTime->setText(s);
}

void TimeView::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
        connect(model,SIGNAL(signalTimeChanged(double)),this,SLOT(setCurrentTime(double)));
    }
}

void TimeView::modelChanged()
{
    if (!model)
    {
        this->leTime->setDisabled(true);
        this->leTmin->setDisabled(true);
        this->leTmax->setDisabled(true);
        this->leHTime->setDisabled(true);
        return;
    }
    else
    {
        this->leTime->setEnabled(true);
        this->leTmin->setEnabled(true);
        this->leTmax->setEnabled(true);
        this->leHTime->setEnabled(true);
    }

    TimeParameters tp = model->getTimeParam();
    last.time = -1000;

    QString buf;
    buf.sprintf("%lg",tp.time);
    this->leTime->setText(buf);

    buf.sprintf("%lg",tp.tmin);
    this->leTmin->setText(buf);

    buf.sprintf("%lg",tp.tmax);
    this->leTmax->setText(buf);

    buf.sprintf("%lg",tp.ht);
    this->leHTime->setText(buf);
}

void TimeView::updateModel()
{
    if (!model) return;
    TimeParameters tp = { 0 };

    tp.time = this->leTime->text().toDouble();
    tp.tmin = this->leTmin->text().toDouble();
    tp.tmax = this->leTmax->text().toDouble();
    tp.ht = this->leHTime->text().toDouble();
    if (tp != last)
    {
        model->setTimeParam(tp);
        last = tp;
    }
}
