#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <QObject>
#include <QVector>
#include <QRectF>
#include <QPair>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <complex>
//#include <cmath>
typedef std::complex<double> complex;

#include "Matrix.h"
struct WPEw
{
    double E,w;
};

struct WPEcw
{
    double w;
    complex E;
};

typedef std::vector<WPEw>  WavePacketEw;
typedef std::vector<WPEcw>  WavePacketEcw;

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
struct SettingParameters
{
    int lineWidth;
    bool operator != (const SettingParameters& o)
    {
        return lineWidth != o.lineWidth;
    }
};

struct ScalesUParameters
{
    double Xmin,Xmax,Hx;
//    double Psimin,Psimax;
    double Umin,Umax;
    bool operator != (const ScalesUParameters& o)
    {
        return Hx != o.Hx
            ||Xmin != o.Xmin
            ||Xmax != o.Xmax
            ||Umin != o.Umin
            ||Umax != o.Umax;
//            ||Psimin != o.Psimin
//            ||Psimax != o.Psimax;
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
struct gParameters
{
    double g,gmin,gmax,hg;
    bool operator != (const gParameters& o)
    {
        return g != o.g
            ||gmin != o.gmin
            ||gmax != o.gmax
            || hg  != o.hg;
    }
};

enum PotentialType { FINITE, PERIODIC, QUASISTATIONARY, SEMIPERIODIC };

struct PhysicalModel : public QObject
{
    Q_OBJECT
public:
    QVector<QPointF> redBoundary;
    QVector<QPointF> yellowBoundary;
    QVector<QPointF> greenBoundary;
    QVector<QPointF> blueBoundary;

    PhysicalModel(QObject * parent = 0);
    EpWP getEpWP() const;
    void setEpWP(const EpWP&);
    EmWP getEmWP() const;
    void setEmWP(const EmWP&);
    void set_EmaxEmin(double E1, double E2);
//    double get_Gmin const { return Gmin; };

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
    void set_G(double v);

    void setUparab(const Uparab&);
//    zParameters getzParam();
    zParameters getzParam() const;
    gParameters getGParam() const;
    TimeParameters getTimeParam() const;
    void  setTimeParam(const TimeParameters&);
    void setSettingParameters(const SettingParameters &);
    SettingParameters getSettingParameters() const;
    void  setzParam(const zParameters&);
    void  setGParam(const gParameters&);
    UAsMW getUAsMW() const;
    void setUAsMW(const UAsMW&);
    QVector<double>  getPsiOfXT(double t, double xmin, double xmax, int npoints, int viewWF);//, bool needBuildWavePacket);
    QVector<double>  getPsiOfKT(double kmin, double kmax, int npoints);
    void readFromXml(QXmlStreamReader *r);
    void writeToXml(QXmlStreamWriter *w);
signals:

    void signalScaleWPXChanged();
    void signalScaleWPKChanged();
    void signalPotentialChanged();
    void signalEnergyChanged(double);
    void signalTransmissionChanged(double);
    void signalQaChanged(double);
    void signalLevelNumberChanged(int);
    void signalEboundChanged();
    void signalScalesUChanged();
    void signalScalePsinChanged();
    void signalScalePhinChanged();
    void signalLevelParametersChanged();
    void signalTimeChanged(double);
    void signalZChanged(double);
    void signalGChanged(double);
    void signalScaleZChanged();
    void signalScaleGChanged();
    void signalWidthChanged();
    void signalWavePacketChanged();
public slots:
    void slotU1();
    void slotU2();

private:
    complex lambda;

    int N;	 // actual number of inner intervals
    QVector<double> d;      /* [u_width]  0..N+1 widths */
    QVector<double> m;      /* [u_mass]   0..N+1 masses */
    //! Heterostructural potential
    QVector<double> Ui;      /* [u_energy] 0..N+1 constant potential (defined by user) */
    //! U bias over whole structure
    double E0, GG, Gmin, Gmax, dG;
    complex Ecmplx;
    double Emin,Emax, hE;
    int width_of_Line;
    int N1;	 // actual number of inner intervals of U1
    QVector<double> U1; //!< Inital potential values for z-animations
    int N2;	 // actual number of inner intervals of U2
    QVector<double> U2; //!< Final potential values for z-animations
    QVector<double> d1; //!< Initial potential step widths for z-animations
    QVector<double> d2; //!< Final potential step widths for z-animations
    QVector<double> m1; //!< Initial potential step masses for z-animations
    QVector<double> m2; //!< Final potential step masses for z-animations
//    double Ub1,Ub2;
    double Ubias;
    int numberOfLevels, LevelNmin, LevelNmax, LevelHn; // min, max and step for the level numbers of wavefunctions at E<0
//    zParameters zold;
    int levelNumber;
    // wavepacket:
    int type_of_WP;
    int wpN;
    double wpE_lo, wpE_hi;
    WavePacketEw EWofWP;
    WavePacketEcw EcWofWP;
    QVector<complex> lambdap;
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
    int colorB;
    void set_Uxz(double z);
    void set_Uxz_forTz(double z);
    void getTatE();
    void getTnatE();
    void getQaatE();
    bool flagBondaryCondition;
    double get_U(int n);//comment
    double getUbias() const { return Ubias; }
    double get_E0() const { return E0; }
    complex get_Ecmplx() const { return Ecmplx; }
    QVector<double> EnOfRealB;
    QVector<double> EnOfImagB;
    QVector<double> getEnOfReal();
    QVector<double> getEnOfImagB();
    QVector<double> Egn;

//    void set_G(double v) {GG=v;}
    complex bN1;
    double get_Time() const { return this->time; }
//---------WavePacket----------------
//    void set_type_of_WP(int iwp) { type_of_WP=iwp; }

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
    void set_LevelNumber(int n);
    int get_LevelNumber() const {return this->levelNumber; };
    void getColors(double Emin, double Emax, double dE, double Gmin, double Gmax, double dG);

//potential:
    void setUbias(double _Ubias);
//    QVector<double> get_U() const { return U; }//added
    QVector<double> get_Ui() const { return Ui; }
    QVector<complex> getEnquasi() const { return Equasi; }

    double get_Ui(int n) const { return Ui.at(n); }
    double get_U1(int n) const { return U1.at(n); }
    double get_U2(int n) const { return U2.at(n); }
    void   set_Ui(int n, double v);
    QVector<double> get_d() const { return d; }
    QVector<double> get_m() const { return m; }
    double get_d(int n) const { return d.at(n); }
    double get_d1(int n) const { return d1.at(n); }
    double get_d2(int n) const { return d2.at(n); }
    void   set_Energy(double v);
    void   set_Time(double v);
     void   set_d(int n, double v);
    void   set_d(int n1, double v1,int n2, double v2);
    double get_m(int n) const { return m.at(n); }
    double get_m1(int n) const { return m1.at(n); }
    double get_m2(int n) const { return m2.at(n); }
    void   set_m(int n, double v);
    void set_Ui_d_m(const QVector<double>& Ui, const QVector<double>& d,const QVector<double>& m);
    void set_U1_d1_m1(const QVector<double>& U1, const QVector<double>& d1,const QVector<double>& m1);
    void set_U2_d2_m2(const QVector<double>& U2, const QVector<double>& d2,const QVector<double>& m2);
    QVector<double> getEn();
    QVector<double> getEgn(bool what);
    double getEn(int n);
    QVector<complex> getEquasi();
    complex getEquasi(int n);
    double get_qa() const { return qa; }
    double Umin,Umax,Psimin,Psimax,Psinmin,Psinmax,Xmin,Xmax,Hx;
    double Phinmin,Phinmax,Kmin,Kmax,Hk;
    double WPKmin,WPKmax;
    double WPXmin,WPXmax;
    QPair<double,double> getUminUmax();
    QPair<double,double> getXminXmax();
    void split_d(int n,double fraction);
    void remove_d(int n);

    friend class ETree;

    void build_k();
    void build_kcmplx();
    void build_RT();
    void build_ab();	 /* u:r,k,d,m,fZ			r:a,b */
    double	 RR;   /* Total reflection */
    double	 TT;   /* Total transmission */
    QVector<complex> a,b;
    QVector<complex> s11,s12,s21,s22;
    double kwave;
    void build_Phi();	 /* u:a,b,p,k,d 			r:Phi */
    void build_Phi_per();	 /* u:a,b,p,k,d 			r:Phi */
    double	 Phi2;
    double	  x;	  /* [u_width] given point */
    double	 Psi2;
    double	 psi_real;   /* real of total Psi */
    double	 psi_imag;   /* real of total Psi */
    void build_Psi();	 /* u:a,b,x,k,d 			r:Psi2 */
    void build_Psi_per();	 /* u:a,b,x,k,d 			r:Psi2 */
    QVector<complex> k;      /* momentum on interval 0..N+1 for channels Ml..Mh */
    complex psi;
    complex phi;
    double funEn;
    double discrim;
    double findEdgeOfBand(double emin, double emax, double dmin, double dmax);
//    void findBands();
//    int zeroPsi();
    int zeroPsiPer(double E);
//    int zeroPsiQuasi(double E);


    void setPotentialType(PotentialType);
    PotentialType getPotentialType() const { return typeOfU; }
    int Nperiod;

private:
    //! Potential including bias
    PotentialType typeOfU;
    QVector<double> U;      /* [u_energy] 0..N+1 constant potential (build by build_U) */
    //    complex r, t; /* reflection & transmission magnitudes, Ml...Mh */
    QVector<double> Ebound;
    QVector<complex> Equasi;
//    QVector<double> EbandUp;
//    QVector<double> EbandDown;
    std::vector<int> NumberofZero;
    //double	  Xmin,Xmax,Umin,Umax;	  /* [u_width] given point */
    double	  time,tmin,tmax,ht;
    double	  zz,zmin,zmax,hz;
    double qa;
//    int	  timeswitch;
    double	 totalRT;  /* sum of totalR and totalT */
    double	 Phi_real;   /* real of total Psi */
    double	 Phi_imag;   /* real of total Psi */

public:
//    double width, height, scalex, scaley;
//    int iwidth, iheight, scalex, scaley;
    int getN() const { return N; }
    int getN1() const { return N1; }
    int getN2() const { return N2; }

    void set_N(int N)
    {
        this->N = N;
//        PotentialType type = getPotentialType();
//    if(type==PERIODIC)
//    {
/*        d.resize(N);
        m.resize(N);
        U.resize(N);
        Ui.resize(N);
        k.resize(N);
        a.resize(N);
        b.resize(N);
        s12.resize(N);
        s21.resize(N);
        s11.resize(N);
        s22.resize(N);*/
//    }
//    if(type==FINITE)
    {
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
    }

        this->psi_imag = 0;
        this->psi_real = 0;
        this->Phi_imag = 0;
        this->Phi_real = 0;
    }
    void set_N1(int N)
    {
        this->N1 = N;

        d1.resize(1+N+1);
        m1.resize(1+N+1);
        U1.resize(1+N+1);
    }
    void set_N2(int N)
    {
        this->N2 = N;

        d2.resize(1+N+1);
        m2.resize(1+N+1);
        U2.resize(1+N+1);
    }
    void set_type_of_WP(int iwp) { type_of_WP=iwp; }
    void set_E0(double e); 	  /* [u_energy] incident wave energy */
    void set_Ecmplx(complex e); 	  /* [u_energy] incident wave energy */
    void set_E00(double e) { E0 = e; }	  /* [u_energy] incident wave energy */
    //#define fix(a,l,u) {if(a<(l))a=(l);if((u)<a)a=(u);}

    QRectF getGoodViewportAtU() const;
    int findNumberOfLevels(double E);
    int findNumberOfQuasiLevels(double E);

      QVector<double> getQuasiPsiOfX(complex E, double xmin, double xmax, int npoints, int viewWF, bool tail);
      QVector<double> getPsiOfX(double E, double xmin, double xmax, int npoints, int viewWF, bool tail);
      QVector<double> getPsiOfX_per(double E, double xmin, double xmax, int npoints, int viewWF);
      QVector<double> getPhiOfk(double E, double kmin, double kmax, int npoints);
      QVector<double> getPhiOfkPer(double kmin, double kmax);
      QVector<double> getTransmissionOfE(double Emin, double Emax, int npoints);
      QVector<double> getTransmissionOfZ(double Zmin, double Zmax, int npoints);
//      void findEdgeOfBands(double Emin, double Emax);
complex findOneQuasiLevel(double _emin, double _emax);
//complex findOneQuasiLevelG(complex Ec0, complex Ec1);

private:
    complex findOneQuasiLevelG(complex Ec0, complex Ec1);
    void matching( );
    void matching_per( );
    void Smatrix();
    void Smatrix_per();
//    void Smatrix_quasi();
    void build_U();
    void set_WPmE();
    void set_WPpE();

    void norm();
//    void norm_per();
    int zeroPsi();
    int zeroPsiQuasi();
    double findOneLevel(double _emin, double _emax);
    void findZerosOfReImbN1(bool first);
    double findZeroOfImagbN1(double E1, double b1, double E2, double b2);
    double findZeroOfRealbN1(double E1, double b1, double E2, double b2);
    void findBoundStates();
    void findQuasiStates();
    void findBands();

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

inline void skipUnknownElement(QXmlStreamReader *r)
{
    Q_ASSERT(r->isStartElement());

    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (r->isStartElement())
            skipUnknownElement(r);
    }
}

#endif
