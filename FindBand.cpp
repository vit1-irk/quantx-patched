#include "FindBand.h"

inline int SIGN(double x)
{
    if (x < 0) return -1;
    if (x > 0) return +1;
    return 0;
}
inline bool CLOSE_ENOUGH(double a, double b)
{
    return fabs(a-b) < 1e-12;
}

/**
 * Find one band center in interval \a a ... \a z.
 * Band center is energy where number of zeros changes by 1.
 * Use dichotomy algorithm here.
 */
EQDN findBandCenter(PhysicalModel *model, EQDN a, EQDN z)
{
    EQDN m = a;
    while (!CLOSE_ENOUGH( a.e, z.e ))
    {
        Q_ASSERT(a.n + 1 == z.n);
        m = EQDN( (a.e + z.e)/2, model );

        if (a.n == m.n)
        {
            a = m;
        }
        else
        {
            z = m;
        }
    }
    return m;
}

/**
 * Find all band centers in interval \a a ... \a z.
 */
QVector<EQDN> findBandCenters(PhysicalModel *model, EQDN a, EQDN z)
{
    if (a.n == z.n)
    {
        return QVector<EQDN>();
    }
    if (a.n + 1 == z.n)
    {
        EQDN c = findBandCenter( model, a, z );
        QVector<EQDN> res;
        res.push_back( c );
        return res;
    }

    EQDN m = EQDN( (a.e + z.e)/2, model );

    QVector<EQDN> lo = findBandCenters( model, a, m );
    QVector<EQDN> hi = findBandCenters( model, m, z );

    lo << hi;

    //TODO: filter out duplicates
    return lo;
}

/**
 * Find one band edge contained in interval \a a ... \a z.
 * The band edge is the energy where discriminant changes sign.
 * Use a modification of linear intercept algorithm.
 */
EQDN findBandEdge1(PhysicalModel *model, EQDN a, EQDN z)
{
    EQDN m = a;
//    while (abs(a.d)>1.e-9||abs(z.d)>1.e-9)
    while (!CLOSE_ENOUGH( a.e, z.e ))
    {
        Q_ASSERT(SIGN(a.d) != SIGN(z.d));
        double e1;
//        double e1 = (a.e * z.d - a.d * z.e) / (z.d - a.d);
//my        double e1 = z.e-z.d*(z.e  - a.e) / (z.d - a.d);
        double thr = 0.1 * (z.e - a.e);
//        if (e1 <= a.e + thr || z.e - thr <= e1)
        {
            e1 = (a.e + z.e)/2;
        }
        m = EQDN( e1, model );

        if (SIGN(a.d) == SIGN(m.d))
        {
            a = m;
        }
        else
        {
            z = m;
        }
    }
    double dif;
    if(a.d<0)
    {
       dif = a.q;
       if(abs(dif)>1e-4) dif=abs(M_PI-abs(a.q));
       else dif=abs(dif);
       if(dif>0.1)
       {
       double dq=a.q;
       }
    }
    else
    {
       dif = z.q;
       if(abs(dif)>1e-4) dif=abs(M_PI-abs(z.q));
       else dif=abs(dif);
       if(dif>0.1)
       {
       double dq=a.q;
       }
    }

    if(abs(a.d)>1e-5||abs(z.d)>1e-5||dif>1e-4)
            {
            double ea=a.e;
            double ez=z.e;
            }
    return a.d < 0 ? a : z;
}

/**
 * Find two band edges contained in interval \a a ... \a z.
 */
QPair<EQDN,EQDN> findBandEdges2(PhysicalModel *model, EQDN a, EQDN z)
{
//    while (abs(a.d)>1.e-9||abs(z.d)>1.e-9) //???
    while (!CLOSE_ENOUGH( a.e, z.e ))
    {
        EQDN m = EQDN( (a.e + z.e)/2, model );
        if (m.d > 0)
        {
            // m belongs to band gap
            EQDN e1 = findBandEdge1( model, a, m );
            EQDN e2 = findBandEdge1( model, m, z );
            return QPair<EQDN,EQDN>( e1, e2 );
        }


        EQDN m1 = EQDN( m.e + 0.01*(z.e - a.e), model );
        if (m1.d > 0)
        {
            // m1 belongs to band gap
            EQDN e1 = findBandEdge1( model, a, m1 ); // a..m1 -> m..m1?
            EQDN e2 = findBandEdge1( model, m1, z );
            return QPair<EQDN,EQDN>( e1, e2 );
        }
/*        if((m1.q-z.q)>0&&(m.q-a.q)>0||(m1.q-z.q)<0&&(m.q-a.q)<0)
        {
            z=m1;
            a=m;
            continue;
        }
        */
        EQDN a1 = EQDN( a.e + 1e-8, model );
        EQDN z1 = EQDN( z.e + 1e-8, model );
        if(((m1.q-m.q)>0&&(a1.q-a.q)>0)||((m1.q-m.q)<0&&(a1.q-a.q)<0))
        {
        //на одной ветке c a
            a=m1;
            continue;
        }
        if(((m1.q-m.q)>0&&(z1.q-z.q)>0)||((m1.q-m.q)<0&&(z1.q-z.q)<0))
        {
        //на одной ветке c z
            z=m;
            continue;
        }
    }
    return QPair<EQDN,EQDN>(a,z);
}

/**
 * Given \a bandCenters locate all band edges in \a a ... \a z.
 */
QVector<EQDN> findBandEdges(PhysicalModel *model, EQDN a, EQDN z, const QVector<EQDN>& bandCenters)
{
    Q_ASSERT( bandCenters.size() > 0 );
    Q_ASSERT( a.e <= bandCenters.first().e );
    Q_ASSERT( bandCenters.last().e <= z.e );

    QVector<EQDN> res;
    int NC = bandCenters.size();
    for (int nc = 0; nc < NC; nc++)
    {
        EQDN c = bandCenters[nc];

        // There may be a band edge in a...c[0], find it
        if (nc == 0 && a.e < c.e && SIGN(a.d) != SIGN(c.d))
        {
            EQDN g = findBandEdge1( model, a, c );
            res.push_back(g);
        }
        // Find 2 edges in c[nc]...c[nc+1]
        if (nc < NC - 1)
        {
            EQDN c1 = bandCenters[nc+1];
            QPair<EQDN,EQDN> p = findBandEdges2(model, c, c1 );
            if(abs(p.first.d)<1.e-5&&abs(p.second.d)<1.e-5)
            {
                res.push_back(p.first); //???
                res.push_back(p.second); //???
            }
            else
            {
                EQDN e1=p.first;
                EQDN e2=p.second;
                res.push_back(p.first); //???
                res.push_back(p.second); //???
            }
        }
        // There may be one or two band edges in c[last]...z, try to find them
        if (nc == NC - 1 && c.e < z.e)
        {
            if (SIGN(c.d) != SIGN(z.d))
            {
                EQDN g = findBandEdge1( model, c, z );
                if(abs(g.d)<1.e-5) res.push_back(g);
                else
                {
                continue;
                }
                break;
            }
            /* else */
            double de = 0.1*(z.e - c.e);
            for (int k = 1 ;; ++k)
            {
                double e1 = c.e + k*de;
                if (e1 > z.e) break;
                EQDN c1 = EQDN( e1, model);
                if (SIGN(c1.d) != SIGN(c.d))
                {
                    EQDN g1 = findBandEdge1( model, c, c1 );
                    EQDN g2 = findBandEdge1( model, c1, z );
                    if(abs(g1.d)<1.e-5&&abs(g2.d)<1.e-5)
                    {
                        res.push_back(g1);
                        res.push_back(g2);
                        break;
                    }
                    else
                    {
                    continue;
                    }
                }
                if ((c1.q <= c.q && c1.q <= z.q) || (c1.q >= c.q && c1.q >= z.q))
                {
                    // c1 suggests a cusp
                    if(c.e<z.e) //???
                    {
                        QPair<EQDN,EQDN> p = findBandEdges2(model, c, z );
                        if(abs(p.second.e-p.first.e)>1.e-9)
                        {
                            res.push_back(p.first);
                            res.push_back(p.second);
                        }
                    }
                    break;
                }
            }
        }
    }
    return res;
}
