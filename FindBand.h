#pragma once

#include "PhysicalModel.h"

struct EQDN
{
    double e,q,d; // energy, quasimomentum, discriminant
    int n;
    EQDN(double E, PhysicalModel *model)
    {
        e = E;
        n = model->zeroPsiPer( e );
        q = model->get_qa();
        d = model->discrim;
    }
    EQDN() : e(0), q(0), d(0), n(0) {}
};

EQDN findBandCenter(PhysicalModel *model, EQDN a, EQDN z);
QVector<EQDN> findBandCenters(PhysicalModel *model, EQDN a, EQDN z);
QVector<EQDN> findBandEdges(PhysicalModel *model, EQDN a, EQDN z, const QVector<EQDN>& bandCenters);
QPair<EQDN,EQDN> findBandEdges2(PhysicalModel *model, EQDN a, EQDN z, int sign_dqde_a);
EQDN findBandEdge1(PhysicalModel *model, EQDN a, EQDN z);
