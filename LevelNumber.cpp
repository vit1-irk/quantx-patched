#include "LevelNumber.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


LevelNumber::LevelNumber(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent,f), model(0)
{
        this->setWindowTitle(tr("Numbers of levels"));
        this->setFont(QFont("Serif", 12, QFont::Bold ));
        QVBoxLayout *vl = new QVBoxLayout(this);
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("nmin"),this));
        h->addWidget(this->leNmin = new QLineEdit(this));
        this->leNmin->setEnabled(true);
        this->leNmin->setToolTip(tr("lower level number>=0"));
        connect(this->leNmin,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
    {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("nmax"),this));
        h->addWidget(this->leNmax = new QLineEdit(this));
        this->leNmax->setEnabled(true);
        this->leNmax->setToolTip(tr("upper level number < Nmax"));
        connect(this->leNmax,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }
        {
        QWidget *line = new QWidget(this);
        QHBoxLayout *h = new QHBoxLayout(line);
        h->addWidget(new QLabel(tr("hn"),this));
        h->addWidget(this->leHn = new QLineEdit(this));
        this->leHn->setEnabled(true);
        this->leHn->setToolTip(tr("level number increment"));
        connect(this->leHn,SIGNAL(editingFinished()),this,SLOT(updateModel()));
        vl->addWidget(line);
    }

    this->setLayout(vl);
    this->modelChanged();
}

LevelNumber::~LevelNumber(void)
{
}

void LevelNumber::setModel(PhysicalModel *_model)
{
    model = _model;
    modelChanged();
}

void LevelNumber::modelChanged()
{
    if (!model)
    {
        this->leNmax->setDisabled(true);
        this->leNmin->setDisabled(true);
        this->leHn->setDisabled(true);
        return;
    }
    else
    {
        this->leNmax->setEnabled(true);
        this->leNmin->setEnabled(true);
        this->leHn->setEnabled(true);
    }

    LevelNumberParameters wp = model->getLevelNumberParameters();
    last.nmin = -1;

    QString buf;
    buf.sprintf("%i",wp.nmin);
    this->leNmin->setText(buf);
    buf.sprintf("%i",wp.nmax);
    this->leNmax->setText(buf);
    buf.sprintf("%i",wp.hn);
    this->leHn->setText(buf);
}

void LevelNumber::updateModel()
{
    if (!model) return;
    LevelNumberParameters wp = { 0 };

    wp.nmin = this->leNmin->text().toInt();
    wp.nmax = this->leNmax->text().toInt();
    wp.hn   = this->leHn->text().toInt();
    if (wp != last)
    {
        model->setLevelNumberParameters(wp);
        last = wp;
    }
}

