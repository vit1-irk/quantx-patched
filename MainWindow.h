#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED
#include <QtGui>
#include <QWidget>
#include <QSplitter>
#include <QDialog>
#include <vector>
#include "plotter.h"
#include "myparam.h"
#include "PotentialViewMultiWell.h"
#include "WPparametersP.h"
#include "WPparametersM.h"
#include "Uparabolic.h"
#include "PotentialModel.h"
#include "PhysicalModel.h"
#include "PotentialViewMovable.h"
#include "MomentumView.h"
#include "EnzView.h"
#include "Zview.h"
#include "WavePacketXView.h"
#include "WavePacketKView.h"
#include "WaveFunctionView.h"
#include "TransmissionView.h"
struct WPE
{
    double E,w;
};

typedef std::vector<WPE>  WavePacket;

//---------------------------------------------------------------------
// Define main window 
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent=0, Qt::WindowFlags f = 0);
//   ~MainWindow()
    ~MainWindow(){ }


public slots:
    void help();
    void windowTopRight();
    void windowDownRight();
    void windowDownLeft();
    void windowTopLeft();
//    void slotScaleX();
//    void slotScaleZ();
//    void slotIntE();
    void slotSetPeriodicPotential(int really) { model->setPotentialType(really ? PERIODIC : FINITE); }

signals:
	void potentialChanged();
    void closed();

private slots:

    void updateMouseMovedTo(QPointF);
//    void createToolBars();
    void slotBound_States();
    void slotCompute_Psi();
    void slotCompute_Phi();
    void slotCompute_D();

    void slotRunWP();
    void slotRunEnz();

    void slotIntN();
    void slotIntE();
//    void slotIntT();
    void slotWPacket();
//     void slotScales();
    void slotScaleX();
    void slotScaleZ();
    void slotScaleP();
    void slotScalePsi();
//    void slotIntervals();
    void slotSetUxt();
    void slotSetEn();
    void slotSetUmwb();
    void slotSetWPEp();
    void slotSetWPEm();
    void slotSetTime();
    void slotSetUs();
    void  slotSetZ();

    void chooseFont();
    void defWP();
    void defSP();
    void init();
    bool save();
    bool saveAs();
    void openFile(const QString &path);
    void createStatusBar();
    void compute();
    void Bound_States();
    void compute_BE();
    void compute_En();
    void compute_PsiXatE();
    void compute_Psin();
    void compute_Psi();
    void compute_Psi_nz();
    void compute_Psi_z();
    void compute_PsixE();
    void compute_PhiofP();
    void compute_Phi();
    void compute_NE();
    void compute_D();
    void compute_TE();
    void compute_Tz();
    WavePacket buildWPpE();
    WavePacket buildWPmE();
    void WavePacketXoft();
    void WavePacketPoft();
    void TEz();
    void TzE();
    void compute_Enz();
    void showU();
    void showEn(double En);
    void compute_Phi_n();

//    void myU1();
//    void myU2();
    //    void slotU1();
//    void slotU2();
//    void Uxz(double z);
    void compute_Uxz();
//    void listLevels();
protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    QSplitter *splitterR;
    QSplitter *splitterL;
    QSplitter *splitterLR;
    void slotWindowsActivated(int);
    void slotWindows();
    void initPlotT(); 
    void initPlotPhik(); 
    void initPlotPsix(); 
    void initPlotUx(); 
    void initPlotEnz(); 
    void initPlotNofE(); 
    void initMenuBar();
    void initStatusBar();
    void initControlDockWindow();
    void initTable();
    void initTableEn();
    void updateValues();
    void setScalesPsi();
//    void initGraphicsView();
//----------Kostia
    void resizeEvent(QResizeEvent * event);
    PotentialViewMultiWell *dialogUAsMW;
    Uparabolic *dialogUparab;
    WPparametersM *dialogWPEm;
    WPparametersP *dialogWPEp;
    TimeView *dialogTime;
    Zview *dialogZ;
    QGroupBox *gbScales;
    QGroupBox *gbIntervals;
    QGroupBox *gbNumrange;
    QToolBox * toolBox;
    PotentialViewMovable *potentialViewMovable;
    MomentumView *momentumView;
    EnzView *enzView;
    WavePacketXView *wavePacketXView;
    WavePacketKView *wavePacketKView;
    WaveFunctionView *waveFunctionView;
    TransmissionView *transmissionView;
//    PotentialViewMovable *potentialViewMovable(PhysicalModel *m, QWidget *parent);
//    QGraphicsView *gv;
    QGroupBox *GBplots, *gbScaleX, *gbScaleZ, *gbScalePsi, *gbScaleP;
    QGroupBox  *gbIntN,*gbIntE, *gbWP,*gbWPl, *gbWPr;
    QWidget * control;
	QWidget * wfWidget;
//--------Kostia-----
    QFont myfont;
    QString curFile;
    QButtonGroup *typeRT;

    QButtonGroup *argRT;
    QComboBox *psi_type, *psix_var, *psip_var,*U_type, *En_type;
    QComboBox *T_type;
    MyParamI Nwb, nmaxLevel, nminLevel, nLevel,nmaxWP, nminWP,hnWP; 
    MyParamD E0,time,Ua,aa,Ub,bb,xmin,xmax,Emin,Emax,hE,Umin,Umax, kmax,hk;
    MyParamD psixmin,psixmax,hx,zz,zmin,zmax,hz;
    MyParamD wpE_lo, wpE_hi, wpN;
    MyParamD Psi2, Refl,Trans,totalRT,tmin, tmax,htime;
//    MyParamD Psi2, psi_phasehth, psit_real,psit_imag;
    QCheckBox *flgTypeU, *flgScale, *flgErase, *flgUx, *flgEraseT;
//    QVector<double> U1; //!< Inital potential values for z-animations
//    QVector<double> U2; //!< Final potential values for z-animations
//    QVector<double> d1; //!< Initial potential step widths for z-animations
//    QVector<double> d2; //!< Final potential step widths for z-animations
//    QVector<double> m1; //!< Initial potential step masses for z-animations
//    QVector<double> m2; //!< Final potential step masses for z-animations
//    double Ubias; 
//    double Ub1,Ub2; 
    QGroupBox *gbviewPsix, *gbviewPsixT,*gbPview,*gbviewM,*gbviewMT,*gbTEview, *gbEnzview;
//    QGroupBox *gbEnzview;
    QGroupBox *grrb;
    QRadioButton *rad1,*rad2;
    QButtonGroup *bgR;
    QPushButton *butEn;
    QPushButton *bRunPsi;
    QPushButton *bRunPhi;
    QPushButton *bRunD;
    
    QPushButton *bRunPsiXT;
    QPushButton *bRunEnz;

    int countW;
    int numOfCurveR,numOfCurveT,numOfCurveNE, numOfCurve, numOfCurveUx, numOfCurveEn;
    Plotter *plotterUx1;
    Plotter *plotterAddUx;
    Plotter *plotterUx;
    Plotter *plotNE;
    Plotter *plotterT;
    Plotter *plotterEnz;
    Plotter *plotterPsi2x;
    Plotter *plotterPhi;
    const QString captionX;
    QVector<int> index_for_sorted_En() const;
    std::vector<double> plotdataUx, plotdataR, plotdataT, plotdataPsi2, plotdataPhi, dataNE, plotdataEnz;
    QAction *exitAction, *aboutAction, *saveAction, *chooseFontAction, *mUx,*mUx1, *mVxt;//, *menuE;
    QMenu *file, *menuUx, *mE;
    std::vector<double> Elevels;
    int Number_of_Levels; 
    //    QComboBox *typeCond;
    PhysicalModel *model;

    QLabel *dispEnergy; // Display of Energy
    QLabel *dispZ; // Display of Energy
    QLabel *dispBias; // Display of Bias
    QLabel *dispTime; // Display of Ttot
    QLabel *dispT; // Display of Ttot
    QLabel *dispR; // Display of Rtot
    QLabel *dispRT; // Display of Rtot+Ttot
    QDialog *winPlotNE, *wPlotPsi2, *wPlotPhi, *wPlotEnz;
//    QDockWidget *wPlotUx;
    QDialog *wPlotUx;
    QWidget *wPlotT;
//    QDialog *wPlotT;
//    QWidget *winTableUx;
//    QDialog *winTableUx;
    QTableView *tableView;
    QTableView *tableViewEn;
//    QListWidget *listEn;
    QLabel *mouseAtX;
    QLabel *mouseAtY;

    QVBoxLayout *leftLayout,*leftTopLayout,*leftDownLayout, *rightDownLayout,*rightTopLayout,*rightLayout, *sideLayout;

//    QGroupBox *gbLeftTop;
//    QGroupBox *gbLeftDown;
//    QGroupBox *gbRightDown;
//    QGroupBox *gbRightTop;
    void window_TE();
    void window_Enz();
    void window_Ux_Psix();
    void window_psi_x();
    void window_psi_xt();
    void window_phi_k();
    void window_phi_kt();
    QComboBox *topLeftWin;
    QComboBox *topRightWin;
    QComboBox *downLeftWin;
    QComboBox *downRightWin;

};

#endif /*MAINWINDOW_H_INCLUDED*/
