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
#include <vector>

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

template<typename T> inline void swap(Matrix<T>& a, Matrix<T>& b) { a.swap(b); }
