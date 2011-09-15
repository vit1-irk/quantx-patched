//qmake -tp vc
#include "MainWindow.h"
#include <math.h>
#include <cmath>

#include "MyParam.h"
#include "PhysicalModel.h"
#include "PotentialModel.h"
#include "LevelModel.h"
#include "QuasiLevelModel.h"
#include "PotentialViewMultiWell.h"
#include "LineEditDelegate.h"
#include "PotentialViewMovable.h"
#include "PlotterDialog.h"
#include <QList>
#include <QRectF>
#include <QGraphicsScene>
#include "BreakStatus.h"
#include "MySplitter.h"

class DoubleValidator : public QRegExpValidator
{
public:
    DoubleValidator(QWidget *parent) : QRegExpValidator(parent)
    {
        const QRegExp x(QString("[+-]?\\d+\\.?\\d*([dDe][+-]?\\d+)?"));
        setRegExp(x);
    }
    ~DoubleValidator(){}
} theDoubleValidator(0);

class PosIntValidator : public QRegExpValidator
{
public:
    PosIntValidator(QWidget *parent) : QRegExpValidator(parent)
    {
        const QRegExp x(QString("\\d+"));
        setRegExp(x);
    }
    ~PosIntValidator(){}
} thePosIntValidator(0);


void MainWindow::helpStart()
{
    QStringList args;
    args.push_back("/A");
//    QString dest;
//    dest.sprintf("nameddest=%s",s);
    args.push_back("start");
    args.push_back("kvant.pdf");
    int i = QProcess::startDetached("C:\\Program Files\\Adobe\\Reader 10.0\\Reader\\AcroRd32.exe",args);
}

void MainWindow::initMenuBar()
{
     QMenu *fileMenu = new QMenu;
     menuBar()->addMenu(fileMenu);
     QFont font( "Serif", 10, QFont::DemiBold );
     menuBar()->setFont(font);
     fileMenu->setFont(font);
     fileMenu->setTitle(tr("File"));
     menuBar()->addSeparator();
//     fileMenu->setFont(QFont("Serif", 12, QFont::Bold ));
//     QMenu *fileMenu = new QMenu(tr("&File"), this);
//     fileMenu->addAction(tr("&Quit"), this, SLOT(close()));
     QAction *quitAction = fileMenu->addAction(tr("Quit"));
//     quitAction->setShortcut(tr("Ctrl+Q"));
     quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
     connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

     QAction *openAction = fileMenu->addAction(tr("&Open..."));
     openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
     connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
     fileMenu->addAction(openAction);
     //TODO: openFile(str)

     QAction *saveAsAction = fileMenu->addAction(tr("&Save As..."));
     saveAsAction->setShortcut(tr("Ctrl+S"));
     connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
     fileMenu->addAction(saveAsAction);

     QAction *helpAction = fileMenu->addAction(tr("&Help"));
     helpAction->setShortcut(tr("Ctrl+H"));
     connect(helpAction, SIGNAL(triggered()), this, SLOT(helpStart()));
     fileMenu->addAction(helpAction);

     aboutAction = new QAction(tr("&О программе"),this);
//     aboutAction->setStatuslTip(tr("Сведения о программе"));
     connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
     fileMenu->addAction(aboutAction);
     fileMenu->addSeparator();

     //     chooseFontAction = new QAction("Choose Font", this);
//     fileMenu->addAction(chooseFontAction);
//     chooseFontAction->setShortcut(tr("Ctrl+F"));
//     connect(chooseFontAction, SIGNAL(triggered()), this, SLOT(chooseFont()));

     QMenu *uMenu = menuBar()->addMenu(tr("Потенциалы"));
     uMenu->setFont(font);//setFont(QFont("Serif", 12, QFont::Bold ));
     QAction *uTable = new QAction(tr("Табличный"), uMenu);
     QAction *uLinear = new QAction(tr("Линейный"),uMenu);
     QAction *uParabolic = new QAction(tr("Параболический"),uMenu);
     QAction *uCh2x = new QAction(tr("U_0/ch^2(x/a)"),uMenu);
     uMenu->addAction(uTable);
     uMenu->addSeparator();
     uMenu->addSeparator();
     uMenu->addAction(uLinear);
     uMenu->addSeparator();
     uMenu->addAction(uParabolic);
     uMenu->addSeparator();
     uMenu->addAction(uCh2x);
     uMenu->addSeparator();
     connect(uTable, SIGNAL(triggered()), this, SLOT(slotSetUxt()));
     connect(uParabolic, SIGNAL(triggered()), this, SLOT(slotSetUs()));
     connect(uLinear, SIGNAL(triggered()), this, SLOT(slotSetUlinear()));
     connect(uCh2x, SIGNAL(triggered()), this, SLOT(slotSetUCh2x()));

     QMenu *wMenu = menuBar()->addMenu(tr("Зависимости"));
     wMenu->setWhatsThis(tr("Основные варианты счета"));
     wMenu->setFont(font);
     QString psi=QChar(0x03C8);
     QString Psi=QChar(0x03A8);
     QString phi=QChar(0x03C6);
     QString Phi=QChar(0x03A6);
     QString to2=QChar(0x00B2);
     QString phinofk=phi+"_n(k)";
     QString Phinofkt=Phi+"_n(k,t)";
     QString psiofx=psi+"(x)";
     QString psinofx=psi+"_n(x)";
     QString mod_psiofx="|"+psiofx+"|"+to2;
     QString mod_phinofk="|"+phinofk+"|"+to2;
     QString mod_Phinofkt="|"+Phinofkt+"|"+to2;

     Uaction = new QAction("U(x) && En", this);
     wMenu->addAction(Uaction);
     wMenu->addSeparator();
     Uaction->setStatusTip(tr("Уровни энергии в потенциале U(x)"));

     QAction *mPsinX = new QAction(psinofx, this);
     wMenu->addAction(mPsinX);
     wMenu->addSeparator();
     mPsinX->setStatusTip(tr("Волновые функции для уровней энергии"));
//     mPsinX->setStatusTip(tr("Wave functions for energy levels"));
     QAction *mPhinK = new QAction(mod_phinofk, this);
     wMenu->addAction(mPhinK);
     wMenu->addSeparator();
     mPhinK->setStatusTip(tr("Импульсное распределение для уровней энергии"));
//     mPhinK->setStatusTip(tr("Momentum distributions for energy levels"));
     QAction *mPsiXT = new QAction(Psi+"(x,t)", this);
     wMenu->addAction(mPsiXT);
     wMenu->addSeparator();
     mPsiXT->setStatusTip(tr("Временная эволюция волнового пакета: координатное распределение"));
//     mPsiXT->setStatusTip(tr("Coordinate distribution: time development of wave packet"));
     QAction *mPhiKT = new QAction(mod_Phinofkt, this);
     wMenu->addAction(mPhiKT);
     wMenu->addSeparator();

     mPsiXT->setStatusTip(tr("Временная эволюция волнового пакета: импульсное распределение"));
//     mPhiKT->setStatusTip(tr("Momentum distribution: time development of wave packet"));
     QAction *mTE = new QAction(tr("T(E)"), this);
     wMenu->addAction(mTE);
     wMenu->addSeparator();
     mTE->setStatusTip(tr("Коэффициент прохождения в зависимости от энергии"));

/*     QAction *mTZ = new QAction(tr("T(z)"), this);
     mTZ->setStatusTip(tr("Коэффициент прохождения в зависимости от z"));
     wMenu->addAction(mTZ);
     wMenu->addSeparator();

     QAction *mEnz = new QAction(tr("Enz"), this);
     wMenu->addAction(mEnz);
     wMenu->addSeparator();
     mEnz->setStatusTip(tr("Уровни энергии как функция деформации потенциала z"));

     QAction *mQE = new QAction(tr("qa(E)"), this);
     wMenu->addAction(mQE);
     wMenu->addSeparator();
     mQE->setStatusTip(tr("Закон дисперсии в периодическом поле"));

     QAction *mEG = new QAction(tr("B_{N+1}(E+iG)"), this);
     wMenu->addAction(mEG);
     wMenu->addSeparator();
     mEG->setStatusTip(tr("Квазистационарные сост. на пл. E+iG, нули Re и Im B_{N+1}" ));
*/
     connect(Uaction, SIGNAL(triggered()), this, SLOT(window_Ux()));
     connect(mPsinX, SIGNAL(triggered()), this, SLOT(window_psi_x()));
     connect(mPhinK, SIGNAL(triggered()), this, SLOT(window_phi_k()));
     connect(mPsiXT, SIGNAL(triggered()), this, SLOT(window_psi_xt()));
     connect(mPhiKT, SIGNAL(triggered()), this, SLOT(window_phi_kt()));
     connect(mTE, SIGNAL(triggered()), this, SLOT(window_TE()));
/*     connect(mTZ, SIGNAL(triggered()), this, SLOT(window_TZ()));
     connect(mEnz, SIGNAL(triggered()), this, SLOT(window_Enz()));
     connect(mQE, SIGNAL(triggered()), this, SLOT(window_QE()));
     connect(mEG, SIGNAL(triggered()), this, SLOT(window_EG()));
*/
     QMenu *adMenu = menuBar()->addMenu(tr("Доп. зависимости"));
     adMenu->setWhatsThis(tr("Additional dependences"));
     adMenu->setFont(font);
     QAction *mTZ = new QAction(tr("T(z)"), this);
     mTZ->setStatusTip(tr("Коэффициент прохождения в зависимости от z"));
     adMenu->addAction(mTZ);
     adMenu->addSeparator();

     QAction *mEnz = new QAction(tr("Enz"), this);
     adMenu->addAction(mEnz);
     adMenu->addSeparator();
     mEnz->setStatusTip(tr("Уровни энергии как функция деформации потенциала z"));

     QAction *mQE = new QAction(tr("qa(E)"), this);
     adMenu->addAction(mQE);
     adMenu->addSeparator();
     mQE->setStatusTip(tr("Закон дисперсии в периодическом поле"));

     QAction *mEG = new QAction(tr("B_{N+1}(E+iG)"), this);
     adMenu->addAction(mEG);
     adMenu->addSeparator();
     mEG->setStatusTip(tr("Квазистационарные сост. на пл. E+iG, нули Re и Im B_{N+1}" ));

     connect(mTZ, SIGNAL(triggered()), this, SLOT(window_TZ()));
     connect(mEnz, SIGNAL(triggered()), this, SLOT(window_Enz()));
     connect(mQE, SIGNAL(triggered()), this, SLOT(window_QE()));
     connect(mEG, SIGNAL(triggered()), this, SLOT(window_EG()));

     QToolBar *bcTool = new QToolBar;
     bcTool->setFont(font);
     addToolBar(bcTool);
     QAction *bcAc = new QAction(tr("Model"), bcTool);
     bcTool->addAction(bcAc);
     bcAc->setToolTip(tr("Граничные условия"));
     connect(bcAc, SIGNAL(triggered()), this, SLOT(slotSetBD()));

     QToolBar *uTool = new QToolBar;
     uTool->setFont(font);
     addToolBar(uTool);
     QAction *uMulti = new QAction(tr("Un"), uTool);
     uTool->addAction(uMulti);
     uTool->addSeparator();
     uMulti->setToolTip(tr("Потенциал из одинаковых ям/барьеров"));
//     uMulti->setToolTip("Multi-well/barrier potential");
     connect(uMulti, SIGNAL(triggered()), this, SLOT(slotSetUmwb()));

     QToolBar *EnTool = new QToolBar;
     addToolBar(EnTool);
     QAction *EnAction = new QAction(tr("En"), EnTool);
     EnTool->addAction(EnAction);
     EnAction->setToolTip(tr("Таблица уровней энергии En"));
//   EnAction->setToolTip("Values of energy levels En");
     connect(EnAction, SIGNAL(triggered()), this, SLOT(slotSetEn()));

     QToolBar *EquasiTool = new QToolBar;
     addToolBar(EquasiTool);
     QAction *EquasiAction = new QAction(tr("(E+iG)n"), EquasiTool);
     EquasiTool->addAction(EquasiAction);
     EquasiAction->setToolTip(tr("Таблица энергий квазистац. состояний"));
//   EnAction->setToolTip("Values of energy levels En");
     connect(EquasiAction, SIGNAL(triggered()), this, SLOT(slotSetEquasi()));

     QToolBar *U1Tool = new QToolBar;
     addToolBar(U1Tool);
     QAction *U1Action = new QAction(tr("U1"), U1Tool);
     U1Tool->addAction(U1Action);
     U1Action->setToolTip(tr("Начальный потенциал"));
//     U1Action->setToolTip("Initial Potential");
     connect(U1Action, SIGNAL(triggered()), model, SLOT(slotU1()));

     QToolBar *U2Tool = new QToolBar;
     addToolBar(U2Tool);
     QAction *U2Action = new QAction(tr("U2"), U2Tool);
     U2Tool->addAction(U2Action);
     U2Action->setToolTip(tr("Конечный потенциал"));
//   U2Action->setToolTip("Final Potential");
     connect(U2Action, SIGNAL(triggered()), model, SLOT(slotU2()));

     QToolBar *ubiasTool = new QToolBar;
     addToolBar(ubiasTool);
     QAction *ubiasAc = new QAction(tr("Ubias"), ubiasTool);
     ubiasTool->addAction(ubiasAc);
     ubiasAc->setToolTip(tr("Падение напряжения"));
     connect(ubiasAc, SIGNAL(triggered()), this, SLOT(slotSetUlinear()));

     QToolBar *widthLineTool = new QToolBar;
     addToolBar(widthLineTool);
     QAction *widthLineAc = new QAction(tr("Setting"), widthLineTool);
     widthLineTool->addAction(widthLineAc);
     widthLineAc->setToolTip(tr("Толщина линий в пикселах"));
     connect(widthLineAc, SIGNAL(triggered()), this, SLOT(slotSetting()));
}
void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе Кванте"),
        tr("Это новая версия программы QUANTX, написанная на Qt. Авторы: Ткаченко О.А., Ткаченко В.А., Коткин Г.Л., Бакшеев Д.Г."));
}

void MainWindow::chooseFont()
{

    bool ok;
    QFont oldfont = myfont;
//    myfont = QFontDialog::getFont(&ok, oldfont, this);
    QFont myfont = QFontDialog::getFont(&ok, QFont("Times", 16), this);
    if (ok)
        this->menuWidget()->setFont(myfont);
}


bool MainWindow::saveAs()
{
    QString fn = QFileDialog::getSaveFileName( this,
        tr("Choose a file name"), ".",
        tr("*.xml"));
    if ( !fn.isEmpty() ) {
        curFile = fn;
        return this->save();
    }
    return false;
}

void MainWindow::slotSetEn()
{
    if (!tableViewEn)
    {
        tableViewEn = new QTableView(this);

        tableViewEn->setWindowFlags(Qt::Window);
        tableViewEn->setFont(QFont("Serif", 12, QFont::Bold ));
        tableViewEn->setWindowTitle("Energy Levels");

        tableViewEn->setCornerButtonEnabled(false);
        tableViewEn->setSortingEnabled(false);
        tableViewEn->setWordWrap(false);

        tableViewEn->setItemDelegate(new LineEditDelegate(this));
        tableViewEn->setTextElideMode(Qt::ElideNone);
#if 0
        tableViewEn->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewEn->setSelectionBehavior(QAbstractItemView::SelectItems);
        tableViewEn->setCursor(Qt::IBeamCursor);
        tableViewEn->setEditTriggers(QAbstractItemView::AllEditTriggers);
        tableViewEn->setItemDelegate(new LineEditDelegate(tableViewEn));
#endif

        LevelModel *lm = new LevelModel(tableViewEn);
        lm->setLevels(model);
        tableViewEn->setModel(lm);

        connect(lm,SIGNAL(modelReset()),tableViewEn,SLOT(resizeColumnsToContents()));
    }

    ///?tableViewEn->reset();
    tableViewEn->show();
    tableViewEn->activateWindow();
    tableViewEn->setFocus();
    tableViewEn->resizeColumnsToContents();
}
void MainWindow::slotSetEquasi()
{
    if (!tableViewEquasi)
    {
        tableViewEquasi = new QTableView(this);
        tableViewEquasi->setWindowFlags(Qt::Window);
        tableViewEquasi->setFont(QFont("Serif", 12, QFont::Bold ));
        tableViewEquasi->setWindowTitle("Quasilevels");

        tableViewEquasi->setCornerButtonEnabled(false);
        tableViewEquasi->setSortingEnabled(false);
        tableViewEquasi->setWordWrap(false);

        tableViewEquasi->setItemDelegate(new LineEditDelegate(this));
#if 0
        tableViewEquasi->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewEquasi->setSelectionBehavior(QAbstractItemView::SelectItems);
        tableViewEquasi->setCursor(Qt::IBeamCursor);
        tableViewEquasi->setEditTriggers(QAbstractItemView::AllEditTriggers);
        tableViewEquasi->setItemDelegate(new LineEditDelegate(tableViewEquasi));
#endif

        QuasiLevelModel *lm = new QuasiLevelModel(tableViewEquasi);
        lm->setQuasiLevels(model);
        tableViewEquasi->setModel(lm);
    }

    tableViewEquasi->reset();
    tableViewEquasi->show();
    tableViewEquasi->activateWindow();
    tableViewEquasi->setFocus();
}


void MainWindow::slotSetUxt()
{
    if (!tableView)
    {
        tableView = new QTableView(this);
        tableView->setWindowFlags(Qt::Window);
        tableView->setFont(QFont("Serif", 12, QFont::Bold ));
        tableView->setWindowTitle("Step-wise potential");

        tableView->setItemDelegate(new LineEditDelegate(this));

        PotentialModel *pm = new PotentialModel(tableView);
        pm->setPotential(model);

        tableView->setModel(pm);
        connect(model,SIGNAL(signalPotentialChanged()),tableView,SLOT(reset()));
    }
    tableView->reset();
    tableView->show();
    tableView->activateWindow();
    tableView->setFocus();
}
void MainWindow::slotSetBD()
{
    if (!boundCondView)
    {
        boundCondView = new BoundaryCondition(this);
        boundCondView->setModel(model);
    }
    boundCondView->show();
    boundCondView->activateWindow();
    boundCondView->setFocus();
}

void  MainWindow::slotSetUs()
{
    if (!dialogUparab)
    {
        dialogUparab = new Uparabolic(this);
        dialogUparab->setModel(model);
    }
    dialogUparab->show();
    dialogUparab->activateWindow();
    dialogUparab->setFocus();
}
void  MainWindow::slotSetUCh2x()
{
    if (!dialogUch2x)
    {
        dialogUch2x = new Uch2x(this);
        dialogUch2x->setModel(model);
    }
    dialogUch2x->show();
    dialogUch2x->activateWindow();
    dialogUch2x->setFocus();
}
void  MainWindow::slotSetUlinear()
{
    if (!dialogUlinear)
    {
        dialogUlinear = new Ulinear(this);
        dialogUlinear->setModel(model);
    }
    dialogUlinear->show();
    dialogUlinear->activateWindow();
    dialogUlinear->setFocus();
}
void  MainWindow::slotSetting()
{
    if (!dialogSetting)
    {
        dialogSetting = new ProgramSetting(this);
        dialogSetting->setModel(model);
    }
    dialogSetting->show();
    dialogSetting->activateWindow();
    dialogSetting->setFocus();
}
void  MainWindow::slotSetUmwb()
{
    if (!dialogUAsMW)
    {
        dialogUAsMW = new PotentialViewMultiWell(this);
        dialogUAsMW->setModel(model);
    }
    dialogUAsMW->show();
    dialogUAsMW->activateWindow();
    dialogUAsMW->setFocus();
}

void MainWindow::updateMouseMovedTo(QPointF f)
{
    QString x,y;
    x.sprintf("x=%lg",f.x());
    y.sprintf("y=%lg",f.y());
    this->mouseAtX->setText(x);
    this->mouseAtY->setText(y);
}

    void MainWindow::window_TE()
{
    if(!teWidget)
    {
    teWidget = new TEWidget(model);
    }
    splitterL->addWidget(teWidget);
    teWidget->show();
}
    void MainWindow::window_QE()
{
    if(!qeWidget)
    {
    qeWidget = new QEWidget(model);
    }
    splitterL->addWidget(qeWidget);
    qeWidget->show();
}
void MainWindow::window_TZ()
{
    if(!tzWidget)
    {
    tzWidget = new TZWidget(model);
    }
    splitterL->addWidget(tzWidget);
    tzWidget->show();
}
void MainWindow::window_Ux()
{
    if(!uxWidget)
    {
    uxWidget = new PotentialMovableWidget(model);//QGroupBox("Wave Function");
    }
    splitterR->addWidget(uxWidget);
    uxWidget->show();
}

void  MainWindow::slotSetTime()
{
    if (!dialogTime)
    {
        dialogTime = new TimeView(this);
        dialogTime->setModel(model);
    }
    dialogTime->show();
    dialogTime->activateWindow();
    dialogTime->setFocus();
}

void  MainWindow::slotSetZ()
{
    if (!dialogZ)
    {
        dialogZ = new Zview(this);
        dialogZ->setModel(model);
    }
    dialogZ->show();
    dialogZ->activateWindow();
    dialogZ->setFocus();

}

void MainWindow::window_psi_xt()
{
    if(!wavePacketXWidget)
    {
    wavePacketXWidget = new WavePacketXWidget(model);
    }
    splitterR->addWidget(wavePacketXWidget);
    wavePacketXWidget->show();
}
void MainWindow::window_psi_x()
{
    if(!waveFunctionWidget)
    {
    waveFunctionWidget = new WaveFunctionWidget(model);//QGroupBox("Wave Function");
    }
    splitterR->addWidget(waveFunctionWidget);
    waveFunctionWidget->show();
}
void MainWindow::window_Enz()
{
    if(!enzWidget)
    {
        enzWidget = new EnzWidget(model);
    }
    splitterL->addWidget(enzWidget);
    enzWidget->show();
}
void MainWindow::window_EG()
{
    if(!egWidget)
    {
        egWidget = new EGWidget(model);
    }
    splitterL->addWidget(egWidget);
    egWidget->show();
}

void MainWindow::window_phi_kt()
{
    if(!wavePacketKWidget)
    {
    wavePacketKWidget = new WavePacketKWidget(model);
    }
    splitterL->addWidget(wavePacketKWidget);
    wavePacketKWidget->show();
}
void MainWindow::window_phi_k()
{
    if(!momentumDistibutionWidget)
    {
        momentumDistibutionWidget = new MomentumDistibutionWidget(model);
    }
    splitterL->addWidget(momentumDistibutionWidget);
    momentumDistibutionWidget->show();
}

void MainWindow::initControlDockWindow()
{
    splitterR = new MySplitter(Qt::Vertical);
    splitterL = new MySplitter(Qt::Vertical);
    splitterLR = new MySplitter(Qt::Horizontal);
    window_Ux();
//    window_TE();
    QVBoxLayout * vl0 = new QVBoxLayout;
    splitterLR->addWidget(splitterL);
    splitterLR->addWidget(splitterR);
    vl0->addWidget(splitterLR);
    setCentralWidget(splitterLR);
//    QVBoxLayout * mainLayout = new QVBoxLayout;
//    mainLayout->addLayout(vl0);
//    QWidget *widget = new QWidget;
//    widget->setLayout(mainLayout);
//    widget->raise();
//    setCentralWidget(widget);
}

void MainWindow::resizeEvent (QResizeEvent * event)
{
    QWidget::resizeEvent(event);
}
void MainWindow::initStatusBar()
{
    createStatusBar();
    this->statusBar()->setFont(QFont("Serif", 10, QFont::DemiBold ));
    this->dispEnergy = new QLabel("Comments:",
            this->statusBar());
    this->statusBar()->addWidget(this->dispEnergy);

 }

/*void MainWindow::Bound_States()
{
    int i = this->En_type->QComboBox::currentIndex();

    QPair<double,double> umin_umax = model->getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;
    if(this->Emin>0) this->Emin=Umin;
    if(this->Emax>0) this->Emax=Umax;
    switch(i)
    {
    case 0:
        compute_En();
        break;
    case 1:
        compute_NE();
        break;
    case 2:
        compute_BE();
        break;
    case 3:
        compute_Enz();
        break;
    default:
        break;
    }
}
*/
/*void MainWindow::compute_NE()
{
    double y,_dE;
    winPlotNE->show();
    winPlotNE->raise();
    plotNE->scaleFixed=false;
    dataNE.clear();
    QPair<double,double> umin_umax = model->getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;

    _dE=this->hE;
    if(_dE<0) _dE=-this->hE;
    if(_dE==0) _dE=(Umax-Umin)/100;
    this->plotNE->clearAll();
//    numOfCurveNE=0;
    for(double x=Umin; x<=Umax; x+=_dE)
    {
        y=double(model->findNumberOfLevels(x));
//        this->dataNE.push_back(x);
        this->dataNE.push_back(y);
        this->dataNE.push_back(x);
    }
    this->plotNE->setCurveData(1,this->dataNE);
//    this->plotNE->setCurveData(this->numOfCurveNE,this->dataNE);
//    this->numOfCurveNE++;
}
*/
/*void MainWindow::compute_D()
{
    wPlotT->show();
    wPlotT->raise();
    if(this->Emin<0) this->Emin=0.01;
    if(this->Emax<0) this->Emax=20.;
    if(this->E0<0) this->E0=0.01;

    if(this->flgEraseT->isChecked())
    {
        plotterT->clearAll();
        this->numOfCurveT=1;
    }
    else
    {
        this->numOfCurveT++;
    }
    int ii = this->T_type->QComboBox::currentIndex();
    switch(ii)
    {
    case 0:
        compute_TE();
        break;
    case 1:
        compute_Tz();
        break;
    case 2:
        TEz();
        break;
    case 3:
        TzE();
        break;
    default:
        break;
    }

}
*/
//inline double squre_module(complex& a){ return fabs(real(a)*real(a))+fabs(imag(a)*imag(a)); }
/*
void MainWindow::compute_TE()
{
    std::vector<double> dataR;
    std::vector<double> dataRR;
    std::vector<double> dataL;
    std::vector<double> dataSum;
    bool flgEr=flgEraseT->isChecked();
    if(flgEr)
    {
        this->numOfCurveT=1;
    }
    else
    {
        this->numOfCurveT++;
    }
    double Emn=this->Emin;
    double Emx=this->Emax;
    if(this->E0>=Emx&&this->hE>0) this->E0=Emn;
    if(this->E0<=Emn&&this->hE<0) this->E0=Emx;
    double EE=this->E0;
    if(EE>this->Emax&&this->hE>0) EE=this->Emin;
    if(EE<this->Emin&&this->hE<0) EE=this->Emax;
    PlotSettings ps;
    bool flg=flgScale->isChecked();
    plotterT->scaleFixed=flg;
    if(flg)
    {
    ps.minX = this->Emin;
    ps.maxX = this->Emax;
    ps.minY = 0;
    ps.maxY = 1.1;
    ps.numYTicks = 5;
    ps.numXTicks = 5;
    plotterT->setPlotSettings(ps);
    }
    double L1=80;//10;//3;//8;
    double L2=70;//50;//7;//4;//10
    double g1=0.2;//-0.2;//-0.1;//3//12
    double g2=0;//0.3;//-0.000;//-0.1;//5
    complex c2, c1, r, fun1, fun2;
    double y,y2,rr,rr1,tt,sum,gamma;
    double E0=586;
    double kk=sqrt(EE/E0);
    double kmn=sqrt(Emn/E0);
    double kmx=sqrt(Emx/E0);
    for(double k=kk; k>=kmn&&k<=kmx; k+=this->hE)
//    for(double E=EE; E>=Emn&&E<=Emx; E+=this->hE)
    {
//                double k = sqrt(E/E0);
                double kL1=k*L1;
                double kL2=k*L2;
                complex exm1=exp(complex(0, -kL1));
                complex exm2=exp(complex(0, -kL2));
 //               complex exm3=exp(complex(0, -kL3));
                complex I=complex(0,1);
            fun1 = 2.*exm1+2.*(g1/k)*sin(kL1);
            fun2 = 2.*exm2+2.*(g2/k)*sin(kL2);
            c2 = 2. * fun1 /(fun2*(2.+I*g1/k)*exm1 + fun1*(exm2+I*(g2/k)*cos(kL2)));
            c1 = c2*fun2/fun1;
            complex r1 = 1.-c1*(g1/k*sin(kL1)+exm1);
                y=squre_module(c1);
                y2=squre_module(c2);
                rr1=g2/k;
                rr1=rr1*rr1;
                double fun=1.+rr1*sin(kL2)*sin(kL2)+g2/k*sin(2*kL2);
//                rr1=4./(6+rr1+3*fun);
//                rr1=4.*(1.+rr1*sin(kL2)*sin(kL2)+g2/k*sin(2*kL2))/(9.+rr1*(1.+3.*sin(kL2)*sin(kL2))+3.*g2/k*sin(2.*kL2));//squre_module(r1);
//                rr1=4./(9.+rr1*(1.+3.*sin(kL2)*sin(kL2))+3.*g2/k*sin(2.*kL2));//squre_module(r1);
                rr1=squre_module(r1);
//                rr1=4./(9+rr1*2.5);
                tt=1-rr1;
                sum=y+y2;
                double gamma1=(g2/k)*sin(kL2)*(2*cos(kL2)+(g2/k)*sin(kL2))/(2.+(g2/k)*sin(kL2)*(2*cos(kL2)+(g2/k)*sin(kL2)));
                double a12=(1+(g2/k)*sin(2*kL2)+((g2/k)*sin(kL2))*((g2/k)*sin(kL2)))/(1+(g1/k)*sin(2*kL1)+((g1/k)*sin(kL1))*((g1/k)*sin(kL1)));
                double gamma2=(a12-1)/(1+a12);
                gamma=(y-y2)/(y+y2);
                double x=2*(kL1+kL2)/3.14159;
        dataL.push_back(x);
        dataL.push_back(y);
        dataR.push_back(x);
        dataR.push_back(y2);
        dataRR.push_back(x);
        dataRR.push_back(gamma2);
        dataSum.push_back(x);
        dataSum.push_back(rr1);// (gamma);
    }
    this->plotterT->setCurveData(this->numOfCurveT,dataL);
    this->numOfCurveT++;
    this->plotterT->setCurveData(this->numOfCurveT,dataR);
    this->numOfCurveT++;
    this->plotterT->setCurveData(this->numOfCurveT,dataSum);
    this->numOfCurveT++;
    this->plotterT->setCurveData(this->numOfCurveT,dataRR);
    this->numOfCurveT++;

}
*/
/*
void MainWindow::compute_TE()
{
    std::vector<double> dataT;
    bool flgEr=flgEraseT->isChecked();
    if(flgEr)
    {
        this->numOfCurveT=1;
    }
    else
    {
        this->numOfCurveT++;
    }
    double Emn=this->Emin;
    double Emx=this->Emax;
    if(this->E0>=Emx&&this->hE>0) this->E0=Emn;
    if(this->E0<=Emn&&this->hE<0) this->E0=Emx;
    double EE=this->E0;
    if(EE>this->Emax&&this->hE>0) EE=this->Emin;
    if(EE<this->Emin&&this->hE<0) EE=this->Emax;
    PlotSettings ps;
    bool flg=flgScale->isChecked();
    plotterT->scaleFixed=flg;
    if(flg)
    {
    ps.minX = this->Emin;
    ps.maxX = this->Emax;
    ps.minY = 0;
    ps.maxY = 1.1;
    ps.numYTicks = 5;
    ps.numXTicks = 5;
    plotterT->setPlotSettings(ps);
    }
        double dd= model->get_d(2);
    for(double E=EE; E>=Emn&&E<=Emx; E+=this->hE)
    {
        this->E0=E;
        compute();
        double x=2*sqrt(E)*dd/3.14159;
        double y=model->TT;
        dataT.push_back(x);
        dataT.push_back(y);
    }
    this->plotterT->setCurveData(this->numOfCurveT,dataT);
    this->E0=this->E0+this->hE;
    this->numOfCurveT++;
}
*/
/*void MainWindow::compute_Tz()
{
    std::vector<double> dataT;
    if(this->flgEraseT->isChecked())
    {
        this->numOfCurveT=0;
    }
    else
    {
        this->numOfCurveT++;
    }
    if(this->zmin<0) this->zmax=0.0;
    if(this->zmax<0) this->zmax=1.;
    double zmn=this->zmin;
    double zmx=this->zmax;
    if(this->zz>=this->zmax&&this->hE>0) this->zz=zmn;
    if(this->zz<=this->zmin&&this->hz<0) this->zz=zmx;
    double z0=this->zz;
    PlotSettings ps;
    bool flg=flgScale->isChecked();
    plotterT->scaleFixed=flg;
    if(flg)
    {
    ps.minX = this->zmin;
    ps.maxX = this->zmax;
    ps.minY = 0;
    ps.maxY = 1.1;
    ps.numYTicks = 5;
    ps.numXTicks = 5;
    plotterT->setPlotSettings(ps);
    }
    for(double z=z0; z>=zmn&&z<=zmx; z+=this->hz)
    {
//        if (getBreakStatus(0)) return;
        this->zz=z;
        model->set_Uxz(z);
        compute();
        double y=model->TT;
        dataT.push_back(z);
        dataT.push_back(y);
    }
    this->plotterT->setCurveData(this->numOfCurveT,dataT);
    this->zz=this->zz+this->hz;

}*/
/*
void MainWindow::TEz()
{
    double z0=this->zz;
    if(z0>=this->zmax&&this->hz>0) z0=this->zmin;
    if(z0<=this->zmin&&this->hz<0) z0=this->zmax;
    double Emn=this->Emin;
    double Emx=this->Emax;
    if(this->hE>0) this->E0=Emn;
    if(this->hE<0) this->E0=Emx;
    for(double z=z0;z>=this->zmin&&z<=this->zmax; z+=this->hz)
    {
        if (getBreakStatus(0)) return;
        model->set_Uxz(z);
        this->zz=z;
        this->compute_TE();
    }
}
void MainWindow::TzE()
{
    double EE=this->E0;
    double zmn=this->zmin;
    double zmx=this->zmax;
    if(this->hz>0) this->zz=zmn;
    if(this->hz<0) this->zz=zmx;
    if(EE>this->Emax&&this->hE>0) EE=this->Emin;
    if(EE<this->Emin&&this->hE<0) EE=this->Emax;
    for(double E=EE;E>=this->Emin&&E<=this->Emax; E+=this->hE)
    {
        if (getBreakStatus(0)) return;
        this->E0=E;
        this->compute_Tz();
    }
}

void MainWindow::compute_BE()
{
    this->wPlotUx->show();
    this->wPlotUx->raise();
    std::vector<double> data;
    std::vector<double> data1;

    QPair<double,double> umin_umax = model->getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;

    double Emn = Umin;
    double Emx = Umax;
    double y;
//    double Emn=this->Emin;
//    double Emx=this->Emax;
    if(this->E0>=Emx&&this->hE>0) this->E0=Emn;
    if(this->E0<=Emn&&this->hE<0) this->E0=Emx;
    for(double E=Emn; E<=Emx; E+=this->hE)
    {
        if(getBreakStatus(0))   return;
        this->E0=E;
        compute();
        y = model->funEn;
//        y = real(model->b[model->getN()+1]);
        if(y>1.) y =1+log(y);
        if(y<-1.) y =-1-log(-y);
        data.push_back(y+0.5*(this->xmax-this->xmin));
        data.push_back(E);
    }
    this->E0=this->E0+this->hE;
    this->plotterUx->setCurveData(this->numOfCurveUx,data);
    this->numOfCurveUx++;
    data1.push_back(0.5*(this->xmax-this->xmin));
    data1.push_back(Emn);
    data1.push_back(0.5*(this->xmax-this->xmin));
    data1.push_back(Emx);
    this->plotterUx->setCurveData(this->numOfCurveUx,data1);
    this->numOfCurveUx++;

}
*/
/*
static void addUx(double xmin,double xmax,PhysicalModel *model, Plotter *plotter)
{
    const int N = model->getN();

    std::vector<double> dataUx;
    double x;
    dataUx.push_back(xmin);
    dataUx.push_back(model->get_U(0));
    x=0;
    dataUx.push_back(0);
    dataUx.push_back(model->get_U(0));
    for(int n=0; n <= N; n++)
//    for(int n=1; n <= N; n++)
    {
        double y=model->get_U(n);
        dataUx.push_back(x);
        dataUx.push_back(y);
        x += model->get_d(n);
        dataUx.push_back(x);
        dataUx.push_back(y);
    }
    dataUx.push_back(x);
    dataUx.push_back(model->get_U(N+1));
    if(xmax>x)
    {
    dataUx.push_back(xmax);
    dataUx.push_back(model->get_U(N+1));
    }
    plotter->setCurveData(0,dataUx);
}

void MainWindow::compute_En()
{
    this->plotterPsi2x->clearAll();
    plotterPsi2x->scaleFixed=false;
    this->plotterUx->clearAll();
    numOfCurveUx=0;
    this->showU();
    this->numOfCurveUx=1;
    QVector<double> Ebound = model->getEn();
    for (int i=0; i < Ebound.size(); i++)
    {
        double En = Ebound[i];
        this->E0 = En;
        compute();
        this->showEn(En);
    }
    compute_Psin();
}
typedef  std::vector<double> mycurve;
typedef  QVector<mycurve> curveSet;

static void updateCurves(curveSet& cs, const QVector<double>& Ebound, double z)
{
    for (int i=0; i < Ebound.size(); i++)
    {
        if (i >= cs.size())
        {
            mycurve a;
            cs.push_back(a);
        }
        cs[i].push_back(z);
        cs[i].push_back(Ebound[i]);
    }
}

static void myrepaint(Plotter *plotter, const curveSet& cs)
{
    for(int i=0; i<cs.size(); i++)
    {
        plotter->setCurveData(i,cs[i]);
    }
}
void MainWindow::compute_Uxz()
{
    if(this->flgTypeU->isChecked())
        model->setPotentialType(PERIODIC);
    else
        model->setPotentialType(FINITE);

    if(this->U_type->QComboBox::currentIndex()==0) showU();
    else
    for(double z=this->zmin;z<=this->zmax; z+=this->hz)
    {
        model->set_Uxz(z);
        showU();
    }

}

void MainWindow::compute_Enz()
{
    wPlotEnz->show();
    wPlotEnz->raise();
    plotterEnz->scaleFixed=false;
    plotterEnz->clearAll();
        curveSet cs;
        int nmxold=-1;
        for(double z=this->zmin;z<=this->zmax; z+=this->hz)
        {
            if (getBreakStatus(0))
                return;
            model->set_Uxz(z);
            int nmx=this->nmaxLevel;
            int nmn=this->nminLevel;
            QVector<double> Ebound = model->getEn();
            if(nmx >= Ebound.size()) nmx = Ebound.size()-1;
            if(nmn<0) nmn=0;
            if(nmxold==-1)
            {
                nmxold=nmx;
                plotterPsi2x->clearAll();
            }
            if(nmx<nmxold)
            {
                for(int id=nmx+2; id<=nmxold+2; id++)
                    this->plotterPsi2x->clearCurve(id);
            }

            updateCurves(cs, Ebound, z);
            myrepaint(plotterEnz,cs);
            compute_Psin();
            compute_Phi_n();
            nmxold=nmx;
        }
}
void MainWindow::compute_Psi_nz()
{
        int nmxold=-1;
        for(double z=this->zmin;z<=this->zmax; z+=this->hz)
        {
            if (getBreakStatus(0))
                return;
            model->set_Uxz(z);
            QVector<double> Ebound = model->getEn();
            int nmx=this->nmaxLevel;
            int nmn=this->nminLevel;
            if(nmx>=Ebound.size()) nmx=Ebound.size()-1;
            if(nmn<0) nmn=0;
            if(nmxold==-1)
            {
                nmxold=nmx;
                plotterPsi2x->clearAll();
            }
            if(nmx<nmxold)
            {
                for(int id=nmx+2; id<=nmxold+2; id++)
                    this->plotterPsi2x->clearCurve(id);
            }

            compute_Psin();
            nmxold=nmx;
        }
}
void MainWindow::compute_Psi_z()
{
    for(double z=this->zmin;z<=this->zmax; z+=this->hz)
    {
        if (getBreakStatus(0))
            return;
        model->set_Uxz(z);
        compute_PsiXatE();
    }
}

void MainWindow::compute_Phi()
{

    int ii=psip_var->QComboBox::currentIndex();
        switch(ii)
        {
        case 0:
            this->compute_Phi_n();
            break;
        case 1:
            this->WavePacketPoft();
            break;
        default:
            break;
        }
}

void MainWindow::compute_Phi_n()
{
    wPlotPhi->show();
    wPlotPhi->raise();
    std::vector<double> data;
    double y;
    double kmin=-this->kmax;
    plotterPhi->scaleFixed=false;
    if(this->flgErase->isChecked())
    {
        this->numOfCurve=0;
    }
    else {
        this->numOfCurve++;
    }
    if(kmax<=0) kmax=10.;
    double dk=kmax/500;
    int imax=this->nmaxLevel;
    int imin=this->nminLevel;
    QVector<double> Ebound = model->getEn();
    data.clear();
    if(imax >= Ebound.size()) imax = Ebound.size()-1;
    if(imin<0) imin=0;
    for(int i=imin;i<=imax; i++)
    {
        this->E0 = Ebound[i];
        compute();
        data.clear();
        for(double kk=kmin; kk<=kmax; kk+=dk)
        {
            model->kwave = kk;
            model->build_Phi();
            y=model->Phi2;
            data.push_back(kk);
            data.push_back(y);
        }
        this->plotterPhi->setCurveData(this->numOfCurve,data);
        this->numOfCurve++;
    }
}
void MainWindow::showU()
{
    const int N = model->getN();
    wPlotUx->show();
    wPlotUx->raise();
    wPlotUx->activateWindow();
    std::vector<double> data;
    data.clear();
    double x,y;
    x=0;
    numOfCurveUx=0;
    //    model->XUscales();
    PlotSettings ps;
    ps.minX = this->xmin;//model->Xmin;
    ps.maxX = this->xmax;//model->Xmax;
    ps.minY = this->Umin;//model->Umin;
    ps.maxY = this->Umax;//model->Umax;
    this->plotterUx->setPlotSettings(ps);
    x=this->xmin;//model->Xmin;
    y=model->get_U(0);
    data.push_back(x);
    data.push_back(y);
    x=0;
    data.push_back(x);
    data.push_back(y);
    for(int n=0; n <= N; n++)
    {
        double y=model->get_U(n);
        data.push_back(x);
        data.push_back(y);
        x += model->get_d(n);
        data.push_back(x);
        data.push_back(y);
    }
    data.push_back(x);
    data.push_back(model->get_U(N+1));
    if(this->xmax<x) this->xmax=x;
    else x =this->xmax;
    data.push_back(x);
    data.push_back(model->get_U(N+1));
    this->plotterUx->setCurveData(numOfCurveUx,data);
    this->numOfCurveUx++;
}
void MainWindow::showEn(double E)
{
        std::vector<double> data;
        data.push_back(this->xmin);
        data.push_back(E);
        data.push_back(this->xmax);
        data.push_back(E);
        this->plotterUx->setCurveData(this->numOfCurveUx,data);
        this->numOfCurveUx++;
}
void MainWindow::compute_Psi()
{

    int ii=psix_var->QComboBox::currentIndex();
    //model->build_U();
        switch(ii)
        {
        case 0:
            this->compute_Psin();
            break;
        case 1:
            this->compute_PsixE();
            break;
        case 2:
            this->compute_PsiXatE();
            break;
        case 3:
            this->compute_Psi_z();
            break;
        case 4:
            this->compute_Psi_nz();
            break;
        case 5:
            this->WavePacketXoft();
            break;
        default:
            break;
        }
}
void MainWindow::setScalesPsi()
{
    PlotSettings ps,psUx;
    bool flg=flgScale->isChecked();
    plotterPsi2x->scaleFixed=flg;
    if(flg)
    {
        ps.minX = this->xmin;
        ps.maxX = this->xmax;
        psUx.maxX = this->xmax;
        psUx.minX = this->xmin;
        ps.minY = psixmin;
        ps.maxY = psixmax;
        psUx.minY = this->Umin;
        psUx.maxY = this->Umax;
        ps.numYTicks = 5;
        ps.numXTicks = 5;
        plotterAddUx->setPlotSettings(psUx);
        plotterPsi2x->setPlotSettings(ps);
    }
    addUx(this->xmin, this->xmax, model, plotterAddUx);

}
void MainWindow::compute_Psin()
{
    const int N = model->getN();
    wPlotPsi2->show();
    wPlotPsi2->raise();
    std::vector<double> data;
    setScalesPsi();
    double dx=this->hx;
    int ii=psi_type->QComboBox::currentIndex();
    int numCurve=2;
    int nmx=this->nmaxLevel;
    int nmn=this->nminLevel;
    QVector<double> Ebound = model->getEn();
    if(nmx >= Ebound.size()) nmx = Ebound.size()-1;
    if(nmn<0) nmn=0;
    double y;
    for(int n=nmn;n<=nmx; n++)
    {
        double E = Ebound[n];
        this->E0=E;
        compute();
        data.clear();
        for(double x=this->xmin; x<=this->xmax; x+=dx)
        {
            model->x = x;
            model->build_Psi();
            switch(ii)
            {
            case 0:
                y=model->Psi2;
                break;
            case 1:
                y=model->psi_real;
                break;
            case 2:
                y=model->psi_imag;
                break;
            default:
                break;
            }
            data.push_back(x);
            data.push_back(y);
        }
        this->plotterPsi2x->setCurveData(numCurve,data);
        numCurve++;
    }
}
void MainWindow::compute_PsixE()
{
    wPlotPsi2->show();
    wPlotPsi2->raise();
    double y;
    if(this->flgErase->isChecked())
    {
        plotterPsi2x->clearAll();
        this->numOfCurve=1;
    }
    else this->numOfCurve++;
    int ii=psi_type->QComboBox::currentIndex();
    std::vector<double> data;
    setScalesPsi();
    double dx=this->hx;//(this->xmax-model->Xmin)/2000.;
    double Emn=this->Emin;
    double Emx=this->Emax;
    if(this->E0>=Emx&&this->hE>0) this->E0=Emn;
    if(this->E0<=Emn&&this->hE<0) this->E0=Emx;
    for(double E=this->E0; E>=this->Emin&&E<=this->Emax; E+=this->hE)
    {
        this->E0=E;
        if(getBreakStatus(0))
            return;
        compute();
        data.clear();
        for(double x=this->xmin; x<=this->xmax; x+=dx)
        {
            model->x=x;
            model->build_Psi();
            switch(ii)
            {
            case 0:
                y=model->Psi2;
                break;
            case 1:
                y=model->psi_real;
                break;
            case 2:
                y=model->psi_imag;
                break;
            default:
                break;
            }
            data.push_back(x);
            data.push_back(y);
        }
        this->plotterPsi2x->setCurveData(this->numOfCurve,data);
    }
    this->numOfCurve++;
    this->E0=this->E0+this->hE;
}
void MainWindow::compute_PsiXatE()
{
    wPlotPsi2->show();
    wPlotPsi2->raise();
    std::vector<double> data;
    std::vector<double> dataUx;
    if(this->flgErase->isChecked())
    {
        plotterPsi2x->clearAll();
        this->numOfCurve=1;
    }
    else this->numOfCurve++;
    setScalesPsi();
    double y;
    double dx=this->hx;//(this->xmax-this->xmin)/300.;
    int ii=psi_type->QComboBox::currentIndex();
    compute();
    for(double x=this->xmin; x<=this->xmax; x+=dx)
    {
        model->x=x;
        model->build_Psi();
        switch(ii)
        {
        case 0:
            y=model->Psi2;
            break;
        case 1:
            y=model->psi_real;
            break;
        case 2:
            y=model->psi_imag;
            break;
        default:
            break;
        }
        data.push_back(x);
        data.push_back(y);
    }
    this->plotterPsi2x->setCurveData(2,data);
}
void MainWindow::compute_PhiofP()
{

    wPlotPhi->show();
    wPlotPhi->raise();
    std::vector<double> data;
    plotterPhi->scaleFixed=false;
    double y;
    double kmin=-this->kmax;
    if(this->kmax<=0) this->kmax=10.;
    double dk=kmax/500;
    if(dk>this->hk) dk=this->hk;
    for(double kk=kmin; kk<=kmax; kk+=dk)
    {
        model->kwave=kk;
        model->build_Phi();
        y=model->Phi2;
        data.push_back(kk);
        data.push_back(y);
    }
    this->plotterPhi->setCurveData(this->numOfCurve,data);
}

void MainWindow::initPlotNofE()
{
    winPlotNE = new QDialog(this);
    QVBoxLayout *vl=new QVBoxLayout(winPlotNE);
    PlotterDialog *wNE = new PlotterDialog(winPlotNE);
    this->plotNE = ((PlotterDialog*)wNE)->plotter();
    winPlotNE->setWindowTitle(tr("Number of levels on energy E"));
    vl->addWidget(wNE);
    winPlotNE->setFont(QFont("Serif", 12, QFont::SemiCondensed ));
    QHBoxLayout *hl = new QHBoxLayout();
    QPushButton *butNE = new QPushButton(tr("RUN N(E)"));
    connect(butNE, SIGNAL(clicked()), this, SLOT(compute_NE()));
    hl->addWidget(butNE);
    hl->addStretch();
    vl->addLayout(hl);
}
void MainWindow::initPlotT()
{
    wPlotT = new QDialog(this);
    QVBoxLayout *vl = new QVBoxLayout(wPlotT);
    PlotterDialog *wT = new PlotterDialog(wPlotT);
    wPlotT->setFont(QFont("Serif", 12, QFont::SemiCondensed ));
    this->plotterT = ((PlotterDialog*)wT)->plotter();
    wPlotT->setWindowTitle(tr("Transmission"));
    vl->addWidget(wT);
    {
        QHBoxLayout *hl = new QHBoxLayout();
        bRunD = new QPushButton(tr("RUN D"));
        connect(bRunD, SIGNAL(clicked()), this, SLOT(slotCompute_D()));
        hl->addWidget(bRunD);

        T_type = new QComboBox(this);
        T_type->addItem(tr("(E)"));
        T_type->addItem(tr("(z)"));
        T_type->addItem(tr("(E,z)"));
        T_type->addItem(tr("(z,E)"));
        T_type->setCurrentIndex(0);
        hl->addWidget(T_type);

        hl->addStretch();

        QPushButton * bsE = new QPushButton("&E scale");
        connect(bsE,SIGNAL(clicked()),this,SLOT(slotIntE()));
        hl->addWidget(bsE);

        QPushButton * bsz = new QPushButton("&z scale");
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);

        flgEraseT= new QCheckBox("Erase",wPlotT);
        flgEraseT->setChecked(true);
        hl->addWidget(flgEraseT);

        hl->addStretch();

        vl->addLayout(hl);

    }


}
void MainWindow::slotCompute_D()
{
    bRunD->setText("STOP D");
    disconnect(bRunD, SIGNAL(clicked()), this, SLOT(slotCompute_D()));
    breakStatus.onButton(bRunD);
    compute_D();
    breakStatus.disconnect();
    bRunD->setText("RUN D");
    connect(bRunD, SIGNAL(clicked()), this, SLOT(slotCompute_D()));
}

void MainWindow::initPlotEnz()
{
    wPlotEnz = new QDialog(this);
    QVBoxLayout *vl=new QVBoxLayout(wPlotEnz);
    PlotterDialog *wEnz = new PlotterDialog(wPlotEnz);
    wPlotEnz->setFont(QFont("Serif", 12, QFont::SemiCondensed ));
    wPlotEnz->setWindowTitle(tr("Energy spectrum deformation on z"));
    this->plotterEnz=((PlotterDialog*)wEnz)->plotter();
    vl->addWidget(wEnz);

    QHBoxLayout *hl = new QHBoxLayout();
    QPushButton *bRun = new QPushButton(tr("RUN En(z)"));
    connect(bRun, SIGNAL(clicked()), this, SLOT(compute_Enz()));
    hl->addWidget(bRun);

    QPushButton * bsz = new QPushButton("&z scale");
    connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
    hl->addWidget(bsz);
    hl->addStretch();

    vl->addLayout(hl);

}

void MainWindow::initPlotPsix()
{
    wPlotPsi2 = new QDialog(this);
    wPlotPsi2->setFont(QFont("Serif", 12, QFont::SemiCondensed ));
    wPlotPsi2->setWindowTitle(tr("Coordinate distribution"));
    QVBoxLayout *vl=new QVBoxLayout(wPlotPsi2);

//    PlotterDialog *wUx = new PlotterDialog(wPlotPsi2);
//    this->plotterAddUx = ((PlotterDialog*)wUx)->plotter();
//    vl->addWidget(wUx);
    plotterAddUx= new Plotter(wPlotPsi2);
    vl->addWidget(plotterAddUx);

    PlotterDialog *wPsi = new PlotterDialog(wPlotPsi2);
    this->plotterPsi2x = ((PlotterDialog*)wPsi)->plotter();
    vl->addWidget(wPsi);
    {
        QHBoxLayout *hl = new QHBoxLayout();

        QPushButton * bsx = new QPushButton("&x scale");
        connect(bsx,SIGNAL(clicked()),this,SLOT(slotScaleX()));
        hl->addWidget(bsx);


        QPushButton * bsy = new QPushButton("&y scales");
        connect(bsy,SIGNAL(clicked()),this,SLOT(slotScalePsi()));
        hl->addWidget(bsy);
        hl->addStretch();

        flgScale= new QCheckBox("Fixed y-scales",wPlotUx);
        flgScale->setChecked(true);
        hl->addWidget(flgScale);


        flgErase= new QCheckBox("Erase",wPlotPsi2);
        flgErase->setChecked(true);
        hl->addWidget(flgErase);
        hl->addStretch();

        vl->addLayout(hl);
    }
    {
        {
            //QString sum=QChar(0x03A3);
            QString sum=QChar(0x2211);

            QString psi=QChar(0x03C8);
               QString Psi=QChar(0x03A8);
            QString to2=QChar(0x00B2);
            QString psiofx=psi+"(x)";
            QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
//            QGroupBox *gb3 = new QGroupBox(tr("Wave function ")+psiofx);

            QHBoxLayout *hl = new QHBoxLayout;
            bRunPsi = new QPushButton(tr("RUN"));
            connect(bRunPsi, SIGNAL(clicked()), this, SLOT(slotCompute_Psi()));
            hl->addWidget(bRunPsi);

            psi_type = new QComboBox(this);
            psi_type->addItem(mod_psiofx);
            psi_type->addItem("real "+psi);
            psi_type->addItem("imag "+psi);
            psi_type->setCurrentIndex(0);
            hl->addWidget(psi_type);

            psix_var = new QComboBox(this);
            psix_var->addItem("n");
            psix_var->addItem("E");
            psix_var->addItem("--");
            psix_var->addItem("z");
            psix_var->addItem("n,z");
            psix_var->addItem("t");
            psix_var->setCurrentIndex(0);
            hl->addWidget(psix_var);

            hl->addStretch();

        QPushButton * btn = new QPushButton("&n");
        connect(btn,SIGNAL(clicked()),this,SLOT(slotIntN()));
        hl->addWidget(btn);

        QPushButton * bsE = new QPushButton("&E");
        connect(bsE,SIGNAL(clicked()),this,SLOT(slotIntE()));
        hl->addWidget(bsE);

        QPushButton * bsz = new QPushButton("&z");
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);

        QPushButton * btwp = new QPushButton("&t");
        connect(btwp,SIGNAL(clicked()),this,SLOT(slotWPacket()));
        hl->addWidget(btwp);
            vl->addLayout(hl);

        }

    }
}
void MainWindow::slotCompute_Psi()
{
    bRunPsi->setText("STOP");
    disconnect(bRunPsi, SIGNAL(clicked()), this, SLOT(slotCompute_Psi()));
    breakStatus.onButton(bRunPsi);
    compute_Psi();
    breakStatus.disconnect();
    bRunPsi->setText("RUN");
    connect(bRunPsi, SIGNAL(clicked()), this, SLOT(slotCompute_Psi()));
}

void MainWindow::initPlotPhik()
{
    wPlotPhi = new QDialog(this);
    QVBoxLayout *vl=new QVBoxLayout(wPlotPhi);
    PlotterDialog *wPhi = new PlotterDialog(wPlotPhi);
    wPlotPhi->setFont(QFont("Serif", 12, QFont::SemiCondensed ));
    this->plotterPhi = ((PlotterDialog*)wPhi)->plotter();
    wPlotPhi->setWindowTitle(tr("Momentum distribution"));
    vl->addWidget(wPhi);
    {
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton * bsk = new QPushButton("&k scale");
        connect(bsk,SIGNAL(clicked()),this,SLOT(slotScaleP()));
        hl->addWidget(bsk);

        hl->addStretch();

        flgErase= new QCheckBox("Erase",wPlotPhi);
        flgErase->setChecked(true);
        hl->addWidget(flgErase);

        hl->addStretch();
        bRunPhi = new QPushButton("RUN |Phi(k)|^2");
        connect(bRunPhi, SIGNAL(clicked()), this, SLOT(slotCompute_Phi()));
        hl->addWidget(bRunPhi);

        psip_var = new QComboBox(this);
        psip_var->addItem(tr("n"));
        psip_var->addItem(tr("t"));
        psip_var->setCurrentIndex(0);
        hl->addWidget(psip_var);

        QPushButton * btn = new QPushButton("&n");
        connect(btn,SIGNAL(clicked()),this,SLOT(slotIntN()));
        hl->addWidget(btn);

        QPushButton * btwp = new QPushButton("&t");
        connect(btwp,SIGNAL(clicked()),this,SLOT(slotWPacket()));
        hl->addWidget(btwp);


        vl->addLayout(hl);
    }

}
void MainWindow::slotCompute_Phi()
{
    bRunPhi->setText("STOP |Phi(k)|^2");
    disconnect(bRunPhi, SIGNAL(clicked()), this, SLOT(slotCompute_Phi()));
    breakStatus.onButton(bRunPhi);
    compute_Phi();
    breakStatus.disconnect();
    bRunPhi->setText("RUN |Phi(k)|^2");
    connect(bRunPhi, SIGNAL(clicked()), this, SLOT(slotCompute_Phi()));
}
void MainWindow::initPlotUx()
{
    wPlotUx = new QDialog(this);
    QVBoxLayout *vl = new QVBoxLayout(wPlotUx);
    PlotterDialog *wUx = new PlotterDialog(wPlotUx);
    wPlotUx->setFont(QFont("Serif", 12, QFont::SemiCondensed ));
    this->plotterUx = ((PlotterDialog*)wUx)->plotter();
    wPlotUx->setWindowTitle(tr("Stepwise U(x)"));
    vl->addWidget(wUx);
    {
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton * bsx = new QPushButton("&x scale");
        connect(bsx,SIGNAL(clicked()),this,SLOT(slotScaleX()));
        hl->addWidget(bsx);

        QPushButton * bsy = new QPushButton("&y scales");
        connect(bsy,SIGNAL(clicked()),this,SLOT(slotScalePsi()));
        hl->addWidget(bsy);

        flgScale= new QCheckBox("Fixed y-scales",wPlotUx);
        flgScale->setChecked(true);
        hl->addWidget(flgScale);

        flgErase= new QCheckBox("Erase",wPlotUx);
        flgErase->setChecked(true);
        hl->addWidget(flgErase);
        hl->addStretch();
        vl->addLayout(hl);

    }

    {
        QHBoxLayout *hl = new QHBoxLayout;
        butEn = new QPushButton(tr("RUN"));
        connect(butEn, SIGNAL(clicked()), this, SLOT(slotBound_States()));
        hl->addWidget(butEn);

        En_type = new QComboBox(this);
        En_type->addItem(tr("E_n & Psi_n"));
        En_type->addItem(tr("N(E)-step"));
        En_type->addItem(tr("B_{N+1}(E)"));
        En_type->addItem(tr("En(z)"));
        En_type->setCurrentIndex(0);
        hl->addWidget(En_type);

        hl->addStretch();

        QPushButton *bRun = new QPushButton(tr("Show U(x)"));
        connect(bRun, SIGNAL(clicked()), this, SLOT(compute_Uxz()));
        hl->addWidget(bRun);

        U_type = new QComboBox(wPlotUx);
        U_type->addItem(tr("--"));
        U_type->addItem(tr("z"));
        U_type->setCurrentIndex(0);
        hl->addWidget(U_type);

        QPushButton * bsz = new QPushButton("&z");
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);
        hl->addStretch();

        vl->addLayout(hl);

    }
}
void MainWindow::slotBound_States()
{
    butEn->setText("STOP");
    disconnect(butEn, SIGNAL(clicked()), this, SLOT(slotBound_States()));
    breakStatus.onButton(butEn);
    Bound_States();
    breakStatus.disconnect();
    butEn->setText("RUN");
    connect(butEn, SIGNAL(clicked()), this, SLOT(slotBound_States()));

}
*/
static void setSomeInitialU(PhysicalModel *m)
{
    UAsMW u = { 1, 1, 1, -20, 0 };
    m->setUAsMW( u );
}

/*char model1[] =
"N=1\n"
"U=0,-1,0\n"
"xmin=-3\n"
;
*/
MainWindow::MainWindow(QWidget *parent, Qt::WFlags f)
: Nwb(1),nmaxLevel(10),nminLevel(0),nLevel(0), nmaxWP(7), nminWP(0),hnWP(1),
E0(-10.), hE(0.01),time(0.), tmin(0.), tmax(1000.0), htime(0.1),Psi2(0.),
totalRT(1.),Ua(-10.0),aa(1.),Ub(0.),bb(0.8),xmin(-3.),xmax(8.),hx(0.01),
//Ubias(0.),
Emin(-10.),Emax(-0.01),Umin(-15.0),Umax(1.),kmax(10.),hk(0.005),
psixmin(-1.2),psixmax(1.5),zmin(0),zmax(1.), hz(0.01), zz(0.),
QMainWindow(parent,f), countW(0), numOfCurve(1),numOfCurveNE(1), numOfCurveT(0), model(0),
wpE_lo(5.), wpE_hi(15.), wpN(30),
tableView(0),boundCondView(0),gbScales(0),gbIntervals(0),dialogSetting(0),
dialogTime(0),dialogZ(0), dialogUAsMW(0),dialogUparab(0),dialogUch2x(0),dialogUlinear(0), dialogWPEm(0),dialogWPEp(0),tableViewEn(0),tableViewEquasi(0), gbScaleX(0),
gbScaleZ(0),gbScaleP(0), gbScalePsi(0),
gbIntN(0),gbIntE(0),  gbWP(0),gbWPr(0),gbWPl(0),bgR(0), bRunPsiXT(0),
teWidget(0),qeWidget(0),tzWidget(0),enzWidget(0),egWidget(0),
wavePacketKWidget(0),momentumDistibutionWidget(0),
uxWidget(0),waveFunctionWidget(0),wavePacketXWidget(0)
{
    this->model = new PhysicalModel();
    UAsMW u2 = { 1, 0.1, 1, -10, 0 };
    model->setUAsMW( u2 );
    model->slotU2();
    UAsMW u1 = { 1, 10, 1, -10, 0 };
    model->setUAsMW( u1 );
    model->slotU1();
//    setSomeInitialU(this->model);
    UAsMW u = { 1, 1, 1, -15, 0 };
    this->model->setUAsMW( u );
    this->initStatusBar();
/*    this->initPlotEnz();
    this->initPlotUx();
    this->initPlotT();
    this->initPlotPsix();
    this->initPlotPhik();
    this->initPlotNofE();*/
    this->initControlDockWindow();
    model->set_Energy(0.1);
    this->initMenuBar();
//    init();
}

bool MainWindow::save()
{
    if (curFile.isEmpty())
    {
        return saveAs();
    }

    QFile f(curFile);
    QString s;
    if (! f.open(QFile::WriteOnly | QFile::Text))
        return false;
    else
    {
        QXmlStreamWriter writer(&f);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement("kvant");
        this->writeToXml(&writer);
        if(model) model->writeToXml(&writer);
        if(waveFunctionWidget) waveFunctionWidget->writeToXml(&writer);
        if(enzWidget) enzWidget->writeToXml(&writer);
        if(egWidget) egWidget->writeToXml(&writer);
        if(teWidget) teWidget->writeToXml(&writer);
        if(tzWidget) tzWidget->writeToXml(&writer);
        if(qeWidget) qeWidget->writeToXml(&writer);
        writer.writeEndElement();
        writer.writeEndDocument();
    }
    f.close();
    statusBar()->showMessage(tr("Saved %1").arg(curFile), 2000);
    return true;
}
//------
void MainWindow::readFromXml(QXmlStreamReader *r)
{
    if(!uxWidget) uxWidget = new PotentialMovableWidget(model);
    if(!waveFunctionWidget) waveFunctionWidget = new WaveFunctionWidget(model);
    if(!wavePacketKWidget) wavePacketKWidget = new WavePacketKWidget(model);
    if(!wavePacketXWidget) wavePacketXWidget = new WavePacketXWidget(model);
    if(!momentumDistibutionWidget) momentumDistibutionWidget = new MomentumDistibutionWidget(model);
    if(!teWidget) teWidget = new TEWidget(model);
    if(!tzWidget) tzWidget = new TZWidget(model);
    if(!enzWidget) enzWidget = new EnzWidget(model);
    if(!egWidget)  egWidget = new EGWidget(model);
    if(!qeWidget)  qeWidget = new QEWidget(model);
    Q_ASSERT(this);
    Q_ASSERT(r->isStartElement() && r->name() == "mainWindow");
    int _visible = 0;
    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
/*        if (r->name() == "MainWindowgeometry")
        {
            QString s = r->readElementText();
            int l,t,w,h;
            sscanf_s(s.toAscii(),"%i %i %i %i",&l,&t,&w,&h);
            this->setGeometry(QRect(l,t,w,h));
        }
        else if (r->name() == "MainWindowpos")
        {
            QString s = r->readElementText();
            int ix,iy;
            sscanf_s(s.toAscii(),"%i %i",&ix,&iy);
            this->move(QPoint(ix,iy));
        }
        else if (r->name() == "TEgeometry")
        {
            QString s = r->readElementText();
            int l,t,w,h;
            sscanf_s(s.toAscii(),"%i %i %i %i",&l,&t,&w,&h);
            teWidget->setGeometry(QRect(l,t,w,h));
        }
        else if (r->name() == "TEpos")
        {
            QString s = r->readElementText();
            int ix,iy;
            sscanf_s(s.toAscii(),"%i %i",&ix,&iy);
            teWidget->move(QPoint(ix,iy));
        }*/
        if (r->name() == "UXisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_Ux();
            }
            else
            {
                this->uxWidget->hide();
            }
        }
        else if (r->name() == "PsiXisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_psi_x();
            }
            else
            {
                this->waveFunctionWidget->hide();
            }
        }
        else if (r->name() == "PsiXTisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_psi_xt();
            }
            else
            {
                this->wavePacketXWidget->hide();
            }
        }
        else if (r->name() == "PhiKisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_phi_k();
            }
            else
            {
                this->momentumDistibutionWidget->hide();
            }
        }
        else if (r->name() == "PhiKTisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_phi_kt();
            }
            else
            {
                this->wavePacketKWidget->hide();
            }
        }
        else if (r->name() == "TEisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_TE();
            }
            else
            {
                this->teWidget->hide();
            }
        }
        else if (r->name() == "TZisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_TZ();
            }
            else
            {
                this->tzWidget->hide();
            }
        }
        else if (r->name() == "EnzisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_Enz();
            }
            else
            {
                this->enzWidget->hide();
            }
        }
        else if (r->name() == "QEisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_QE();
            }
            else
            {
                this->qeWidget->hide();
            }
        }
        else if (r->name() == "EGisVisible")
        {
            _visible = r->readElementText().toInt();
            if (_visible)
            {
                window_EG();
            }
            else
            {
                this->egWidget->hide();
            }
        }
        else
            skipUnknownElement(r);
    }
}


void MainWindow::writeToXml(QXmlStreamWriter *w)
{
    if(!uxWidget) uxWidget = new PotentialMovableWidget(model);
    if(!waveFunctionWidget) waveFunctionWidget = new WaveFunctionWidget(model);
    if(!wavePacketKWidget) wavePacketKWidget = new WavePacketKWidget(model);
    if(!wavePacketXWidget) wavePacketXWidget = new WavePacketXWidget(model);
    if(!momentumDistibutionWidget) momentumDistibutionWidget = new MomentumDistibutionWidget(model);
    if(!teWidget) teWidget = new TEWidget(model);
    if(!tzWidget) tzWidget = new TZWidget(model);
    if(!enzWidget) enzWidget = new EnzWidget(model);
    if(!egWidget)  egWidget = new EGWidget(model);
    if(!qeWidget)  qeWidget = new QEWidget(model);
    w->writeStartElement("mainWindow");
    {
        QString s;
        QRect g;
/*        QPoint psn;
        psn= this->pos();
        s.sprintf("%i %i",psn.x(),psn.y());
        w->writeTextElement("MainWindowpos",s);

        g = this->geometry();
        s.sprintf("%i %i %i %i",g.left(),g.top(),g.width(),g.height());
        w->writeTextElement("MainWindowgeometry",s);

        psn= this->teWidget->pos();
        s.sprintf("%i %i",psn.x(),psn.y());
        w->writeTextElement("TEpos",s);
        g = this->teWidget->geometry();
        s.sprintf("%i %i %i %i",g.left(),g.top(),g.width(),g.height());
        w->writeTextElement("TEgeometry",s);*/
        s.sprintf("%i",this->uxWidget->isVisible() ? 1 : 0);
        w->writeTextElement("UXisVisible",s);

        s.sprintf("%i",this->waveFunctionWidget->isVisible() ? 1 : 0);
        w->writeTextElement("PsiXisVisible",s);

        s.sprintf("%i",this->momentumDistibutionWidget->isVisible() ? 1 : 0);
        w->writeTextElement("PhiKisVisible",s);

        s.sprintf("%i",this->wavePacketKWidget->isVisible() ? 1 : 0);
        w->writeTextElement("PhiKTisVisible",s);

        s.sprintf("%i",this->teWidget->isVisible() ? 1 : 0);
        w->writeTextElement("TEisVisible",s);

        s.sprintf("%i",this->tzWidget->isVisible() ? 1 : 0);
        w->writeTextElement("TZisVisible",s);

        s.sprintf("%i",this->enzWidget->isVisible() ? 1 : 0);
        w->writeTextElement("EnzisVisible",s);

        s.sprintf("%i",this->qeWidget->isVisible() ? 1 : 0);
        w->writeTextElement("QEisVisible",s);

        s.sprintf("%i",this->egWidget->isVisible() ? 1 : 0);
        w->writeTextElement("EGisVisible",s);
    }
    w->writeEndElement();
}

//------
bool MainWindow::openFile()
{
    QString fileName;
//    int _visibleTE;
    fileName = QFileDialog::getOpenFileName(this, "Choose a data file","", "*.xml");
    if (fileName.isEmpty())
        return false;
    else
    {
        //--------------------------------------
        QFile f(fileName);//curFile);

        if (!f.open(QFile::ReadOnly | QFile::Text))
            return false;

        QXmlStreamReader reader(&f);
        while (!reader.atEnd())
        {
            reader.readNext();
            if (! reader.isStartElement()) continue;
            if (reader.name() == "kvant")
            {
                while (!reader.atEnd())
                {
                    reader.readNext();
                    if (reader.name() == "mainWindow")
                    {
                        this->readFromXml(&reader);
                    }
                    else if (reader.name() == "model")
                    {
                        model->readFromXml(&reader);
                    }
/*                    else if (reader->name() == "TEisVisible")
                    {
                        _visibleTE = this->readElementText().toInt();
                        if (_visibleTE) teWidget->show();
                        else teWidget->hide();
                        teWidget->readFromXml(&reader);
                    }
                    else if (reader->name() == "TEgeometry")
                    {
                        QString s = reader->readElementText();
                        int l,t,w,h;
                        sscanf_s(s.toAscii(),"%i %i %i %i",&l,&t,&w,&h);
                        teWidget->setGeometry(QRect(l,t,w,h));
                        if (_visibleTE) teWidget->show();
                        else teWidget->hide();
                    }
                         if(!teWidget)
                         {
                                 teWidget = new TEWidget(model);

                         }*/
                    else if (reader.name() == "Psix")
                    {
                         waveFunctionWidget->readFromXml(&reader);
                    }
                    else if (reader.name() == "TE")
                    {
                         teWidget->readFromXml(&reader);
                    }
                    else if (reader.name() == "TZ")
                    {
                         tzWidget->readFromXml(&reader);
                    }
                    else if (reader.name() == "Enz")
                    {
                        enzWidget->readFromXml(&reader);
                    }
                    else if (reader.name() == "QE")
                    {
                        qeWidget->readFromXml(&reader);
                    }
                    else if (reader.name() == "EGplane")
                    {
                        egWidget->readFromXml(&reader);
                    }
                }
            }
        }
        if (reader.hasError())
        {
            QMessageBox::warning(0,
                tr("Warning"),
                reader.errorString(),
                "Ok",
                QString::null,
                0,
                1);
        }
        f.close();
        update();
//        statusBar()->showMessage(tr("Saved %1").arg(curFile), 20000);
        return true;
    }
}

void MainWindow::createStatusBar()
{

}



void MainWindow::help()
{
    }
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
     case Qt::Key_Escape:
         setBreakStatus(1);
         break;
     case Qt::Key_Pause :
         setBreakStatus(1);
         break;
     default:
         setBreakStatus(1); //?????
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit(closed());
    //event->accept();
    event->ignore();
}
