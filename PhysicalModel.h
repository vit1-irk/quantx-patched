#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <QObject>
#include <QVector>
#include <QRectF>
#include <QPair>
#include <complex>
//#include <cmath>
typedef std::complex<double> complex;

#include "Matrix.h"
struct WPEw
{
    double E,w;
};

typedef std::vector<WPEw>  WavePacketEw;

struct UAsMW
{
    int numberOfWells;
    double wa,wb,ua,ub;//,ubias;
    bool operator != (const UAsMW& o) 
    { 
        return numberOfWells != o.numberOfWells 
            || wa != o.wa || wb != o.wb 
            || ua != o.ua || ub != o.ub;// || ubias != o.ubias; 
    }
};

struct LevelNumberParameters
{
    int nmin, nmax, hn;
    bool operator != (const LevelNumberParameters& o) 
    { 
        return nmin != o.nmin 
            || nmax != o.nmax || hn != o.hn; 
    }
};
struct EmWP
{
    int nmin, nmax, hn;
    bool operator != (const EmWP& o) 
    { 
        return nmin != o.nmin 
            || nmax != o.nmax || hn != o.hn; 
    }
};

struct EpWP
{
    int numberOfModes;
    double E_low, E_high;
    bool operator != (const EpWP& o) 
    { 
        return numberOfModes != o.numberOfModes
            || E_low != o.E_low 
            || E_high != o.E_high; 
    }
};

struct Uparab
{
    int numberOfIntervals;
    double U0, Width;
    bool operator != (const Uparab& o) 
    { 
        return numberOfIntervals != o.numberOfIntervals
            || U0 != o.U0 
            || Width != o.Width; 
    }
};

struct TimeParameters
{
    double time,tmin,tmax,ht;
    bool operator != (const TimeParameters& o) 
    { 
        return time != o.time
            ||tmin != o.tmin 
            ||tmax != o.tmax 
            || ht  != o.ht; 
    }
};

struct ScalesUParameters
{
    double Xmin,Xmax,Hx;
    double Psimin,Psimax;
    double Umin,Umax;
    bool operator != (const ScalesUParameters& o) 
    { 
        return Hx != o.Hx
            ||Xmin != o.Xmin 
            ||Xmax != o.Xmax 
            ||Umin != o.Umin 
            ||Umax != o.Umax 
            ||Psimin != o.Psimin 
            ||Psimax != o.Psimax; 
    }
};

struct ScalePsinParameters
{
    double Xmin,Xmax,Hx;
    double Psinmin,Psinmax;
    bool operator != (const ScalePsinParameters& o) 
    { 
        return Hx != o.Hx
            ||Xmin != o.Xmin 
            ||Xmax != o.Xmax 
            ||Psinmin != o.Psinmin 
            ||Psinmax != o.Psinmax; 
    }
};
struct ScaleWPXParameters
{
    double Xmin,Xmax,Hx;
    double WPXmin,WPXmax;
    bool operator != (const ScaleWPXParameters& o) 
    { 
        return Hx != o.Hx
            ||Xmin != o.Xmin 
            ||Xmax != o.Xmax 
            ||WPXmin != o.WPXmin 
            ||WPXmax != o.WPXmax; 
    }
};
struct ScaleWPKParameters
{
    double Kmin,Kmax,Hk;
    double WPKmin,WPKmax;
    bool operator != (const ScaleWPKParameters& o) 
    { 
        return Hk != o.Hk
            ||Kmin != o.Kmin 
            ||Kmax != o.Kmax 
            ||WPKmin != o.WPKmin 
            ||WPKmax != o.WPKmax; 
    }
};

struct ScalePhinParameters
{
    double Kmin,Kmax,Hk;
    double Phinmin,Phinmax;
    bool operator != (const ScalePhinParameters& o) 
    { 
        return Hk != o.Hk
            ||Kmin != o.Kmin 
            ||Kmax != o.Kmax 
            ||Phinmin != o.Phinmin 
            ||Phinmax != o.Phinmax; 
    }
};

struct zParameters
{
    double z,zmin,zmax,hz;
    bool operator != (const zParameters& o) 
    { 
        return z != o.z
            ||zmin != o.zmin 
            ||zmax != o.zmax 
            || hz  != o.hz; 
    }
};

enum PotentialType { FINITE, PERIODIC };

struct PhysicalModel : public QObject
{
    Q_OBJECT
public:
    PhysicalModel(QObject * parent = 0);
    EpWP getEpWP() const;
    void setEpWP(const EpWP&);
    EmWP getEmWP() const;
    void setEmWP(const EmWP&);
    LevelNumberParameters getLevelNumberParameters() const;
    void setLevelNumberParameters(const LevelNumberParameters&);
    ScaleWPXParameters getScaleWPXParam() const;
    void setScaleWPXParam(const ScaleWPXParameters&);
    ScaleWPKParameters getScaleWPKParam() const;
    void setScaleWPKParam(const ScaleWPKParameters&);
    ScalePsinParameters getScalePsinParam() const;
    void setScalePsinParam(const ScalePsinParameters&);
    ScalePhinParameters getScalePhinParam() const;
    void setScalePhinParam(const ScalePhinParameters&);
    ScalesUParameters getScalesUParam() const;
    void setScalesUParam(const ScalesUParameters&);
    Uparab getUparab() const;
    void set_z(double v);

    void setUparab(const Uparab&);
//    zParameters getzParam();
    zParameters getzParam() const;
    TimeParameters getTimeParam() const;
    void  setTimeParam(const TimeParameters&);
//    void  setzParam(zParameters&);
    void  setzParam(const zParameters&);
    UAsMW getUAsMW() const;
    void setUAsMW(const UAsMW&);
    QVector<double>  getPsiOfXT(double t, double xmin, double xmax, int npoints, int viewWF);//, bool needBuildWavePacket);
    QVector<double>  getPsiOfKT(double kmin, double kmax, int npoints);
signals:

    void signalScaleWPXChanged();
    void signalScaleWPKChanged();
    void signalPotentialChanged();
    void signalEnergyChanged(double);
    void signalTransmissionChanged(double);
    void signalEboundChanged();
    void signalScalesUChanged();
    void signalScalePsinChanged();
    void signalScalePhinChanged();
    void signalLevelNumberChanged();
    void signalTimeChanged(double);
    void signalZChanged(double);
    void signalScaleZChanged();
    void signalWavePacketChanged();
public slots:
    void slotU1();
    void slotU2();

private:
    int N;	 // actual number of inner intervals
    QVector<double> d;      /* [u_width]  0..N+1 widths */
    QVector<double> m;      /* [u_mass]   0..N+1 masses */
    //! Heterostructural potential
    QVector<double> Ui;      /* [u_energy] 0..N+1 constant potential (defined by user) */
    //! U bias over whole structure
    double Ub;
    double E0;
    QVector<double> U1; //!< Inital potential values for z-animations
    QVector<double> U2; //!< Final potential values for z-animations
    QVector<double> d1; //!< Initial potential step widths for z-animations
    QVector<double> d2; //!< Final potential step widths for z-animations
    QVector<double> m1; //!< Initial potential step masses for z-animations
    QVector<double> m2; //!< Final potential step masses for z-animations
    double Ub1,Ub2; 
    double Ubias; 
    int numberOfLevels, LevelNmin, LevelNmax, LevelHn; // min, max and step for the level numbers of wavefunctions at E<0
    zParameters zold;
 
    // wavepacket:
    int type_of_WP;
    int wpN;
    double wpE_lo, wpE_hi;
    WavePacketEw EWofWP;
    Matrix<complex> kp;
    Matrix<complex> ap;
    Matrix<complex> bp;
    int nminWP, nmaxWP, hnWP; // min, max and step for the level numbers of the wavepacket at E<0

    bool need_build_U;
    bool need_build_En;
    bool need_build_WP;

    void markUchanged(); 
    void markWPchanged();

public:
//    void slotU1();
//    void slotU2();
    void set_Uxz(double z);
    void set_Uxz_forTz(double z);
    void getTatE(); 
    bool flagBondaryCondition;
//    double getTatE(double E);
  double get_U(int n);//comment
    double get_Ub() const { return Ub; }
    double getUbias() const { return Ubias; }
    double get_E0() const { return E0; }
    double get_Time() const { return this->time; }
//---------WavePacket---------------- 
    void set_type_of_WP(int iwp);
    int get_type_of_WP() const { return this->type_of_WP;};
   //En<0
    int get_nminWP() const { return nminWP; };
    int get_nmaxWP() const { return nmaxWP; };
    int get_hnWP() const { return hnWP; };
    void set_nminWP(int n1);
    void set_nmaxWP(int n2);
    void set_hnWP(int hn);
   //E>0
    double get_EminWP() const { return this->wpE_lo; };
    double get_EmaxWP() const { return this->wpE_hi; };
    int get_NofWP() const { return this->wpN; };
    void set_EminWP(double v);
    void set_EmaxWP(double v);
    void set_NofWP(int n);

//potential:
    void set_Ub(double _Ub);
    void setUbias(double _Ubias);
//    QVector<double> get_U() const { return U; }//added
    QVector<double> get_Ui() const { return Ui; }
    double get_Ui(int n) const { return Ui.at(n); }
//    double get_U(int n) const { return U.at(n); }//added
    void   set_Ui(int n, double v);
    QVector<double> get_d() const { return d; }
    double get_d(int n) const { return d.at(n); }
    void   set_Energy(double v);
    void   set_Time(double v);
     void   set_d(int n, double v);
    void   set_d(int n1, double v1,int n2, double v2);
    double get_m(int n) const { return m.at(n); }
    void   set_m(int n, double v);
    void set_Ui_d_m_Ub(const QVector<double>& Ui, const QVector<double>& d,const QVector<double>& m, const double& Ub);
//    void set_E_lo_E_hi_wpN(double E_lo,double E_hi, int wpN);
//    void set_nminWP_nmaxWP_hnWP(int nminWP, int nmaxWP, int hnWP);
    QVector<double> getEn();
    double getEn(int n);
    double Umin,Umax,Psimin,Psimax,Psinmin,Psinmax,Xmin,Xmax,Hx;  
    double Phinmin,Phinmax,Kmin,Kmax,Hk;  
    double WPKmin,WPKmax;  
    double WPXmin,WPXmax;  
    QPair<double,double> getUminUmax();
    QPair<double,double> getXminXmax();
 /*   int get_LevelNmin() const { return this->LevelNmin; };
    int get_LevelNmax() const { return this->LevelNmax; };
    int get_LevelHn() const { return LevelHn; };
    void set_LevelNmin(int n1);
    void set_LevelNmax(int n2);
    void set_LevelHn(int hn);
*/
    void split_d(int n,double fraction);
    void remove_d(int n);

    friend class ETree;

    void build_k();
    void build_RT();
    void build_ab();	 /* u:r,k,d,m,fZ			r:a,b */
    double	 RR;   /* Total reflection */
    double	 TT;   /* Total transmission */
    QVector<complex> a,b;
    QVector<complex> s11,s12,s21,s22;
    double kwave;
    void build_Phi();	 /* u:a,b,p,k,d 			r:Phi */
    double	 Phi2;
    double	  x;	  /* [u_width] given point */
    double	 Psi2;
    double	 psi_real;   /* real of total Psi */
    double	 psi_imag;   /* real of total Psi */
    void build_Psi();	 /* u:a,b,x,k,d 			r:Psi2 */
    QVector<complex> k;      /* momentum on interval 0..N+1 for channels Ml..Mh */
    complex psi;
    complex phi;
    double funEn;

    void setPotentialType(PotentialType);
    PotentialType getPotentialType() const { return typeOfU; }

private:
    PotentialType typeOfU;
    int Nperiod;
    //! Potential including bias
    QVector<double> U;      /* [u_energy] 0..N+1 constant potential (build by build_U) */
//    QVector<double> U_d;

    //    complex r, t; /* reflection & transmission magnitudes, Ml...Mh */
    //    Vector<double> psi_phases_old, psi_phases_base;
    //    Vector<double> psi_phases;/* Psi phases in channels */
    QVector<double> Ebound;
    QVector<double> EbandUp;
    QVector<double> EbandDown;
    std::vector<int> NumberofZero;
    //double	  Xmin,Xmax,Umin,Umax;	  /* [u_width] given point */
    double	  time,tmin,tmax,ht;
    double	  zz,zmin,zmax,hz;
//    int	  timeswitch;
    double	 totalRT;  /* sum of totalR and totalT */
    //    double   psi_phase_total_old,psi_phase_total_base;
    //    double	 psi_phasehth;	 /* Psi phase in h-th channel */
    double	 Phi_real;   /* real of total Psi */
    double	 Phi_imag;   /* real of total Psi */

public:
//    double width, height, scalex, scaley; 
//    int iwidth, iheight, scalex, scaley; 
    int getN() const { return N; }

    void set_N(int N)
    {
        this->N = N;

        d.resize(1+N+1);
        m.resize(1+N+1);
        U.resize(1+N+1);
        Ui.resize(1+N+1);
        k.resize(1+N+1);
        a.resize(1+N+1);
        b.resize(1+N+1);
        s12.resize(1+N+1);
        s21.resize(1+N+1);
        s11.resize(1+N+1);
        s22.resize(1+N+1);


        this->psi_imag = 0;
        this->psi_real = 0;
        this->Phi_imag = 0;
        this->Phi_real = 0;

    }

    void set_E0(double e) { E0 = e; }	  /* [u_energy] incident wave energy */
    //#define fix(a,l,u) {if(a<(l))a=(l);if((u)<a)a=(u);}

    QRectF getGoodViewportAtU() const;
    int findNumberOfLevels(double E);

      QVector<double> getPsiOfX(double E, double xmin, double xmax, int npoints, int viewWF);
      QVector<double> getPhiOfk(double E, double kmin, double kmax, int npoints);
      QVector<double> getTransmissionOfE(double Emin, double Emax, int npoints); 
      QVector<double> getTransmissionOfZ(double Zmin, double Zmax, int npoints); 

private:
    void matching( );
    void Smatrix();
    void build_U();
    void set_WPmE();
    void set_WPpE();

    void norm();
    int zeroPsi();
    double findOneLevel(double _emin, double _emax);
    void findBandEdges(double emin, double emax, double Edown, double Eup);

    void findBoundStates();
};

/* CONSTANTS
------------*/   

const double u_eV	= 1.;//1.6021892e-19; /* J */
const double u_meV	= 1.;//1e-3*u_eV;	 /* J */
const double u_me	= 1.;//9.109534e-31;  /* kg */
const double u_nm	= 1.;//1e-9; 		 /* m */
const double u_hbar 	= 1.;//1.0545887e-34; /* J*s */

/* when E[eV], m[me], d[nm]
---------------------------*/
const double hbar = 1.;//u_hbar/sqrt(u_me*u_meV)/u_nm;
const double hbar2 = 1.;//hbar*hbar;
const double ps2tt = 1.;//u_meV*1e-12/u_hbar;

#define u_energy "meV"
#define u_width  "nm"
#define u_mass	 "m_e"
#define u_time	 "ps"

inline double squaremod(complex& c) 
{
    return real(c)*real(c) + imag(c)*imag(c); 
}

const double M_PI = 3.1415926535897932384626433832795;

#endif
