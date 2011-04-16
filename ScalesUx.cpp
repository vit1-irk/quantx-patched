#include "ScalesUx.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


ScalesUx::ScalesUx(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
    this->setWindowTitle(tr("Scales:"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);

    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Umin"),this));
        h->addWidget(this->leUmin= new QLineEdit(this));
        this->leUmin->setToolTip(tr("Lower value of potential"));
        connect(this->leUmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("Umax"),this));
        h->addWidget(this->leUmax= new QLineEdit(this));
        this->leUmax->setToolTip(tr("High value of potential"));
        connect(this->leUmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
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
/*    {
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
    }*/
    this->setLayout(vl);
    this->modelChanged();
}

ScalesUx::~ScalesUx(void)
{
}


void ScalesUx::setModel(PhysicalModel *_model)
{
    if (model != _model)
    {
        disconnect(model,0,this,0);
        model = _model;
        modelChanged();
    }
}

void ScalesUx::modelChanged()
{
    if (!model)
    {
        this->leHx->setDisabled(true);
        this->leXmin->setDisabled(true);
        this->leXmax->setDisabled(true);
        this->leUmin->setDisabled(true);
        this->leUmax->setDisabled(true);
//        this->lePsimin->setDisabled(true);
//        this->lePsimax->setDisabled(true);
        return;
    }
    else
    {
        this->leHx->setEnabled(true);
        this->leXmin->setEnabled(true);
        this->leXmax->setEnabled(true);
        this->leUmin->setEnabled(true);
        this->leUmax->setEnabled(true);
//        this->lePsimin->setEnabled(true);
//        this->lePsimax->setEnabled(true);
    }

    ScalesUParameters tp = model->getScalesUParam();
    last.Hx = -1000;

    QString buf;
    buf.sprintf("%lg",tp.Xmin);
    this->leXmin->setText(buf);

    buf.sprintf("%lg",tp.Xmax);
    this->leXmax->setText(buf);

    buf.sprintf("%lg",tp.Umin);
    this->leUmin->setText(buf);

    buf.sprintf("%lg",tp.Umax);
    this->leUmax->setText(buf);

//    buf.sprintf("%lg",tp.Psimax);
//    this->lePsimax->setText(buf);

//    buf.sprintf("%lg",tp.Psimin);
//    this->lePsimin->setText(buf);

    buf.sprintf("%lg",tp.Hx);
    this->leHx->setText(buf);
}

void ScalesUx::updateModel()
{
    if (!model) return;
    ScalesUParameters tp = { 0 };

    tp.Hx = this->leHx->text().toDouble();
    tp.Xmin = this->leXmin->text().toDouble();
    tp.Xmax = this->leXmax->text().toDouble();
    tp.Umin = this->leUmin->text().toDouble();
    tp.Umax = this->leUmax->text().toDouble();
//    tp.Psimin = this->lePsimin->text().toDouble();
//    tp.Psimax = this->lePsimax->text().toDouble();
    if (tp != last)
    {
        model->setScalesUParam(tp);
        last = tp;
    }
}
