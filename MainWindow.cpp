#include "MainWindow.h"
#include <math.h>
#include "MyParam.h"
#include "PhysicalModel.h"
#include "PotentialScene.h"
#include "PotentialModel.h"
#include "LevelModel.h"
#include "PotentialViewMultiWell.h"
#include "LineEditDelegate.h"
#include "PotentialViewMovable.h"
#include "PlotterDialog.h"


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




void MainWindow::initMenuBar()
{
     QMenu *fileMenu = menuBar()->addMenu(tr("File"));
//     QMenu *fileMenu = new QMenu(tr("&File"), this);
//     fileMenu->addAction(tr("&Quit"), this, SLOT(close()));
     QAction *quitAction = fileMenu->addAction(tr("Quit"));
     quitAction->setShortcut(tr("Ctrl+Q"));
     quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
     connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

     QAction *openAction = fileMenu->addAction(tr("&Open..."));
     openAction->setShortcut(tr("Ctrl+O"));
     connect(openAction, SIGNAL(triggered()), this, SLOT(openFile())); //TODO: openFile(str)

     QAction *saveAction = fileMenu->addAction(tr("&Save As..."));
     saveAction->setShortcut(tr("Ctrl+S"));
     connect(saveAction, SIGNAL(activated()), this, SLOT(saveAs()));

     aboutAction = new QAction(tr("&О программе"),this);
     aboutAction->setStatusTip(tr("Сведения о программе"));
     connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
     fileMenu->addAction(aboutAction);  
     fileMenu->addSeparator();

     //     chooseFontAction = new QAction("Choose Font", this);
//     fileMenu->addAction(chooseFontAction);  
//     chooseFontAction->setShortcut(tr("Ctrl+F"));
//     connect(chooseFontAction, SIGNAL(activated()), this, SLOT(chooseFont()));
    

//     QMenu *helpMenu = menuBar()->addMenu("&Help");
//     helpMenu->addAction(aboutAction);
}
void MainWindow::openFile(const QString &path)
 {
     QString fileName;
     if (path.isNull())
         fileName = QFileDialog::getOpenFileName(this, "Choose a data file",
                                                 "", "*.cht");
     else
         fileName = path;

     if (!fileName.isEmpty()) {
         QFile file(fileName);

         if (file.open(QFile::ReadOnly | QFile::Text)) {
             QTextStream stream(&file);
             QString line;

/*             model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

             int row = 0;
             do {
                 line = stream.readLine();
                 if (!line.isEmpty()) {

                     model->insertRows(row, 1, QModelIndex());

                     QStringList pieces = line.split(",", QString::SkipEmptyParts);
                     model->setData(model->index(row, 0, QModelIndex()),
                                    pieces.value(0));
                     model->setData(model->index(row, 1, QModelIndex()),
                                    pieces.value(1));
                     model->setData(model->index(row, 0, QModelIndex()),
                                    QColor(pieces.value(2)), Qt::DecorationRole);
                     row++;
                 }
             } while (!line.isEmpty());

 */            file.close();
             statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
         }
     }
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
        tr("*.dat*"));
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

        LevelModel *lm = new LevelModel(tableViewEn);
        lm->setLevels(model);
        tableViewEn->setModel(lm);
    }

    tableViewEn->reset();
    tableViewEn->show();
    tableViewEn->activateWindow();
    tableViewEn->setFocus();
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
void  MainWindow::slotScaleP()
{   
        if (!gbScaleP) 
        {
            gbScaleP = new QGroupBox(this);
            gbScaleP->setWindowTitle("k-scales:");
            gbScaleP->setWindowFlags(Qt::Window);
            gbScaleP->setFont(QFont("Serif", 12, QFont::Bold )); 
            QVBoxLayout *vl = new QVBoxLayout;
            this->kmax.setDisplay(("kmax=-kmin:"), ("maximal value of wave number"),vl);
            this->hk.setDisplay(("hk:"), ("k-increment"),vl);
            gbScaleP->setLayout(vl);
        }
    gbScaleP->show(); 
    gbScaleP->raise();//activateWindow();
    gbScaleP->setFocus();
}
void  MainWindow::slotScaleX()
{   
    if (!gbScaleX) 
    {
        gbScaleX = new QGroupBox(this);
        gbScaleX->setWindowTitle("x-scales:");
        gbScaleX->setWindowFlags(Qt::Window);
        gbScaleX->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        this->xmin.setDisplay(("xmin"),("lower bond of x-interval"),vl);
        this->xmax.setDisplay(("xmax"),("upper bond of x-interval"),vl);
        this->hx.setDisplay(("hx"),("x-increment"),vl);
        gbScaleX->setLayout(vl);
    }
    gbScaleX->show(); 
    gbScaleX->activateWindow();
    gbScaleX->setFocus();
}
void  MainWindow::slotScaleZ()
{   
    if (!gbScaleZ) 
    {
        gbScaleZ = new QGroupBox(this);
        gbScaleZ->setWindowTitle("z interval");
        gbScaleZ->setWindowFlags(Qt::Window);
        gbScaleZ->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl0 = new QVBoxLayout;
        this->zz.setDisplay(("z"),("z-- current value of z"),vl0);
        this->zmin.setDisplay(("zmin"),("lower bond of z-interval"),vl0);
        this->zmax.setDisplay(("zmax"),("upper bond of z-interval"),vl0);
        this->hz.setDisplay(("hz"),("z-increment"),vl0);
    {   
        QGroupBox *gb = new QGroupBox("U(x,z)=U_1(x)*(1-z)+U_2(x)*z"); 
        QVBoxLayout *vl = new QVBoxLayout;
        QHBoxLayout *hl = new QHBoxLayout;
        QPushButton *bInit = new QPushButton(tr("U_1(x,z=0) = U(x)")); 
        connect(bInit, SIGNAL(clicked()), this, SLOT(slotU1()));
        hl->addWidget(bInit);

        QPushButton *bFin = new QPushButton(tr("U_2(x,z=1) = U(x)")); 
        connect(bFin, SIGNAL(clicked()), this, SLOT(slotU2()));
        hl->addWidget(bFin);

        vl->addLayout(hl);
        gb->setLayout(vl);
        vl0->addWidget(gb);
    }

        gbScaleZ->setLayout(vl0);
    }
    gbScaleZ->show(); 
    gbScaleZ->activateWindow();
    gbScaleZ->setFocus();
}
void  MainWindow::slotScalePsi()
{   
    if (!gbScalePsi) 
    {
        gbScalePsi = new QGroupBox(this);
        gbScalePsi->setWindowTitle("Y-scales for plots Psi(x) and U(x)");
        gbScalePsi->setWindowFlags(Qt::Window);
        gbScalePsi->setFont(QFont("Serif", 12, QFont::Bold )); 
        QVBoxLayout *vl = new QVBoxLayout;
        this->psixmin.setDisplay(("Psixmin"),("Y-scale for wave function plots"),vl);
        this->psixmax.setDisplay(("Psixmax"),("Y-scale for wave function plots"),vl);
        this->Umin.setDisplay(("Umin"),("lower bond of potential"),vl);
        this->Umax.setDisplay(("Umax"),("upper bond of potential"),vl);
        gbScalePsi->setLayout(vl);
    }
    gbScalePsi->show(); 
    gbScalePsi->raise();//->raactivateWindow();
    gbScalePsi->setFocus();
}
void  MainWindow::slotIntN()
{   
        if (!gbIntN) 
        {
            gbIntN = new QGroupBox(this);
            gbIntN->setWindowTitle("Number Levels interval");
            gbIntN->setWindowFlags(Qt::Window);
            gbIntN->setFont(QFont("Serif", 12, QFont::Bold )); 
            QVBoxLayout *vl=new QVBoxLayout;
            this->nminLevel.setDisplay(("nmin"),("lower bond of the level number interval>=0"),vl);
            this->nmaxLevel.setDisplay(("nmax"),("upper bond of the level number interval<N"),vl);
            this->nLevel.setDisplay(("n"),("number of level"),vl);
            gbIntN->setLayout(vl);
        }
    gbIntN->show(); 
    gbIntN->raise();//activateWindow();
    gbIntN->setFocus();
}

void  MainWindow::slotIntE()
{   
        if (!gbIntE) 
        {
            gbIntE = new QGroupBox(this);
            gbIntE->setWindowFlags(Qt::Window);
            gbIntE->setWindowTitle("E-interval");
            gbIntE->setFont(QFont("Serif", 12, QFont::Bold )); 
            QVBoxLayout *vl=new QVBoxLayout;
            this->E0.setDisplay(("E"),("energy"),vl);
            this->Emin.setDisplay(("E_min"),("lower bound of energy, meV"),vl);
            this->Emax.setDisplay(("E_max"),("upper bound of energy, meV"),vl);
            this->hE.setDisplay(("h_E"),("energy increment"),vl);
            gbIntE->setLayout(vl);
        }
    gbIntE->show(); 
    gbIntE->raise();//activateWindow();
    gbIntE->setFocus();
}
void  MainWindow::defSP()
{   
    if (!gbWPl) 
    {
            gbWPl=new QGroupBox(this);
            gbWPl->setWindowTitle("Superposition of wavefunctions E<0");
            gbWPl->setWindowFlags(Qt::Window);
            QVBoxLayout *vl=new QVBoxLayout;
            this->nminWP.setDisplay(("nmin"),("lower bond of the level number interval>=0"),vl);
            this->nmaxWP.setDisplay(("nmax"),("upper bond of the level number interval<N"),vl);
            this->hnWP.setDisplay(("hn"),("number level increment"),vl);
            gbWPl->setLayout(vl);
    }
    gbWPl->show(); 
    gbWPl->raise();//activateWindow();
    gbWPl->setFocus();
}
void  MainWindow::defWP()
{   
    if (!gbWPr) 
    {
        gbWPr=new QGroupBox(this);
        gbWPr->setFont(QFont("Serif", 12, QFont::Bold )); 
        gbWPr->setWindowTitle("Wavepacket definition E>0");
        gbWPr->setWindowFlags(Qt::Window);
        QVBoxLayout *vl=new QVBoxLayout;
        this->wpE_lo.setDisplay(("E_low"),("lower bound of the wp energy, meV"),vl);
        this->wpE_hi.setDisplay(("E_high"),("upper bound of the wp energy, meV"),vl);
        this->wpN.setDisplay(("Nwp"),("the number of wavepacket modes"),vl);
        gbWPr->setLayout(vl);
    }
    gbWPr->show(); 
    gbWPr->raise();//activateWindow();
    gbWPr->setFocus();
}
void  MainWindow::slotWPacket()
{   
        if (!gbWP) 
        {

            gbWP = new QGroupBox(this);//"Wavepacket definition:");
            gbWP->setFont(QFont("Serif", 12, QFont::Bold )); 
            gbWP->setWindowFlags(Qt::Window);
            gbWP->setWindowTitle("Time and wavepacket definition");
            QVBoxLayout *vl=new QVBoxLayout;
//            QHBoxLayout *hl=new QHBoxLayout;
//            QVBoxLayout *vlr=new QVBoxLayout;
//            QVBoxLayout *vll=new QVBoxLayout;
            {
                this->time.setDisplay(("time"), ("time"),vl);
                this->htime.setDisplay(("delta_t"),("time increment"),vl);
                this->tmin.setDisplay(("tmin"),("lower bound of the time interval"),vl);
                this->tmax.setDisplay(("tmax"),("upper bound of the time interval"),vl);
            }


            grrb=new QGroupBox("Scattering or Bound States?");
            rad1= new QRadioButton("E>0");
            rad2= new QRadioButton("E<0");

            bgR= new QButtonGroup(gbWP);
            bgR->setExclusive(true);
            bgR->addButton(rad1,1);
            bgR->addButton(rad2,2);
            bgR->button(2)->setChecked(true);
            
            QHBoxLayout *hlr=new QHBoxLayout;
            hlr->addWidget(rad1);
            hlr->addWidget(rad2);
            grrb->setLayout(hlr);

            vl->addWidget(grrb);


    QPushButton *butWP= new QPushButton( "Wavepacket definition");
    QMenu *menuWP =new QMenu();
    menuWP->setFont(QFont("Serif", 12, QFont::Bold )); 
    QAction *wpAction = menuWP->addAction(tr("&Wavepacket E>0"));
//    wpAction->setFont(QFont("Serif", 12, QFont::Bold ));
    QAction *spAction = menuWP->addAction(tr("&Superposition E<0"));
//     quitAction->setShortcut(tr("Ctrl+Q"));
//     quitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
     connect(wpAction, SIGNAL(triggered()), this, SLOT(defWP()));
     connect(spAction, SIGNAL(triggered()), this, SLOT(defSP()));
    butWP->setMenu(menuWP);
    vl->addWidget(butWP);

            gbWP->setLayout(vl);
            
        }
    gbWP->show(); 
    gbWP->raise();//activateWindow();
    gbWP->setFocus();
}
/*static const char* image_xpm[]=
{
"8 8 4 1", 
"  c #000000",
". c #848200",
"+ c #848284",
"@ c #d6d3ce",
"@@@@@@@@"
"@ @@@@ @"
"@ @@@@ @"
"@ @@@@ @"
"@ @@@@ @"
"@ @@@  @"
"@@   @ @"
"@@@@@@@@"
}
*/
void MainWindow::slotU1()
{
    const int N = model->getN();
    U1.resize(0,N+1);
    d1.resize(0,N);
//    int MU = model->Ui.size();
//    int Md = model->d.size();
//    int M=model->N;
    for(int n=1; n<=N; n++)
    {
      U1(n)= model->Ui(n);
      d1(n)= model->d(n);
    }
    U1(N+1)=model->Ui(N+1);
    U1(0)=model->Ui(0);
    d1(0)=0;
}
void MainWindow::slotU2()
{
    const int N = model->getN();
    U2.resize(0,N+1);
    d2.resize(0,N);
    for(int n=1; n<=N; n++)
    {
      U2(n)= model->Ui(n);
      d2(n)= model->d(n);
    }
    U2(N+1)=model->Ui(N+1);
    U2(0)=model->Ui(0);
    d2(0)=0;
}
void MainWindow::Uxz(double z)
{
    if(U1.size()==U2.size())
    {
        int M=U1.size()-2;        
        if (M < 0) return;
        model->set_N ( M );
//        int M1=model->N;
        model->Ui.resize(0,M+1);
        model->d.resize(0,M);
        model->m.resize(0,M+1);
        for(int n=1; n<=M; n++)
        {
            model->Ui(n)= U1(n)+z*(U2(n)-U1(n));
            model->d(n)= d1(n)+z*(d2(n)-d1(n));
            model->m(n)=0.5;
        }
        model->Ui(M+1)=U1(M+1)+z*(U2(M+1)-U1(M+1));
        model->Ui(0)=U1(0)+z*(U2(0)-U1(0));
        model->d(0)=0.;
        model->m(0)=0.5;
        model->m(M+1)=0.5;
        model->build_U();
    }
}

void MainWindow::updateMouseMovedTo(QPointF f)
{
    QString x,y;
    x.sprintf("x=%lg",f.x());
    y.sprintf("y=%lg",f.y());
    this->mouseAtX->setText(x);
    this->mouseAtY->setText(y);
}
void MainWindow::initControlDockWindow()
{
    control = new QWidget();
    QVBoxLayout *vl0 = new QVBoxLayout();
    
    QVBoxLayout * leftLayout = new QVBoxLayout;
    QGroupBox *gbview = new QGroupBox("Graphic definition of potential");
    leftLayout->addWidget(gbview);
//	gbview->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	gbview->setMinimumHeight(250);

    {    
        QVBoxLayout *vl = new QVBoxLayout();
        scene = new PotentialScene(model,this);
        potentialViewMovable = new PotentialViewMovable(scene);
        vl->addWidget(potentialViewMovable);

        QHBoxLayout *hl3 = new QHBoxLayout();

        mouseAtX = new QLabel("x-coord");
        hl3->addWidget(mouseAtX);
        mouseAtY = new QLabel("y-coord");
        hl3->addWidget(mouseAtY);

        vl->addLayout(hl3);

        connect(potentialViewMovable,SIGNAL(infoMouseMovedTo(QPointF)),
            this,SLOT(updateMouseMovedTo(QPointF)));


        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton * reset = new QPushButton("&Reset Potential");	
        //connect(reset,SIGNAL(clicked()),scene,SLOT(clearPotential()));
        connect(reset,SIGNAL(clicked()),scene,SLOT(redrawU()));
        connect(reset,SIGNAL(clicked()),scene,SLOT(redrawEn()));
        hl->addWidget(reset);		

        QPushButton * addseg = new QPushButton("&Add Potential Segment");	
        connect(addseg,SIGNAL(clicked()),scene,SLOT(addNewSegment()));
        hl->addWidget(addseg);
        hl->addStretch();
        vl->addLayout(hl);

        QHBoxLayout *hla = new QHBoxLayout();
        QPushButton * fl = new QPushButton("Find Levels");	
        fl->setDisabled(true);
        //connect(fl,SIGNAL(clicked()),scene,SLOT(calculateEnergies()));
        hla->addWidget(fl);

        QPushButton *butEn = new QPushButton(tr("List of Energy &Levels")); 
        connect(butEn, SIGNAL(clicked()), this, SLOT(slotSetEn()));
        hla->addWidget(butEn);

        vl->addLayout(hla);

        gbview->setLayout(vl);


    }

    {    
        QGroupBox *gbp = new QGroupBox("Numeric definition of potential");
        QHBoxLayout *hl = new QHBoxLayout;
        QPushButton *butUxt = new QPushButton(tr("Table &view")); 
//        butUxt->setDefault(true);
        connect(butUxt, SIGNAL(clicked()), this, SLOT(slotSetUxt()));
        hl->addWidget(butUxt);

        QPushButton *butUmwb = new QPushButton(tr("&Multi-barrier/well potential")); 
//        butUmwb->setDefault(true);
        connect(butUmwb, SIGNAL(clicked()), this, SLOT(slotSetUmwb()));
        hl->addWidget(butUmwb);
        hl->addStretch();
        gbp->setLayout(hl);
        leftLayout->addWidget(gbp);
//	gbp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
//	gbp->setMinimumWidth(250);
    }

            QGroupBox *gb01 = new QGroupBox(tr("Windows:"));
            {

                    QVBoxLayout *vl = new QVBoxLayout;
                {
                    QHBoxLayout *hl = new QHBoxLayout;
                    QPushButton *butUx = new QPushButton(tr("Potential &U")); 
                    connect(butUx, SIGNAL(clicked()), this, SLOT(compute_Uxz()));
                    hl->addWidget(butUx);

                    //           QPushButton *butEn = new QPushButton(tr("&Find levels")); 
                    //           connect(butEn, SIGNAL(clicked()), this, SLOT(Bound_States()));
                    //           hl->addWidget(butEn);


                    QPushButton *butPsix = new QPushButton("Wave functions");
                    connect(butPsix, SIGNAL(clicked()), this, SLOT(compute_Psi()));
                    hl->addWidget(butPsix);
                    vl->addLayout(hl);
                }
                {
                    QHBoxLayout *hl = new QHBoxLayout;
                    QPushButton *butPhi = new QPushButton(tr("Impulse distribution")); 
                    connect(butPhi, SIGNAL(clicked()), this, SLOT(compute_Phi()));
                    hl->addWidget(butPhi);

                    QPushButton *butT = new QPushButton(tr("Transmission &D")); 
                    connect(butT, SIGNAL(clicked()), this, SLOT(compute_T()));
                    hl->addWidget(butT);
                    vl->addLayout(hl);
                }
                gb01->setLayout(vl);
                vl0->addWidget(gb01);
 //       vl->addStretch();
 //       vl0->addStretch();
            }
            leftLayout->addWidget(gb01);
/*        {
            QGroupBox *gb = new QGroupBox("Parameters:");
            QVBoxLayout *vl = new QVBoxLayout();

            {
                QHBoxLayout *hl = new QHBoxLayout();
                QPushButton * btn = new QPushButton("level &n");	
                connect(btn,SIGNAL(clicked()),this,SLOT(slotIntN()));
                hl->addWidget(btn);		

                QPushButton * btE = new QPushButton("&energy E");	
                connect(btE,SIGNAL(clicked()),this,SLOT(slotIntE()));
                hl->addWidget(btE);
                vl->addLayout(hl);
            }
*/
/*            {
                QHBoxLayout *hl = new QHBoxLayout;
                QPushButton * btwp = new QPushButton("time &t");	
                connect(btwp,SIGNAL(clicked()),this,SLOT(slotWPacket()));
                hl->addWidget(btwp);	

                QPushButton *bsk = new QPushButton(tr("&k scale")); 
                connect(bsk, SIGNAL(clicked()), this, SLOT(slotScaleP()));
                hl->addWidget(bsk);
                vl->addLayout(hl);

            }
        */
/*            {
                QHBoxLayout *hl = new QHBoxLayout();
                QPushButton * bsx = new QPushButton("&x scale");	
                connect(bsx,SIGNAL(clicked()),this,SLOT(slotScaleX()));
                hl->addWidget(bsx);		

                QPushButton * bsz = new QPushButton("&z scale");	
                connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
                hl->addWidget(bsz);		
                vl->addLayout(hl);

            }

            {
                QHBoxLayout *hl = new QHBoxLayout();
                QPushButton * bsy = new QPushButton("&y scales");	
                connect(bsy,SIGNAL(clicked()),this,SLOT(slotScalePsi()));
                hl->addWidget(bsy);		

                flgScale= new QCheckBox("Fixed y-scales",gb);
                flgScale->setChecked(true);
                hl->addWidget(flgScale);
                vl->addLayout(hl);
            }
            */
             
                //        flgErase= new QCheckBox("Erasable plot",gb);
                //        flgErase->setChecked(true);
                //        hl->addWidget(flgErase);
 //               vl->addLayout(hl);

//            gb->setLayout(vl);
//            vl0->addWidget(gb);  //???
//        }
/*
        QGroupBox *gb01 = new QGroupBox(tr("Windows:"));
        {
            QVBoxLayout *vld = new QVBoxLayout;
            gb01->setLayout(vld);
            vl0->addWidget(gb01);

            QPushButton *butUx = new QPushButton(tr("Potential &U")); 
            connect(butUx, SIGNAL(clicked()), this, SLOT(compute_Uxz()));
            vld->addWidget(butUx);

            QPushButton *butT = new QPushButton(tr("Transmission &D")); 
            connect(butT, SIGNAL(clicked()), this, SLOT(compute_T()));
            vld->addWidget(butT);

            QPushButton *butPsix = new QPushButton("Wave functions");
            connect(butPsix, SIGNAL(clicked()), this, SLOT(compute_Psi()));
            vld->addWidget(butPsix);

            QPushButton *butPhi = new QPushButton(tr("Impulse distribution")); 
            connect(butPhi, SIGNAL(clicked()), this, SLOT(compute_Phi()));
            vld->addWidget(butPhi);
*/
            /*       {
            QGroupBox *gb = new QGroupBox(tr("Potential"));
            QVBoxLayout *vl = new QVBoxLayout;
            QHBoxLayout *hl = new QHBoxLayout;
            U_type = new QComboBox(this);
            U_type->addItem(tr("U(x)"));
            U_type->addItem(tr("U(x,z)"));
            U_type->setCurrentIndex(0);
            hl->addWidget(U_type);

            QPushButton *butUx = new QPushButton(tr("show &U")); 
 //           connect(butUx, SIGNAL(clicked()), this, SLOT(compute_U()));//update_UxNwells();
            connect(butUx, SIGNAL(clicked()), this, SLOT(compute_Uxz()));//update_UxNwells();
            hl->addWidget(butUx);
            vl->addLayout(hl);

            gb->setLayout(vl);
            vld->addWidget(gb);  //??
//            vld->addLayout(vl);
        }
*/
/*        {
            QGroupBox *gb = new QGroupBox(tr("Bound States: E<0"));
            QVBoxLayout *vl = new QVBoxLayout;
            QHBoxLayout *hl = new QHBoxLayout;
            En_type = new QComboBox(this);
            En_type->addItem(tr("E_n & Psi_n"));
            En_type->addItem(tr("N(E)-step"));
            En_type->addItem(tr("B_{N+1}(E)"));
            En_type->addItem(tr("En(z)"));
            En_type->setCurrentIndex(0);
            hl->addWidget(En_type);

            QPushButton *butEn = new QPushButton(tr("&find levels")); 
            connect(butEn, SIGNAL(clicked()), this, SLOT(Bound_States()));
            hl->addWidget(butEn);
            vl->addLayout(hl);

            gb->setLayout(vl);
            vld->addWidget(gb);  //???
        }
        */
/*        {
            QGroupBox *gb = new QGroupBox(tr("Transmission D: E>0"));
            QVBoxLayout *vl = new QVBoxLayout;
            QHBoxLayout *hl = new QHBoxLayout;
            T_type = new QComboBox(this);
            T_type->addItem(tr("D(E)"));
            T_type->addItem(tr("D(z)"));
            T_type->addItem(tr("D(E,z)"));
            T_type->addItem(tr("D(z,E)"));
            T_type->setCurrentIndex(0);
            hl->addWidget(T_type);

            QPushButton *butT = new QPushButton(tr("run &D")); 
//            QPushButton *butT = new QPushButton(tr("transparency &D")); 
//            connect(butT, SIGNAL(clicked()), this, SLOT(TEz()));
            connect(butT, SIGNAL(clicked()), this, SLOT(compute_T()));
            hl->addWidget(butT);
            vl->addLayout(hl);

            gb->setLayout(vl);
            vld->addWidget(gb);  //???
        }
*/
 /*       { 
            //QString sum=QChar(0x03A3);
            QString sum=QChar(0x2211);

            QString psi=QChar(0x03C8);
            QString Psi=QChar(0x03A8);
            QString to2=QChar(0x00B2);
            QString psiofx=psi+"(x)";
            QString mod_psiofx="|"+psiofx+"|"+to2;//+QChar(0x2082);
            QGroupBox *gb3 = new QGroupBox(tr("Wave function ")+psiofx);
            QVBoxLayout *vl = new QVBoxLayout;
            QHBoxLayout *hl = new QHBoxLayout;
            psi_type = new QComboBox(this);
            psi_type->addItem(mod_psiofx);
//            psi_type->addItem(tr("|Psi(x)|^2"));
            psi_type->addItem("real "+psi);
            psi_type->addItem("imag "+psi);
            psi_type->setCurrentIndex(0);
            vl->addWidget(psi_type);

            psix_var = new QComboBox(this);
            psix_var->addItem(psi+"_n(x,n)");
            psix_var->addItem(psi+"(x,E)");
            psix_var->addItem(psiofx);
            psix_var->addItem(sum+Psi+"_n(x,t)");
            psix_var->setCurrentIndex(0);
            vl->addWidget(psix_var);
            hl->addLayout(vl);

//            QHBoxLayout *hlb = new QHBoxLayout;

//            QPushButton *butPsix = new QPushButton(this);
//            butPsix->setIcon(QIcon("images/car.png"));
//            butPsix->setIcon(QIcon("images/pointer.png"));
//            butPsix->adjustSize();
//            butPsix->minimumSize();
//            butPsix->show();
//            QPushButton *butPsix = new QPushButton("\Psi(x)");
    QVBoxLayout *vlr = new QVBoxLayout();
    flgUx= new QCheckBox("add U(x)",gb3);
    flgUx->setChecked(true);
    vlr->addWidget(flgUx);
            QString run=QChar(0x2B55);//(0x25C9);//(0x25BC);
            QPushButton *butPsix = new QPushButton("run "+psi);
//            QPushButton *butPsix = new QPushButton(tr("&psi(x)"));
//            butPsix->setFont(QFont("Symbol", 12, QFont::Bold ));
            connect(butPsix, SIGNAL(clicked()), this, SLOT(compute_Psi()));
            vlr->addWidget(butPsix);

            hl->addLayout(vlr);
//            hl->addLayout(vl);
            gb3->setLayout(hl);
            vld->addWidget(gb3);
        }
*/
/*{
    QGroupBox *gb3 = new QGroupBox(tr("&Impulse distribution |Phi_n(k)|^2"));
    QVBoxLayout *vl = new QVBoxLayout;
    QHBoxLayout *hl = new QHBoxLayout;

    psip_var = new QComboBox(this);
    psip_var->addItem(tr("Phi_n(k,n)"));
    psip_var->addItem(tr("sum(Phi_n(k,t))"));
    psip_var->setCurrentIndex(0);
    hl->addWidget(psip_var);


    QPushButton *butPhi = new QPushButton(tr("run p&hi")); 
    connect(butPhi, SIGNAL(clicked()), this, SLOT(compute_Phi()));
    hl->addWidget(butPhi);
    vl->addLayout(hl);

    gb3->setLayout(vl);
    vld->addWidget(gb3);
}
*/
/*{
    QHBoxLayout *hl = new QHBoxLayout();
    flgErase= new QCheckBox("Erasable plot",gb01);
    flgErase->setChecked(true);
    hl->addWidget(flgErase);

    QPushButton *stopB = new QPushButton(tr("STOP")); 
    stopB->setShortcut(tr("Alt+S"));
    connect(stopB, SIGNAL(clicked()), this, SLOT(stopCalc()));
    hl->addWidget(stopB);

    vld->addLayout(hl);
}
*/
//}
vl0->addStretch(1);
control->setLayout(vl0);
//wfWidget = new QWidget;
//QVBoxLayout * wfLayout = new QVBoxLayout();
//wfWidget->setLayout(wfLayout);
//wfWidget->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
//wfWidget->setMinimumWidth(250);

QHBoxLayout * mainLayout = new QHBoxLayout;
mainLayout->addLayout(leftLayout);
mainLayout->addWidget(control);
QWidget * widget = new QWidget;
widget->setLayout(mainLayout);
setCentralWidget(widget);

}
void MainWindow::resizeEvent (QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    //XXX view->fitInView(scene->sceneRect());
}
void MainWindow::compute()
{
    model->E0 = this->E0;
    model->build_k();
    model->build_ab();
    if(model->E0 - model->U(model->getN()+1) > 0)
        model->build_RT();
    else
    {
        model->RR=1;
        model->TT=0;
    }
    updateValues();
    }
//--------------
void MainWindow::updateValues()
    {

    this->E0.updateDisplay();
    this->zz.updateDisplay();
    this->time.updateDisplay();
    this->Ubias.updateDisplay();
    this->nLevel.updateDisplay();

    QString s;
    double E=this->E0;
    s.sprintf("Energy E: %.3lg",E);
//    E = s.toDouble();
    this->dispEnergy->setText(s);
    this->dispEnergy->update();

    double z=this->zz;
    s.sprintf("z: %.3lg",z);
//    z = s.toDouble();
    this->dispZ->setText(s);
    this->dispZ->update();

    double t=this->time;
    s.sprintf("Time: %.3lg",t);
//    z = s.toDouble();
    this->dispTime->setText(s);
    this->dispTime->update();

    double Ub=this->Ubias;
    s.sprintf("Bias: %.3lg",Ub);
//    z = s.toDouble();
    this->dispBias->setText(s);
    this->dispBias->update();

    if(model->E0-model->U(model->getN() + 1)>0) 
    {
//    double R0=model->RR; 
    double T0=model->TT;
    double RTtotal=model->totalRT;
/*    s.sprintf("R: %.3lg",R0);
    z = s.toDouble();
    this->dispR->setText(s);
    this->dispR->update();
*/
    s.sprintf("T: %.3lg",T0);
//    z = s.toDouble();
    this->dispT->setText(s);
    this->dispT->update();

    s.sprintf("(R+T)tot: %.3lg",RTtotal);
//    z = s.toDouble();
    this->dispRT->setText(s);
    this->dispRT->update();
    }
}
void MainWindow::initStatusBar()
{
    createStatusBar();
    this->statusBar()->setFont(QFont("Serif", 10, QFont::DemiBold )); 
    this->dispEnergy = new QLabel("Energy E: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispEnergy);

    this->dispZ = new QLabel("z: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispZ);

    this->dispTime = new QLabel("Time t: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispTime);

    this->dispBias = new QLabel("Bias: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispBias);
/*    this->dispR = new QLabel("R: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispR);
    */
    this->dispT = new QLabel("T: --------------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispT);
    this->dispRT = new QLabel("(T+R): ---------",
            this->statusBar());
    this->statusBar()->addWidget(this->dispRT);
 }

void MainWindow::Bound_States()
{
    int i = this->En_type->QComboBox::currentIndex();
    double Umin = 1e300, Umax = -1e300;
    double q;
    int Nmax;
    model->build_U();
        Nmax = model->getN ();
    for (int i = 0; i < Nmax+1; ++i)
    {   
        q = model->U(i);
        if (q > Umax) Umax = q; 
        if (q < Umin) Umin = q;
    }
    if(model->U(0)>model->U(Nmax+1)) Umax=model->U(Nmax+1);
    else Umax=model->U(0);
//    if(this->U(Nmax+1)>0) Umax=0.;
//    else Umax=this->U(Nmax+1);
    Umin=Umin+1e-8;
    Umax=Umax-1e-6;
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
void MainWindow::compute_NE()
{
    double y,q,_dE;
    winPlotNE->show();
    winPlotNE->raise();
    plotNE->scaleFixed=false;
    double Umin = 1e300, Umax = -1e300;
    int Nmax;
    Nmax = model->getN ();
    dataNE.clear();
    for (int i = 0; i < Nmax+1; ++i)
    {   
        q = model->U(i);
        if (q > Umax) Umax = q; 
        if (q < Umin) Umin = q;
    }
        if(model->U(Nmax+1)>0) Umax=0.;
        else Umax=model->U(Nmax+1);
        Umin=Umin+1e-8;
        Umax=Umax-1e-6;
    _dE=this->hE;
    if(_dE<0) _dE=-this->hE;
    if(_dE==0) _dE=(Umax-Umin)/100;
    this->plotNE->clearAll(); 
//    numOfCurveNE=0;
    for(double x=Umin; x<=Umax; x+=_dE)
    {    
        y=double(model->findNumberOfLevels(x));
        this->dataNE.push_back(x);
        this->dataNE.push_back(y);
    }
    this->plotNE->setCurveData(1,this->dataNE);
//    this->plotNE->setCurveData(this->numOfCurveNE,this->dataNE);
//    this->numOfCurveNE++;
}
void MainWindow::compute_T()
{
    wPlotT->show();
    wPlotT->raise();
//    plotterT->scaleFixed=false;
    model->build_U();
    if(this->Emin<0) this->Emin=0.01;
    if(this->Emax<0) this->Emax=20.;
    if(this->E0<0) this->E0=0.01;
    if(this->flgErase->isChecked()) 
    {
        plotterT->clearAll();
        this->numOfCurveT=0;
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
void MainWindow::compute_TE()
{
    std::vector<double> dataT;
    if(this->flgErase->isChecked()) 
    {
        this->numOfCurveT=0;
    }
    else this->numOfCurveT++;
    double Emn=this->Emin;
    double Emx=this->Emax;
//    if(this->E0<0&&this->hE>0) this->E0=Emn;
//    if(this->E0<0&&this->hE<0) this->E0=Emx;
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
    for(double E=EE; E>=Emn&&E<=Emx; E+=this->hE)
    {
        this->E0=E;
        compute();
        double y=model->TT;
        dataT.push_back(E);
        dataT.push_back(y);
    }
    this->plotterT->setCurveData(this->numOfCurveT,dataT);
    this->E0=this->E0+this->hE;

}

void MainWindow::compute_Tz()
{
    std::vector<double> dataT;
    if(this->flgErase->isChecked()) 
    {
        this->numOfCurveT=0;
    }
    else this->numOfCurveT++;
    if(this->zmin<0) this->zmax=0.0;
    if(this->zmax<0) this->zmax=1.;
    double zmn=this->zmin;
    double zmx=this->zmax;
//    if(this->zz<0&&this->hz>0) this->zz=zmn;
//    if(this->zz<0&&this->hz<0) this->zz=zmx;
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

        this->zz=z;
        Uxz(z);
        compute();
        double y=model->TT;
        dataT.push_back(z);
        dataT.push_back(y);
    }
    this->plotterT->setCurveData(this->numOfCurveT,dataT);
    this->zz=this->zz+this->hz;

}
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
        Uxz(z);
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
    double Umin = 1e300, Umax = -1e300;
    double q;
    int Nmax;
    Nmax = model->getN ();
    double x=0;
    for (int i = 0; i < Nmax+1; ++i)
    {   
        q = model->U(i);
        x+=model->d(i);
        if (q > Umax) Umax = q; 
        if (q < Umin) Umin = q;
    }
    if(model->U(0)>model->U(Nmax+1)) Umax=model->U(Nmax+1);
    else Umax=model->U(0);
    double Emn=Umin+1e-8;
    double Emx=Umax-1e-6;
    double xmax=x;
    double y;
//    double Emn=this->Emin;
//    double Emx=this->Emax;
    if(this->E0>=Emx&&this->hE>0) this->E0=Emn;
    if(this->E0<=Emn&&this->hE<0) this->E0=Emx;
    for(double E=Emn; E<=Emx; E+=this->hE)
    {
        if(getBreakStatus(0))   return;
        this->E0=E;
        model->E0=E;
        compute();
        y=real(model->b(Nmax +1));
        if(y>1.) y =1+log(y);
        if(y<-1.) y =-1-log(-y);
        data.push_back(y+0.5*xmax);
        data.push_back(E);
    }
    this->E0=this->E0+this->hE;
    this->plotterUx->setCurveData(this->numOfCurveUx,data);
    this->numOfCurveUx++;
    data1.push_back(0.5*xmax);
    data1.push_back(Emn);
    data1.push_back(0.5*xmax);
    data1.push_back(Emx);
    this->plotterUx->setCurveData(this->numOfCurveUx,data1);
    this->numOfCurveUx++;

}
static void addUx(double xmin,double xmax, double yscale, PhysicalModel *model, Plotter *plotter)
{
    const int N = model->getN();
    std::vector<double> dataUx;
    double x,y;
    x=xmin;
    int nmin=0;
    int nmax = N;
    if(nmin==0) 
    {
        y=yscale*model->U(0);
        dataUx.push_back(x);
        dataUx.push_back(y);
        dataUx.push_back(0.);
        dataUx.push_back(y);
        x=0;
    }
    else 
    {
        y=yscale*model->U(nmin+1);
        x=0;
        for(int n=1;n<=nmin+1;n++) 
            x +=model->d(n);
        dataUx.push_back(xmin);
        dataUx.push_back(y);
        dataUx.push_back(x);
        dataUx.push_back(y);
    }
    for(int n=nmin+1;n<=nmax;n++)
    {
        double y=yscale*model->U(n);
        dataUx.push_back(x);
        dataUx.push_back(y);
        x +=model->d(n);
        dataUx.push_back(x);
        dataUx.push_back(y);
    }
    dataUx.push_back(x);
    dataUx.push_back(yscale*model->U(N+1));
    if(xmax<x) xmax=x; 
    else x =xmax;
    dataUx.push_back(x);
    dataUx.push_back(yscale*model->U(N+1));
    plotter->setCurveData(0,dataUx);
}

void MainWindow::compute_En()
{
    model->findBoundStates();
    this->plotterPsi2x->clearAll(); 
    plotterPsi2x->scaleFixed=false;
    this->plotterUx->clearAll(); 
    numOfCurveUx=0;
    this->showU();
    this->numOfCurveUx=1;
    for (int i=0; i<model->Ebound.size(); i++)
    {
        double En=model->Ebound[i];
        this->E0=En;
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
    if(this->U_type->QComboBox::currentIndex()==0) showU();
    else
    for(double z=this->zmin;z<=this->zmax; z+=this->hz)
    { 
        Uxz(z);
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
            Uxz(z);
            model->findBoundStates();
            int nmx=this->nmaxLevel;
            int nmn=this->nminLevel;
            if(nmx>=model->Ebound.size()) nmx=model->Ebound.size()-1;
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

            updateCurves(cs, model->Ebound, z);
            myrepaint(plotterEnz,cs);
            compute_Psin(); 
            nmxold=nmx;
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
        plotterPhi->clearAll();
        this->numOfCurve=0;
    }
    else this->numOfCurve++;
    if(kmax<=0) kmax=10.;
    double dk=kmax/500;
    int imax=this->nmaxLevel;
    int imin=this->nminLevel;
    if(imax>=model->Ebound.size()) imax=model->Ebound.size()-1;
    if(imin<0) imin=0;
//    this->numOfCurve=0;
    for(int i=imin;i<=imax; i++)
    {
        this->E0=model->Ebound[i];
        compute();
        data.clear();
        for(double kk=kmin; kk<=kmax; kk+=dk)
        {
            model->kwave=kk;
            model->build_Phi();
            y=model->Phi2;
            data.push_back(kk);
            data.push_back(y);
        }
        this->plotterPhi->setCurveData(this->numOfCurve,data);
        compute_PhiofP();
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
    y=model->U(0);
    data.push_back(x);
    data.push_back(y);
    x=0;
    data.push_back(x);
    data.push_back(y);
    for(int n=1; n <= N; n++)
    {
        double y=model->U(n);
        data.push_back(x);
        data.push_back(y);
        x +=model->d(n);
        data.push_back(x);
        data.push_back(y);
    }
    data.push_back(x);
    data.push_back(model->U(N+1));
    if(this->xmax<x) this->xmax=x; 
    else x =this->xmax;
    data.push_back(x);
    data.push_back(model->U(N+1));
    this->plotterUx->setCurveData(numOfCurveUx,data);
    this->numOfCurveUx++;
}
void MainWindow::showEn(double E)
{
        std::vector<double> data;
        data.push_back(this->xmin); 
//        data.push_back(model->Xmin); 
        data.push_back(E);
        data.push_back(this->xmax);
//        data.push_back(model->Xmax);
        data.push_back(E);
        this->plotterUx->setCurveData(this->numOfCurveUx,data);
        this->numOfCurveUx++;
}
void MainWindow::compute_Psi()
{       

    int ii=psix_var->QComboBox::currentIndex();
    model->build_U();
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
            this->WavePacketXoft();
            break;
        default:
            break; 
        }
}
/*static void addUx(double xmin,double xmax, double yscale, PhysicalModel *model, Plotter *plotter)
{
    std::vector<double> dataUx;
    double x,y;
    x=xmin;
    int nmin=0;
    int nmax=model->N;
    if(nmin==0) 
    {
        y=model->Ui(0);
        dataUx.push_back(x);
        dataUx.push_back(y);
        dataUx.push_back(0.);
        dataUx.push_back(y);
        x=0;
    }
    else 
    {
        y=yscale*model->U(nmin+1);
        x=0;
        for(int n=1;n<=nmin+1;n++) 
            x +=model->d(n);
        dataUx.push_back(xmin);
        dataUx.push_back(y);
        dataUx.push_back(x);
        dataUx.push_back(y);
    }
    for(int n=nmin+1;n<=nmax;n++)
    {
        double y=yscale*model->U(n);
        dataUx.push_back(x);
        dataUx.push_back(y);
        x +=model->d(n);
        dataUx.push_back(x);
        dataUx.push_back(y);
    }
    dataUx.push_back(x);
    dataUx.push_back(yscale*model->U(model->N+1));
    if(xmax<x) xmax=x; 
    else x =xmax;
    dataUx.push_back(x);
    dataUx.push_back(yscale*model->U(model->N+1));
    plotter->setCurveData(0,dataUx);
}
*/
void MainWindow::compute_Psin()
{
    const int N = model->getN();
    wPlotPsi2->show();
    wPlotPsi2->raise();
    std::vector<double> data;
/*        if(this->flgErase->isChecked()) 
    {
        plotterPsi2x->clearAll();
        this->numOfCurve=0;
    }
    else this->numOfCurve++;
    */
//    model->XUscales();
    PlotSettings ps;
    bool flg=flgScale->isChecked();
    plotterPsi2x->scaleFixed=flg;
    if(flg)
    {
    ps.minX = this->xmin;
    ps.maxX = this->xmax;
    ps.minY = psixmin;
    ps.maxY = psixmax;
    ps.numYTicks = 5;
    ps.numXTicks = 5;
//    ps.adjust();
    plotterPsi2x->setPlotSettings(ps);
    }
        if(this->flgUx->isChecked()) 
    {
    double ymax=abs(this->Umin);
    if(ymax<abs(this->Umax)) ymax=abs(this->Umax);
    if(ymax==0) ymax=1;
    double scaleU=psixmax/ymax;
    if(scaleU==0) scaleU=0.05;
    addUx(this->xmin, this->xmax, scaleU, model, plotterPsi2x);
        }
    double dx=this->hx;
//    double dx=(this->xmax-this->xmin)/300.;
    int ii=psi_type->QComboBox::currentIndex();
    int numCurve=2;
    int nmx=this->nmaxLevel;
    int nmn=this->nminLevel;
    if(nmx>=model->Ebound.size()) nmx=model->Ebound.size()-1;
    if(nmn<0) nmn=0;
    double y;
    for(int n=nmn;n<=nmx; n++)
    {
        double E=model->Ebound[n];
        this->E0=E;
        compute();
        model->b(N+1)=0;
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
//    plotterPsi2x->scaleFixed=this->FlagScale;
    PlotSettings ps;
    bool flg=flgScale->isChecked();
    plotterPsi2x->scaleFixed=flg;
    if(flg)
    {
    ps.minX = this->xmin;
    ps.maxX = this->xmax;
    ps.minY = psixmin;
    ps.maxY = psixmax;
    ps.numYTicks = 5;
    ps.numXTicks = 5;
    plotterPsi2x->setPlotSettings(ps);
    }
    double ymax=abs(this->Umin);
    if(ymax<abs(this->Umax)) ymax=abs(this->Umax);
    if(ymax==0) ymax=1;
    double scaleU=psixmax/ymax;
    if(scaleU==0) scaleU=0.05;
//    addUx(this->xmin, this->xmax, scaleU, model, plotterPsi2x);
    //    if(this->Emin<0) model->Xmax=model->Xmax*1.1/1.3;
//    double dx=(this->xmax-this->xmin)/2000.;
    double dx=this->hx;//(this->xmax-model->Xmin)/2000.;
//    this->plotterPsi2x->clearAll();
//    for(double E=this->Emin; E<=this->Emax; E+=this->hE)
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
//    this->plotterPsi2x->clearCurve(2);
    data.clear();
    for(double x=this->xmin; x<=this->xmax; x+=dx)
//    for(double x=model->Xmin; x<=model->Xmax; x+=dx)
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
            if(this->flgUx->isChecked()) addUx(this->xmin, this->xmax, scaleU, model, plotterPsi2x);
    this->plotterPsi2x->setCurveData(this->numOfCurve,data);
    }
    this->E0=this->E0+this->hE;
    }
/*

 /*   void MainWindow::compute_Psin()
{
    wPlotPsi2->show();
    wPlotPsi2->raise();
    std::vector<double> data;
//    model->XUscales();
    double ymax=abs(this->Umin);
    if(ymax<abs(this->Umax)) ymax=abs(this->Umax);
    if(ymax==0) ymax=1;
    double scaleU=psixmax/ymax;
    if(scaleU==0) scaleU=0.05;
    addUx(this->xmin, this->xmax, scaleU, model, plotterPsi2x);
//  addUx(model, plotterPsi2x);
//    double dx=(model->Xmax-model->Xmin)/300.;
    double dx=(this->xmax-this->xmin)/300.;
    int ii=psi_type->QComboBox::currentIndex();
    int numCurve=2;
    int nmx=this->nmaxLevel;
    int nmn=this->nminLevel;
    if(nmx>=model->Ebound.size()) nmx=model->Ebound.size()-1;
    if(nmn<0) nmn=0;
    double x,y;
    for(int n=nmn;n<=nmx; n++)
    {
        double E=model->Ebound[n];
        this->E0=E;
        compute();
        model->b(model->N+1)=0;
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
        this->plotterPsi2x->setCurveData(numCurve,data);
        numCurve++;
    }
}
*/
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
//    data.clear(); ??????????
    PlotSettings ps;
    bool flg=flgScale->isChecked();
    plotterPsi2x->scaleFixed=flg;
    if(flg)
    {
    ps.minX = this->xmin;
    ps.maxX = this->xmax;
    ps.minY = psixmin;
    ps.maxY = psixmax;
    ps.numYTicks = 5;
    ps.numXTicks = 5;
//    ps.adjust();
    plotterPsi2x->setPlotSettings(ps);
    }
            if(this->flgUx->isChecked()) 
    {
    double ymax=abs(this->Umin);
    if(ymax<abs(this->Umax)) ymax=abs(this->Umax);
    if(ymax==0) ymax=1;
    double scaleU=psixmax/ymax;
    if(scaleU==0) scaleU=0.05;
    addUx(this->xmin, this->xmax, scaleU, model, plotterPsi2x);
            }
    double y;
    double dx=this->hx;//(this->xmax-this->xmin)/300.;
//    double dx=(this->xmax-this->xmin)/300.;
//    if(dx>this->hx) dx=this->hx;
    int ii=psi_type->QComboBox::currentIndex();
//    int ivar=psix_var->QComboBox::currentIndex();
//    double E=model->Ebound[this->nLevel];
//    this->E0=E;
    model->E0=this->E0;
    compute();
//    int NZ=model->zeroPsi();
//    int NL=model->findNumberOfLevels(this->E0);
//     int Nx=0;
//    double yRold=0.1;
//    model->b(model->N+1)=0;
    for(double x=this->xmin; x<=this->xmax; x+=dx)
//    for(double x=model->Xmin; x<=model->Xmax; x+=dx)
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
/*        if(ivar==2)
        {
            double yR=model->psi_real;
            if(yR>0&&yRold<0||yR<0&&yRold>0) Nx=Nx+1;
            yRold=yR;
            if(Nx>NL-1)
            {
                y=0;
            }
        }
*/
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
//    this->numOfCurve++;
}

/*void MainWindow::initPlotNofE() 
{
    winPlotNE = new QDialog(this);
//    winPlotNE->setFocus();
    QVBoxLayout *layout=new QVBoxLayout(winPlotNE);
    QLabel *labelNE = new QLabel("Number of levels on energy E");
    labelNE->setFont(QFont("Serif", 16, QFont::Bold )); 
    labelNE->setAlignment(Qt::AlignCenter);
    labelNE->setFrameStyle(QFrame::Panel | QFrame::Raised); 
    this->plotNE= new Plotter(winPlotNE);
    this->plotNE->setCurveData(1, this->dataNE);
    layout->addWidget(this->plotNE);
    layout->addWidget(labelNE);
}
*/
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
    QPushButton *butNE = new QPushButton(tr("RUN")); 
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
        QPushButton * bsE = new QPushButton("&E scale");	
        connect(bsE,SIGNAL(clicked()),this,SLOT(slotIntE()));
        hl->addWidget(bsE);		

        QPushButton * bsz = new QPushButton("&z scale");	
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);	
        hl->addStretch();
  //      vl->addLayout(hl);
 //   }
 //   {
 //       QHBoxLayout *hl = new QHBoxLayout;
        T_type = new QComboBox(this);
        T_type->addItem(tr("D(E)"));
        T_type->addItem(tr("D(z)"));
        T_type->addItem(tr("D(E,z)"));
        T_type->addItem(tr("D(z,E)"));
        T_type->setCurrentIndex(0);
        hl->addWidget(T_type);

        QPushButton *butT = new QPushButton(tr("RUN")); 
        connect(butT, SIGNAL(clicked()), this, SLOT(compute_T()));
        hl->addWidget(butT);

        QPushButton *stopB = new QPushButton(tr("STOP")); 
        stopB->setShortcut(tr("Alt+S"));
        connect(stopB, SIGNAL(clicked()), this, SLOT(stopCalc()));
        hl->addWidget(stopB);

        flgErase= new QCheckBox("Erase",wPlotT);
        flgErase->setChecked(true);
        hl->addWidget(flgErase);

        hl->addStretch();

        vl->addLayout(hl);

    }


}

/*void MainWindow::initPlotEnz() 
{
    wPlotEnz = new QDialog(this);
//    wPlotEnz -> setFocus();
    wPlotEnz->activateWindow();
    QVBoxLayout *layT=new QVBoxLayout(wPlotEnz);
    QLabel *labelT = new QLabel("En(z)");
    labelT->setFont(QFont("Serif", 16, QFont::Bold )); 
    labelT->setAlignment(Qt::AlignCenter);
    labelT->setFrameStyle(QFrame::Panel | QFrame::Raised); 
    this->plotterEnz= new Plotter(wPlotEnz);
    this->plotterEnz->setCurveData(1, this->plotdataEnz);
    layT->addWidget(this->plotterEnz);
    layT->addWidget(labelT);
}
*/
/*void MainWindow::initPlotPsix() 
{
    wPlotPsi2 = new QDialog(this);
//    wPlotPsi2->setFocus();
    QVBoxLayout *layPsi2x=new QVBoxLayout(wPlotPsi2);
    QLabel *labelPsi2 = new QLabel("Wave function/Probability density on x");
    labelPsi2->setFont(QFont("Serif", 16, QFont::Bold )); 
    labelPsi2->setAlignment(Qt::AlignCenter);
    labelPsi2->setFrameStyle(QFrame::Panel | QFrame::Raised); 
    this->plotterPsi2x= new Plotter(wPlotPsi2);
    this->plotterPsi2x->setCurveData(1, this->plotdataPsi2);
    layPsi2x->addWidget(this->plotterPsi2x);
    layPsi2x->addWidget(labelPsi2);
}
*/
/*
void MainWindow::initPlotPhik() 
{
    wPlotPhi = new QDialog(this);
//    wPlotPhi->setFocus();
    QVBoxLayout *layPhi=new QVBoxLayout(wPlotPhi);
    QLabel *labelPhi = new QLabel("Impulse distribution");
    labelPhi->setFont(QFont("Serif", 16, QFont::Bold )); 
    labelPhi->setAlignment(Qt::AlignCenter);
    labelPhi->setFrameStyle(QFrame::Panel | QFrame::Raised); 
    this->plotterPhi= new Plotter(wPlotPhi);
    this->plotterPhi->setCurveData(1, this->plotdataPhi);
    layPhi->addWidget(this->plotterPhi);
    layPhi->addWidget(labelPhi);
}*/
/*void MainWindow::initPlotUx() 
{
    wPlotUx = new QDialog(this);
//    wPlotUx->setFocus();
    QVBoxLayout *layUx=new QVBoxLayout(wPlotUx); 
    QLabel *labelUx = new QLabel("Potential U(x)");
    labelUx->setFont(QFont("Serif", 16, QFont::Bold )); 
    labelUx->setAlignment(Qt::AlignCenter);
    labelUx->setFrameStyle(QFrame::Panel | QFrame::Raised); 
    this->plotterUx= new Plotter(wPlotUx);
    this->plotterUx->setCurveData(1, this->plotdataUx);
    layUx->addWidget(this->plotterUx);
    layUx->addWidget(labelUx);
}
*/
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
    QPushButton * bsz = new QPushButton("&z scale");	
    connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
    hl->addWidget(bsz);		
    hl->addStretch();

    QPushButton *bRun = new QPushButton(tr("RUN")); 
    connect(bRun, SIGNAL(clicked()), this, SLOT(compute_Enz()));
    hl->addWidget(bRun);

    vl->addLayout(hl);

}

void MainWindow::initPlotPsix() 
{
    wPlotPsi2 = new QDialog(this);
    QVBoxLayout *vl=new QVBoxLayout(wPlotPsi2);
    PlotterDialog *wPsi = new PlotterDialog(wPlotPsi2);
    wPlotPsi2->setFont(QFont("Serif", 12, QFont::SemiCondensed )); 
    this->plotterPsi2x = ((PlotterDialog*)wPsi)->plotter();
    wPlotPsi2->setWindowTitle(tr("Coordinate distribution"));
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

        hl->addStretch();

        QPushButton * bsz = new QPushButton("&z scale");	
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);		

        QPushButton * btn = new QPushButton("level &n");	
        connect(btn,SIGNAL(clicked()),this,SLOT(slotIntN()));
        hl->addWidget(btn);		

        QPushButton * btwp = new QPushButton("time &t");	
        connect(btwp,SIGNAL(clicked()),this,SLOT(slotWPacket()));
        hl->addWidget(btwp);	
//        hl->addStretch();
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
            psi_type = new QComboBox(this);
            psi_type->addItem(mod_psiofx);
            psi_type->addItem("real "+psi);
            psi_type->addItem("imag "+psi);
            psi_type->setCurrentIndex(0);
            hl->addWidget(psi_type);

            psix_var = new QComboBox(this);
            psix_var->addItem(psi+"_n(x,n)");
            psix_var->addItem(psi+"(x,E)");
            psix_var->addItem(psiofx);
            psix_var->addItem(sum+Psi+"_n(x,t)");
            psix_var->setCurrentIndex(0);
            hl->addWidget(psix_var);

            flgUx= new QCheckBox("add U(x)",wPlotPsi2);
            flgUx->setChecked(true);
            hl->addWidget(flgUx);
            hl->addStretch();
     //       vl->addLayout(hl);
      //  }
      //  {
            //    QString run=QChar(0x2B55);//(0x25C9);//(0x25BC);
            //   QPushButton *butPsix = new QPushButton("run "+psi);
        //    QHBoxLayout *hl = new QHBoxLayout;
            QPushButton *bRun = new QPushButton(tr("RUN")); 
            connect(bRun, SIGNAL(clicked()), this, SLOT(compute_Psi()));
            hl->addWidget(bRun);

            QPushButton *stopB = new QPushButton(tr("STOP")); 
            stopB->setShortcut(tr("Alt+S"));
            connect(stopB, SIGNAL(clicked()), this, SLOT(stopCalc()));
            hl->addWidget(stopB);
            vl->addLayout(hl);

            flgErase= new QCheckBox("Erase",wPlotPsi2);
            flgErase->setChecked(true);
//            hl->addStretch();
        hl->addWidget(flgErase);
        }

    }
}

void MainWindow::initPlotPhik() 
{
    wPlotPhi = new QDialog(this);
    QVBoxLayout *vl=new QVBoxLayout(wPlotPhi);
    PlotterDialog *wPhi = new PlotterDialog(wPlotPhi);
    wPlotPhi->setFont(QFont("Serif", 12, QFont::SemiCondensed )); 
    this->plotterPhi = ((PlotterDialog*)wPhi)->plotter();
    wPlotPhi->setWindowTitle(tr("Impulse distribution"));
    vl->addWidget(wPhi);
    {
        QHBoxLayout *hl = new QHBoxLayout();
        QPushButton * bsk = new QPushButton("&k scale");	
        connect(bsk,SIGNAL(clicked()),this,SLOT(slotScaleP()));
        hl->addWidget(bsk);		

        hl->addStretch();

        QPushButton * btn = new QPushButton("level &n");	
        connect(btn,SIGNAL(clicked()),this,SLOT(slotIntN()));
        hl->addWidget(btn);		

        QPushButton * btwp = new QPushButton("time &t");	
        connect(btwp,SIGNAL(clicked()),this,SLOT(slotWPacket()));
        hl->addWidget(btwp);

        flgErase= new QCheckBox("Erase",wPlotPhi);
        flgErase->setChecked(true);
        hl->addWidget(flgErase);

        hl->addStretch();
        psip_var = new QComboBox(this);
        psip_var->addItem(tr("Phi_n(k,n)"));
        psip_var->addItem(tr("sum(Phi_n(k,t))"));
        psip_var->setCurrentIndex(0);
        hl->addWidget(psip_var);

        QPushButton *bRun = new QPushButton(tr("RUN")); 
        connect(bRun, SIGNAL(clicked()), this, SLOT(compute_Phi()));
        hl->addWidget(bRun);

        QPushButton *stopB = new QPushButton(tr("STOP")); 
        stopB->setShortcut(tr("Alt+S"));
        connect(stopB, SIGNAL(clicked()), this, SLOT(stopCalc()));
        hl->addWidget(stopB);


        vl->addLayout(hl);
    }

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
        hl->addStretch();
        QPushButton * bsz = new QPushButton("&z scale");	
        connect(bsz,SIGNAL(clicked()),this,SLOT(slotScaleZ()));
        hl->addWidget(bsz);		

        U_type = new QComboBox(wPlotUx);
        U_type->addItem(tr("U(x)"));
        U_type->addItem(tr("U(x,z)"));
        U_type->setCurrentIndex(0);
        hl->addWidget(U_type);

        QPushButton *bRun = new QPushButton(tr("Show U")); 
        connect(bRun, SIGNAL(clicked()), this, SLOT(compute_Uxz()));
        hl->addWidget(bRun);

//        QPushButton *stopB = new QPushButton(tr("STOP")); 
//        stopB->setShortcut(tr("Alt+S"));
//        connect(stopB, SIGNAL(clicked()), this, SLOT(stopCalc()));
//        hl->addWidget(stopB);
//        hl->addStretch();
        vl->addLayout(hl);

    }

            {
 //           QGroupBox *gb = new QGroupBox(tr("Bound States: E<0"));
//            QVBoxLayout *vl = new QVBoxLayout;
            QHBoxLayout *hl = new QHBoxLayout;
            En_type = new QComboBox(this);
            En_type->addItem(tr("E_n & Psi_n"));
            En_type->addItem(tr("N(E)-step"));
            En_type->addItem(tr("B_{N+1}(E)"));
            En_type->addItem(tr("En(z)"));
            En_type->setCurrentIndex(0);
            hl->addWidget(En_type);

            QPushButton *butEn = new QPushButton(tr("RUN")); 
            connect(butEn, SIGNAL(clicked()), this, SLOT(Bound_States()));
            hl->addWidget(butEn);

            flgErase= new QCheckBox("Erase",wPlotUx);
            flgErase->setChecked(true);
            hl->addWidget(flgErase);

            hl->addStretch();
            vl->addLayout(hl);

        }

//        gb->setLayout(vl);

}
static void setSomeInitialU(PhysicalModel *m)
{
    UAsMW u = { 2, 5, 1, -10, 0 , 0};
    m->setUAsMW( u );
}

MainWindow::MainWindow(QWidget *parent, Qt::WFlags f)
: Nwb(1),nmaxLevel(10),nminLevel(0),nLevel(0), nmaxWP(7), nminWP(0),hnWP(1),  
E0(-10.), hE(0.01),time(0.), tmin(0.), tmax(1000.0), htime(0.1),Psi2(0.),
totalRT(1.),Ua(-10.0),aa(1.),Ub(0.),bb(0.8),xmin(-3.),xmax(8.),hx(0.01),
Ubias(0.),Emin(-10.),Emax(-0.01),Umin(-15.0),Umax(1.),kmax(10.),hk(0.005),
psixmin(-1.2),psixmax(1.5),zmin(0),zmax(1.), hz(0.01), zz(0.),
QMainWindow(parent,f), numOfCurve(1),numOfCurveNE(1), numOfCurveT(0), model(0), 
wpE_lo(5.), wpE_hi(15.), wpN(30), 
tableView(0),gbScales(0),gbIntervals(0),dialogUAsMW(0),tableViewEn(0), gbScaleX(0),
gbScaleZ(0),gbScaleP(0), gbScalePsi(0),
gbIntN(0),gbIntE(0),  gbWP(0),gbWPr(0),gbWPl(0),bgR(0) 
{
    this->model = new PhysicalModel();
    UAsMW u1 = { 2, 0.1, 1, -10, 0 , 0};
    model->setUAsMW( u1 );
    this->slotU1();
    UAsMW u2 = { 2, 5, 1, -10, 0 ,0};
    model->setUAsMW( u2 );
    this->slotU2();
    setSomeInitialU(this->model);
    this->initStatusBar();
    this->initPlotEnz();
    this->initPlotUx();
    this->initPlotT();
    this->initPlotPsix();
    this->initPlotPhik();
    this->initPlotNofE();
    this->initControlDockWindow();
    this->initMenuBar(); 
    init();
//    compute_En();
}

bool MainWindow::save()
{
    if (this->curFile.isEmpty())
    {
        return this->saveAs();
    }
    QString nV=this->curFile+"V";
    QFile fV(nV);
    //    QFile f(this->curFile);
    if(fV.exists()){
        int n=QMessageBox::warning(0,
            tr("Warning"),
            "File with this name has already existed,"
            "\n Do you want to rewrite it?",
            "Yes",
            "No",
            QString::null,
            0,
            1
            );
        if(n){
            return this->saveAs(); //выбираем новое имя
        }
    }
    //Saving the file!
    fV.open(QIODevice::WriteOnly|QIODevice::Truncate);

    QTextStream o(&fV);
    o << "Percolation model:\n";
    o << "Rows Cols Seed Conduct Temp Vg sigmaU Ex\n";
    /*        QString s1;
    s1.sprintf("%i %i %i %lg %lg %lg %lg% lg\n",int(this->rows),int(this->cols),int(this->seed),
    double(model->conductivity), double(this->T), double(this->U),
    double(this->sigmaU),double(this->Ex));
    o << s1;
    */
    fV.close();
    //    statusBar()->showMessage(tr("Saved '%1'").arg(fV), 2009);
    return TRUE;
}





void MainWindow::createStatusBar()
{

}



void MainWindow::help()
{
    static QMessageBox *about;
 /*   if (!about)
    {
              char buf[200];
//        MKLGetVersionString(buf,sizeof(buf)-1);
        buf[sizeof(buf)-1] = 0;
        about = new QMessageBox("Photon-assisted transmission",buf,
        QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
         
    }
    about->setButtonText(1, "Dismiss" );
    about->show();
*/
    }

void MainWindow::init()
{
}


WavePacket MainWindow::buildWPmE()
{
    WavePacket result;
    result.clear();
    if(model->Ebound.size()==0) model->findBoundStates();
    if(this->nmaxWP>=model->Ebound.size()) this->nmaxWP=model->Ebound.size()-1;
    if(this->nminWP<0) this->nminWP=0;
    int NwpEm=(this->nmaxWP-this->nminWP)/this->hnWP + 1;
    result.resize(NwpEm);
    int j=0;
    for(int i=this->nminWP;i<=this->nmaxWP;i+=this->hnWP)
    {
//        if(j>=NwpEm) break;  
        result[j].E = model->Ebound[i];
        j++;
    } 
    Emin=result[0].E;
    Emax=result[NwpEm-1].E;
    if(Emin==Emax) 
    {
        result[0].w =1.;
        return result;
    }
    double W=0;
    double nc=0.5*(this->nminWP+this->nmaxWP);
    double C=0.5*(this->nmaxWP-this->nminWP);
    C*=C;C=6/C;
    j=0;
    for(int i=this->nminWP;i<=this->nmaxWP;i+=this->hnWP)
    {
//    for(int i=0;i<NwpEm;i++)
        double z=(i-nc);z*=z*C;
        W += result[j].w = exp(-z);
        j++;
    }
    for(int i=0;i<NwpEm;i++)
        result[i].w /=W;
    return result;
}
/*WavePacket MainWindow::buildWPmE()
{
    WavePacket result;
    result.clear();
    if(model->Ebound.size()==0) model->findBoundStates();
    if(this->nmaxWP>=model->Ebound.size()) this->nmaxWP=model->Ebound.size()-1;
    if(this->nminWP<0) this->nminWP=0;
    int NwpEm=(this->nmaxWP-this->nminWP)/this->hnWP + 1;
    result.resize(NwpEm);
    int j=0;
    for(int i=this->nminWP;i<=this->nmaxWP;i+=this->hnWP)
    {
//        if(j>=NwpEm) break;  
        result[j].E = model->Ebound[i];
        j++;
    } 
    Emin=result[0].E;
    Emax=result[NwpEm-1].E;
    if(Emin==Emax) 
    {
        result[0].w =1.;
        return result;
    }
    double W=0;
    double Ec=0.5*(Emin+Emax);
    double C=(Emax-Emin)/2;C*=C;C=6/C;
    for(int i=0;i<NwpEm;i++)
    {
        double z=(result[i].E-Ec);z*=z*C;
        W += result[i].w = exp(-z);
    }
    for(int i=0;i<NwpEm;i++)
        result[i].w /=W;
    return result;
}*/
/*
WavePacket MainWindow::buildWPpE()
{
    std::vector<double> kN;
    WavePacket result;
    result.clear();
    result.resize(this->wpN);
    kN.resize(this->wpN);
    if(this->wpE_hi==this->wpE_lo||this->wpN==1)
    {
    result.resize(1);
    result[0].E=this->wpE_hi;
    result[0].w=1.;
    return result;
    }
    if(this->wpE_hi<this->wpE_lo)
    {
    double a=this->wpE_hi;
    double b=this->wpE_lo;
    this->wpE_hi=b;
    this->wpE_lo=a;
    }
    double kNmin=sqrt(2/hbar2 * model->m(model->N+1) * ( this->wpE_lo) );
    double kNmax=sqrt(2/hbar2 * model->m(model->N+1) * ( this->wpE_hi) );
    double dk=(kNmax-kNmin)/(this->wpN-1);
//    double de=(this->wpE_hi-this->wpE_lo)/(this->wpN-1);
    double a = 2/hbar2;
    for(int i=0;i<this->wpN;i++)
    {
        kN[i] = kNmin+dk*i;
        double EE=kN[i]; EE*=EE; EE=EE/a/model->m(model->N+1);
        result[i].E =EE;
    } 
    double W=0;
    double C=(kN[wpN-1]-kN[0])/2;C*=C;C=3/C;
    double kc=(kN[0]+kN[wpN-1])/2;
//    wpweightEi.resize(this->wpN);
    for(int i=0;i<wpN;i++)
    {
        double z=(kN[i]-kc);z*=z*C;
        W += result[i].w = exp(-z);
    }
    for(int i=0;i<wpN;i++)
        result[i].w /=W;
    return result;
}
*/
/*
WavePacket MainWindow::buildWPpE()
{
    std::vector<double> kN;
    WavePacket result;
    result.clear();
    result.resize(this->wpN);
    kN.resize(this->wpN);
    if(this->wpE_hi==this->wpE_lo||this->wpN==1)
    {
    result.resize(1);
    result[0].E=this->wpE_hi;
    result[0].w=1.;
    return result;
    }
    if(this->wpE_hi<this->wpE_lo)
    {
    double a=this->wpE_hi;
    double b=this->wpE_lo;
    this->wpE_hi=b;
    this->wpE_lo=a;
    }
    double de=(this->wpE_hi-this->wpE_lo)/(this->wpN-1);
    double a = 2;///hbar2;
    for(int i=0;i<this->wpN;i++)
    {
        result[i].E = this->wpE_lo+i*de;
        double c = a * model->m(model->N+1) * ( result[i].E);
        kN[i] = sqrt( c);
    } 
    double W=0;
    double C=(kN[wpN-1]-kN[0])/2;C*=C;C=3/C;
    double kc=(kN[0]+kN[wpN-1])/2;
//    wpweightEi.resize(this->wpN);
    for(int i=0;i<wpN;i++)
    {
        double z=(kN[i]-kc);z*=z*C;
        W += result[i].w = exp(-z);
    }
    for(int i=0;i<wpN;i++)
        result[i].w /=W;
    return result;
}
*/ 
WavePacket MainWindow::buildWPpE()
{
    WavePacket result;
    result.clear();
    std::vector<double> wpweightEi, wpEi;
    result.resize(this->wpN);
    if(this->wpE_hi==this->wpE_lo||this->wpN==1)
    {
    result.resize(1);
    result[0].E=this->wpE_hi;
    result[0].w=1.;
    return result;
    }
    if(this->wpE_hi<this->wpE_lo)
    {
    double a=this->wpE_hi;
    double b=this->wpE_lo;
    this->wpE_hi=b;
    this->wpE_lo=a;
    }
    double de=(this->wpE_hi-this->wpE_lo)/(this->wpN-1);
    for(int i=0;i<this->wpN;i++)
    {
        result[i].E = this->wpE_lo+i*de;
    } 
    double W=0;
    double C=(wpE_hi-wpE_lo)/2;C*=C;C=6/C;
    double Ec=(wpE_hi+wpE_lo)/2;
    double z0=wpE_hi-Ec+de;z0*=z0*C;
    double ex0=exp(-z0);
    for(int i=0;i<wpN;i++)
    {
        double z=(result[i].E-Ec);z*=z*C;
        W += result[i].w = exp(-z)-ex0;

    }
    for(int i=0;i<wpN;i++)
        result[i].w /=W;
    return result;

}
void MainWindow::WavePacketXoft()
{
    const int N = model->getN();

    WavePacket result;
    std::vector<double> data;
    std::vector<complex> expt;
    wPlotPsi2->show();
    wPlotPsi2->raise();
    bool flg=flgScale->isChecked();
    plotterPsi2x->scaleFixed=flg;
    if(flg)
    {
        PlotSettings ps;
        ps.minX = this->xmin;
        ps.maxX = this->xmax;
        ps.minY = psixmin;
        ps.maxY = psixmax;
        ps.numYTicks = 5;
        ps.numXTicks = 5;
        //    ps.adjust();
        plotterPsi2x->setPlotSettings(ps);
    }
    if(flgUx->isChecked()){
    double ymax=abs(this->Umin);
    if(ymax<abs(this->Umax)) ymax=abs(this->Umax);
    if(ymax==0) ymax=1;
    double scaleU=psixmax/ymax;
    if(scaleU==0) scaleU=0.05;
    addUx(this->xmin, this->xmax, scaleU, model, plotterPsi2x);
    }
    double y;
    this->numOfCurve=0;
    int ii=psi_type->QComboBox::currentIndex();
    int iwp;
    if(!this->bgR) iwp=1;
    else iwp=bgR->checkedId();
    switch(iwp)
    {
    case 2: // E_n
        result=buildWPmE();  
        break;
    case 1: //E>0
        result=buildWPpE();  
        break;
    default:
        break;
    }

    int M=result.size();
    Matrix<complex> kp(0, M-1, 0, N+1);
    Matrix<complex> ap(0, M-1, 0, N+1);
    Matrix<complex> bp(0, M-1, 0, N+1);
    expt.resize(M);
    for(int p=0;p<M;p++)
    {
        model->E0= result[p].E;
        model->build_k();
        model->build_ab();
        for(int n=0; n <= N+1; n++)
        {
            kp(p,n)=model->k(n);
            ap(p,n)=model->a(n);
            bp(p,n)=model->b(n);
        }
    }
    //-----------------------time-------
    for (double t=this->time; t>=this->tmin&&t<=this->tmax; t+=this->htime)
    {
        this->time=t;
        updateValues();
        if (getBreakStatus(0)) 
        {
            return;
        }
        data.clear();
        complex Psit=0;
        for(int p=0;p<M;p++)
        {

            complex c = exp(-complex(0, result[p].E*t));
            if(result[p].E- model->U(N+1) >0) c = c*exp(complex(0,real(kp(p,N+1))*this->xmax) );
            //            if(wpEi[p]- model->U(0) >0) c = c*exp(-complex(0,real(kp(p,0))*this->xmin) );
            expt[p]=c;
        }
        for(double x=this->xmin; x<=this->xmax; x+=this->hx)
            //        for(double x=model->Xmin; x<=model->Xmax; x+=dx)
        {
            model->x=x;
            complex Psit=(0.,0.);
            for(int p=0;p<M;p++)
            {
                model->E0= result[p].E;
                for(int n=0; n <= N+1; n++)
                {
                    model->k(n)=kp(p,n);
                    model->a(n)=ap(p,n);
                    model->b(n)=bp(p,n);
                }
                if (model->E0 < model->U(N+1))
                    model->b(N+1)=0.; 
                model->build_Psi();
                complex yy=model->psi;
                Psit += model->psi*result[p].w*expt[p];

            }
            switch(ii)
            {
            case 0: 
                y=squaremod(Psit);
                break; 
            case 1: 
                y=real(Psit);
                break;
            case 2: 
                y=imag(Psit);
                break;
            default:
                break; 
            }
            data.push_back(x);
            data.push_back(y);
        }
        this->plotterPsi2x->setCurveData(2,data);
    }
}
void MainWindow::WavePacketPoft()
{
    WavePacket result;
    wPlotPhi->show();
    wPlotPhi->raise();
    std::vector<double> data;
    std::vector<complex> expt;
    double y;
    this->numOfCurve=0;
//    bool flg=flgScale->isChecked();
    plotterPhi->scaleFixed=false;
 // E_n
        result=buildWPmE();  
    int M=result.size();
    const int N = model->getN();

    Matrix<complex> kp(0, M-1, 0, N+1);
    Matrix<complex> ap(0, M-1, 0, N+1);
    Matrix<complex> bp(0, M-1, 0, N+1);
    expt.resize(M);
    for(int p=0;p<M;p++)
    {
        model->E0= result[p].E;
        model->build_k();
        model->build_ab();
        for(int n=0; n <= N+1; n++)
        {
            kp(p,n)=model->k(n);
            ap(p,n)=model->a(n);
            bp(p,n)=model->b(n);
        }
    }

    //-----------------------time-------
    double kmin=-this->kmax;
    if(this->kmax<=0) kmax=10.;
    double dk=kmax/500;
    if(dk>this->hk) dk=this->hk;
    for (double t=this->time; t>=this->tmin&&t<=this->tmax; t+=this->htime)
    {
        data.clear();
        complex Psit=0;
        this->time=t;
        updateValues();
        if (getBreakStatus(0))
            return;
        for(int p=0;p<M;p++)
        {
            expt[p] = exp(-complex(0, result[p].E*t));
        }
        for(double kk=kmin; kk<=this->kmax; kk+=dk)
        {
            int N = model->getN();

            complex Psit=(0.,0.);
            model->kwave=kk;
            for(int p=0;p<M;p++)
            {
                model->E0= result[p].E;
                for(int n=0; n <= N+1; n++)
                {
                    model->k(n)=kp(p,n);
                    model->a(n)=ap(p,n);
                    model->b(n)=bp(p,n);
                }
                model->b(N+1) = 0.; 
                model->build_Phi();
                complex yy=model->phi;
                Psit += model->phi*result[p].w*expt[p];
            }
                y=squaremod(Psit);
            data.push_back(kk);
            data.push_back(y);
        }
        this->plotterPhi->setCurveData(2,data);
    }
}
void MainWindow::stopCalc()
{
     setBreakStatus(1);
}

static volatile int break_status;

void setBreakStatus(int newStatus)
{
    break_status = newStatus;
}

int getBreakStatus(int newStatus)
{
    qApp->processEvents(QEventLoop::AllEvents);
    int ret = break_status;
    break_status = newStatus;
    return ret;
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
         getBreakStatus(0);
         QWidget::keyPressEvent(event);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit(closed());
    //event->accept();
    event->ignore();
}
