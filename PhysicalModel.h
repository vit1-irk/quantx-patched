#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <QObject>
#include <QVector>
#include <vector>
#include <complex>

typedef std::complex<double> complex;

template <typename T>
class Vector
{
public:
    Vector() : l(0), u(0) {}
    Vector(int _l, int _u) : l(_l), u(_u) 
    {
        v.resize(_u-_l+1);
    }
    Vector(const Vector& other) : l(other.l), u(other.u)
    {
        resize(l,u);
        for (int i = 0; i < other.size(); ++i)
        {
            v[i] = other.v[i];
        }
    }
    Vector& operator = (const Vector& other)
    {
        resize(other.l,other.u);
        for (int i = 0; i < other.size(); ++i)
        {
            v[i] = other.v[i];
        }
        return *this;
    }
    ~Vector() {}
    void resize(int _l, int _u) 
    { 
        l = _l;
        u = _u;
        v.resize(_u - _l + 1);
    }
    int size() const { return v.size(); }
    int lower() const { return l; }
    int upper() const { return u; }
    void swap(Vector<T>& a) { v.swap(a.v); } 
    T& operator()(int i) { return v[i - l]; }
    void zero() { std::fill(v.begin(), v.end(), T(0)); }
    int operator != (const Vector<T>& other) const
    {
        for (int i = 0; i < v.size(); ++i)
        {
            if (v[i] != other.v[i]) return 1;
        }
        return 0;
    }
private:
    std::vector<T> v;
    int l, u;
};


template <typename T>
class Matrix
{
public:
    Matrix() : rl(0), ru(0), cl(0), cu(0) {}
    Matrix(int _rl, int _ru, int _cl, int _cu) 
        : rl(_rl), ru(_ru), cl(_cl), cu(_cu) 
    {
        v.resize((ru-rl+1)*(cu-cl+1));
    }
    ~Matrix() {}
    void resize(int _rl, int _ru, int _cl, int _cu) 
    { 
        rl = _rl;
        ru = _ru;
        cl = _cl;
        cu = _cu;
        v.resize((ru-rl+1)*(cu-cl+1));
    }
    int rlower() const { return rl; }
    int rupper() const { return ru; }
    int clower() const { return cl; }
    int cupper() const { return cu; }
    int size() const { return v.size(); }
    void swap(Matrix<T>& a) { v.swap(a.v); }
    T& operator()(int r, int c) { return v[(r-rl) + (ru-rl+1)*(c-cl)]; }
    void zero() { std::fill(v.begin(), v.end(), T(0)); }
private:
    std::vector<T> v;
    int rl, ru, cl, cu;
};

template<typename T> inline void swap(T& a, T& b) { T c = a; a = b; b = c; }
template<typename T> inline void swap(Vector<T>& a, Vector<T>& b) { a.swap(b); }
template<typename T> inline void swap(Matrix<T>& a, Matrix<T>& b) { a.swap(b); }


struct UAsMW
{
    int numberOfWells;
    double wa,wb,ua,ub,ubias;
    bool operator != (const UAsMW& o) 
    { 
        return numberOfWells != o.numberOfWells 
            || wa != o.wa || wb != o.wb 
            || ua != o.ua || ub != o.ub || ubias != o.ubias; 
    }
};

struct PhysicalModel : public QObject
{
    Q_OBJECT
public:
    PhysicalModel(QObject * parent = 0);

    UAsMW getUAsMW() const;
    void setUAsMW(const UAsMW&);

signals:
    void signalPotentialChanged();
    void signalEboundChanged();

public slots:
    void slotPotentialChanged();


private:
    int N;	 // actual number of inner intervals
public:
    Vector<double> d;      /* [u_width]  0..N widths */
    Vector<double> m;      /* [u_mass]   0..N+1 masses */

    //! Potential including bias
    Vector<double> U;      /* [u_energy] 0..N+1 constant potential (build by build_U) */
    Vector<double> U_d;

    //! Heterostructural potential
    Vector<double> Ui;      /* [u_energy] 0..N+1 constant potential (defined by user) */

    double E0;
    Vector<complex> k;      /* momentum on interval 0..N+1 for channels Ml..Mh */
    //    complex r, t; /* reflection & transmission magnitudes, Ml...Mh */
    //    Vector<double> psi_phases_old, psi_phases_base;
    //    Vector<double> psi_phases;/* Psi phases in channels */
    Vector<complex> a,b;
    QVector<double> Ebound;
    std::vector<int> NumberofZero;
    complex psi;
    complex phi;
    double Ub; // U bias over whole structure
    double	  x;	  /* [u_width] given point */
    double	  Xmin,Xmax,Umin,Umax;	  /* [u_width] given point */
    double	  Time;
    double kwave;
    int	  timeswitch;
    double	 RR;   /* Total reflection */
    double	 TT;   /* Total transmission */
    double	 totalRT;  /* sum of totalR and totalT */
    //    double   psi_phase_total_old,psi_phase_total_base;
    double	 Psi2;
    double	 Phi2;
    //    double	 psi_phasehth;	 /* Psi phase in h-th channel */
    double	 psi_real;   /* real of total Psi */
    double	 Phi_real;   /* real of total Psi */
    double	 psi_imag;   /* real of total Psi */
    double	 Phi_imag;   /* real of total Psi */

    double width, height, scalex, scaley; 
//    int iwidth, iheight, scalex, scaley; 
    int getN() const { return N; }

    void set_N(int N)
    {
        this->N = N;

        d.resize(0,N);
        m.resize(0,N+1);
        U.resize(0,N+1);
        Ui.resize(0,N+1);
        k.resize(0,N+1);
        a.resize(0,N+1);
        b.resize(0,N+1);


        this->Time = 0;
        this->timeswitch = 0;
        //        this->psi2_avgt = 0;
        //        this->psi_phase_total = 0;
        //        this->psi_phase_total_base = 0;
        //        this->psi_phase_total_old = 0;
        //        this->psi_phases.zero();
        //        this->psi_phases_base.zero();
        //        this->psi_phases_old.zero();
        this->psi_imag = 0;
        this->psi_real = 0;
        this->Phi_imag = 0;
        this->Phi_real = 0;

    }

    void set_E0(double e) { E0 = e; }	  /* [u_energy] incident wave energy */
    //#define fix(a,l,u) {if(a<(l))a=(l);if((u)<a)a=(u);}
public:
    void build_U();
    void build_k();
    void build_RT();
    void XUscales();
    void norm();
    void build_ab();	 /* u:r,k,d,m,fZ			r:a,b */
    void build_Psi();	 /* u:a,b,x,k,d 			r:Psi2 */
    void build_Phi();	 /* u:a,b,p,k,d 			r:Phi */
    int findNumberOfLevels(double E);
    int zeroPsi();
    double findOneLevel(double _emin, double _emax);
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
