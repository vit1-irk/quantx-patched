#include "PhysicalModel.h"
#include <cmath>

// Auxiliary class for finding energy levels
class ETree
{
    double emin,emax;
    PhysicalModel *model;
    int nemin, nemax;
    ETree *left, *right;
public:
    ETree(double _emin, double _emax,PhysicalModel *_model, int _nemin, int _nemax)
        : emin(_emin), emax(_emax), model(_model),
          nemin(_nemin), nemax(_nemax), left(0), right(0)
    {
    }
    ~ETree()
    {
        if(left) delete left;
        if(right) delete right;
    }

    void compute(QVector<double>& E)
    {
        double e,emid;
        int nlevels;
        if (nemax-nemin == 1)
        {
            e = model->findOneLevel(emin,emax);
            E.push_back(e);
            return;
        }
        if (nemax-nemin == 0)
        {
            return;
        }
        if(nemax-nemin > 1)
        {
        emid = (emax + emin)/2;
        nlevels = model->findNumberOfLevels(emid);
        if(abs(emax-emin)<1e-8) 
        {
         for(int i=1; i<=nemax-nemin; i++)
//         for(int i=1; i<=nlevels-nemin; i++)
         {
            E.push_back(emid);
         }
            return;
        }
        }
        left = new ETree(emin,emid,model,nemin,nlevels);
        right = new ETree(emid,emax,model,nlevels,nemax);
        left->compute(E);
        right->compute(E);
    }
};

void PhysicalModel::findBoundStates()
{
    double Umin = 1e300, Umax = -1e300;
    double q;
    int Nmax;
    Nmax = getN ();
    for (int i = 0; i < Nmax+1; ++i)
    {   
        q = this->U(i);
        if (q > Umax) Umax = q; 
        if (q < Umin) Umin = q;
    }
    if(this->U(0)>this->U(Nmax+1)) Umax=this->U(Nmax+1);
    else Umax=this->U(0);
//    if(this->U(Nmax+1)>0) Umax=0.;
//    else Umax=this->U(Nmax+1);
    Umin=Umin+1e-8;
    Umax=Umax-1e-6;
    int nmax = this->findNumberOfLevels(Umax);
    int nmin = 0;
    this->Ebound.clear();
    ETree *mytree = new ETree(Umin, Umax, this, nmin, nmax);
    mytree->compute(this->Ebound);
    delete mytree;
    //emit(signalEboundChanged());
}
/*************************************************
  k(n) = sqrt( 2*m(n)/h2*(E0-U(n)) );
-------------------------------------------------*/
void PhysicalModel::build_k() /* input:m,E0,U --- result:k*/
{
 restart:
    double a = 2/hbar2;
    for (int n = 0; n <= this->N + 1; n++)
    {
//        this->k(n) = waveNumber(n);
        double c = a * this->m(n) * (this->E0 - this->U(n) );
        {
            if(fabs(c) < 1e-10) //TODO: process correctly the case when E0 = U(n)
            {
                this->set_E0(this->E0 + 1e-10);
                goto restart;
            }
            if (c>0)
                this->k(n) = complex(sqrt( c), 0);
            else
                this->k(n) = complex(0, sqrt(-c));
        }
    }
}

/*-----------------------------------------------
  This function calculates reflection/transmission coefficients

  totalR = r#(i)*r(i) 

  totalT = t#*t k(N+1)/k(0)*m(0)/m(N+1) было

  TT=|t|^2*k(0)/k(N+1)*m(N+1)/m(0); в другую сторону
  RR=|r|^2
-------------------------------------------------*/
void PhysicalModel::build_RT() /* input:r,t,k,m  --- result:R,T,totalR,totalT,totalRT */
{
    double aa = real(k(0))==0 ? 0 : m(N+1)/m(0);
//    complex r = this->a(N+1)/this->b(N+1);
//    complex t = this->b(0)/this->b(N+1);
    complex r = this->a(N+1);
    complex t = this->b(0);
    double rr,tt;
    rr=squaremod(r);
    tt=squaremod(t);

    // k's are:
    // k0 - incident wave vector
    // kl - reflected vector (going to the left)
    // kr - transmitted vector (going to the right)
    double k0,kr;
    RR=rr;
    TT=tt;
    if(this->E0<=this->U(0)) 
    {
        RR=1;
        TT=0;
    }
    else
    {
        k0=real(k(0));
        if(this->E0<=this->U(this->N+1))
        {
            TT=0;
            RR=1.;
        }
        else
        {
            kr=real(this->k(this->N+1));
            TT=tt*k0/kr*aa;
            RR=rr;  
        }
    }  
    
    totalRT = RR + TT;
}
/*
void Model::build_RT() 
{
    double aa = real(k(0))==0 ? 0 : m(0)/m(N+1);
    complex r = this->b(0);
    complex t = this->a(N+1);
    double rr,tt;
    rr=squaremod(r);
    tt=squaremod(t);

    // k's are:
    // k0 - incident wave vector
    // kl - reflected vector (going to the left)
    // kr - transmitted vector (going to the right)
    double k0,kr;
    if(this->E0<=this->U(0)) 
    {
        RR=1;
        TT=0;
    }
    else
    {
        k0=real(k(0));
        if(this->E0<this->U(this->N+1))
        {
            TT=0;
            RR=1.;
        }
        else
        {
            kr=real(k(N+1));
            TT=tt*kr/k0*aa;
            RR=rr;  
        }
    }  
    
    totalRT = RR + TT;
}
*/

//#define fix(a,l,u) {if(a<(l))a=(l);if((u)<a)a=(u);}

/*-----------------------------------------------
  Makes U(x) = Ui(x) + Uc*x
-------------------------------------------------*/
void PhysicalModel::build_U() /* input:Ub,Ui --- result:U */
{
    if (! need_build_U)
        return;

    U(0) = Ui(0);
    
    double width=0; 
    for (int n=1; n <= this->N; n++) 
        width += this->d(n);

    double pos=0;
    for(int n=this->N; n >= 0; n--)
    {
        pos += this->d(n)/2.;
        U(n) = Ui(n) + pos/width * this->Ub;
        pos += this->d(n)/2.;
    }
    U_d = d;
//    U(N+1) = Ui(N+1) + this->Ub;
/*    for(int n=1; n <= this->N; n++)
    {
        pos += this->d(n)/2.;
        U(n) = Ui(n) + pos/width * this->Ub;
        pos += this->d(n)/2.;
    }
    U(N+1) = Ui(N+1) + this->Ub;
*/
    need_build_U = false;
    emit(signalPotentialChanged());
}

void PhysicalModel::XUscales() /* input:Ub,Ui --- result:U */
{
    double x,y;
    double ymin = 1e300, ymax = -1e300;
    x=0;
    build_U(); 
    for(int n=1;n<=this->N;n++)
    {
        x += d(n);
        if(n==this->N) Xmax=x;
        y = this->U(n);
        if (y > ymax) ymax = y; 
        if (y < ymin) ymin = y;
    }
    if(this->U(this->N+1)>ymax) ymax=this->U(this->N+1);
    Xmax=1.3*Xmax;
    Xmin=-0.3*Xmax;  
    Umax=1.1*ymax;
    Umin=1.1*ymin;
    if(Umax==0) Umax=0.5*abs(Umin);
    if(Umin==0) Umin=-0.1*abs(Umax);
//--------------------------
    this->width=(Xmax-Xmin);
    this->height=(Umax-Umin);
//    this->iwidth=300;
//    this->iheight=250;
//    this->scalex=this->iwidth/(Xmax-Xmin);
//    this->scaley=this->iheight/(Umax-Umin);
    this->scalex=this->width/(Xmax-Xmin);
    this->scaley=this->height/(Umax-Umin);
//------------------
}
inline double module(complex& a){ return fabs(real(a))+fabs(imag(a)); }


/*-----------------------------------------------
This function calculates the coefficients a & b.
-------------------------------------------------*/
void PhysicalModel::build_ab() /* u:r,k,d,m r:a,b */
{
    /* set ..,0,1,0,.. and ..,r,r,r,.. */
    complex up,dn, aa, bb, zz;
    /*    if(this->E0-this->U(0)>0)
    { 
    a(N+1)=1;
    b(N+1)=0;
    for(int i=N+1; i>=1; i--)
    {
    up = 0.5*exp(complex(0.,1.)*this->k(i-1)*this->d(i-1));
    dn = 0.5*exp(-complex(0.,1.)*this->k(i-1)*this->d(i-1));
    complex K12= (this->k(i)/this->k(i-1))*(this->m(i-1)/this->m(i));
    complex KPa= (complex(1.,0)+K12)*dn;
    complex KMa= (complex(1.,0)-K12)*dn;
    complex KPb= (complex(1.,0)+K12)*up;
    complex KMb= (complex(1.,0)-K12)*up;
    aa=a(i)*KPa+b(i)*KMa;        
    bb=a(i)*KMb+b(i)*KPb;  
    a(i-1)=aa;
    b(i-1)=bb;
    }
    for(int i=N+1; i>=0; i--)
    {
    b(i)=b(i)/a(0);
    a(i)=a(i)/a(0);
    }
    }

    else
    */
    {
        a(0)=0. ;
        b(0)=1.;
        for(int i=0; i<=N; i++)
        {
            up = 0.5*exp(complex(0.,1.)*this->k(i)*this->d(i));
            dn = 0.5*exp(-complex(0.,1.)*this->k(i)*this->d(i));
            complex K12= (this->k(i)/this->k(i+1))*(this->m(i+1)/this->m(i));
            complex KPa= (complex(1.,0)+K12)*up;
            complex KMa= (complex(1.,0)-K12)*dn;
            complex KPb= (complex(1.,0)+K12)*dn;
            complex KMb= (complex(1.,0)-K12)*up;
            complex aa=a(i)*KPa+b(i)*KMa;        
            complex bb=a(i)*KMb+b(i)*KPb;  
            a(i+1)=aa;
            b(i+1)=bb;
        }
        if(this->E0-this->U(N+1)>0)
        {
            for(int i=0; i<=N+1; i++)
            {
                a(i)=a(i)/b(N+1);
                b(i)=b(i)/b(N+1);
            }
        }
    }

}
void PhysicalModel::norm()
{
    double s=0.5*(squaremod(this->b(0))/imag(this->k(0))+squaremod(this->a(this->N+1))/imag(this->k(this->N+1)));
    for (int n = 1; n <= this->N; n++)
    {
      complex aj=this->a(n);
      complex bj=this->b(n);
      double a= this->E0-this->U(n);
      if(a>0)
      {
      double arg= real(this->k(n))*this->d(n);
      if(arg!=0)
      {
      s = s + this->d(n)*(( squaremod(aj)+squaremod(bj) )+sin(arg)/arg*real(aj*conj(bj)*exp(complex(0,1.)*arg)+bj*conj(aj)*exp(-complex(0,1.)*arg)));
      }
      else s = s + this->d(n)*(( squaremod(aj)+squaremod(bj) )+real(aj*conj(bj)+bj*conj(aj)));
      
      }
      else
      {
      double arg= imag(this->k(n))*this->d(n);
      if(arg!=0)
      {
      s = s + this->d(n)*real((aj*conj(bj)+bj*conj(aj))+sinh(arg)/arg*(squaremod(aj)*exp(-arg)+squaremod(bj)*exp(arg)));
      }
      else s = s + this->d(n)*real((aj*conj(bj)+bj*conj(aj))+(squaremod(aj)+squaremod(bj)));
      }
    }
    for (int n = 0; n <= this->N + 1; n++)
    {
     this->a(n)=this->a(n)/sqrt(s);
     this->b(n)=this->b(n)/sqrt(s);
    }
}
/*-----------------------------------------------
Calculates Psi(x) at x.
-------------------------------------------------*/
void PhysicalModel::build_Psi() /* input:a,b,x,k,d ------ result:Psi2 */
{
//   psi(x)=a(i)*exp(ik_j*(x-x_{j-1})+b(i)*exp(-ik_j*(x-x_{j-1})
//   x_{j-1}<=x<=x_j

    /* first determine n  so that x belongs to d(n)
    ----------------------------------------------*/
    double xt=x;
    int n;
    
    if(this->E0-this->U(N+1) <0&&this->E0-this->U(0)) norm();
    for(n=0;n<=N;n++)
    {
        xt-=d(n);
        if(xt<0)
        {
            xt += d(n);
            break;
        }
    }
    /* obtain Psi2 at x
    -------------------*/
         complex up=exp( complex(0,1)*(  k(n)*xt ) );
        complex dn=exp( complex(0,1)*( -k(n)*xt  ) );
//        complex up=exp( complex(0,1)*(  k(n)*xt  ) );
//        complex dn=exp( complex(0,1)*( -k(n)*xt  ) );
        this->psi = a(n)*up + b(n)*dn;
        Psi2 = squaremod(psi);
        psi_real=real(psi);
        psi_imag=imag(psi);
}
/*-----------------------------------------------
Calculates Phi(p) at p.
-------------------------------------------------*/
void PhysicalModel::build_Phi() /* input:En,a,b,x,k,d ------ result:Phi(p) */
{
        this->norm();
        double xr=0.;
//        double zr,zi;
//        double zr,zi,z0,z1;
        double kw = this->kwave;
        this->phi=complex(0.,0.);
        complex exp_xr;
        for(int n=1; n<=this->N; n++)
           {
           xr+=d(n);

           complex kj=this->k(n);
           complex kjdn=this->k(n)*d(n);
           exp_xr = exp(-complex(0.,kw*xr));
           complex exp_kjdjp=exp(complex(0,1)*kjdn);
           complex exp_kwdjm=exp(complex(0,-kw*d(n)));
           complex exp_kjdjm=exp(complex(0,-1)*kjdn);
           complex exp_kwdjp=exp(complex(0,kw*d(n)));
           if(kw!=kj&&kw!=-kj)
           {
               phi = phi + complex(0, -1.)*exp_xr*(a(n)*(exp_kjdjp-exp_kwdjp)/(kj-kw)-b(n)*(exp_kjdjm-exp_kwdjp)/(kj+kw));
           }
           else
           {
               complex exp_xlm = exp(complex(0.,-kw*(xr-d(n))));
               complex exp_xlp = exp(complex(0.,kw*(xr-d(n))));
               if(kw==kj&&kw!=0)  phi = phi + exp_xlm*(a(n)*d(n)+b(n)*exp_kwdjp*sin(kw*d(n))/kw);
               if(kw==-kj&&kw!=0) phi = phi + exp_xlp*(b(n)*d(n)+a(n)*exp_kwdjm*sin(kw*d(n))/kw);
               if(kw==kj && kw==0) phi = phi + d(n)*(a(n)+b(n));
           }
           //}
//  zr=real(phi);
//  zi=imag(phi);
    }
       phi= phi+ b(0)/(imag(this->k(0))-complex(0,kw));//+a(N+1)*exp(complex(0.,-kw*xr))/(this->k(N+1)-kw));
//        phi= phi+complex(0.,1.)*b(0)/(this->k(0)+kw);//+a(N+1)*exp(complex(0.,-kw*xr))/(this->k(N+1)-kw));
        phi= phi+a(N+1)*exp(complex(0.,-kw*xr))/(imag(this->k(N+1))+complex(0.,kw));
        
        Phi2 = squaremod(phi);
        Phi_real=real(phi);
        Phi_imag=imag(phi);
       }
int PhysicalModel::zeroPsi()
{
    int Nmax, nzero, nzero_i, n_total;
    Nmax = getN ();
    nzero=0;
    for (int i = 1; i <= Nmax; ++i)
    {
        nzero_i=0; 
        if(this->E0>this->U(i))
        {
        double arg = real(k(i))*d(i); 
        double  psi_j   = real(a(i)*exp(complex(0.,arg))+ b(i)*exp(complex(0.,-arg)));
        double cj = psi_j/real(a(i)+b(i));
        double cs = cos(arg);
        double sn = sin(arg);
        double phase_j= atan((cs-cj)/sn);
        double arg_j=arg+phase_j;
        int j=0;
            while(arg_j>0.5*M_PI*(1+2*j))
            {
              j++;
             }
            nzero_i=j;
         nzero=nzero+nzero_i;
        }
        else 
        {
        double arg = imag(k(i))*d(i); 
        double psi2=real(a(i)+b(i));
        double psi1=real(a(i)*exp(-arg)+b(i)*exp(arg));
        if(psi1>0&&psi2<0||psi1<0&&psi2>0)
        {
            nzero=nzero+1;
        }
        }
     n_total=nzero;
    }
        return nzero;
}
int PhysicalModel::findNumberOfLevels(double E)
{       
        int Nmax, nzero;
        Nmax = getN ();
        this->set_E0(E);
//        this->E0 = E;
        this->build_k();
        this->build_ab();
        nzero=zeroPsi(); //number of zeros of wave function inside (0, x_Nmax) at E near zero
        double psi1=real(a(Nmax+1)+b(Nmax+1));
        double psi2=real(b(Nmax+1));
        if(psi1>0&&psi2<0||psi1<0&&psi2>0) 
        {return nzero+1;} 
        else
        {return nzero;}
}
 double PhysicalModel::findOneLevel(double emin, double emax)
{
        double E =(emin +emax)*.5;
        this->set_E0(E);
        this->build_k();
        this->build_ab();
        double y=real(this->b(this->N+1));
        this->set_E0(emin);
        this->build_k();
        this->build_ab();
        double y1=real(this->b(this->N+1));
        double E1=emin;
        if(y>0&&y1>0||y<0&&y1<0) 
        { 
         this->set_E0(emax);
         this->build_k();
         this->build_ab();
         y1=real(this->b(this->N+1));
         E1=emax;
         }
        while(fabs(E-E1)>1.e-12)
        {
         this->set_E0((E +E1)*.5);;
         this->build_k();
         this->build_ab();
         double y=real(this->b(this->N+1));
        if(y>0&&y1>0||y<0&&y1<0) 
        { E1=this->E0;
          y1=y;
         }
        else
        {
          E=this->E0;
        }
        }
        return this->E0;
}

UAsMW PhysicalModel::getUAsMW() const
{
    UAsMW u;
    u.numberOfWells = 1;
    u.wa = 5;
    u.wb = 1;
    u.ua = -10;
    u.ub = 0;
    u.ubias = 0;
    return u;
}

void PhysicalModel::setUAsMW(const UAsMW& u)
{
    this->set_N(2*u.numberOfWells - 1);
    this->m(0) = 0.5;
    this->d(0) = 0;
    this->Ui(0) = 0;
    this->Ub = u.ubias; 
    for (int n = 1; n <= u.numberOfWells; n++)
    {
        int i = 2*n-1;
        this->Ui(i) = u.ua;
        this->Ui(i+1) = u.ub;
        this->d(i) = u.wa;
        if (i < 2*u.numberOfWells - 1)
            this->d(i+1) = u.wb;
        this->m(i) = 0.5;
        this->m(i+1) = 0.5;
    }
    this->Ui(this->N+1) = 0;
    this->m(this->N+1) = 0.5;
    this->build_U(); 
    //emit(signalPotentialChanged());
}

PhysicalModel::PhysicalModel(QObject *parent)
: QObject(parent),
  N(0), E0(0), psi(0), Ub(0), x(0), 
 Xmin(0),Xmax(0),Umin(0),Umax(0),
 Time(0), kwave(0),
 timeswitch(0),
 RR(0), TT(0), totalRT(0), Psi2(0), Phi2(0), 
 psi_real(0), Phi_real(0),
 psi_imag(0), Phi_imag(0),
 width(5), height(5), scalex(1), scaley(1)
// iwidth(0), iheight(0), scalex(1), scaley(1)
{
}

void PhysicalModel::slotPotentialChanged()
{
    Vector<double> saveU = U;
    Vector<double> saved = U_d;
    build_U();
//    XUscales();
    if (saveU != U||saved != U_d)
    {
        emit(signalPotentialChanged());
    }

    QVector<double> saveEbound = Ebound;
    findBoundStates();
    if (saveEbound != Ebound)
    {
        emit(signalEboundChanged());
    }
}

void PhysicalModel::set_Ui_d_m(const QVector<double>& _Ui,
                               const QVector<double>& _d,
                               const QVector<double>& _m)
{
    bool changed = false;

    int _N = _Ui.size() - 2;
    if (_N != Ui.size() - 2)
    {
        changed = true;
        this->set_N(_N);
    }
    for (int n = 0; n <= _N+1; ++n)
    {
        if (Ui(n) != _Ui[n])
        {
            Ui(n) = _Ui[n];
            changed = true;
        }
        if (m(n) != _m[n])
        {
            m(n) = _m[n];
            changed = true;
        }
        if (0 < n && n <= N && d(n) != _d[n])
        {
            d(n) = _d[n];
            changed = true;
        }
    }
    if (changed)
    {
        need_build_U = true;
    }
}

void PhysicalModel::set_d(int n, double v)
{
    if (d(n) != v)
    {
        d(n) = v;
        need_build_U = true;
    }
}
void PhysicalModel::set_Ui(int n, double v)
{
    if (Ui(n) != v)
    {
        Ui(n) = v;
        need_build_U = true;
    }
}
void PhysicalModel::set_m(int n, double v)
{
    if (m(n) != v)
    {
        m(n) = v;
        need_build_U = true;
    }
}
