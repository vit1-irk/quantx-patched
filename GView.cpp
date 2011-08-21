#include "GView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>


GView::GView(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("G parameters:"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel("G",this));
        h->addWidget(this->leG = new QLineEdit(this));
        this->leG->setEnabled(true);
        this->leG->setToolTip(tr("Current value of G"));
        connect(this->leG,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Gmin"),this));
        h->addWidget(this->leGmin= new QLineEdit(this));
        this->leGmin->setToolTip(tr("Lower bound of G interval"));
        connect(this->leGmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Gmax"),this));
        h->addWidget(this->leGmax= new QLineEdit(this));
        this->leGmax->setToolTip(tr("High bound of G interval"));
        connect(this->leGmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hG"),this));
        h->addWidget(this->leHG= new QLineEdit(this));
        this->leHG->setToolTip(tr("G increment"));
        connect(this->leHG,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

GView::~GView(void)
{
}
void GView::setCurrentG(double d)
{
    QString s;
    s.sprintf("%lg",d);
    this->leG->setText(s);
}

void GView::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
        connect(model,SIGNAL(signalGChanged(double)),this,SLOT(setCurrentG(double)));

  }

void GView::modelChanged()
{
    if (!model)
    {
        this->leG->setDisabled(true);
        this->leGmin->setDisabled(true);
        this->leGmax->setDisabled(true);
        this->leHG->setDisabled(true);
        return;
    }
    else
    {
        this->leG->setEnabled(true);
        this->leGmin->setEnabled(true);
        this->leGmax->setEnabled(true);
        this->leHG->setEnabled(true);
    }

    gParameters tp = model->getGParam();
    last.g = -1000;

    QString buf;
    buf.sprintf("%lg",tp.g);
    this->leG->setText(buf);

    buf.sprintf("%lg",tp.gmin);
    this->leGmin->setText(buf);

    buf.sprintf("%lg",tp.gmax);
    this->leGmax->setText(buf);

    buf.sprintf("%lg",tp.hg);
    this->leHG->setText(buf);
}

void GView::updateModel()
{
    if (!model) return;
    gParameters tp = { 0 };

    tp.g = this->leG->text().toDouble();
    tp.gmin = this->leGmin->text().toDouble();
    tp.gmax = this->leGmax->text().toDouble();
    tp.hg = this->leHG->text().toDouble();
    if (tp != last)
    {
        model->setGParam(tp);
        last = tp;
    }
}
