#include "FindBand.h"

#define IFNOT(expr) if (!(expr))

inline int SIGN(double x)
{
    if (x < 0) return -1;
    if (x > 0) return +1;
    return 0;
}
inline int SIGN(int x)
{
    if (x < 0) return -1;
    if (x > 0) return +1;
    return 0;
}
inline bool CLOSE_ENOUGH(double a, double b)
{
    return fabs(a-b) < 1e-9;
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
        IFNOT(a.n + 1 == z.n)
        {
            Q_ASSERT(0);
        }
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
 * Find band center next above \a z.
 */
EQDN findNextBandCenter(PhysicalModel *model, EQDN z )
{
    double de = 1.0;
    while (1)
    {
        EQDN u = EQDN( z.e + de, model );
        if (z.n + 1 == u.n)
        {
            return findBandCenter(model,z,u);
        }
        if (z.n == u.n)
        {
            z = u;
            de *= 2;
        }
        else
        {
            de /= 3;
        }
    }
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

//    EQDN xx = lo.last();
//    EQDN yy = findNextBandCenter(model, z );
//    if (yy.e > xx.e)
//    {
//        lo.push_back(yy);
//    }

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
        IFNOT ( SIGN(a.d) != SIGN(z.d) )
        {
            Q_ASSERT(0);
        }
        double e1 = (a.e * z.d - a.d * z.e) / (z.d - a.d);
        double thr = 0.1 * (z.e - a.e);
        if (e1 <= a.e + thr || z.e - thr <= e1)
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
    return a.d < 0 ? a : z;
}

/**
 * Find two band edges contained in interval \a a ... \a z.
 */
QPair<EQDN,EQDN> findBandEdges2(PhysicalModel *model, EQDN a, EQDN z, int dqde_a)
{
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


//        EQDN m1 = EQDN( m.e + 1e-8, model );
        double h=0.25*(z.e - a.e);
        EQDN mm = EQDN( m.e - h, model );
        EQDN mp = EQDN( m.e + h, model );
        if (mm.d > 0)
        {
            // m1 belongs to band gap
            EQDN e1 = findBandEdge1( model, a, mm ); // a..m1 -> m..m1?
            EQDN e2 = findBandEdge1( model, mm, z );
            return QPair<EQDN,EQDN>( e1, e2 );
        }
        if (mp.d > 0)
        {
            // m1 belongs to band gap
            EQDN e1 = findBandEdge1( model, a, mp ); // a..m1 -> m..m1?
            EQDN e2 = findBandEdge1( model, mp, z );
            return QPair<EQDN,EQDN>( e1, e2 );
        }
        int dqde_mm = SIGN(m.q - mm.q);
        int dqde_mp = SIGN(mp.q - m.q);
        int dqde_z = -dqde_a;
        if ( SIGN(dqde_mm) == SIGN(dqde_a) )
        {
            a = mm;
        }
        if ( SIGN(dqde_mp) == SIGN(dqde_z) )
        {
            z = mp;
        }
        if((a.d<0&&abs(a.d)<1e-10)&&(z.d<=0&&abs(z.d)<1e-10))
        {
            return QPair<EQDN,EQDN>(a,z);
        }
/*        if((a.d<0&&abs(a.d)<1e-10)&&(z.d<=0&&abs(z.d)<1e-10))
        {
            return QPair<EQDN,EQDN>(a,z);
        }
*/
//        EQDN a1 = EQDN( a.e + h, model );
//        EQDN z1 = EQDN( z.e - h, model );
//        if(((m1.q-m.q)>=0&&(a1.q-a.q)>=0)||((m1.q-m.q)<=0&&(a1.q-a.q)<=0))
//        {
//            //на одной ветке c a
//            a=m1;
//            continue;
//        }
//        if(((m1.q-m.q)>=0&&(z1.q-z.q)>=0)||((m1.q-m.q)<=0&&(z1.q-z.q)<=0))
//        {
//            //на одной ветке c z
//            z=m;
//            continue;
//        }
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
    //Q_ASSERT( bandCenters.last().e <= z.e );

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
            int dqde_nc = (nc%2) ? -1 : +1;
            QPair<EQDN,EQDN> p = findBandEdges2( model, c, c1, dqde_nc );
            res.push_back(p.first);
            res.push_back(p.second);
        }
        // There may be one or two band edges in c[last]...z, try to find them
        if (nc == NC - 1 && c.e < z.e)
        {
            if (SIGN(c.d) != SIGN(z.d))
            {
                EQDN g = findBandEdge1( model, c, z );
                res.push_back(g);
                break;
            }
            /* else */
            double DELTAE = 0.1*(z.e - c.e);
            for (int k = 1; ; ++k)
            {
                double de = c.e + k*DELTAE;
                if (de > z.e) break;
                EQDN d = EQDN( de, model);
                if (SIGN(d.d) != SIGN(c.d))
                {
                    EQDN g1 = findBandEdge1( model, c, d );
                    EQDN g2 = findBandEdge1( model, d, z );
                    res.push_back(g1);
                    res.push_back(g2);
                    break;
                }
                EQDN c1 = EQDN( c.e + 1e-9, model);
                EQDN d1 = EQDN( d.e + 1e-9, model);
                if (d1.d > 0)
                {
                    EQDN g1 = findBandEdge1( model, c, d1 );
                    EQDN g2 = findBandEdge1( model, d1, z );
                    res.push_back(g1);
                    res.push_back(g2);
                    break;
                }
                if (SIGN(d1.q-d.q) != SIGN(c1.q - c.q))
                {
                    int dqde_nc = (nc%2) ? -1 : +1;
                    QPair<EQDN,EQDN> p = findBandEdges2( model, c, d1, dqde_nc );
                    res.push_back(p.first);
                    res.push_back(p.second);
                    break;
                }
            }
        }
    }
    while (res.size()>0 && res.last().e > z.e)
    {
        res.pop_back();
    }
    return res;
}
