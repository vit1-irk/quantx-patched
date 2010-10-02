#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED
#include <QtGui>
//#include <qtoolbar.h>
#include <QWidget>
#include <vector>
#include "plotter.h"
#include "myparam.h"
//#include "PotentialScene.h"
#include "PotentialViewMultiWell.h"
#include "PotentialModel.h"
struct Model;


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

signals:
	void potentialChanged();
    void closed();

private slots:
//    void createToolBars();
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
    void compute_PsixE();
    void compute_PhiofP();
    void compute_Phi();
    void compute_NE();
    void compute_T();
    void compute_TE();
    void compute_Tz();
    WavePacket buildWPpE();
    WavePacket buildWPmE();
    void TEz();
    void TzE();
    void compute_Enz();
    void showU();
    void showEn(double En);
    void compute_Phi_n();
    void WavePacketXoft();
    void WavePacketPoft();
    void stopCalc();
    void slotU1();
    void slotU2();
    void Uxz(double z);
    void compute_Uxz();
//    void listLevels();
protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    void slotWindows();
    void slotWindowsActivated(int);
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
//    void initGraphicsView();
//----------Kostia
    void resizeEvent(QResizeEvent * event);
    PotentialViewMultiWell *dialogUAsMW;
    QGroupBox *gbScales;
    QGroupBox *gbIntervals;
    QGroupBox *gbNumrange;
    QToolBox * toolBox;
	QGraphicsScene * scene;
	QGraphicsView * view;
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
//     std::vector<double> wpweightEi;
    MyParamI Nwb, nmaxLevel, nminLevel, nLevel,nmaxWP, nminWP,hnWP; 
    MyParamD E0,time,Ua,aa,Ub,bb,xmin,xmax,Ubias,Emin,Emax,hE,Umin,Umax, kmax,hk;
    MyParamD psixmin,psixmax,hx,zz,zmin,zmax,hz;
    MyParamD Ubiasmin,Ubiasmax,hUbias, wpE_lo, wpE_hi, wpN;
    MyParamD Psi2, Refl,Trans,totalRT,tmin, tmax,htime;
//    MyParamD Psi2, psi_phasehth, psit_real,psit_imag;
    QCheckBox *flgScale, *flgErase, *flgUx;
//    int Erase, wave_packet_type;
    Vector<double> U1; //!< Inital potential values for z-animations
    Vector<double> U2; //!< Final potential values for z-animations
    Vector<double> d1; //!< Initial potential step widths for z-animations
    Vector<double> d2; //!< Final potential step widths for z-animations

    QGroupBox *grrb;
    QRadioButton *rad1,*rad2;
    QButtonGroup *bgR;
    int numOfCurveR,numOfCurveT,numOfCurveNE, numOfCurve, numOfCurveUx, numOfCurveEn;
    Plotter *plotterUx1;
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
    Model *model;

    QLabel *dispEnergy; // Display of Energy
    QLabel *dispZ; // Display of Energy
    QLabel *dispBias; // Display of Bias
    QLabel *dispTime; // Display of Ttot
    QLabel *dispT; // Display of Ttot
    QLabel *dispR; // Display of Rtot
    QLabel *dispRT; // Display of Rtot+Ttot
    QDialog *winPlotNE, *wPlotT, *wPlotPsi2, *wPlotPhi, *wPlotUx, *wPlotEnz;
//    QWidget *winTableUx;
//    QDialog *winTableUx;
    QTableView *tableView;
    QTableView *tableViewEn;
//    QListWidget *listEn;
};

int getBreakStatus(int newStatus = 0);
void setBreakStatus(int newStatus);

#endif /*MAINWINDOW_H_INCLUDED*/
