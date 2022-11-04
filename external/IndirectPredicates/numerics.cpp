/****************************************************************************
* Indirect predicates for geometric constructions					        *
*                                                                           *
* Consiglio Nazionale delle Ricerche                                        *
* Istituto di Matematica Applicata e Tecnologie Informatiche                *
* Sezione di Genova                                                         *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Authors: Marco Attene                                                     *
* Copyright(C) 2019: IMATI-GE / CNR                                         *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation; either version 3 of the License, or (at  *
* your option) any later version.                                           *
*                                                                           *
* This program is distributed in the hope that it will be useful, but       *
* WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser  *
* General Public License for more details.                                  *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with this program.  If not, see http://www.gnu.org/licenses/.       *
*                                                                           *
****************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To compile on MSVC: use /fp:strict
// On GNU GCC: use -frounding-math
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "numerics.h"

void expansionObject::Two_Two_Prod(const double a1, const double a0, const double b1, const double b0, double* h)
{
	double _ch, _cl, _m, _n;
	Split(a0, _ah, _al);
	Split(b0, _bh, _bl);
	Two_Product_2Presplit(a0, _ah, _al, b0, _bh, _bl, _i, h[0]);
	Split(a1, _ch, _cl);
	Two_Product_2Presplit(a1, _ch, _cl, b0, _bh, _bl, _j, _0);
	Two_Sum(_i, _0, _k, _1);
	Quick_Two_Sum(_j, _k, _l, _2);
	Split(b1, _bh, _bl);
	Two_Product_2Presplit(a0, _ah, _al, b1, _bh, _bl, _i, _0);
	Two_Sum(_1, _0, _k, h[1]);
	Two_Sum(_2, _k, _j, _1);
	Two_Sum(_l, _j, _m, _2);
	Two_Product_2Presplit(a1, _ch, _cl, b1, _bh, _bl, _j, _0);
	Two_Sum(_i, _0, _n, _0);
	Two_Sum(_1, _0, _i, h[2]);
	Two_Sum(_2, _i, _k, _1);
	Two_Sum(_m, _k, _l, _2);
	Two_Sum(_j, _n, _k, _0);
	Two_Sum(_1, _0, _j, h[3]);
	Two_Sum(_2, _j, _i, _1);
	Two_Sum(_l, _i, _m, _2);
	Two_Sum(_1, _k, _i, h[4]);
	Two_Sum(_2, _i, _k, h[5]);
	Two_Sum(_m, _k, h[7], h[6]);
}

int expansionObject::Gen_Sum(const int elen, const double *e, const int flen, const double *f, double *h)
{
	double Q, Qn, hh, en = e[0], fn = f[0];
	int e_k, f_k, h_k;

	h_k = e_k = f_k = 0;
	if ((fn > en) == (fn > -en)) { Q = en; e_k++; } else { Q = fn; f_k++; }

	if ((e_k < elen) && (f_k < flen))
	{
		en = e[e_k]; fn = f[f_k];
		if ((fn > en) == (fn > -en)) { Quick_Two_Sum(en, Q, Qn, hh); e_k++; } else { Quick_Two_Sum(fn, Q, Qn, hh); f_k++; }
		Q = Qn;
		if (hh != 0.0) h[h_k++] = hh;
		while ((e_k < elen) && (f_k < flen))
		{
			en = e[e_k]; fn = f[f_k];
			if ((fn > en) == (fn > -en)) { Two_Sum(Q, en, Qn, hh); e_k++; } else { Two_Sum(Q, fn, Qn, hh); f_k++; }
			Q = Qn;
			if (hh != 0.0) h[h_k++] = hh;
		}
	}

	while (e_k < elen)
	{
		en = e[e_k++];
		Two_Sum(Q, en, Qn, hh);
		Q = Qn;
		if (hh != 0.0) h[h_k++] = hh;
	}

	while (f_k < flen)
	{
		fn = f[f_k++];
		Two_Sum(Q, fn, Qn, hh);
		Q = Qn;
		if (hh != 0.0) h[h_k++] = hh;
	}
	if ((Q != 0.0) || (h_k == 0)) h[h_k++] = Q;

	return h_k;
}

int expansionObject::Gen_Diff(const int elen, const double *e, const int flen, const double *f, double *h)
{
	double Q, Qn, hh, en = e[0], fn = -f[0];
	int e_k, f_k, h_k;

	h_k = e_k = f_k = 0;
	if ((fn > en) == (fn > -en)) { Q = en; e_k++; } else { Q = fn; f_k++; }

	if ((e_k < elen) && (f_k < flen))
	{
		en = e[e_k]; fn = -f[f_k];
		if ((fn > en) == (fn > -en)) { Quick_Two_Sum(en, Q, Qn, hh); e_k++; } else { Quick_Two_Sum(fn, Q, Qn, hh); f_k++; }
		Q = Qn;
		if (hh != 0.0) h[h_k++] = hh;
		while ((e_k < elen) && (f_k < flen))
		{
			en = e[e_k]; fn = -f[f_k];
			if ((fn > en) == (fn > -en)) { Two_Sum(Q, en, Qn, hh); e_k++; } else { Two_Sum(Q, fn, Qn, hh); f_k++; }
			Q = Qn;
			if (hh != 0.0) h[h_k++] = hh;
		}
	}

	while (e_k < elen)
	{
		en = e[e_k++];
		Two_Sum(Q, en, Qn, hh);
		Q = Qn;
		if (hh != 0.0) h[h_k++] = hh;
	}

	while (f_k < flen)
	{
		fn = -f[f_k++];
		Two_Sum(Q, fn, Qn, hh);
		Q = Qn;
		if (hh != 0.0) h[h_k++] = hh;
	}
	if ((Q != 0.0) || (h_k == 0)) h[h_k++] = Q;

	return h_k;
}


int expansionObject::Gen_Scale(const int elen, const double *e, const double& b, double *h)
{
	double Q, sum, hh, pr1, pr0, enow;
	int e_k, h_k;

	Split(b, _bh, _bl);
	Two_Prod_PreSplit(e[0], b, _bh, _bl, Q, hh);
	h_k = 0;
	if (hh != 0) h[h_k++] = hh;

	for (e_k = 1; e_k < elen; e_k++)
	{
		enow = e[e_k];
		Two_Prod_PreSplit(enow, b, _bh, _bl, pr1, pr0);
		Two_Sum(Q, pr0, sum, hh);
		if (hh != 0) h[h_k++] = hh;
		Quick_Two_Sum(pr1, sum, Q, hh);
		if (hh != 0) h[h_k++] = hh;
	}
	if ((Q != 0.0) || (h_k == 0)) h[h_k++] = Q;

	return h_k;
}


void expansionObject::Two_Square(const double& a1, const double& a0, double *x)
{
	Square(a0, _j, x[0]);
	_0 = a0 + a0;
	Two_Prod(a1, _0, _k, _1);
	Two_One_Sum(_k, _1, _j, _l, _2, x[1]);
	Square(a1, _j, _1);
	Two_Two_Sum(_j, _1, _l, _2, x[5], x[4], x[3], x[2]);
}

int expansionObject::Sub_product(const int alen, const double *a, const int blen, const double *b, double *h)
{
	if (alen == 1) return Gen_Scale(blen, b, a[0], h);
	int partial = 2 * alen * blen;
	int allmem = 2 * (partial + blen);
	double ph1_p[1024];
	double *ph1 = (allmem>1024) ? ((double *)malloc(allmem * sizeof(double))) : (ph1_p);
	double *ph2 = ph1 + partial;
	double *th = ph2 + partial;
	double *ph[2] = { ph1, ph2 };
	int first = 0;
	int phl = Gen_Scale(blen, b, a[0], ph[0]);

	for (int i = 1; i < alen; i++)
	{
		int thl = Gen_Scale(blen, b, a[i], th);
		first = i & 1;
		phl = Gen_Sum(phl, ph[(i+1)&1], thl, th, ph[first]);
	}
	if (first) for (int i = 0; i < phl; i++) h[i] = ph2[i];
	else for (int i = 0; i < phl; i++) h[i] = ph1[i];
	if (allmem>1024) free(ph1);
	return phl;
}


int expansionObject::Gen_Product(const int alen, const double *a, const int blen, const double *b, double *h)
{
	if (blen == 1) return Gen_Scale(alen, a, b[0], h);
	else if (alen < blen) return Sub_product(alen, a, blen, b, h);
	else return Sub_product(blen, b, alen, a, h);
}


double expansionObject::To_Double(const int elen, const double *e)
{
	double Q = e[0];
	for (int e_i = 1; e_i < elen; e_i++) Q += e[e_i];
	return Q;
}

#ifdef USE_SIMD_INSTRUCTIONS
void interval_number::init()
{
	zero = _mm_setzero_pd();
	sign_low_mask = _mm_slli_epi32(_mm_set_epi32(1, 0, 0, 0), 31);
	sign_high_mask = _mm_slli_epi32(_mm_set_epi32(0, 0, 1, 0), 31);
}

__m128d interval_number::zero;
__m128i interval_number::sign_low_mask, interval_number::sign_high_mask;

#endif

void initFPU()
{
#ifdef IS64BITPLATFORM
#ifdef USE_SIMD_INSTRUCTIONS
	interval_number::init();
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
#else
#ifdef USE_SIMD_INSTRUCTIONS
#error "USE_SIMD_INSTRUCTIONS cannot be defined in 32-bit mode"
#endif
#ifdef ISVISUALSTUDIO
	_control87(_PC_53, _MCW_PC); /* Set FPU control word for double precision. */
#else
	int cword;
	cword = 4722;                 /* set FPU control word for double precision */
	_FPU_SETCW(cword);
#endif
#endif
}
