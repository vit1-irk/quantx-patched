#include "ScalePsin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


ScalePsin::ScalePsin(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle("Scales for wavefunction(x):");
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("xmin"),this));
        h->addWidget(this->leXmin= new QLineEdit(this));
        this->leXmin->setToolTip(tr("Lower value of x-interval"));
        connect(this->leXmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("xmax"),this));
        h->addWidget(this->leXmax= new QLineEdit(this));
        this->leXmax->setToolTip(tr("High value of x-interval"));
        connect(this->leXmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hx"),this));
        h->addWidget(this->leHx= new QLineEdit(this));
        this->leHx->setToolTip(tr("x increment"));
        connect(this->leHx,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Psi_min"),this));
        h->addWidget(this->lePsimin= new QLineEdit(this));
        this->lePsimin->setToolTip(tr("Lower value of Psi"));
        connect(this->lePsimin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Psi_max"),this));
        h->addWidget(this->lePsimax= new QLineEdit(this));
        this->lePsimax->setToolTip(tr("High value of Psi"));
        connect(this->lePsimax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    this->setLayout(vl);
    this->modelChanged();
}

ScalePsin::~ScalePsin(void)
{
}


void ScalePsin::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
}

void ScalePsin::modelChanged()
{
    if (!model)
    {
        this->leHx->setDisabled(true);
        this->leXmin->setDisabled(true);
        this->leXmax->setDisabled(true);
        this->lePsimin->setDisabled(true);
        this->lePsimax->setDisabled(true);
        return;
    }
    else
    {
        this->leHx->setEnabled(true);
        this->leXmin->setEnabled(true);
        this->leXmax->setEnabled(true);
        this->lePsimin->setEnabled(true);
        this->lePsimax->setEnabled(true);
    }

    ScalePsinParameters tp = model->getScalePsinParam();
    last.Hx = -1000;

    QString buf;
    buf.sprintf("%lg",tp.Xmin);
    this->leXmin->setText(buf);

    buf.sprintf("%lg",tp.Xmax);
    this->leXmax->setText(buf);

    buf.sprintf("%lg",tp.Psinmax);
    this->lePsimax->setText(buf);

    buf.sprintf("%lg",tp.Psinmin);
    this->lePsimin->setText(buf);

    buf.sprintf("%lg",tp.Hx);
    this->leHx->setText(buf);
}

void ScalePsin::updateModel()
{
    if (!model) return;
    ScalePsinParameters tp = { 0 };

    tp.Hx = this->leHx->text().toDouble();
    tp.Xmin = this->leXmin->text().toDouble();
    tp.Xmax = this->leXmax->text().toDouble();
    tp.Psinmin = this->lePsimin->text().toDouble();
    tp.Psinmax = this->lePsimax->text().toDouble();
    if (tp != last)
    {
        model->setScalePsinParam(tp);
        last = tp;
    }
}
