#include "PhysicalModel.h"
#include <cmath>
#include <QRectF>
#include <QPolygonF>

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
class ETreePer
{
    double emin,emax;
    PhysicalModel *model;
    int nemin, nemax;
    double dmin, dmax;
    ETreePer *left, *right;
public:
    ETreePer(double _emin, double _emax,PhysicalModel *_model, int _nemin, int _nemax, double _dmin, double _dmax)
        : emin(_emin), emax(_emax), model(_model),
        nemin(_nemin), nemax(_nemax), dmin(_dmin), dmax(_dmax), left(0), right(0)
    {
    }
    ~ETreePer()
    {
        if(left) delete left;
        if(right) delete right;
    }

    void compute(QVector<double>& E)
    {
        double e,emid, dmid;
        //        int nlevels;
        int nzero;
 /*       if (nemax-nemin == 1)
        {
            if(dmin>0&&dmax>0||dmin<0&&dmax<0)
            {
                emid = (emax + emin)/2;
                nzero = model->zeroPsiPer(emid);//findNumberOfLevels(emid);
                dmid  = model->discrim;
                if(abs(emax-emin)<1e-8)
                {
                    for(int i=1; i<=nemax-nemin; i++)
                    {
                        E.push_back(emid);
                    }
                    return;
                }
            }
            else
            {
                e = model->findEdgeOfBand(emin,emax,dmin,dmax);
                E.push_back(e);
                return;
            }
        }*/
        if (nemax-nemin == 0)
        {
            if(dmin>0&&dmax>0) return;
            if(dmin<0&&dmax<0) 
            {
                double he=(emax-emin)/100;
                nzero = model->zeroPsiPer(emin+he);
                double dminP  = model->discrim;
                double difmin=dminP-dmin;
                nzero = model->zeroPsiPer(emax-he);
                double dmaxM  = model->discrim;
                double difmax=dmax-dmaxM;
                if (difmax>0&&difmin>0||difmax<0&&difmin<0) 
                {
                    return;
                }
                else
                {
                    emid = (emax + emin)/2;
                    nzero = model->zeroPsiPer(emid);//findNumberOfLevels(emid);
                    dmid  = model->discrim;
                    if(abs(emax-emin)<1e-9)
                    {
                        if(abs(dmid)<1e-9) E.push_back(emid);
                        return;
                    }
                    left = new ETreePer(emin,emid,model,nemin,nzero, dmin, dmid);
                    right = new ETreePer(emid,emax,model,nzero,nemax, dmid, dmax);
                    left->compute(E);
                    right->compute(E);
                    return;
                }
            }
            else
            {
                e = model->findEdgeOfBand(emin,emax,dmin,dmax);
                E.push_back(e);
                return;
            }
        }
        if(nemax-nemin >= 1)
        {
            emid = (emax + emin)/2;
            nzero = model->zeroPsiPer(emid);//findNumberOfLevels(emid);
            dmid  = model->discrim;
            if(abs(emax-emin)<1e-8)
            {
                for(int i=1; i<=nemax-nemin; i++)
                {
                   if(abs(dmid)<1e-8) E.push_back(emid);
                }
                return;
            }
        }
        left = new ETreePer(emin,emid,model,nemin,nzero, dmin, dmid);
        right = new ETreePer(emid,emax,model,nzero,nemax, dmid, dmax);
        left->compute(E);
        right->compute(E);
    }
};
double PhysicalModel::findEdgeOfBand(double emin, double emax, double dmin, double dmax)
{ 
    int nmax, nmin, nmid;
    double E = emax;
    double y=dmax;
    double y1=dmin;
    double E1=emin;
    while(fabs(E-E1)>1.e-12)
    {
        double Ec=(E +E1)*.5;
        nmax=zeroPsiPer(Ec);
        double yc=this->discrim;
        if(yc>0&&y1<0||yc<0&&y1>0)
        {
            E=Ec;
            y=yc;
        }
        else
        {
            E1=Ec;
            y1=yc;
        }
    }
        return this->E0;
}
int PhysicalModel::zeroPsiPer(double E)
{
        this->set_E0(E);
        build_k_per();
        build_ab();
        return this->zeroPsi();
}
void PhysicalModel::findBands()
{
    double dE=1e-5;
    double Eold=this->E0;
    double E, emax;
    QPair<double,double> umin_umax = getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;
    int nmax = this->zeroPsiPer(Umax);
    double dmax = this->discrim;
    int nmin = this->zeroPsiPer(Umin);
    double dmin = this->discrim;
    this->Ebound.clear();
    ETreePer *mytree = new ETreePer(Umin, Umax, this, nmin, nmax, dmin, dmax);
    mytree->compute(this->Ebound);
    int NN=Ebound.size();
    int jj;
    double ee;
        for (int i = 0; i < Ebound.size(); ++i)
        {
            double e=Ebound[i];
            ee=e;
            jj=i;
         }
    delete mytree;
    this->E0=Eold;
}

QPair<double,double> PhysicalModel::getUminUmax()
{
    if (need_build_U)
    {
        build_U();
        need_build_U = false;
    }
    double Umin = 1e300, Umax = -1e300;
    double q;
    int Nmax;
    Nmax = getN ();
    if(this->typeOfU==FINITE)
//    if(this->typeOfU==0)
    {
        for (int i = 0; i < Nmax+1; ++i)
        {
            q = this->U[i];
            if (q > Umax) Umax = q;
            if (q < Umin) Umin = q;
        }
        if(this->U[0] > this->U[Nmax+1]) Umax=this->U[Nmax+1];
        else Umax=this->U[0];
    }
    else
    {
        for (int i = 0; i < Nmax+1; ++i)
        {
            q = this->U[i];
            if (q > Umax) Umax = q;
            if (q < Umin) Umin = q;
        }
    }
    Umin=Umin+1e-8;
    Umax=Umax-1e-8;
    return QPair<double,double>(Umin,Umax);
}
QPair<double,double> PhysicalModel::getXminXmax()
{
    double Xmax,Xmin;
    if (need_build_U)
    {
        build_U();
        need_build_U = false;
    }
    double x=0.;
    int Nmax;
    Nmax = getN ();
    if(this->typeOfU==FINITE)
    {
        for (int i = 1; i < Nmax+1; ++i)
        {
            x += this->d[i];
        }
        Xmax=1.5*x;
        Xmin=-0.5*x;
    }
    else
    {
//        d[0]=1;
//        for (int i = 0; i < Nmax+1; ++i)
        for (int i = 1; i < Nmax; ++i)
        {
            x = this->d[i];
        }
        Xmax=this->Nperiod*x;
        Xmin=-d[0];
    }
    return QPair<double,double>(Xmin,Xmax);
}

void PhysicalModel::findBoundStates()
{
    double dE=1e-5;
    double Eold=this->E0;
    double E, emax;
    QPair<double,double> umin_umax = getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;
    int nmax = this->findNumberOfLevels(Umax);
    this->numberOfLevels=nmax;
    int nmin = 0;
    this->Ebound.clear();
    ETree *mytree = new ETree(Umin, Umax, this, nmin, nmax);
    mytree->compute(this->Ebound);
    int NN=Ebound.size();
    int jj;
    double ee;
        for (int i = 0; i < Ebound.size(); ++i)
        {
            double e=Ebound[i];
            ee=e;
            jj=i;
        }
    delete mytree;
/*    if(this->typeOfU==PERIODIC)
    {
        this->EbandUp.clear();
        this->EbandDown.clear();
        EbandUp=Ebound;
        double emin=Umin;
        for (int i = 0; i < Ebound.size(); ++i)
        {
            if(i==0) emin=Umin;
            else emin=Ebound[i-1];
            emax=Ebound[i];
            E=findOneLevel(emin,Ebound[i]);
            EbandDown.push_back(E);
        }
    }*/
    this->E0=Eold;
}
/*************************************************
  k(n) = sqrt( 2*m(n)/h2*(E0-U(n)) );
-------------------------------------------------*/
void PhysicalModel::build_k() /* input:m,E0,U --- result:k*/
{
 restart:
    double a = 2/hbar2;
    if(this->typeOfU==PERIODIC)
{
    U[0]=U[N];
    d[0]=d[N];
    m[0]=m[N];
    U[N+1]=U[1];
    d[N+1]=d[1];
    m[N+1]=m[1];
/*    U[N+1]=U[0];
    double dd=1;
    d[0]=dd;
    d[N+1]=d[0];
    m[N+1]=m[0];*/
}

    for (int n = 0; n <= this->N + 1; n++)
    {
        double c = a * this->m[n] * (this->E0 - this->U[n] );
        {
            if(fabs(c) < 1e-10) //TODO: process correctly the case when E0 = U(n)
            {
                this->set_E0(this->E0 + 1e-10);
                goto restart;
            }
            if (c>0)
                this->k[n] = complex(sqrt( c), 0);
            else
                this->k[n] = complex(0, sqrt(-c));
        }
    }
}
/*************************************************
  k(n) = sqrt( 2*m(n)/h2*(E0-U(n)) );
-------------------------------------------------*/
void PhysicalModel::build_k_per() /* input:m,E0,U --- result:k*/
{
 restart:
    double a = 2/hbar2;
{
    U[0]=U[N];
    d[0]=d[N];
    m[0]=m[N];
    U[N+1]=U[1];
    d[N+1]=d[1];
    m[N+1]=m[1];
/*    U[N+1]=U[0];
    double dd=1;
    d[0]=dd;
    d[N+1]=d[0];
    m[N+1]=m[0];*/
}

    for (int n = 0; n <= this->N + 1; n++)
    {
        double c = a * this->m[n] * (this->E0 - this->U[n] );
        {
            if(fabs(c) < 1e-10) //TODO: process correctly the case when E0 = U(n)
            {
                this->set_E0(this->E0 + 1e-10);
                goto restart;
            }
            if (c>0)
                this->k[n] = complex(sqrt( c), 0);
            else
                this->k[n] = complex(0, sqrt(-c));
        }
    }
}

/*-----------------------------------------------
  This function calculates reflection/transmission coefficients

  totalR = r#(i)*r(i)

  totalT = t#*t k(N+1)/k(0)*m(0)/m(N+1) ����

  TT=|t|^2*k(0)/k(N+1)*m(N+1)/m(0); � ������ �������
  RR=|r|^2
-------------------------------------------------*/
void PhysicalModel::build_RT() /* input:r,t,k,m  --- result:R,T,totalR,totalT,totalRT */
{
    double aa = real(k[0])==0 ? 0 : m[N+1]/m[0];
    complex r = this->a[N+1];
    complex t = this->b[0];
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
    if(this->E0 <= this->U[0])
    {
        RR=1;
        TT=0;
    }
    else
    {
        k0=real(k[0]);
        if(this->E0 <= this->U[this->N+1])
        {
            TT=0;
            RR=1.;
        }
        else
        {
            kr=real(this->k[this->N+1]);
            TT=tt*k0/kr*aa;
            RR=rr;
        }
    }

    totalRT = RR + TT;
}

//#define fix(a,l,u) {if(a<(l))a=(l);if((u)<a)a=(u);}
/*
//-----------------------------------------------
  Makes U(x) = Ui(x) + Uc*x
//-------------------------------------------------
void PhysicalModel::build_U()
// input:Ub,Ui --- result:U
{
    if (! need_build_U)
        return;
    U[0] = Ui[0] + this->Ub;

    double width=0;
    int nn=d.size();
    for (int n=0; n < d.size(); n++) //!!!n=1
//    for (int n=0; n <= this->N; n++) //!!!n=1
        width += this->d[n];

    double pos=0;
    int NN=d.size();
    for(int n=d.size()-1; n >= 0; n--)
//    for(int n=this->N; n >= 0; n--)
    {
        pos += this->d[n]/2.;
        double dd=d[n];
        double UU=Ui[n];
        U[n] = Ui[n] + pos/width * this->Ub;
        pos += this->d[n]/2.;
    }
//    U_d = d;

    if (getPotentialType() == PERIODIC)
    {
        U[N+1] = U[0];
        d[N+1] = d[0];
        m[N+1] = m[0];
    }
    need_build_U = false;///!!!!!!!!!!!
}
*/
void PhysicalModel::build_U() /* input:Ub,Ui --- result:U */
{
    if (! need_build_U)
        return;
    U[0] = Ui[0];
    for(int n=d.size()-1; n >= 0; n--)
    {
        U[n] = Ui[n];
    }
    need_build_U = false;
}

QRectF PhysicalModel::getGoodViewportAtU() const
{
    double x,y;
    double ymin = 1e300, ymax = -1e300;
    x=0;
    //    build_U();
    double Xmax, Xmin, Umax, Umin;
    for (int n=0; n <= getN(); n++) ///n=1!!!!!!!
    {
        x += get_d(n);
        if (n == getN()) Xmax = x;
//        y = get_U(n);
      y = get_Ui(n);
        if (y > ymax) ymax = y;
        if (y < ymin) ymin = y;
    }
//    if (get_U(getN()+1) > ymax)
  if (get_Ui(getN()+1) > ymax)
//        ymax = get_U(getN()+1);
      ymax = get_Ui(getN()+1);

    Xmax=1.3*Xmax;
    Xmin=-0.3*Xmax;
    Umax=1.1*ymax;
    Umin=1.1*ymin;
    if(Umax==0) Umax=0.5*abs(Umin);
    if(Umin==0) Umin=-0.1*abs(Umax);
    return QRectF(QPointF(Xmin,Umin),QPointF(Xmax,Umax));
}
inline double module(complex& a){ return fabs(real(a))+fabs(imag(a)); }


/*-----------------------------------------------
This function calculates the coefficients a & b.
-------------------------------------------------*/
 void PhysicalModel::build_ab()
/* u:r,k,d,m r:a,b */
{
//    typeOfU=PERIODIC;
    if(typeOfU==FINITE)
    {
        Smatrix();
        this->funEn=real(b[N+1]);
        if(this->E0 > 0)
        {
            for(int i=0; i<=N+1; i++)
            {
                a[i]=a[i]/b[N+1];
                b[i]=b[i]/b[N+1];
            }
            double aa=real(b[N+1]);
        }
    }
    else
    {    
        Smatrix_per();
    }
/*    if(typeOfU==0)
    {
        a[0]=0. ;
        b[0]=1.;
        matching();
        if(this->E0 > 0)
        {
            for(int i=0; i<=N+1; i++)
            {
                a[i]=a[i]/b[N+1];
                b[i]=b[i]/b[N+1];
            }
        }
        this->funEn=real(b[N+1]);
    }
    */
/*    else    // periodical potential
   {
        a[1]=1. ;
        b[1]=0.;
        U[N+1]=U[1];
        d[N+1]=d[1];
        m[N+1]=m[1];
        matching();
        complex alpha = a[N+1];
        complex beta = b[N+1];
        QVector<complex> a1=a;
        QVector<complex> b1=b;
        a[1]=0. ;
        b[1]=1.;
        matching();
        complex gamma = a[N+1];
        complex delta = b[N+1];
        complex sum = alpha+delta;
        complex dis = sum*sum-4.;
//        complex bk = dis;
         double sq = sqrt(abs(dis));
        bool con1;
        con1 = real(dis)>0;
        //wave run to the right
        int kw=1;
        bool con2 = kw>0;
//        complex lambda;
        if(con1&&con2) lambda=.5*(sum+sq);
        if(con1&&!con2) lambda=.5*(sum-sq);
        if(!con1&&con2) lambda=complex(.5*real(sum), 0.5*sq);
        if(!con1&&!con2) lambda=complex(.5*real(sum), -0.5*sq);
        complex cs;
        if(real(sum)>0&&con1) cs = 0.5*(sum+sq);
        if(real(sum)<0&&con1) cs = 0.5*(sum-sq);
        for(int i=1; i<=N+1; i++)
//        for(int i=0; i<=N+1; i++)
        {
            a[i]=gamma*a1[i]+(lambda-alpha)*a[i];
            b[i]=gamma*b1[i]+(lambda-alpha)*b[i];
        }
        a[0]= 
        this->funEn=real(dis);
    }*/
}
 void PhysicalModel::build_ab_periodic()
/* u:r,k,d,m r:a,b */
{
    Smatrix_per();
    return;
}
/*
 // periodical potential
    {
        a[1]=1. ;
        b[1]=0.;
        U[N+1]=U[1];
        d[N+1]=d[1];
        m[N+1]=m[1];
        matching_per();
        complex ca=a[2];
        complex cb=b[2];
         complex alpha = a[N+1];
        complex beta = b[N+1];
        QVector<complex> a1=a;
        QVector<complex> b1=b;
        a[1]=0. ;
        b[1]=1.;
//        a[0]=0. ;
//        b[0]=1.;
        matching_per();
        complex ca1=a[2];
        complex cb1=b[2];
        complex gamma = a[N+1];
        complex delta = b[N+1];
        complex sum = alpha+delta;
        complex dis = sum*sum-4.;
//        complex bk = dis;
         double sq = sqrt(abs(dis));
        bool con1;
        con1 = real(dis)>0;
        //wave run to the right
        int kw=1;
        bool con2 = kw>0;
        complex lambda;
        if(con1&&con2) lambda=.5*(sum+sq);
        if(con1&&!con2) lambda=.5*(sum-sq);
        if(!con1&&con2) lambda=complex(.5*real(sum), 0.5*sq);
        if(!con1&&!con2) lambda=complex(.5*real(sum), -0.5*sq);
        complex cs;
        if(real(sum)>0&&con1) cs = 0.5*(sum+sq);
        if(real(sum)<0&&con1) cs = 0.5*(sum-sq);
        for(int i=1; i<=N; i++)
        {
            a[i]=gamma*a1[i]+(lambda-alpha)*a[i];
            b[i]=gamma*b1[i]+(lambda-alpha)*b[i];
        }
        complex a0=a[N];
        complex b0=b[N];
        complex aa1=a[1];
        complex bb1=b[1];

        a[0]=a[N]/lambda;
        b[0]=b[N]/lambda;
        a[N+1]=a[1]*lambda;
        b[N+1]=b[1]*lambda;
        this->funEn=real(dis);
    }
}
*/

/*void PhysicalModel::build_ab()
/// u:r,k,d,m r:a,b
{

     a[0]=0. ;
    b[0]=1.;
    matching();
    if(this->E0 > 0)
    {
        for(int i=0; i<=N+1; i++)
        {
            a[i]=a[i]/b[N+1];
            b[i]=b[i]/b[N+1];
        }
    }
}
*/
void PhysicalModel::matching_per()
{
    for(int i=1; i<=N; i++)
    {
        complex Im = complex(0.,1.);
        complex up = 0.5*exp(Im*this->k[i]*this->d[i]);
        complex dn = 0.5*exp(-Im*this->k[i]*this->d[i]);
        complex K12= (this->k[i]/this->k[i+1])*(this->m[i+1]/this->m[i]);
        complex KPa= (1.+K12)*up;
        complex KMa= (1.-K12)*dn;
        complex KPb= (1.+K12)*dn;
        complex KMb=  (1.-K12)*up;
        complex aa=a[i]*KPa+b[i]*KMa;
        complex bb=a[i]*KMb+b[i]*KPb;
        a[i+1]=aa;
        b[i+1]=bb;

    }
}
void PhysicalModel::matching()
{
    for(int i=0; i<=N; i++)
    {
        complex Im = complex(0.,1.);
        complex up = 0.5*exp(Im*this->k[i]*this->d[i]);
        complex dn = 0.5*exp(-Im*this->k[i]*this->d[i]);
        complex K12= (this->k[i]/this->k[i+1])*(this->m[i+1]/this->m[i]);
        complex KPa= (1.+K12)*up;
        complex KMa= (1.-K12)*dn;
//        complex KPa= (complex(1.,0)+K12)*up;
//        complex KMa= (complex(1.,0)-K12)*dn;
//        complex KPb= (complex(1.,0)+K12)*dn;
//        complex KMb= (complex(1.,0)-K12)*up;
        complex KPb= (1.+K12)*dn;
        complex KMb=  (1.-K12)*up;
        complex aa=a[i]*KPa+b[i]*KMa;
        complex bb=a[i]*KMb+b[i]*KPb;
        a[i+1]=aa;
        b[i+1]=bb;

    }
}
void PhysicalModel::Smatrix()
{
    complex p11,p12,p22,p21;
    int Na=a.size();
    int Nb=b.size();
    int Nk=k.size();
    for(int i=0; i<=N; i++)
    {
        complex Im = complex(0.,1.);
        complex up = 0.5*exp(Im*this->k[i]*this->d[i]);
        complex dn = 0.5*exp(-Im*this->k[i]*this->d[i]);
        double mm=m[i];
        double m1=m[i+1];

        complex K12= (this->k[i]/this->k[i+1])*(this->m[i+1]/this->m[i]);
        complex KPa= (1.+K12)*up;
        complex KMa= (1.-K12)*dn;
        complex KPb= (1.+K12)*dn;
        complex KMb=  (1.-K12)*up;
        complex ss=1./KPb;
        s11[i]=-KMb*ss;
        s12[i]=ss;
        s21[i]=KPa-KMa*KMb*ss;
        s22[i]=KMa*ss;
    }
    p11=s11[0];
    p12=s12[0];
    p21=s21[0];
    p22=s22[0];
    for(int i=1; i<=N; i++)
    {
       complex com=1./(1.-p12*s21[i]);
       complex com1=p11*com;
       p11=s11[i]* com1;
       p21=p21+p22*s21[i]*com1;
       p22=p22*(1.+s21[i]*com*p12)*s22[i];
       p12=s12[i]+s11[i]*com*p12*s22[i];
    }
a[0]=0;
b[0]=p12/p22;
a[N+1]=1.;
b[N+1]=1./p22;
    complex  aa1=b[0];
    complex bb1=b[N+1];
/*    for(int i=N; i>=1; i--)
    {
        a[i]=(a[i+1]-s22[i]*b[i+1])/s21[i];
        b[i] =s11[i]*a[i]+s12[i]*b[i+1];
    }
*/
      for(int i=0; i<=N; i++)
    {
        b[i+1]=(b[i]-s11[i]*a[i])/s12[i];
        a[i+1] =s21[i]*a[i]+s22[i]*b[i+1];
    }
    complex  aa=b[0];
    complex aa0=a[0];
    complex bb=b[N+1];
   for(int i=N+1; i>=0; i--)
    {
        a[i]=a[i]/b[0];
        b[i]=b[i]/b[0];
    }
    aa0=a[0];
    bb=b[N+1];
}
void PhysicalModel::Smatrix_per()
// periodical potential
{   
    U[N+1]=U[1];
    d[N+1]=d[1];
    m[N+1]=m[1];
    U[0]=U[N];
    d[0]=d[N];
    m[0]=m[N];
    complex p11,p12,p22,p21;
    int Na=a.size();
    int Nb=b.size();
    int Nk=k.size();
    for(int i=1; i<=N; i++)
    {
        complex Im = complex(0.,1.);
        complex up = 0.5*exp(Im*this->k[i]*this->d[i]);
        complex dn = 0.5*exp(-Im*this->k[i]*this->d[i]);
        double mm=m[i];
        double m1=m[i+1];

        complex K12= (this->k[i]/this->k[i+1])*(this->m[i+1]/this->m[i]);
        complex KPa= (1.+K12)*up;
        complex KMa= (1.-K12)*dn;
        complex KPb= (1.+K12)*dn;
        complex KMb=  (1.-K12)*up;
        complex ss=1./KPb;
        s11[i]=-KMb*ss;
        s12[i]=ss;
        s21[i]=KPa-KMa*KMb*ss;
        s22[i]=KMa*ss;
    }
    p11=s11[1];
    p12=s12[1];
    p21=s21[1];
    p22=s22[1];
    for(int i=2; i<=N; i++)
    {
       complex com=1./(1.-p12*s21[i]);
       complex com1=p11*com;
       p11=s11[i]* com1;
       p21=p21+p22*s21[i]*com1;
       p22=p22*(1.+s21[i]*com*p12)*s22[i];
       p12=s12[i]+s11[i]*com*p12*s22[i];
    }
//first time:psi_minus
a[1]=0;
b[1]=p12/p22;
a[N+1]=1.;
b[N+1]=1./p22;
for(int i=1; i<=N; i++)
{
b[i+1]=(b[i]-s11[i]*a[i])/s12[i];
a[i+1] =s21[i]*a[i]+s22[i]*b[i+1];
}
for(int i=N+1; i>=1; i--)
{
complex ca=a[i]/b[1];
complex cb=b[i]/b[1];
a[i]=a[i]/b[1];
b[i]=b[i]/b[1];
}
        complex gamma = a[N+1];
        complex delta = b[N+1];
//        complex alpha = a[N+1];
//        complex beta = b[N+1];
        QVector<complex> a1=a;
        QVector<complex> b1=b;

//second time:psi_plus
a[1]=-p12/p11;
b[1]=0;
a[N+1]=p22-p21*p12/p11;
b[N+1]=1.;
for(int i=1; i<=N; i++)
{
b[i+1]=(b[i]-s11[i]*a[i])/s12[i];
a[i+1] =s21[i]*a[i]+s22[i]*b[i+1];
}
for(int i=N+1; i>=1; i--)
{
complex ca=a[i]/a[1];
complex cb=b[i]/a[1];
a[i]=a[i]/a[1];
b[i]=b[i]/a[1];
}
complex aa0=a[1];
complex aa1=a[2];
        complex alpha = a[N+1];
        complex beta = b[N+1];
//        complex gamma = a[N+1];
//        complex delta = b[N+1];
        complex sum = alpha+delta;
        complex dis = sum*sum-4.;
        this->discrim=real(dis);
        double sq = sqrt(abs(dis));
        bool con1;
        con1 = real(dis)>0; 
        //wave run to the right
        int kw=1;
        bool con2 = kw>0;
//        complex lambda;
        if(con1&&con2) lambda=.5*(sum+sq);
        if(con1&&!con2) lambda=.5*(sum-sq);
        if(!con1&&con2) lambda=complex(.5*real(sum), 0.5*sq);
        if(!con1&&!con2) lambda=complex(.5*real(sum), -0.5*sq);
        complex cs;
        if(real(sum)>0&&con1) cs = 0.5*(sum+sq);
        if(real(sum)<0&&con1) cs = 0.5*(sum-sq);
        for(int i=1; i<=N; i++)
        {
            complex ca=gamma*a1[i]+(lambda-alpha)*a[i];
            complex cb=gamma*b1[i]+(lambda-alpha)*b[i];
            a[i]=gamma*a[i]+(lambda-alpha)*a1[i];
            b[i]=gamma*b[i]+(lambda-alpha)*b1[i];
//            a[i]=gamma*a1[i]+(lambda-alpha)*a[i];
//            b[i]=gamma*b1[i]+(lambda-alpha)*b[i];
        }
        double p=squaremod(lambda);
        if(abs(p-1.)>1.e-8)
        {
        this->qa=0;
        }
        else
        {
        double cs=real(lambda);
        this->qa=abs(acos(cs));
        }
        complex anorm=a[1]+b[1];
        for(int i=N; i>=1; i--)
        {
            a[i]=a[i]/anorm;
            b[i]=b[i]/anorm;
        }
        complex a0=a[N];
        complex b0=b[N];
        a[0]=a[N]/lambda;
        b[0]=b[N]/lambda;
        a[N+1]=a[1]*lambda;
        b[N+1]=b[1]*lambda;
        this->funEn=real(dis);
}

/*void PhysicalModel::norm_per()
{
    double s;
    s=0;
    for (int n = 1; n <= this->N; n++)
    {
        complex aj=this->a[n];
        complex bj=this->b[n];
        double a = this->E0 - this->U[n];
        if(a>0)
        {
            double arg= real(this->k[n])*this->d[n];
            if(arg!=0)
            {
                s = s + this->d[n]*(( squaremod(aj)+squaremod(bj) )+sin(arg)/arg*real(aj*conj(bj)*exp(complex(0,1.)*arg)+bj*conj(aj)*exp(-complex(0,1.)*arg)));
            }
            else s = s + this->d[n]*(( squaremod(aj)+squaremod(bj) )+real(aj*conj(bj)+bj*conj(aj)));

        }
        else
        {
            double arg= imag(this->k[n])*this->d[n];
            if(arg!=0)
            {
                s = s + this->d[n]*real((aj*conj(bj)+bj*conj(aj))+sinh(arg)/arg*(squaremod(aj)*exp(-arg)+squaremod(bj)*exp(arg)));
            }
            else s = s + this->d[n]*real((aj*conj(bj)+bj*conj(aj))+(squaremod(aj)+squaremod(bj)));
        }
    }
    if(s>0)
    {
        for (int n = 0; n <= this->N + 1; n++)
        {
            this->a[n]=this->a[n]/sqrt(s);
            this->b[n]=this->b[n]/sqrt(s);
        }
    }
}*/
void PhysicalModel::norm()
{
    double s;
    int n0;
    s=0;
    n0=1;
    if(typeOfU==FINITE)
    {
        s=0.5*(
            squaremod(this->b[0])/imag(this->k[0])
            +squaremod(this->a[this->N+1])/imag(this->k[this->N+1])
            );
    }
    for (int n = n0; n <= this->N; n++)
    {
        complex aj=this->a[n];
        complex bj=this->b[n];
        double a = this->E0 - this->U[n];
        if(a>0)
        {
            double arg= real(this->k[n])*this->d[n];
            if(arg!=0)
            {
                s = s + this->d[n]*(( squaremod(aj)+squaremod(bj) )+sin(arg)/arg*real(aj*conj(bj)*exp(complex(0,1.)*arg)+bj*conj(aj)*exp(-complex(0,1.)*arg)));
            }
            else s = s + this->d[n]*(( squaremod(aj)+squaremod(bj) )+real(aj*conj(bj)+bj*conj(aj)));

        }
        else
        {
            double arg= imag(this->k[n])*this->d[n];
            if(arg!=0)
            {
                s = s + this->d[n]*real((aj*conj(bj)+bj*conj(aj))+sinh(arg)/arg*(squaremod(aj)*exp(-arg)+squaremod(bj)*exp(arg)));
            }
            else s = s + this->d[n]*real((aj*conj(bj)+bj*conj(aj))+(squaremod(aj)+squaremod(bj)));
        }
    }
    if(s>0)
    {
        for (int n = 0; n <= this->N + 1; n++)
        {
            this->a[n]=this->a[n]/sqrt(s);
            this->b[n]=this->b[n]/sqrt(s);
        }
    }
}
/*-----------------------------------------------
Calculates Psi(x) at x.
-------------------------------------------------*/
//void PhysicalModel::build_Psi() /* input:a,b,x,k,d ------ result:Psi2 */
//{
//   psi(x)=a(i)*exp(ik_j*(x-x_{j})+b(i)*exp(-ik_j*(x-x_{j})
//   x_{j}<=x<=x_{j+1} d(j)=x_{j+1}-x_j

//    first determine n  so that x belongs to d(n)
//    double xt=x;
/*    int n;

    if ((this->E0 < 0 && this->E0 - this->U[0] < 0)||typeOfU>0)  norm();
    for (n = 0; n <= N; n++)
    {
        xt -= d[n];
        if(xt<0)
        {
            xt += d[n];
            break;
        }
    }
  // obtain Psi2 at x
        if(xt>xN)
        {
    for (int n = 0; n <= this->N; n++)
         {
           a(n) = lqmda*a(n)
           b(n) = lqmda*b(n)
         }
        }

        complex up=exp( complex(0,1)*(  k[n]*xt ) );
        complex dn=exp( complex(0,1)*( -k[n]*xt  ) );
        this->psi = a[n]*up + b[n]*dn;
        Psi2 = squaremod(psi);
        psi_real=real(psi);
        psi_imag=imag(psi);
}
*/
void PhysicalModel::build_Psi() /* input:a,b,x,k,d ------ result:Psi2 */
{
//   psi(x)=a(i)*exp(ik_j*(x-x_{j})+b(i)*exp(-ik_j*(x-x_{j})
//   x_{j}<=x<=x_{j+1} d(j)=x_{j+1}-x_j

    /* first determine n  so that x belongs to d(n)
    ----------------------------------------------*/
    double xt=x;
    double xN;
    int n;
    xN=-d[0];
    for (n = 0; n <= N; n++)
    {
        xN+= d[n];
    }
    if ((this->E0 < 0 && this->E0 - this->U[0] < 0)||typeOfU==PERIODIC)  norm();
    for (n = 0; n <= N; n++)
    {
        xt -= d[n];
      if(xt<0)
        {
            xt += d[n];
            break;
        }
    }
   // obtain Psi2 at x
        complex up=exp( complex(0,1)*(  k[n]*xt ) );
        complex dn=exp( complex(0,1)*( -k[n]*xt  ) );
        this->psi = a[n]*up + b[n]*dn;        
        Psi2 = squaremod(psi);
        psi_real=real(psi);
        psi_imag=imag(psi);
}
void PhysicalModel::build_Psi_per() /* input:a,b,x,k,d ------ result:Psi2 */
{
//   psi(x)=a(i)*exp(ik_j*(x-x_{j})+b(i)*exp(-ik_j*(x-x_{j})
//   x_{j}<=x<=x_{j+1} d(j)=x_{j+1}-x_j

    /* first determine n  so that x belongs to d(n)
    ----------------------------------------------*/
    double xt=x;
    double xN;
    int n;
    xN=0;
    complex ddd=this->lambda;
    for (n = 1; n <= N; n++)
    {
        xN+= d[n];
    }
    norm();
//    norm_per();
        QVector<complex> a1=this->a;
        QVector<complex> b1=this->b;
        while(xt>xN)
        {
        xt-=xN;
    for (int j = 1; j <= this->N; j++)
         {
           a1[j] = this->lambda*a1[j];
           b1[j] = this->lambda*b1[j];
         }
        }

        while(xt<0)
        {
        xt+=xN;
    for (int j = 1; j <= this->N; j++)
         {
             a1[j] = a1[j]/this->lambda;
             b1[j] = b1[j]/this->lambda;
         }
        }

    for (n = 1; n <= N; n++)
    {
        xt -= d[n];
      if(xt<0)
        {
            xt += d[n];
            break;
        }
    }
    /* obtain Psi2 at x
    -------------------*/
        
        complex up=exp( complex(0,1)*(  k[n]*xt ) );
        complex dn=exp( complex(0,1)*( -k[n]*xt  ) );
        complex a1n = a1[n];
        complex b1n = b1[n];
        complex ksi= a1n*up+b1n*dn;
        this->psi = a1[n]*up + b1[n]*dn;        
        Psi2 = squaremod(ksi);
        psi_real=real(ksi);
        psi_imag=imag(ksi);
}
/*-----------------------------------------------
Calculates Phi(p) at p: finite potential
-------------------------------------------------*/
void PhysicalModel::build_Phi() 
// input:En,a,b,x,k,d ------ result:Phi(p) 
{
    this->norm();
    double xr=0.;
    double kw = this->kwave;
    this->phi=complex(0.,0.);
    complex exp_xr;
    for(int n=1; n<=this->N; n++)
    {
        xr += d[n];

        complex kj=this->k[n];
        complex kjdn=this->k[n]*d[n];
        exp_xr = exp(-complex(0.,kw*xr));
        complex exp_kjdjp=exp(complex(0,1)*kjdn);
        complex exp_kwdjm=exp(complex(0,-kw*d[n]));
        complex exp_kjdjm=exp(complex(0,-1)*kjdn);
        complex exp_kwdjp=exp(complex(0,kw*d[n]));
        if(kw!=kj&&kw!=-kj)
        {
            phi = phi + complex(0, -1.)*exp_xr*(a[n]*(exp_kjdjp-exp_kwdjp)/(kj-kw)-b[n]*(exp_kjdjm-exp_kwdjp)/(kj+kw));
        }
        else
        {
            complex exp_xlm = exp(complex(0.,-kw*(xr-d[n])));
            complex exp_xlp = exp(complex(0.,kw*(xr-d[n])));
            if(kw==kj&&kw!=0)  phi = phi + exp_xlm*(a[n]*d[n]+b[n]*exp_kwdjp*sin(kw*d[n])/kw);
            if(kw==-kj&&kw!=0) phi = phi + exp_xlp*(b[n]*d[n]+a[n]*exp_kwdjm*sin(kw*d[n])/kw);
            if(kw==kj && kw==0) phi = phi + d[n]*(a[n]+b[n]);
        }
    }
    phi= phi+ b[0]/(imag(this->k[0])-complex(0,kw));//+a(N+1)*exp(complex(0.,-kw*xr))/(this->k(N+1)-kw));
    //        phi= phi+complex(0.,1.)*b(0) /(this->k(0)+kw);//+a(N+1)*exp(complex(0.,-kw*xr))/(this->k(N+1)-kw));
    phi= phi+a[N+1]*exp(complex(0.,-kw*xr))/(imag(this->k[N+1])+complex(0.,kw));

    Phi2 = squaremod(phi);
    Phi_real=real(phi);
    Phi_imag=imag(phi);
}

/*-----------------------------------------------
Calculates Phi(p) at p: periodic potential
-------------------------------------------------*/
void PhysicalModel::build_Phi_per() 
// input:En,a,b,x,k,d ------ result:Phi(p) 
{
     double p=squaremod(lambda);
     if(abs(p-1.)>1.e-8) return;
    this->norm();
    double     ax=0; 
        for(int n=1; n<=this->N; n++)
        {
        ax += d[n];
        }
        double kw = 2*M_PI/ax;
//        double cs=real(this->lambda);
//        this->qa=abs(acos(cs));


    double xr=0.;
    double kw = this->kwave;
    this->phi=complex(0.,0.);
    complex exp_xr;
    for(int n=1; n<=this->N; n++)
    {
        xr += d[n];

        complex kj=this->k[n];
        complex kjdn=this->k[n]*d[n];
        exp_xr = exp(-complex(0.,kw*xr));
        complex exp_kjdjp=exp(complex(0,1)*kjdn);
        complex exp_kwdjm=exp(complex(0,-kw*d[n]));
        complex exp_kjdjm=exp(complex(0,-1)*kjdn);
        complex exp_kwdjp=exp(complex(0,kw*d[n]));
        if(kw!=kj&&kw!=-kj)
        {
            phi = phi + complex(0, -1.)*exp_xr*(a[n]*(exp_kjdjp-exp_kwdjp)/(kj-kw)-b[n]*(exp_kjdjm-exp_kwdjp)/(kj+kw));
        }
        else
        {
            complex exp_xlm = exp(complex(0.,-kw*(xr-d[n])));
            complex exp_xlp = exp(complex(0.,kw*(xr-d[n])));
            if(kw==kj&&kw!=0)  phi = phi + exp_xlm*(a[n]*d[n]+b[n]*exp_kwdjp*sin(kw*d[n])/kw);
            if(kw==-kj&&kw!=0) phi = phi + exp_xlp*(b[n]*d[n]+a[n]*exp_kwdjm*sin(kw*d[n])/kw);
            if(kw==kj && kw==0) phi = phi + d[n]*(a[n]+b[n]);
        }
    }
    phi= phi+ b[0]/(imag(this->k[0])-complex(0,kw));//+a(N+1)*exp(complex(0.,-kw*xr))/(this->k(N+1)-kw));
    //        phi= phi+complex(0.,1.)*b(0) /(this->k(0)+kw);//+a(N+1)*exp(complex(0.,-kw*xr))/(this->k(N+1)-kw));
    phi= phi+a[N+1]*exp(complex(0.,-kw*xr))/(imag(this->k[N+1])+complex(0.,kw));

    Phi2 = squaremod(phi);
    Phi_real=real(phi);
    Phi_imag=imag(phi);
}
//*int PhysicalModel::zeroPsi_per()
{
    int Nmax, nzero, nzero_i, n_total;
    Nmax = getN ();
    nzero=0;
//    if(typeOfU==FINITE) i0=1;
//    else i0=0;
    for (int i = 1; i <= Nmax; ++i)
    {
        nzero_i=0;
        if (this->E0 > this->U[i])
        {
            double arg = real(k[i])*d[i];
            double  psi_j   = real(a[i]*exp(complex(0.,arg))+ b[i]*exp(complex(0.,-arg)));
            double cj = psi_j/real(a[i]+b[i]);
            double cs = cos(arg);
            double sn = sin(arg);
            double phase_j= atan((cs-cj)/sn);
            double arg_j=arg+phase_j;
            int j=0;
            while(arg_j > 0.5*M_PI*(1+2*j))
            {
                j++;
            }
            nzero_i=j;
            nzero=nzero+nzero_i;
        }
        else
        {
            double arg = imag(k[i])*d[i];
            double psi2=real(a[i]+b[i]);
            double psi1=real(a[i]*exp(-arg)+b[i]*exp(arg));
            if (psi1>0 && psi2<0 || psi1<0 && psi2>0)
            {
                nzero=nzero+1;
            }
        }
        n_total=nzero;
    }
    return nzero;
}*/
int PhysicalModel::zeroPsi()
{
    int Nmax, nzero, nzero_i, n_total;
    Nmax = getN ();
    nzero=0;
    for (int i = 1; i <= Nmax; ++i)
    {
        nzero_i=0;
        if (this->E0 > this->U[i])
        {
            double arg = real(k[i])*d[i];
            double  psi_j   = real(a[i]*exp(complex(0.,arg))+ b[i]*exp(complex(0.,-arg)));
            double cj = psi_j/real(a[i]+b[i]);
            double cs = cos(arg);
            double sn = sin(arg);
            double phase_j= atan((cs-cj)/sn);
            double arg_j=arg+phase_j;
            int j=0;
            while(arg_j > 0.5*M_PI*(1+2*j)+1.e-8)
            {
                j++;
            }
            nzero_i=j;
            nzero=nzero+nzero_i;
        }
        else
        {
            double arg = imag(k[i])*d[i];
            double psi2=real(a[i]+b[i]);
            double psi1=real(a[i]*exp(-arg)+b[i]*exp(arg));
            if (psi1>0 && psi2<0 || psi1<0 && psi2>0)
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
    QPair<double,double> umin_umax = getUminUmax();
    double Umin = umin_umax.first;
    double Umax = umin_umax.second;
    if(Umin>=0&&Umax>=0) return 0;
    int Nmax, nzero;
    Nmax = getN ();
    this->set_E0(E);
    this->build_k();
    this->build_ab();
    nzero=zeroPsi(); //number of zeros of wave function inside (0, x_Nmax) at E near zero
    if(typeOfU==FINITE)
    {
        double psi1=real(a[Nmax+1]+b[Nmax+1]);
        double psi2=real(b[Nmax+1]);
        if(psi1>0&&psi2<0||psi1<0&&psi2>0)
        return nzero+1;
        else
        return nzero;
    }
    else return nzero;
}
double PhysicalModel::findOneLevel(double emin, double emax)
{
//        double E =(emin +emax)*.5;
//        this->set_E0(E);
    double E = emax;
    this->set_E0(E);
        this->build_k();
        this->build_ab();
        double y=this->funEn;
        this->set_E0(emin);
        this->build_k();
        this->build_ab();
        double y1=this->funEn;
        double E1=emin;
        if(y>0&&y1>0||y<0&&y1<0)
        {
         this->set_E0(emax);
         this->build_k();
         this->build_ab();
         y1=this->funEn;
         E1=emax;
         }
        while(fabs(E-E1)>1.e-12)
        {
         this->set_E0((E +E1)*.5);;
         this->build_k();
         this->build_ab();
         double y=this->funEn;
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
void PhysicalModel::setUbias(double u)
{
//    if(Ubias!=u)
    {
        this->Ubias=u;
        Ui[0] = Ui[0] + this->Ubias;

        double width=0;
        int nn=d.size();
        for (int n=0; n < d.size(); n++) //!!!n=1
            width += this->d[n];

        double pos=0;
        int NN=d.size();
        for(int n=d.size()-1; n > 0; n--)
        {
            pos += this->d[n]/2.;
            double dd=d[n];
            double UU=Ui[n];
            Ui[n] = Ui[n] + pos/width * this->Ubias;
            pos += this->d[n]/2.;
        }
        markUchanged();
    }
}

Uparab PhysicalModel::getUparab() const
{
    Uparab u;
    u.numberOfIntervals = 30;
    u.U0 = 10;
    u.Width = 6;
    return u;
}

void PhysicalModel::setUparab(const Uparab& u)
{
    int nn=u.numberOfIntervals;
    this->set_N(nn-1);
    int NN=this->N;
    double dd=u.Width/nn;
    double u0=u.U0;
    this->m[0] = 0.5;
    this->d[0] = 0;
    this->Ui[0] = 0;
    double y_i,uu;
    for (int n = 1; n < nn; n++)
    {
        this->d[n] = dd;
        y_i=(2.*n/nn-1.);
        uu=u0*(y_i*y_i-1);
        this->Ui[n] = uu;
        this->m[n] = 0.5;
    }
    this->Ui[nn]=0;
    this->m[nn] = 0.5;
    this->d[nn] = 0;
    markUchanged();
}
zParameters PhysicalModel::getzParam() const
{
zParameters tp;
tp.z=this->zz;
tp.zmin=this->zmin;
tp.zmax=this->zmax;
tp.hz=this->hz;
return tp;
}

ScalesUParameters PhysicalModel::getScalesUParam() const
{
ScalesUParameters tp;
tp.Hx=this->Hx;
tp.Xmin=this->Xmin;
tp.Xmax=this->Xmax;
tp.Umin=this->Umin;
tp.Umax=this->Umax;
//tp.Psimin=this->Psimin;
//tp.Psimax=this->Psimax;
return tp;
}
ScalePsinParameters PhysicalModel::getScalePsinParam() const
{
ScalePsinParameters tp;
tp.Hx=this->Hx;
tp.Xmin=this->Xmin;
tp.Xmax=this->Xmax;
tp.Psinmin=this->Psinmin;
tp.Psinmax=this->Psinmax;
return tp;
}
ScaleWPXParameters PhysicalModel::getScaleWPXParam() const
{
ScaleWPXParameters tp;
tp.Hx=this->Hx;
tp.Xmin=this->Xmin;
tp.Xmax=this->Xmax;
tp.WPXmin=this->WPXmin;
tp.WPXmax=this->WPXmax;
return tp;
}
ScaleWPKParameters PhysicalModel::getScaleWPKParam() const
{
ScaleWPKParameters tp;
tp.Hk=this->Hk;
tp.Kmin=this->Kmin;
tp.Kmax=this->Kmax;
tp.WPKmin=this->WPKmin;
tp.WPKmax=this->WPKmax;
return tp;
}
ScalePhinParameters PhysicalModel::getScalePhinParam() const
{
ScalePhinParameters tp;
tp.Hk=this->Hk;
tp.Kmin=this->Kmin;
tp.Kmax=this->Kmax;
tp.Phinmin=this->Phinmin;
tp.Phinmax=this->Phinmax;
return tp;
}
TimeParameters PhysicalModel::getTimeParam() const
{
 TimeParameters tp;
tp.time=this->time;
tp.tmin=this->tmin;
tp.tmax=this->tmax;
tp.ht=this->ht;
return tp;
}
SettingParameters PhysicalModel::getSettingParameters() const
{
SettingParameters tp;
tp.lineWidth=this->width_of_Line;
return tp;
}
void PhysicalModel::set_z(double v)
{
 zParameters u = getzParam();
 if(v!=u.z)
 {
     u.z=v;
     setzParam(u);
 }
}

void  PhysicalModel::setzParam(const zParameters& u)
{
    bool changed = false;
    double v=u.z;
    double E_old=this->E0;
    if(v!=this->zz)
    {
        this->zz=v;
        this->set_Uxz(v);
        emit(signalZChanged(v));
    }
    this->E0=E_old;
    v=u.zmin;
    if(v!=this->zmin)
    {
        changed = true;
        this->zmin=u.zmin;
    }
    v=u.zmax;
    if(v!=this->zmax)
    {
        changed = true;
        this->zmax=u.zmax;
    }
    v=u.hz;
    if(v!=this->hz)
    {
        changed = true;
        this->hz=u.hz;
    }
    if (changed)
    {
        emit(signalScaleZChanged());
    }

}
void  PhysicalModel::setTimeParam(const TimeParameters& u)
{
 double v=u.time;
 if(v!=this->time)
 {
     this->time=v;
     emit(signalTimeChanged(v));
 }
 this->tmin=u.tmin;
 this->tmax=u.tmax;
 if(this->ht!=u.ht)
 {
 this->ht=u.ht;
 }
}
void  PhysicalModel::setSettingParameters(const SettingParameters &u)
{
 if(u.lineWidth!=this->width_of_Line)
 {
     this->width_of_Line=u.lineWidth;
     emit(signalWidthChanged());
 }
}
void  PhysicalModel::set_LevelNumber(int n)
{
 if(levelNumber!=n)
 {
     this->levelNumber=n;
     emit(signalLevelNumberChanged(n));
     if(n>=0&&n<Ebound.size())
     {
     this->set_Energy(this->Ebound[n]);   
     }
 }
}
void  PhysicalModel::setScalesUParam(const ScalesUParameters& u)
{
 bool changed_x = false;
 bool changed_y = false;
 double v=u.Xmax;
 if(v!=this->Xmax)
 {
     this->Xmax=v;
     changed_x = true;
 }
 v=u.Xmin;
 if(v!=this->Xmin)
 {
     changed_x = true;
     this->Xmin=v;
 }
 v=u.Umax;
 if(v!=this->Umax)
 {
     this->Umax=v;
     changed_y = true;
 }
 v=u.Umin;
 if(v!=this->Umin)
 {
     changed_y = true;
     this->Umin=v;
 }
/* v=u.Psimax;
 if(v!=this->Psimax)
 {
     this->Psimax=v;
     changed_y = true;
 }

 v=u.Psimin;
 if(v!=this->Psimin)
 {
     changed_y = true;
     this->Psimin=v;
 }*/

 if(this->Hx!=u.Hx)
 {
     changed_x = true;
     this->Hx=u.Hx;
 }
    if (changed_x||changed_y)
    {
    emit(signalScalesUChanged());
    }
    if (changed_x)
    {
    ScalePsinParameters t;
    t.Xmax=Xmax;
    t.Xmin=Xmin;
    t.Hx=Hx;
  //  t.Psinmax=Psinmax;
  //  t.Psinmin=this->Psinmin;
    emit(signalScalePsinChanged());
    ScaleWPXParameters twp;
    twp.Xmax=Xmax;
    twp.Xmin=Xmin;
    twp.Hx=Hx;
    twp.WPXmax=WPXmax;
    twp.WPXmin=this->WPXmin;
    emit(signalScaleWPXChanged());
    }

}
void  PhysicalModel::setScalePhinParam(const ScalePhinParameters& u)
{
 bool changed = false;
 double v=u.Kmax;
 if(v!=this->Kmax)
 {
     this->Kmax=v;
     changed = true;
 }
 v=u.Kmin;
 if(v!=this->Kmin)
 {
     changed = true;
     this->Kmin=v;
 }
 v=u.Phinmax;
 if(v!=this->Phinmax)
 {
     this->Phinmax=v;
     changed = true;
 }

 v=u.Phinmin;
 if(v!=this->Phinmin)
 {
     changed = true;
     this->Phinmin=v;
 }

 if(this->Hk!=u.Hk)
 {
 changed = true;
 this->Hk=u.Hk;
 }
    if (changed)
    {
    emit(signalScalePhinChanged());
    }
}

void  PhysicalModel::setScaleWPXParam(const ScaleWPXParameters& u)
{
    bool changed_x = false;
    bool changed_y = false;
    double v=u.Xmax;
    if(v!=this->Xmax)
    {
        this->Xmax=v;
        changed_x = true;
    }
    v=u.Xmin;
    if(v!=this->Xmin)
    {
        changed_x = true;
        this->Xmin=v;
    }
    v=u.WPXmax;
    if(v!=this->WPXmax)
    {
        this->WPXmax=v;
        changed_y = true;
    }

    v=u.WPXmin;
    if(v!=this->WPXmin)
    {
        changed_y = true;
        this->WPXmin=v;
    }

    if(this->Hx!=u.Hx)
    {
        changed_x = true;
        this->Hx=u.Hx;
    }
    if (changed_x||changed_y)
    {
        emit(signalScaleWPXChanged());
    }
    if (changed_x)
    {
        emit(signalScalesUChanged());
    }
}
void  PhysicalModel::setScaleWPKParam(const ScaleWPKParameters& u)
{
 bool changed_x = false;
 bool changed_y = false;
 double v=u.Kmax;
 if(v!=this->Kmax)
 {
     this->Kmax=v;
     changed_x = true;
 }
 v=u.Kmin;
 if(v!=this->Kmin)
 {
     changed_x = true;
     this->Kmin=v;
 }
 v=u.WPKmax;
 if(v!=this->WPKmax)
 {
     this->WPKmax=v;
     changed_y = true;
 }

 v=u.WPKmin;
 if(v!=this->WPKmin)
 {
     changed_y = true;
     this->WPKmin=v;
 }

 if(this->Hk!=u.Hk)
 {
 changed_x = true;
 this->Hk=u.Hk;
 }
    if (changed_x||changed_y)
    {
    emit(signalScaleWPKChanged());
    }
/*    if (changed_x)
    {
    emit(signalScalesUChanged());
    }*/
}
void  PhysicalModel::setScalePsinParam(const ScalePsinParameters& u)
{
 bool changed_x = false;
 bool changed_y = false;
 double v=u.Xmax;
 if(v!=this->Xmax)
 {
     this->Xmax=v;
     changed_x = true;
 }
 v=u.Xmin;
 if(v!=this->Xmin)
 {
     changed_x = true;
     this->Xmin=v;
 }
 v=u.Psinmax;
 if(v!=this->Psinmax)
 {
     this->Psinmax=v;
     changed_y = true;
 }

 v=u.Psinmin;
 if(v!=this->Psinmin)
 {
     changed_y = true;
     this->Psinmin=v;
 }

 if(this->Hx!=u.Hx)
 {
 changed_x = true;
 this->Hx=u.Hx;
 }
    if (changed_x||changed_y)
    {
    emit(signalScalePsinChanged());
    }
    if (changed_x)
    {
        ScalesUParameters tp;
        tp.Hx=this->Hx;
        tp.Xmin=this->Xmin;
        tp.Xmax=this->Xmax;
        tp.Umin=this->Umin;
        tp.Umax=this->Umax;
//        tp.Psimin=this->Psimin;
//        tp.Psimax=this->Psimax;
        emit(signalScalesUChanged());
    }
}

void  PhysicalModel::setEpWP(const EpWP& u)
{
 bool changed = false;
 int n = u.numberOfModes;
 if(n!=this->wpN)
 {
 this->wpN = u.numberOfModes;
 changed = true;
 }
 double v=u.E_high;
 if(v!=this->wpE_hi)
 {
 this->wpE_hi = u.E_high;
 changed = true;
 }
 v=u.E_low;
 if(v!=this->wpE_lo)
 {
 this->wpE_lo = v;
 changed = true;
 }
 if(type_of_WP!=1)
 {
      changed = true;
      type_of_WP=1;
 }
    if (changed)
    {
    need_build_WP = true;
//    emit(signalWavePacketChanged());
    }
}
void  PhysicalModel::setEmWP(const EmWP& u)
{
  bool changed = false;
  int n = u.nmin;
 if(n!=this->nminWP)
 {
    this->nminWP = n;
 changed = true;
 }
  n = u.nmax;
 if(n!=this->nmaxWP)
 {
    this->nmaxWP = n;
 changed = true;
 }
  n = u.hn;
 if(n!=this->hnWP)
 {
    this->hnWP = n;
 changed = true;
 }
 if(type_of_WP!=2)
 {
      changed = true;
      type_of_WP=2;
 }
    if (changed)
    {
    need_build_WP = true;
    }
}

void  PhysicalModel::setLevelNumberParameters(const LevelNumberParameters& u)
{
  bool changed = false;
  int n = u.nmin;
//  if(n<0) n=0;
 if(n!=this->LevelNmin)
 {
    this->LevelNmin = n;
 changed = true;
 }
  n = u.nmax;
//  if(n<0) n=0;
  if(n!=this->LevelNmax)
 {
    this->LevelNmax = n;
 changed = true;
 }
  n = u.hn;
//  if(n==0) n=1;
  if(n!=this->LevelHn)
 {
    this->LevelHn = n;
 changed = true;
 }
    if (changed)
    {
    emit(signalLevelParametersChanged());
    }
}
LevelNumberParameters  PhysicalModel::getLevelNumberParameters() const
{
    LevelNumberParameters u;
    u.nmin=this->LevelNmin;
    u.nmax=this->LevelNmax;
    u.hn=this->LevelHn;
    return u;
}
EmWP  PhysicalModel::getEmWP() const
{
    EmWP u;
    u.nmin=this->nminWP;
    u.nmax=this->nmaxWP;
    u.hn=this->hnWP;
    return u;
}
EpWP  PhysicalModel::getEpWP() const
{
    EpWP u;
    u.E_low=this->wpE_lo;
    u.E_high=this->wpE_hi;
    u.numberOfModes=this->wpN;
    return u;
}

UAsMW PhysicalModel::getUAsMW() const
{
    UAsMW u;
    u.numberOfWells = 1;
    u.wa = 5;
    u.wb = 1;
    u.ua = -10;
    u.ub = 0;
//    u.ubias = 0;
    return u;
}

void PhysicalModel::setUAsMW(const UAsMW& u)
{
    int nn=2*u.numberOfWells - 1;
    this->set_N(nn);

    this->m[0] = 0.5;
    this->d[0] = 0;
    this->Ui[0] = 0;
//    this->Ub = u.ubias;
for (int n = 1; n <= u.numberOfWells; n++)
    {
        int i = 2*n-1;
        this->Ui[i] = u.ua;
        this->Ui[i+1] = u.ub;
        this->d[i] = u.wa;
        this->d[i+1] = u.wb;
        this->m[i] = 0.5;
        this->m[i+1] = 0.5;
    }
    this->Ui[this->N+1] = 0;
    this->m[this->N+1] = 0.5;
    this->d[this->N+1] = 0;
    markUchanged();
}

PhysicalModel::PhysicalModel(QObject *parent)
: QObject(parent),
width_of_Line(2), levelNumber(0),
  N(1), N1(1),N2(1),E0(0.001), psi(0), x(0),
  kwave(0),
 time(0),tmin(0),tmax(100),ht(0.01),
 zz(-1),zmin(0.),zmax(1.),hz(0.01),
Hx(0.01), Xmax(15.), Xmin(-1), Umin(-15),Umax(10),
Psimin(-1.), Psimax(4.),
Psinmin(-1.), Psinmax(1.),
WPXmin(-0.1), WPXmax(1.),
WPKmin(-0.1), WPKmax(5.),
Phinmin(-0.2), Phinmax(10.),
Hk(0.025), Kmax(5.), Kmin(-5),
 RR(0), TT(0), totalRT(0), Psi2(0), Phi2(0),
 psi_real(0), Phi_real(0),Ubias(0),
 psi_imag(0), Phi_imag(0), 
// typeOfU(PERIODIC),
 typeOfU(FINITE),
 need_build_WP(true),
 LevelNmax(10),LevelNmin(0),LevelHn(1),numberOfLevels(0),Nperiod(1),
 nminWP(0), nmaxWP(10), hnWP(1), wpE_lo(5.),wpE_hi(15.), wpN(30), type_of_WP(2)
// xmin(-1.),xmax(15.)

{
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
        if (Ui[n] != _Ui[n])
        {
            Ui[n] = _Ui[n];
            changed = true;
        }
        if (m[n] != _m[n])
        {
            m[n] = _m[n];
            changed = true;
        }
        if( d[n] != _d[n])
        {
            d[n] = _d[n];
            changed = true;
        }
    }

    if (changed)
    {
        markUchanged();
    }
}
void PhysicalModel::set_U1_d1_m1(const QVector<double>& _U1,
                               const QVector<double>& _d1,
                               const QVector<double>& _m1)
{
    int _N = _U1.size() - 2;
    if (_N != U1.size() - 2)
    {
        this->set_N1(_N);
    }
    for (int n = 0; n <= _N+1; ++n)
    {
        if (U1[n] != _U1[n])
        {
            U1[n] = _U1[n];
        }
        if (m1[n] != _m1[n])
        {
            m1[n] = _m1[n];
        }
        if( d1[n] != _d1[n])
        {
            d1[n] = _d1[n];
        }
    }
}
void PhysicalModel::set_U2_d2_m2(const QVector<double>& _U2,
                               const QVector<double>& _d2,
                               const QVector<double>& _m2)
{
    int _N = _U2.size() - 2;
    if (_N != U2.size() - 2)
    {
        this->set_N2(_N);
    }
    for (int n = 0; n <= _N+1; ++n)
    {
        if (U2[n] != _U2[n])
        {
            U2[n] = _U2[n];
        }
        if (m2[n] != _m2[n])
        {
            m2[n] = _m2[n];
        }
        if( d2[n] != _d2[n])
        {
            d2[n] = _d2[n];
        }
    }
}

void PhysicalModel::set_d(int n, double v)
{
    if (d[n] != v)
    {
        d[n] = v;
        markUchanged();
    }
}
void PhysicalModel::setPotentialType(PotentialType t)
{
    if (t != typeOfU)
    {
        typeOfU = t;
        markUchanged();
    }
}
void PhysicalModel::set_d(int n1, double v1,int n2, double v2)
{
    int changed = 0;
    if (d[n1] != v1)
    {
        d[n1] = v1;
        changed = 1;
    }
    if (d[n2] != v2)
    {
        d[n2] = v2;
        changed = 1;
    }
    if (changed)
        markUchanged();
}
void PhysicalModel::split_d(int n, double fraction)
{
    if (0.05 < fraction && fraction < 0.95)
    {
        double wl = d[n] * fraction;
        double wr = d[n] - wl;
        N += 1;
        d.insert(n,wl);        d[n+1] = wr;
        m.insert(n,m[n]);
        U.insert(n,U[n]);
        Ui.insert(n,Ui[n]);
        k.push_back(0.0);
        a.push_back(0.0);
        b.push_back(0.0);
        s12.push_back(0.0);
        s21.push_back(0.0);
        s11.push_back(0.0);
        s22.push_back(0.0);
        markUchanged();
    }
}
void PhysicalModel::remove_d(int n)
{
    if (0 < N && 0 < n && n <= N)
    {
        N -= 1;
        d.remove(n);
        m.remove(n);
        U.remove(n);
        Ui.remove(n);
        k.remove(n);
        a.remove(n);
        b.remove(n);
        s12.remove(n);
        s22.remove(n);
        s21.remove(n);
        s11.remove(n);
        markUchanged();
    }
}
void PhysicalModel::set_Ui(int n, double v)
{
    if (Ui[n] != v)
    {
        Ui[n] = v;
        markUchanged();
    }
}
void PhysicalModel::set_m(int n, double v)
{
    if (m[n] != v)
    {
        m[n] = v;
        markUchanged();
    }
}
/*void PhysicalModel::set_Ub(double v)
{
    if (Ub != v)
    {
        Ub = v;
        markUchanged();
    }
}*/
void PhysicalModel::set_Energy(double v)
{
    if (this->E0 != v)
    {
        this->E0 = v;
        set_E0(v);
        emit(signalEnergyChanged(v));
    }
}
void PhysicalModel::set_Time(double v)
{
    if (time != v)
    {
        this->time = v;
        emit(signalTimeChanged(v)); 
    }
}
void PhysicalModel::markUchanged()
{
    need_build_U = true;
    build_U();
    if(this->typeOfU==FINITE)
    {
    need_build_En = true;
    need_build_WP = true;
    emit(signalPotentialChanged());
    emit(signalEboundChanged());
    }
    else
    {
    need_build_En = true;
    emit(signalPotentialChanged());
    emit(signalEboundChanged());
    }
}
QVector<double> PhysicalModel::getEn()
{
    if (need_build_U)
    {
        build_U();
        need_build_U = false;
        need_build_En = true;
        need_build_WP = true;
    }
    if (need_build_En)
    {
        double Eold=this->E0;
        if(this->typeOfU==FINITE) findBoundStates();
        if(this->typeOfU==PERIODIC) findBands();
        this->E0=Eold;
        need_build_En = false;
    }
    return Ebound;
}

double PhysicalModel::getEn(int n)
{
    if (need_build_U)
    {
        build_U();
        need_build_U = false;
        need_build_En = true;
    }
    if (need_build_En)
    {
        if(this->typeOfU==FINITE) findBoundStates();
        if(this->typeOfU==PERIODIC) findBands();
        need_build_En = false;
    }
    return Ebound[n];
}

double PhysicalModel::get_U(int n)
{
    if (need_build_U)
    {
        build_U();
        need_build_U = false;
    }
    return U[n];
}
QVector<double> PhysicalModel::getPsiOfX_per(double E, double xmin, double xmax, int npoints, int viewWF)
{
    double y,tt,num;
    tt=0;
    double Eold=this->E0;
    set_E0(E);
//      build_k();
//      build_ab();
        build_k_per();
        build_ab_periodic();
    QVector<double> waveFunction(npoints);
    double dx = (xmax-xmin)/(npoints-1);
    for (int i=0; i < npoints; i++)
    {
        x = xmin + dx*i;
//        if(tail) this->b[N+1]=0.;
        build_Psi_per();
        y = psi_real;
        if(viewWF==1) y = psi_imag;
        else if(viewWF>1) y = Psi2;
        if (fabs(y) > 1e6)
        {
            if(y>0) y = 1e6;
            if(y<0) y = -1e6;
        }
        if (fabs(y) < 1e-8)
        {
            if(y>0) y = 1e-8;
            if(y<0) y = -1e-8;
        }
        waveFunction[i] = y;
    }
    //---------------
    this->E0=Eold;
    if(E0>0)
    {
        build_k();
        build_ab();
        build_RT();
    }
    else
    {
        num = findNumberOfLevels(E0);
    }
    if(E0>0) emit(signalTransmissionChanged(TT));
    else  emit(signalTransmissionChanged(num));
    return waveFunction;
}
QVector<double> PhysicalModel::getPsiOfX(double E, double xmin, double xmax, int npoints, int viewWF,bool tail)
{
    double y,tt,num;
    tt=0;
    double Eold=this->E0;
    set_E0(E);
    if(E>0)
    {
        build_k();
        build_ab();
        build_RT();
        tt = this->TT;
    }
    else
    {
        num = findNumberOfLevels(E);
    }
    QVector<double> waveFunction(npoints);
    double dx = (xmax-xmin)/(npoints-1);
    //   double fmin=1e-6;
    //   double fmax=1e2;
    for (int i=0; i < npoints; i++)
    {
        x = xmin + dx*i;
        if(tail) this->b[N+1]=0.;
        build_Psi();
        y = psi_real;
        if(viewWF==1) y = psi_imag;
        else if(viewWF>1) y = Psi2;
        if (fabs(y) > 1e6)
        {
            if(y>0) y = 1e6;
            if(y<0) y = -1e6;
        }
        if (fabs(y) < 1e-8)
        {
            if(y>0) y = 1e-8;
            if(y<0) y = -1e-8;
        }
        /*        if (fabs(y)<fmin)
        {
        y=0.;
        }
        if (fabs(y)>fmax)
        {
        if(y>fmax) y = fmax;
        else y=-fmax;
        }
        */
        waveFunction[i] = y;
    }
    //---------------
    this->E0=Eold;
    if(E0>0)
    {
        build_k();
        build_ab();
        build_RT();
    }
    else
    {
        num = findNumberOfLevels(E0);
    }
    if(E0>0) emit(signalTransmissionChanged(TT));
    else  emit(signalTransmissionChanged(num));
    return waveFunction;
}
void PhysicalModel::getTatE()//(double E)
{
    double num;
    if(this->E0>0)
        {
        build_k();
        build_ab();
        build_RT();
        }
        else
        {
            num = findNumberOfLevels(E0);
        }
        if(E0>0) emit(signalTransmissionChanged(TT));
        else  emit(signalTransmissionChanged(num));
}
QVector<double> PhysicalModel::getTransmissionOfE(double Emin, double Emax, int npoints)
{
    QVector<double> transmission(npoints);
    double dE = (Emax-Emin)/(npoints-1);
    double y;
    double Eold=this->E0;
    double z=this->zz;
    for (int i=0; i < npoints; i++)
    {
        set_E0(Emin+i*dE);
        if(this->typeOfU==FINITE)
        {
            build_k();
            build_ab();
            if(Emin+i*dE>0)
            {
                build_RT();
                transmission[i] = this->TT;
            }
            else
            {
                if(this->Ebound.size()>=1) y = 0.1*findNumberOfLevels(Emin+i*dE);
                else y=0;
                transmission[i] = y;
            }
        }
        else
        {
//        build_k_per();
//        build_ab_periodic();
        int iy=this->zeroPsiPer(this->E0);
//        transmission[i] = real(this->lambda);
//        transmission[i] = squaremod(this->lambda);
//        transmission[i] = this->funEn;//this->TT;
        transmission[i] = this->qa;

        }
    }
    this->E0=Eold;
    return transmission;

}
/*QVector<double> PhysicalModel::getTransmissionOfE(double Emin, double Emax, int npoints)
{
    QVector<double> transmission(npoints);
    double dE = (Emax-Emin)/(npoints-1);
    double y;
    double Eold=this->E0;
    double z=this->zz;
    for (int i=0; i < npoints; i++)
    {
        set_E0(Emin+i*dE);
        build_k();
         build_ab();
        if(Emin+i*dE>0)
        {
        build_RT();
         transmission[i] = this->TT;
        }
        else
        {
            if(this->Ebound.size()>=1) y = 0.1*findNumberOfLevels(Emin+i*dE);
            else y=0;
            transmission[i] = y;
        }
    }
    this->E0=Eold;
    return transmission;

}*/
void PhysicalModel::set_Uxz_forTz(double z)
{
    if(U1.size()==U2.size())
    {
        int N = U1.size()-2;
        set_N(U1.size()-2);
         if (N < 0) return;

        QVector<double> Uz(1+N+1);
        QVector<double> dz(1+N+1);
        QVector<double> mz(1+N+1);
         double UU,mm,dd;
        for(int n=0; n<=N+1; n++)
        {
            Ui[n] = U1[n] + z*(U2[n]-U1[n]);
            U[n] = Ui[n];
            d[n] = d1[n] + z*(d2[n]-d1[n]);
            m[n] = m1[n] + z*(m2[n]-m1[n]);
        }
    }
}
QVector<double> PhysicalModel::getTransmissionOfZ(double Zmin, double Zmax, int npoints)
{
    QVector<double> transmission(npoints);
    double dZ = (Zmax-Zmin)/(npoints-1);
    double y;
    double Eold=this->E0;
    double zold=this->zz;
 //   QVector<double> Uold;//    = get_Ui();
 //   Uold.resize(U.size());
    QVector<double> Uold = get_Ui();
 
    QVector<double> dold = get_d();
    QVector<double> mold = get_m();

    for (int i=0; i < npoints; i++)
    {
        double z=Zmin+i*dZ;
        set_Uxz_forTz(z);
        set_E0(Eold);
        build_k();
        build_ab();
        if(this->E0>0)
        {
            build_RT();
            transmission[i] = this->TT;
        }
        else
        {
            if(this->Ebound.size()>=1) y = 0.1*findNumberOfLevels(this->E0);
            else y=0;
            transmission[i] = y;
        }
    }
    this->E0=Eold;
    if(zold==-1)
    {
        zold=(Zmin+Zmax)*0.5;
        set_z(zold);
    }
    else
        set_Uxz_forTz(zold);

    set_Ui_d_m(Uold,dold,mold);
    int NU=this->U.size();
    int Nk=this->k.size();
    int Ns=this->s11.size();
    return transmission;
}

QVector<double> PhysicalModel::getPhiOfk(double E, double kmin, double kmax, int npoints)
{
    set_E0(E);
    build_k();
    build_ab();
    if(E<0)
    {
    b[U.size()-1]=0.;
    }
    QVector<double> waveFunction(npoints);
    double dk = (kmax-kmin)/(npoints-1);
   for (int i=0; i < npoints; i++)
    {
        this->kwave = kmin+i*dk;
        build_Phi();
        double y = Phi2;
        waveFunction[i] = y;
    }
    return waveFunction;
}
void PhysicalModel::set_Uxz(double z)
{
    if(U1.size()==U2.size())
    {
        int N = U1.size()-2;
        set_N(U1.size()-2);
         if (N < 0) return;

        QVector<double> Uz(1+N+1);
        QVector<double> dz(1+N+1);
        QVector<double> mz(1+N+1);
         double UU,mm,dd;
        for(int n=0; n<=N+1; n++)
        {
            UU = U1[n] + z*(U2[n]-U1[ n]);
            dd = d1[n] + z*(d2[n]-d1[n]);
            mm = m1[n] + z*(m2[n]-m1[n]);
            Ui[n] = U1[n] + z*(U2[n]-U1[n]);
            d[n] = d1[n] + z*(d2[n]-d1[n]);
            m[n] = m1[n] + z*(m2[n]-m1[n]);
        }
            this->markUchanged();
    }
}
void PhysicalModel::slotU1()
{
    const int N = getN();
    set_N1(N);
/*    N1=N;
    U1.resize(1+N+1);
    d1.resize(1+N+1);
    m1.resize(1+N+1);*/
    for(int n=0; n<=N+1; n++)
    {
        U1[n] = get_Ui(n);
        d1[n] = get_d(n);
        m1[n] = get_m(n);
    }
}
void PhysicalModel::slotU2()
{
    const int N = getN();
    set_N2(N);
    for(int n=0; n<=N+1; n++)
    {
      U2[n] = get_Ui(n);
        d2[n] = get_d(n);
        m2[n] = get_m(n);
    }
}
void PhysicalModel::set_WPmE()
{
    QVector<double> Ebound = this->getEn();
    if(Ebound.size()==0) return;
    int nn=Ebound.size();
    int nmax,nmin,hn;
    nmin=this->nminWP;
    nmax=this->nmaxWP;
    hn=this->hnWP;
    if(this->nmaxWP >= Ebound.size()) nmax = Ebound.size()-1;
    if(this->nminWP<0) nmin=0;
    if(this->nminWP>= Ebound.size()) nmin=Ebound.size()-1;;
    if(this->nmaxWP < this->nminWP) nmax = nmin;
    if(this->hnWP<0) hn=-this->hnWP;
    if(hn==0) hn=1;
    int NwpEm=(nmax-nmin)/hn + 1;
    EWofWP.clear();
    EWofWP.resize(NwpEm);
    int j=0;
    for(int i=nmin;i<=nmax;i+=hn)
    {
        EWofWP[j].E = Ebound[i];
        j++;
    }
    double Emin=EWofWP[0].E;
    double Emax=EWofWP[NwpEm-1].E;
    if(Emin==Emax)
    {
        EWofWP[0].w =1.;
    }
    else
    {
    double W=0;
    double nc=0.5*(nmin+nmax);
    double C=0.5*(nmax-nmin);
     C*=C;C=6/C;
    j=0;
    for(int i=nmin;i<=nmax;i+=hn)
    {
        double z=(i-nc);z*=z*C;
        W += EWofWP[j].w = exp(-z);
        j++;
    }
    for(int i=0;i<NwpEm;i++)
        EWofWP[i].w /=W;
    }
//-----------------------
    const int N = this->getN();
    int M=EWofWP.size();
    kp.resize(0, M-1, 0, N+1);
    ap.resize(0, M-1, 0, N+1);
    bp.resize(0, M-1, 0, N+1);
    for(int p=0;p<M;p++)
    {
        this->set_E0(EWofWP[p].E);
        this->build_k();
        this->build_ab();
        for(int n=0; n <= N+1; n++)
        {
            kp(p,n)=this->k[n];
            ap(p,n)=this->a[n];
            bp(p,n)=this->b[n];
        }
    }
    //-----------------------
}

void PhysicalModel::set_WPpE()//int wpN, double wpE_lo, double wpE_hi)
{
    EWofWP.clear();
    if(wpN<=0) wpN=1;
    if(wpE_hi==wpE_lo||wpN==1)
    {
        EWofWP.resize(1);
        EWofWP[0].E=wpE_hi;
        EWofWP[0].w=1.;
        return;
    }
    EWofWP.resize(wpN);
    if(wpE_hi<wpE_lo)
    {
        double a=wpE_hi;
        double b=wpE_lo;
        wpE_hi=b;
        wpE_lo=a;
    }
    double de=(wpE_hi-wpE_lo)/(wpN-1);
    for(int i=0;i<wpN;i++)
    {
        EWofWP[i].E = wpE_lo+i*de;
    }
    double W=0;
    double C=(wpE_hi-wpE_lo)/2;C*=C;C=6/C;
    double Ec=(wpE_hi+wpE_lo)/2;
    double z0=wpE_hi-Ec+de;z0*=z0*C;
    double ex0=exp(-z0);
    for(int i=0;i<wpN;i++)
    {
        double z=(EWofWP[i].E-Ec);z*=z*C;
        W += EWofWP[i].w = exp(-z)-ex0;

    }
    for(int i=0;i<wpN;i++)
        EWofWP[i].w /=W;

    //-----------------------
    const int N = this->getN();
    int M=EWofWP.size();
    kp.resize(0, M-1, 0, N+1);
    ap.resize(0, M-1, 0, N+1);
    bp.resize(0, M-1, 0, N+1);
    for(int p=0;p<M;p++)
    {
        this->set_E0(EWofWP[p].E);
        this->build_k();
        this->build_ab();
        for(int n=0; n <= N+1; n++)
        {
            kp(p,n)=this->k[n];
            ap(p,n)=this->a[n];
            bp(p,n)=this->b[n];
        }
    }
    //-----------------------
}

QVector<double>  PhysicalModel::getPsiOfXT(double t, double xmin, double xmax, int npoints, int viewWF)//, bool need_build_WavePacket)
{
    double Eold=this->E0;
    QVector<double> waveFunction(npoints);
    const int N = this->getN();
    this->time=t;
//    type_of_WP=this->get_type_of_WP();
    if(this->need_build_WP)
    {
        if(type_of_WP==1)
        {
            this->set_WPpE();
        }
        else
        {
            this->set_WPmE();
            if(Ebound.size()==0)
            {
                for (int i=0; i < npoints; i++)
                {
                    waveFunction[i]=0;
                }
                need_build_WP=false;
                return waveFunction;
            }
        }
        need_build_WP=false;
    }

        int M=EWofWP.size();
        std::vector<complex> expt;
        expt.resize(M);
        complex Psit=0;
        for(int p=0;p<M;p++)
        {

            complex c = exp(-complex(0, EWofWP[p].E*this->time));
//            if(this->get_U(N+1) >0) c = c*exp(complex(0,real(kp(p,N+1))*xmax) );
            if(EWofWP[p].E- this->get_U(N+1) >0) c = c*exp(complex(0,real(kp(p,N+1))*xmax) );
            expt[p]=c;
        }
        double y;
        double dx = (xmax-xmin)/(npoints-1);
        double fmin=1e-6;
        double fmax=1e4;
        for (int i=0; i < npoints; i++)
        {
            this->x = xmin + dx*i;
            complex Psit=(0.,0.);
            for(int p=0;p<M;p++)
            {
                this->set_E0(EWofWP[p].E);
                for(int n=0; n <= N+1; n++)
                {
                    this->k[n]=kp(p,n);
                    this->a[n]=ap(p,n);
                    this->b[n]=bp(p,n);
                }
                //               if (model->E0 < model->get_U(N+1))
                //                   model->b[N+1]=0.;
                this->build_Psi();
                Psit += this->psi*EWofWP[p].w*expt[p];

            }
            switch(viewWF)
            {
            case 2:
                y=squaremod(Psit);
                break;
            case 0:
                y=real(Psit);
                break;
            case 1:
                y=imag(Psit);
                break;
            default:
                break;
            }
            if (fabs(y)<fmin)
            {
                y=0.;
            }
            if (fabs(y)>fmax)
            {
                if(y>fmax) y = fmax;
                else y=-fmax;
            }
            waveFunction[i]=y;
        }
        TimeParameters tp;
        tp=this->getTimeParam();
        tp.time=this->time;
        if(tp.ht!=this->ht)
        {
             this->ht=tp.ht;
        }
        emit(signalTimeChanged(this->time));
        this->E0=Eold;
    return waveFunction;
}
QVector<double>  PhysicalModel::getPsiOfKT(double kmin, double kmax, int npoints)
{
    double Eold;
    Eold=this->E0;
    const int N = this->getN();
    int M=EWofWP.size();
    if(M==0)
    {
        this->set_WPmE();
        M=EWofWP.size();
    }
        QVector<double> waveFunction(npoints);
        std::vector<complex> expt;
        expt.resize(M);
        complex Psit=0;
        for(int p=0;p<M;p++)
        {

            complex c = exp(-complex(0, EWofWP[p].E*this->time));
            expt[p]=c;
        }
        double y;
        double dk = (kmax-kmin)/(npoints-1);
        double fmin=1e-6;
        double fmax=1e4;
        for (int i=0; i < npoints; i++)
        {
            this->kwave = kmin+i*dk;
            complex Psit=(0.,0.);
            for(int p=0;p<M;p++)
            {
                this->set_E0(EWofWP[p].E);
                for(int n=0; n <= N+1; n++)
                {
                    this->k[n]=kp(p,n);
                    this->a[n]=ap(p,n);
                    this->b[n]=bp(p,n);
                }
                //               if (model->E0 < model->get_U(N+1))
                //                   model->b[N+1]=0.;
//                this->build_Psi();
//                model->b[N+1] = 0.;
                this->build_Phi();
                Psit += this->phi*EWofWP[p].w*expt[p];

            }
            y=squaremod(Psit);
            if (fabs(y)<fmin)
            {
                y=0.;
            }
            if (fabs(y)>fmax)
            {
                if(y>fmax) y = fmax;
                else y=-fmax;
            }
            waveFunction[i]=y;
        }
        this->E0=Eold;
    return waveFunction;
}

///////////////////////////////////////////////////////////////////////////////////
class ModelXML
{
    PhysicalModel *model;
    QXmlStreamReader *r;
    QXmlStreamWriter *w;
public:
    ModelXML(PhysicalModel *_m,QXmlStreamReader *_r,QXmlStreamWriter *_w)
        : model(_m), r(_r), w(_w) {}
    void read();
    void write();
    void readWidthOfLine();
    void writeWidthOfLine();
    void readTime();
    void writeTime();
    void readLevelNumber();
    void writeLevelNumber();
    void readZdef();
    void writeZdef();
    void readE0();
    void writeE0();
    void readTypeWP();
    void writeTypeWP();
    void readScalesUx();
    void writeScalesUx();
    void readScalesPsi();
    void writeScalesPsi();
    void readScalesWPX();
    void writeScalesWPX();
    void readWPm();
    void writeWPm();
    void readWPp();
    void writeWPp();
    void readScalesWPK();
    void writeScalesWPK();
    void readScalesPhi();
    void writeScalesPhi();
    void readUbias();
    void writeUbias();
    void readUdm();
    void writeUdm();
    void readUdm1();
    void writeUdm1();
    void readUdm2();
    void writeUdm2();
    void readStep(double *u,double *d, double *m);
};

void ModelXML::read()
{
    Q_ASSERT(r->isStartElement() && r->name() == "model");
    while (!r->atEnd())
    {
        r->readNext();
        if ( r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "E0")
        {
            readE0();
        }
        else if (r->name() == "TypeWP")
        {
            readTypeWP();
        }
        else if (r->name() == "Ubias")
        {
            readUbias();
        }
        else if (r->name() == "udm")
        {
            readUdm();
        }
        else if (r->name() == "udm1")
        {
            readUdm1();
        }
        else if (r->name() == "udm2")
        {
            readUdm2();
        }
        else if (r->name() == "time")
        {
            readTime();
        }
        else if (r->name() == "LevelNumber")
        {
            readLevelNumber();
        }
        else if (r->name() == "Zdef")
        {
            readZdef();
        }
        else if (r->name() == "ScalesUx")
        {
            readScalesUx();
        }
        else if (r->name() == "ScalesPsi")
        {
            readScalesPsi();
        }
        else if (r->name() == "ScalesWPX")
        {
            readScalesWPX();
        }
        else if (r->name() == "WPm")
        {
            readWPm();
        }
        else if (r->name() == "WPp")
        {
            readWPp();
        }
        else if (r->name() == "ScalesWPK")
        {
            readScalesWPK();
        }
        else if (r->name() == "ScalesPhi")
        {
            readScalesPhi();
        }
        else
        {
            skipUnknownElement(r);
        }

    }
}

void ModelXML::write()
{
    w->writeStartElement("model");
    writeWidthOfLine();
    writeE0();
    writeUbias();
    writeUdm();
    writeUdm1();
    writeUdm2();
    writeTime();
    writeLevelNumber();
    writeZdef();
    writeScalesUx();
    writeScalesPsi();
    writeScalesWPX();
    writeWPm();
    writeWPp();
    writeTypeWP();
    writeScalesWPK();
    writeScalesPhi();
    w->writeEndElement();
}

void ModelXML::readE0()
{
    Q_ASSERT(r->isStartElement() && r->name() == "E0");

    QString s = r->readElementText();
    model->set_E0(s.toDouble());
}
void ModelXML::readTypeWP()
{
    Q_ASSERT(r->isStartElement() && r->name() == "TypeWP");

    QString s = r->readElementText();
    model->set_type_of_WP(s.toInt());
}
void ModelXML::readUbias()
{
    Q_ASSERT(r->isStartElement() && r->name() == "Ubias");

    QString s = r->readElementText();
    model->setUbias(s.toDouble());
}
void ModelXML::writeE0()
{
    QString s;
    s.sprintf("%lg",model->get_E0());
    w->writeTextElement("E0",s);
}
void ModelXML::writeTypeWP()
{
    QString s;
    s.sprintf("%i",model->get_type_of_WP());
    w->writeTextElement("TypeWP",s);
}
void ModelXML::writeUbias()
{
    QString s;
    s.sprintf("%lg",model->getUbias());
    w->writeTextElement("Ubias",s);
}

void ModelXML::readUdm()
{
    Q_ASSERT(r->isStartElement() && r->name() == "udm");

    QVector<double> u,d,m;

    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "step")
        {
            double du,dd,dm;
            readStep(&du,&dd,&dm);
            u.push_back(du);
            d.push_back(dd);
            m.push_back(dm);
        }
        else
            skipUnknownElement(r);
    }
    model->set_Ui_d_m(u,d,m);
}
void ModelXML::readUdm1()
{
    Q_ASSERT(r->isStartElement() && r->name() == "udm1");

    QVector<double> u,d,m;

    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "step")
        {
            double du,dd,dm;
            readStep(&du,&dd,&dm);
            u.push_back(du);
            d.push_back(dd);
            m.push_back(dm);
        }
        else
            skipUnknownElement(r);
    }
    model->set_U1_d1_m1(u,d,m);
}
void ModelXML::readUdm2()
{
    Q_ASSERT(r->isStartElement() && r->name() == "udm2");

    QVector<double> u,d,m;

    while (!r->atEnd())
    {
        r->readNext();
        if (r->isEndElement())
            break;
        if (!r->isStartElement())
            continue;
        if (r->name() == "step")
        {
            double du,dd,dm;
            readStep(&du,&dd,&dm);
            u.push_back(du);
            d.push_back(dd);
            m.push_back(dm);
        }
        else
            skipUnknownElement(r);
    }
    model->set_U2_d2_m2(u,d,m);
}
void ModelXML::readStep(double *u, double *d, double *m)
{
    Q_ASSERT(r->isStartElement() && r->name() == "step");

    QString s = r->readElementText();
    *u = *d = 0; *m = 1;
    sscanf_s(s.toAscii(),"%lg %lg %lg",u,d,m);
}
void ModelXML::writeUdm()
{
    w->writeStartElement("udm");
    for (int n = 0; n < model->getN() + 2; ++n)
    {
        QString step;
        double u=model->get_Ui(n);
        double d=model->get_d(n);
        double m=model->get_m(n);
        step.sprintf("%lg %lg %lg",u,d,m);
        w->writeTextElement("step",step);
    }
    w->writeEndElement();
}
void ModelXML::writeUdm1()
{
    w->writeStartElement("udm1");
    for (int n = 0; n < model->getN1() + 2; ++n)
    {
        QString step;
        double u=model->get_U1(n);
        double d=model->get_d1(n);
        double m=model->get_m1(n);
        step.sprintf("%lg %lg %lg",u,d,m);
        w->writeTextElement("step",step);
    }
    w->writeEndElement();
}
void ModelXML::writeUdm2()
{
    w->writeStartElement("udm2");
    for (int n = 0; n < model->getN2() + 2; ++n)
    {
        QString step;
        double u=model->get_U2(n);
        double d=model->get_d2(n);
        double m=model->get_m2(n);
        step.sprintf("%lg %lg %lg",u,d,m);
        w->writeTextElement("step",step);
    }
    w->writeEndElement();
}
void ModelXML::writeTime()
{
    QString s;
    TimeParameters tp=model->getTimeParam();
    double t=tp.time;
    double ht=tp.ht;
    double tmin=tp.tmin;
    double tmax=tp.tmax;
    s.sprintf("%lg %lg %lg %lg",t,ht,tmin,tmax);
    w->writeTextElement("time",s);
}
void ModelXML::writeZdef()
{
    QString s;
    zParameters tp = model->getzParam();
    double z=tp.z;
    double hz=tp.hz;
    double zmin=tp.zmin;
    double zmax=tp.zmax;
    s.sprintf("%lg %lg %lg %lg",z,hz,zmin,zmax);
    w->writeTextElement("Zdef",s);
}
void ModelXML::writeLevelNumber()
{
    QString s;
    LevelNumberParameters tp = model->getLevelNumberParameters();
    int hn=tp.hn;
    int nmin=tp.nmin;
    int nmax=tp.nmax;
    s.sprintf("%i %i %i",hn,nmin,nmax);
    w->writeTextElement("LevelNumber",s);
}
void ModelXML::writeWidthOfLine()
{
    QString s;
    SettingParameters wp=model->getSettingParameters();
    int iw=wp.lineWidth;
    s.sprintf("%i",iw);
    w->writeTextElement("Setting",s);
}
void ModelXML::writeWPm()
{
    QString s;
    EmWP tp = model->getEmWP();
    int hn=tp.hn;
    int nmin=tp.nmin;
    int nmax=tp.nmax;
    s.sprintf("%i %i %i",hn,nmin,nmax);
    w->writeTextElement("WPm",s);
}
void ModelXML::writeWPp()
{
    QString s;
    EpWP tp = model->getEpWP();
    int nn=tp.numberOfModes;
    double Emin=tp.E_low;
    double Emax=tp.E_high;
    s.sprintf("%i %lg %lg",nn,Emin,Emax);
    w->writeTextElement("WPp",s);
}
void ModelXML::writeScalesUx()
{
    QString s;
    ScalesUParameters tp = model->getScalesUParam();

    double hx=tp.Hx;
    double xmin=tp.Xmin;
    double xmax=tp.Xmax;
    double Umin=tp.Umin;
    double Umax=tp.Umax;
    s.sprintf("%lg %lg %lg %lg %lg",hx,xmin,xmax,Umin,Umax);
    w->writeTextElement("ScalesUx",s);
}
void ModelXML::writeScalesPsi()
{
    QString s;
    ScalePsinParameters tp = model->getScalePsinParam();

    double hx=tp.Hx;
    double xmin=tp.Xmin;
    double xmax=tp.Xmax;
    double Psimin=tp.Psinmin;
    double Psimax=tp.Psinmax;
    s.sprintf("%lg %lg %lg %lg %lg",hx,xmin,xmax,Psimin,Psimax);
    w->writeTextElement("ScalesPsi",s);
}
void ModelXML::writeScalesWPX()
{
    QString s;
    ScaleWPXParameters tp = model->getScaleWPXParam();

    double hx=tp.Hx;
    double xmin=tp.Xmin;
    double xmax=tp.Xmax;
    double Psimin=tp.WPXmin;
    double Psimax=tp.WPXmax;
    s.sprintf("%lg %lg %lg %lg %lg",hx,xmin,xmax,Psimin,Psimax);
    w->writeTextElement("ScalesWPX",s);
}
void ModelXML::writeScalesWPK()
{
    QString s;
    ScaleWPKParameters tp = model->getScaleWPKParam();

    double hk=tp.Hk;
    double kmin=tp.Kmin;
    double kmax=tp.Kmax;
    double Psimin=tp.WPKmin;
    double Psimax=tp.WPKmax;
    s.sprintf("%lg %lg %lg %lg %lg",hk,kmin,kmax,Psimin,Psimax);
    w->writeTextElement("ScalesWPK",s);
}
void ModelXML::writeScalesPhi()
{
    QString s;
    ScalePhinParameters tp = model->getScalePhinParam();

    double hk=tp.Hk;
    double kmin=tp.Kmin;
    double kmax=tp.Kmax;
    double Phimin=tp.Phinmin;
    double Phimax=tp.Phinmax;
    s.sprintf("%lg %lg %lg %lg %lg",hk,kmin,kmax,Phimin,Phimax);
    w->writeTextElement("ScalesPhi",s);
}

void ModelXML::readTime()
{
    Q_ASSERT(r->isStartElement() && r->name() == "time");
    double t,ht,tmin,tmax;
    t=0;
    ht=0.1;
    tmin=0;
    tmax=1000;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg",&t,&ht,&tmin,&tmax);
    TimeParameters tp;
    tp.time=t;
    tp.ht=ht;
    tp.tmin=tmin;
    tp.tmax=tmax;
    model->setTimeParam(tp);
}
void ModelXML::readWidthOfLine()
{
    Q_ASSERT(r->isStartElement() && r->name() == "Setting");
    QString s = r->readElementText();
    int iw;
    sscanf_s(s.toAscii(),"%i",iw);
    SettingParameters wp;  
    wp.lineWidth=iw;
    model->setSettingParameters(wp);  
}

void ModelXML::readLevelNumber()
{
    Q_ASSERT(r->isStartElement() && r->name() == "LevelNumber");
    int hn,nmin,nmax;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%i %i %i",&hn,&nmin,&nmax);
    LevelNumberParameters tp;
    tp.hn=hn;
    tp.nmin=nmin;
    tp.nmax=nmax;
    model->setLevelNumberParameters(tp);
}
void ModelXML::readWPm()
{
    Q_ASSERT(r->isStartElement() && r->name() == "WPm");
    int hn,nmin,nmax;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%i %i %i",&hn,&nmin,&nmax);
    EmWP tp;
    tp.hn=hn;
    tp.nmin=nmin;
    tp.nmax=nmax;
    model->setEmWP(tp);
}

void ModelXML::readWPp()
{
    Q_ASSERT(r->isStartElement() && r->name() == "WPp");
    int nn;
    double Emin, Emax;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%i %lg %lg",&nn,&Emin,&Emax);
    EpWP wp;
    wp.numberOfModes = nn;
    wp.E_low = Emin;
    wp.E_high = Emax;
    model->setEpWP(wp);
}

void ModelXML::readZdef()
{
    Q_ASSERT(r->isStartElement() && r->name() == "Zdef");
    double z,hz,zmin,zmax;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg",&z,&hz,&zmin,&zmax);
    zParameters tp;
    tp.z=z;
    tp.hz=hz;
    tp.zmin=zmin;
    tp.zmax=zmax;
    model->setzParam(tp);
}
void ModelXML::readScalesUx()
{
    Q_ASSERT(r->isStartElement() && r->name() == "ScalesUx");
    double hx,xmin,xmax,Umin,Umax;
    ScalesUParameters tp;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg %lg",&hx,&xmin,&xmax,&Umin,&Umax);
    tp.Hx=hx;
    tp.Xmin=xmin;
    tp.Xmax=xmax;
    tp.Umax=Umax;
    tp.Umin=Umin;
    model->setScalesUParam(tp);
}
void ModelXML::readScalesPsi()
{
    Q_ASSERT(r->isStartElement() && r->name() == "ScalesPsi");
    double hx,xmin,xmax,Psimin,Psimax;
    ScalePsinParameters tp;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg %lg",&hx,&xmin,&xmax,&Psimin,&Psimax);
    tp.Hx=hx;
    tp.Xmin=xmin;
    tp.Xmax=xmax;
    tp.Psinmax=Psimax;
    tp.Psinmin=Psimin;
    model->setScalePsinParam(tp);
}
void ModelXML::readScalesWPX()
{
    Q_ASSERT(r->isStartElement() && r->name() == "ScalesWPX");
    double hx,xmin,xmax,Psimin,Psimax;
    ScaleWPXParameters tp;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg %lg",&hx,&xmin,&xmax,&Psimin,&Psimax);
    tp.Hx=hx;
    tp.Xmin=xmin;
    tp.Xmax=xmax;
    tp.WPXmax=Psimax;
    tp.WPXmin=Psimin;
    model->setScaleWPXParam(tp);
}
void ModelXML::readScalesWPK()
{
    Q_ASSERT(r->isStartElement() && r->name() == "ScalesWPK");
    double hk,kmin,kmax,Psimin,Psimax;
    ScaleWPKParameters tp;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg %lg",&hk,&kmin,&kmax,&Psimin,&Psimax);
    tp.Hk=hk;
    tp.Kmin=kmin;
    tp.Kmax=kmax;
    tp.WPKmax=Psimax;
    tp.WPKmin=Psimin;
    model->setScaleWPKParam(tp);
}
void ModelXML::readScalesPhi()
{
    Q_ASSERT(r->isStartElement() && r->name() == "ScalesPhi");
    double hk,kmin,kmax,Phimin,Phimax;
    ScalePhinParameters tp;
    QString s = r->readElementText();
    sscanf_s(s.toAscii(),"%lg %lg %lg %lg %lg",&hk,&kmin,&kmax,&Phimin,&Phimax);
    tp.Hk=hk;
    tp.Kmin=kmin;
    tp.Kmax=kmax;
    tp.Phinmax=Phimax;
    tp.Phinmin=Phimin;
    model->setScalePhinParam(tp);
}

void PhysicalModel::readFromXml(QXmlStreamReader *r)
{
    Q_ASSERT(r->isStartElement() && r->name() == "model");

    ///PhysicalModel newModel;
    ///ModelXML reader(&newModel, r, NULL);
    ///reader.read();
    ///if (!reader.error()) setFrom(newModel);

    ModelXML reader(this, r, NULL);
    reader.read();
}
void PhysicalModel::writeToXml(QXmlStreamWriter *w)
{
    ModelXML writer(this, NULL, w);
    writer.write();
}


