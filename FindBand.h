/*
 *  Copyright (C) 2011 Olga Tkachenko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

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
