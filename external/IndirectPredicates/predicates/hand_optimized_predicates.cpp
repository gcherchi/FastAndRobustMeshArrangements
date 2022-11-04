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

/* TODO: include incircle too. */


#include "implicit_point.h"

#pragma intrinsic(fabs)

////////////////////// O R I E N T   2 D ////////////////////////////////////

inline int orient2d_filtered(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y)
{
	const double dl = (p2x - p1x) * (p3y - p1y);
    const double dr = (p2y - p1y) * (p3x - p1x);
    const double det = dl - dr;
    const double eb = 3.3306690738754706e-016 * (fabs(dl) + fabs(dr));
	return ((det >= eb) - (-det >= eb));
}

int orient2d_exact(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y)
{
    double acx[2], acy[2], bcx[2], bcy[2], dtl[2], dtr[2], B[4];
    double s[2], t[2], u[4], C1[8], C2[12], D[16];
    int C1l, C2l, Dl;
    expansionObject o;

    acx[1] = (p1x - p3x);
    bcx[1] = (p2x - p3x);
    acy[1] = (p1y - p3y);
    bcy[1] = (p2y - p3y);

    o.Two_Prod(acx[1], bcy[1], dtl);
    o.Two_Prod(acy[1], bcx[1], dtr);
    o.Two_Two_Diff(dtl, dtr, B);

    const double dsm = (fabs(dtl[1]) + fabs(dtr[1]));
    double det = expansionObject::To_Double(4, B);
    double eb = 2.2204460492503146e-16 * dsm;
    Dl = ((det >= eb) - (-det >= eb));
    if (Dl) return Dl;

    o.Two_Diff_Back(p1x, p3x, acx);
    o.Two_Diff_Back(p2x, p3x, bcx);
    o.Two_Diff_Back(p1y, p3y, acy);
    o.Two_Diff_Back(p2y, p3y, bcy);

    if ((acx[0] == 0.0) && (acy[0] == 0.0) && (bcx[0] == 0.0) && (bcy[0] == 0.0)) return ((det > 0) - (det < 0));

    eb = 1.1093356479670487e-31 * dsm + 3.3306690738754706e-16 * fabs(det);
    det += (acx[1] * bcy[0] + bcy[1] * acx[0]) - (acy[1] * bcx[0] + bcx[1] * acy[0]);
    Dl = ((det >= eb) - (-det >= eb));
    if (Dl) return Dl;

    o.Two_Prod(acx[0], bcy[1], s);
    o.Two_Prod(acy[0], bcx[1], t);
    o.Two_Two_Diff(s, t, u);
    C1l = o.Gen_Sum(4, B, 4, u, C1);

    o.Two_Prod(acx[1], bcy[0], s);
    o.Two_Prod(acy[1], bcx[0], t);
    o.Two_Two_Diff(s, t, u);
    C2l = o.Gen_Sum(C1l, C1, 4, u, C2);

    o.Two_Prod(acx[0], bcy[0], s);
    o.Two_Prod(acy[0], bcx[0], t);
    o.Two_Two_Diff(s, t, u);
    Dl = o.Gen_Sum(C2l, C2, 4, u, D);

    det = D[Dl - 1];
    return ((det >= eb) - (-det >= eb));
}

int orient2d(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y)
{
    const int ret = orient2d_filtered(p1x, p1y, p2x, p2y, p3x, p3y);
    if (ret) return ret;
    return orient2d_exact(p1x, p1y, p2x, p2y, p3x, p3y);
}


////////////////////// O R I E N T   3 D ////////////////////////////////////

static inline int o3d_subfilt(const double fadx, const double fbdx, const double fcdx,
    const double fady, const double fbdy, const double fcdy,
    const double fadz, const double fbdz, const double fcdz)
{
    const double fbdxcdy = fbdx * fcdy * fadz; const double fcdxbdy = fcdx * fbdy * fadz;
    const double fcdxady = fcdx * fady * fbdz; const double fadxcdy = fadx * fcdy * fbdz;
    const double fadxbdy = fadx * fbdy * fcdz; const double fbdxady = fbdx * fady * fcdz;

    const double det = (fbdxcdy - fcdxbdy) + (fcdxady - fadxcdy) + (fadxbdy - fbdxady);
    const double eb = 7.7715611723761027e-016 * (fabs(fbdxcdy) + fabs(fcdxbdy) + fabs(fcdxady) + fabs(fadxcdy) + fabs(fadxbdy) + fabs(fbdxady));
    return ((det >= eb) - (-det >= eb));
}

int orient3d_filtered(double px, double py, double pz, double qx, double qy, double qz, double rx, double ry, double rz, double sx, double sy, double sz)
{
    const double fadx = qx - px, fbdx = rx - px, fcdx = sx - px;
    const double fady = qy - py, fbdy = ry - py, fcdy = sy - py;
    const double fadz = qz - pz, fbdz = rz - pz, fcdz = sz - pz;

    return o3d_subfilt(fadx, fbdx, fcdx, fady, fbdy, fcdy, fadz, fbdz, fcdz);
}


inline void supo3d1(expansionObject& o,
    const double* c1, const double* c2, const double* c3, const double* c4, const double* c5, const double* c6,
	double* a1, double* a2, double& i, double* k1, double* k2, double* k3,
	double* k4, int& l1, int& l2)
{
	if (c1[0] == 0.0) {
		if (c2[0] == 0.0) {
			a1[0] = a2[0] = 0.0;
			l1 = l2 = 1;
		}
		else {
			i = -c2[0];
			o.Two_Prod(i, c3[1], a1);
			o.Two_Prod(c2[0], c4[1], a2);
			l1 = l2 = 2;
		}
	}
	else {
		if (c2[0] == 0.0) {
			i = -c1[0];
			o.Two_Prod(c1[0], c5[1], a1);
			o.Two_Prod(i, c6[1], a2);
			l1 = l2 = 2;
		}
		else {
			o.Two_Prod(c1[0], c5[1], k1);
			o.Two_Prod(c2[0], c3[1], k2);
			o.Two_Two_Diff(k1, k2, a1);
			o.Two_Prod(c2[0], c4[1], k3);
			o.Two_Prod(c1[0], c6[1], k4);
			o.Two_Two_Diff(k3, k4, a2);
			l1 = l2 = 4;
		}
	}
}

#define supo3d2(o, c1, c2, c3, c4, u, fl, fin, wh, c5, i, c6, c7) \
    if (c1[0] != 0.0) {\
        if (c2[0] != 0.0) {\
            o.Two_Prod(c1[0], c2[0], c3);\
            o.Two_One_Prod(c3, c4[1], u);\
            fl = o.Gen_Sum(fl, fin[wh], 4, u, fin[!wh]);\
            wh = !wh;\
            if (c4[0] != 0.0) {\
                o.Two_One_Prod(c3, c4[0], u);\
                fl = o.Gen_Sum(fl, fin[wh], 4, u, fin[!wh]);\
                wh = !wh;\
            }\
        }\
        if (c5[0] != 0.0) {\
            i = -c1[0];\
            o.Two_Prod(i, c5[0], c6);\
            o.Two_One_Prod(c6, c7[1], u);\
            fl = o.Gen_Sum(fl, fin[wh], 4, u, fin[!wh]);\
            wh = !wh;\
            if (c7[0] != 0.0) {\
                o.Two_One_Prod(c6, c7[0], u);\
                fl = o.Gen_Sum(fl, fin[wh], 4, u, fin[!wh]);\
                wh = !wh;\
            }\
        }\
    }


int orient3d_exact(double pdx, double pdy, double pdz, double pax, double pay, double paz, double pbx, double pby, double pbz, double pcx, double pcy, double pcz)
{
	double eb, det;
	double adx[2], bdx[2], cdx[2], ady[2], bdy[2], cdy[2], adz[2], bdz[2], cdz[2];
	double bdxcdy[2], cdxbdy[2], cdxady[2], adxcdy[2], adxbdy[2], bdxady[2];
	double bc[4], ca[4], ab[4];
	double bdxt_cdy[2], cdxt_bdy[2], cdxt_ady[2];
	double adxt_cdy[2], adxt_bdy[2], bdxt_ady[2];
	double bdyt_cdx[2], cdyt_bdx[2], cdyt_adx[2];
	double adyt_cdx[2], adyt_bdx[2], bdyt_adx[2];
	double bdxt_cdyt[2], cdxt_bdyt[2], cdxt_adyt[2];
	double adxt_cdyt[2], adxt_bdyt[2], bdxt_adyt[2];
	double u[4], v[12], w[16];
	double adet[8], bdet[8], cdet[8], abdet[16];
	double fin[2][192];
	int wh = 0;
	double at_b[4], at_c[4], bt_c[4], bt_a[4], ct_a[4], ct_b[4];
	double bct[8], cat[8], abt[8];
	int alen, blen, clen, finlen, vlen, wlen;
	int at_blen, at_clen, bt_clen, bt_alen, ct_alen, ct_blen;
	int bctlen, catlen, abtlen;
	int ablen;
	double inv;
	int ri;

	expansionObject o;

	adx[1] = pax - pdx;
	bdx[1] = pbx - pdx;
	cdx[1] = pcx - pdx;
	ady[1] = pay - pdy;
	bdy[1] = pby - pdy;
	cdy[1] = pcy - pdy;
	adz[1] = paz - pdz;
	bdz[1] = pbz - pdz;
	cdz[1] = pcz - pdz;

	o.Two_Prod(bdx[1], cdy[1], bdxcdy);
	o.Two_Prod(cdx[1], bdy[1], cdxbdy);
	o.Two_Two_Diff(bdxcdy, cdxbdy, bc);
	alen = o.Gen_Scale(4, bc, adz[1], adet);

	o.Two_Prod(cdx[1], ady[1], cdxady);
	o.Two_Prod(adx[1], cdy[1], adxcdy);
	o.Two_Two_Diff(cdxady, adxcdy, ca);
	blen = o.Gen_Scale(4, ca, bdz[1], bdet);

	o.Two_Prod(adx[1], bdy[1], adxbdy);
	o.Two_Prod(bdx[1], ady[1], bdxady);
	o.Two_Two_Diff(adxbdy, bdxady, ab);
	clen = o.Gen_Scale(4, ab, cdz[1], cdet);

	ablen = o.Gen_Sum(alen, adet, blen, bdet, abdet);
	finlen = o.Gen_Sum(ablen, abdet, clen, cdet, fin[wh]);

	double xx1 = bdxcdy[1] * adz[1]; double xx2 = cdxbdy[1] * adz[1];
	double yy1 = cdxady[1] * bdz[1]; double yy2 = adxcdy[1] * bdz[1];
	double zz1 = adxbdy[1] * cdz[1]; double zz2 = bdxady[1] * cdz[1];
	double pm = fabs(xx1) + fabs(xx2) + fabs(yy1) + fabs(yy2) + fabs(zz1) + fabs(zz2);

	det = expansionObject::To_Double(finlen, fin[wh]);
	eb = 3.3306690738754731e-016 * pm;
	ri = (det >= eb) - (-det >= eb);
	if (ri) return ri;

	o.Two_Diff_Back(pax, pdx, adx);
	o.Two_Diff_Back(pbx, pdx, bdx);
	o.Two_Diff_Back(pcx, pdx, cdx);
	o.Two_Diff_Back(pay, pdy, ady);
	o.Two_Diff_Back(pby, pdy, bdy);
	o.Two_Diff_Back(pcy, pdy, cdy);
	o.Two_Diff_Back(paz, pdz, adz);
	o.Two_Diff_Back(pbz, pdz, bdz);
	o.Two_Diff_Back(pcz, pdz, cdz);

	if ((adx[0] == 0.0) && (bdx[0] == 0.0) && (cdx[0] == 0.0) &&
		(ady[0] == 0.0) && (bdy[0] == 0.0) && (cdy[0] == 0.0) &&
		(adz[0] == 0.0) && (bdz[0] == 0.0) && (cdz[0] == 0.0)) return (det > 0) - (det < 0);

	eb = 3.2047474274603644e-031 * pm + 1.1102230246251565e-016 * fabs(det);
	det += (adz[1] * ((bdx[1] * cdy[0] + cdy[1] * bdx[0])
		- (bdy[1] * cdx[0] + cdx[1] * bdy[0]))
		+ adz[0] * (bdx[1] * cdy[1] - bdy[1] * cdx[1]))
		+ (bdz[1] * ((cdx[1] * ady[0] + ady[1] * cdx[0])
			- (cdy[1] * adx[0] + adx[1] * cdy[0]))
			+ bdz[0] * (cdx[1] * ady[1] - cdy[1] * adx[1]))
		+ (cdz[1] * ((adx[1] * bdy[0] + bdy[1] * adx[0])
			- (ady[1] * bdx[0] + bdx[1] * ady[0]))
			+ cdz[0] * (adx[1] * bdy[1] - ady[1] * bdx[1]));
	ri = (det >= eb) - (-det >= eb);
	if (ri) return ri;

	// Filters did not work. Compute exactly...
	supo3d1(o, adx, ady, bdx, cdx, bdy, cdy, at_b, at_c, inv,
		adxt_bdy, adyt_bdx, adyt_cdx, adxt_cdy, at_blen, at_clen);

	supo3d1(o, bdx, bdy, cdx, adx, cdy, ady, bt_c, bt_a, inv,
		bdxt_cdy, bdyt_cdx, bdyt_adx, bdxt_ady, bt_alen, bt_clen);

	supo3d1(o, cdx, cdy, adx, bdx, ady, bdy, ct_a, ct_b, inv,
		cdxt_ady, cdyt_adx, cdyt_bdx, cdxt_bdy, ct_alen, ct_blen);

	bctlen = o.Gen_Sum(bt_clen, bt_c, ct_blen, ct_b, bct);
	wlen = o.Gen_Scale(bctlen, bct, adz[1], w);
	finlen = o.Gen_Sum(finlen, fin[wh], wlen, w, fin[(!wh)]); wh = !wh;

	catlen = o.Gen_Sum(ct_alen, ct_a, at_clen, at_c, cat);
	wlen = o.Gen_Scale(catlen, cat, bdz[1], w);
	finlen = o.Gen_Sum(finlen, fin[wh], wlen, w, fin[(!wh)]); wh = !wh;

	abtlen = o.Gen_Sum(at_blen, at_b, bt_alen, bt_a, abt);
	wlen = o.Gen_Scale(abtlen, abt, cdz[1], w);
	finlen = o.Gen_Sum(finlen, fin[wh], wlen, w, fin[(!wh)]); wh = !wh;

	if (adz[0] != 0.0) {
		vlen = o.Gen_Scale(4, bc, adz[0], v);
		finlen = o.Gen_Sum(finlen, fin[wh], vlen, v, fin[(!wh)]); wh = !wh;
	}
	if (bdz[0] != 0.0) {
		vlen = o.Gen_Scale(4, ca, bdz[0], v);
		finlen = o.Gen_Sum(finlen, fin[wh], vlen, v, fin[(!wh)]); wh = !wh;
	}
	if (cdz[0] != 0.0) {
		vlen = o.Gen_Scale(4, ab, cdz[0], v);
		finlen = o.Gen_Sum(finlen, fin[wh], vlen, v, fin[(!wh)]); wh = !wh;
	}

	supo3d2(o, adx, bdy, adxt_bdyt, cdz, u, finlen, fin, wh, cdy, inv, adxt_cdyt, bdz);
	supo3d2(o, bdx, cdy, bdxt_cdyt, adz, u, finlen, fin, wh, ady, inv, bdxt_adyt, cdz);
	supo3d2(o, cdx, ady, cdxt_adyt, bdz, u, finlen, fin, wh, bdy, inv, cdxt_bdyt, adz);

	if (adz[0] != 0.0) {
		wlen = o.Gen_Scale(bctlen, bct, adz[0], w);
		finlen = o.Gen_Sum(finlen, fin[wh], wlen, w, fin[!wh]); wh = !wh;
	}
	if (bdz[0] != 0.0) {
		wlen = o.Gen_Scale(catlen, cat, bdz[0], w);
		finlen = o.Gen_Sum(finlen, fin[wh], wlen, w, fin[!wh]); wh = !wh;
	}
	if (cdz[0] != 0.0) {
		wlen = o.Gen_Scale(abtlen, abt, cdz[0], w);
		finlen = o.Gen_Sum(finlen, fin[wh], wlen, w, fin[!wh]);	wh = !wh;
	}

	det = fin[wh][finlen - 1];
	return (det > 0) - (det < 0);
}


int orient3d(double px, double py, double pz, double qx, double qy, double qz, double rx, double ry, double rz, double sx, double sy, double sz)
{
   const int ret = orient3d_filtered(px, py, pz, qx, qy, qz, rx, ry, rz, sx, sy, sz);
   if (ret) return ret;
   return orient3d_exact(px, py, pz, qx, qy, qz, rx, ry, rz, sx, sy, sz);
}


////////////////////// I N S P H E R E ////////////////////////////////////

static inline int isp_subfilt(const double  aex, const double  bex, const double  cex, const double  dex, 
    const double  aey, const double  bey, const double  cey, const double  dey, 
    const double  aez, const double  bez, const double  cez, const double  dez)
{
    const double aexbey = aex * bey;
    const double bexaey = bex * aey;
    const double ab = aexbey - bexaey;
    const double bexcey = bex * cey;
    const double cexbey = cex * bey;
    const double bc = bexcey - cexbey;
    const double cexdey = cex * dey;
    const double dexcey = dex * cey;
    const double cd = cexdey - dexcey;
    const double dexaey = dex * aey;
    const double aexdey = aex * dey;
    const double da = dexaey - aexdey;
    const double aexcey = aex * cey;
    const double cexaey = cex * aey;
    const double ac = aexcey - cexaey;
    const double bexdey = bex * dey;
    const double dexbey = dex * bey;
    const double bd = bexdey - dexbey;
    const double abc1 = aez * bc;
    const double abc2 = bez * ac;
    const double abc3 = cez * ab;
    const double abc4 = abc1 + abc3;
    const double abc = abc4 - abc2;
    const double bcd1 = bez * cd;
    const double bcd2 = cez * bd;
    const double bcd3 = dez * bc;
    const double bcd4 = bcd1 + bcd3;
    const double bcd = bcd4 - bcd2;
    const double cda1 = cez * da;
    const double cda2 = dez * ac;
    const double cda3 = aez * cd;
    const double cda4 = cda1 + cda3;
    const double cda = cda4 + cda2;
    const double dab1 = dez * ab;
    const double dab2 = aez * bd;
    const double dab3 = bez * da;
    const double dab4 = dab1 + dab3;
    const double dab = dab4 + dab2;
    const double al1 = aex * aex;
    const double al2 = aey * aey;
    const double al3 = aez * aez;
    const double al4 = al1 + al2;
    const double alift = al4 + al3;
    const double bl1 = bex * bex;
    const double bl2 = bey * bey;
    const double bl3 = bez * bez;
    const double bl4 = bl1 + bl2;
    const double blift = bl4 + bl3;
    const double cl1 = cex * cex;
    const double cl2 = cey * cey;
    const double cl3 = cez * cez;
    const double cl4 = cl1 + cl2;
    const double clift = cl4 + cl3;
    const double dl1 = dex * dex;
    const double dl2 = dey * dey;
    const double dl3 = dez * dez;
    const double dl4 = dl1 + dl2;
    const double dlift = dl4 + dl3;
    const double ds1 = dlift * abc;
    const double ds2 = clift * dab;
    const double dl = ds2 - ds1;
    const double dr1 = blift * cda;
    const double dr2 = alift * bcd;
    const double dr = dr2 - dr1;
    const double det = dl + dr;

    double _tmp_fabs;

    double max_var = 0.0;

    if ((_tmp_fabs = fabs(aex)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(bex)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(cex)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(dex)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(aey)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(bey)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(cey)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(dey)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(aez)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(bez)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(cez)) > max_var) max_var = _tmp_fabs;
    if ((_tmp_fabs = fabs(dez)) > max_var) max_var = _tmp_fabs;
    double epsilon = max_var;
    epsilon *= epsilon;
    epsilon *= epsilon;
    epsilon *= max_var;
    epsilon *= 1.145750161413163e-13;
    return ((det >= epsilon) - (-det >= epsilon));
}

int insphere_filtered(const double pax, const double pay, const double paz, const double pbx, const double pby, const double pbz, const double pcx, const double pcy, const double pcz, const double pdx, const double pdy, const double pdz, const double pex, const double pey, const double pez)
{
    return isp_subfilt(pax - pex, pbx - pex, pcx - pex, pdx - pex,
        pay - pey, pby - pey, pcy - pey, pdy - pey,
        paz - pez, pbz - pez, pcz - pez, pdz - pez);
}


int insphere_interval(const interval_number pax, const interval_number pay, const interval_number paz, const interval_number pbx, const interval_number pby, const interval_number pbz, const interval_number pcx, const interval_number pcy, const interval_number pcz, const interval_number pdx, const interval_number pdy, const interval_number pdz, const interval_number pex, const interval_number pey, const interval_number pez)
{
    setFPUModeToRoundUP();
    const interval_number aex(pax - pex);
    const interval_number bex(pbx - pex);
    const interval_number cex(pcx - pex);
    const interval_number dex(pdx - pex);
    const interval_number aey(pay - pey);
    const interval_number bey(pby - pey);
    const interval_number cey(pcy - pey);
    const interval_number dey(pdy - pey);
    const interval_number aez(paz - pez);
    const interval_number bez(pbz - pez);
    const interval_number cez(pcz - pez);
    const interval_number dez(pdz - pez);
    const interval_number aexbey(aex * bey);
    const interval_number bexaey(bex * aey);
    const interval_number ab(aexbey - bexaey);
    const interval_number bexcey(bex * cey);
    const interval_number cexbey(cex * bey);
    const interval_number bc(bexcey - cexbey);
    const interval_number cexdey(cex * dey);
    const interval_number dexcey(dex * cey);
    const interval_number cd(cexdey - dexcey);
    const interval_number dexaey(dex * aey);
    const interval_number aexdey(aex * dey);
    const interval_number da(dexaey - aexdey);
    const interval_number aexcey(aex * cey);
    const interval_number cexaey(cex * aey);
    const interval_number ac(aexcey - cexaey);
    const interval_number bexdey(bex * dey);
    const interval_number dexbey(dex * bey);
    const interval_number bd(bexdey - dexbey);
    const interval_number abc1(aez * bc);
    const interval_number abc2(bez * ac);
    const interval_number abc3(cez * ab);
    const interval_number abc4(abc1 + abc3);
    const interval_number abc(abc4 - abc2);
    const interval_number bcd1(bez * cd);
    const interval_number bcd2(cez * bd);
    const interval_number bcd3(dez * bc);
    const interval_number bcd4(bcd1 + bcd3);
    const interval_number bcd(bcd4 - bcd2);
    const interval_number cda1(cez * da);
    const interval_number cda2(dez * ac);
    const interval_number cda3(aez * cd);
    const interval_number cda4(cda1 + cda3);
    const interval_number cda(cda4 + cda2);
    const interval_number dab1(dez * ab);
    const interval_number dab2(aez * bd);
    const interval_number dab3(bez * da);
    const interval_number dab4(dab1 + dab3);
    const interval_number dab(dab4 + dab2);
    const interval_number al1(aex * aex);
    const interval_number al2(aey * aey);
    const interval_number al3(aez * aez);
    const interval_number al4(al1 + al2);
    const interval_number alift(al4 + al3);
    const interval_number bl1(bex * bex);
    const interval_number bl2(bey * bey);
    const interval_number bl3(bez * bez);
    const interval_number bl4(bl1 + bl2);
    const interval_number blift(bl4 + bl3);
    const interval_number cl1(cex * cex);
    const interval_number cl2(cey * cey);
    const interval_number cl3(cez * cez);
    const interval_number cl4(cl1 + cl2);
    const interval_number clift(cl4 + cl3);
    const interval_number dl1(dex * dex);
    const interval_number dl2(dey * dey);
    const interval_number dl3(dez * dez);
    const interval_number dl4(dl1 + dl2);
    const interval_number dlift(dl4 + dl3);
    const interval_number ds1(dlift * abc);
    const interval_number ds2(clift * dab);
    const interval_number dl(ds2 - ds1);
    const interval_number dr1(blift * cda);
    const interval_number dr2(alift * bcd);
    const interval_number dr(dr2 - dr1);
    const interval_number det(dl + dr);
    setFPUModeToRoundNEAR();

    if (!det.signIsReliable()) return Filtered_Sign::UNCERTAIN;
    return det.sign();
}

int insphere_exact(const double pax, const double pay, const double paz, const double pbx, const double pby, const double pbz, const double pcx, const double pcy, const double pcz, const double pdx, const double pdy, const double pdz, const double pex, const double pey, const double pez)
{
    expansionObject o;
    double aex[2];
    o.two_Diff(pax, pex, aex);
    double bex[2];
    o.two_Diff(pbx, pex, bex);
    double cex[2];
    o.two_Diff(pcx, pex, cex);
    double dex[2];
    o.two_Diff(pdx, pex, dex);
    double aey[2];
    o.two_Diff(pay, pey, aey);
    double bey[2];
    o.two_Diff(pby, pey, bey);
    double cey[2];
    o.two_Diff(pcy, pey, cey);
    double dey[2];
    o.two_Diff(pdy, pey, dey);
    double aez[2];
    o.two_Diff(paz, pez, aez);
    double bez[2];
    o.two_Diff(pbz, pez, bez);
    double cez[2];
    o.two_Diff(pcz, pez, cez);
    double dez[2];
    o.two_Diff(pdz, pez, dez);
    double aexbey[8];
    int aexbey_len = o.Gen_Product(2, aex, 2, bey, aexbey);
    double bexaey[8];
    int bexaey_len = o.Gen_Product(2, bex, 2, aey, bexaey);
    double ab[16];
    int ab_len = o.Gen_Diff(aexbey_len, aexbey, bexaey_len, bexaey, ab);
    double bexcey[8];
    int bexcey_len = o.Gen_Product(2, bex, 2, cey, bexcey);
    double cexbey[8];
    int cexbey_len = o.Gen_Product(2, cex, 2, bey, cexbey);
    double bc[16];
    int bc_len = o.Gen_Diff(bexcey_len, bexcey, cexbey_len, cexbey, bc);
    double cexdey[8];
    int cexdey_len = o.Gen_Product(2, cex, 2, dey, cexdey);
    double dexcey[8];
    int dexcey_len = o.Gen_Product(2, dex, 2, cey, dexcey);
    double cd[16];
    int cd_len = o.Gen_Diff(cexdey_len, cexdey, dexcey_len, dexcey, cd);
    double dexaey[8];
    int dexaey_len = o.Gen_Product(2, dex, 2, aey, dexaey);
    double aexdey[8];
    int aexdey_len = o.Gen_Product(2, aex, 2, dey, aexdey);
    double da[16];
    int da_len = o.Gen_Diff(dexaey_len, dexaey, aexdey_len, aexdey, da);
    double aexcey[8];
    int aexcey_len = o.Gen_Product(2, aex, 2, cey, aexcey);
    double cexaey[8];
    int cexaey_len = o.Gen_Product(2, cex, 2, aey, cexaey);
    double ac[16];
    int ac_len = o.Gen_Diff(aexcey_len, aexcey, cexaey_len, cexaey, ac);
    double bexdey[8];
    int bexdey_len = o.Gen_Product(2, bex, 2, dey, bexdey);
    double dexbey[8];
    int dexbey_len = o.Gen_Product(2, dex, 2, bey, dexbey);
    double bd[16];
    int bd_len = o.Gen_Diff(bexdey_len, bexdey, dexbey_len, dexbey, bd);
    double abc1_p[32], * abc1 = abc1_p;
    int abc1_len = o.Gen_Product_With_PreAlloc(2, aez, bc_len, bc, &abc1, 32);
    double abc2_p[32], * abc2 = abc2_p;
    int abc2_len = o.Gen_Product_With_PreAlloc(2, bez, ac_len, ac, &abc2, 32);
    double abc3_p[32], * abc3 = abc3_p;
    int abc3_len = o.Gen_Product_With_PreAlloc(2, cez, ab_len, ab, &abc3, 32);
    double abc4_p[32], * abc4 = abc4_p;
    int abc4_len = o.Gen_Sum_With_PreAlloc(abc1_len, abc1, abc3_len, abc3, &abc4, 32);
    double abc_p[32], * abc = abc_p;
    int abc_len = o.Gen_Diff_With_PreAlloc(abc4_len, abc4, abc2_len, abc2, &abc, 32);
    double bcd1_p[32], * bcd1 = bcd1_p;
    int bcd1_len = o.Gen_Product_With_PreAlloc(2, bez, cd_len, cd, &bcd1, 32);
    double bcd2_p[32], * bcd2 = bcd2_p;
    int bcd2_len = o.Gen_Product_With_PreAlloc(2, cez, bd_len, bd, &bcd2, 32);
    double bcd3_p[32], * bcd3 = bcd3_p;
    int bcd3_len = o.Gen_Product_With_PreAlloc(2, dez, bc_len, bc, &bcd3, 32);
    double bcd4_p[32], * bcd4 = bcd4_p;
    int bcd4_len = o.Gen_Sum_With_PreAlloc(bcd1_len, bcd1, bcd3_len, bcd3, &bcd4, 32);
    double bcd_p[32], * bcd = bcd_p;
    int bcd_len = o.Gen_Diff_With_PreAlloc(bcd4_len, bcd4, bcd2_len, bcd2, &bcd, 32);
    double cda1_p[32], * cda1 = cda1_p;
    int cda1_len = o.Gen_Product_With_PreAlloc(2, cez, da_len, da, &cda1, 32);
    double cda2_p[32], * cda2 = cda2_p;
    int cda2_len = o.Gen_Product_With_PreAlloc(2, dez, ac_len, ac, &cda2, 32);
    double cda3_p[32], * cda3 = cda3_p;
    int cda3_len = o.Gen_Product_With_PreAlloc(2, aez, cd_len, cd, &cda3, 32);
    double cda4_p[32], * cda4 = cda4_p;
    int cda4_len = o.Gen_Sum_With_PreAlloc(cda1_len, cda1, cda3_len, cda3, &cda4, 32);
    double cda_p[32], * cda = cda_p;
    int cda_len = o.Gen_Sum_With_PreAlloc(cda4_len, cda4, cda2_len, cda2, &cda, 32);
    double dab1_p[32], * dab1 = dab1_p;
    int dab1_len = o.Gen_Product_With_PreAlloc(2, dez, ab_len, ab, &dab1, 32);
    double dab2_p[32], * dab2 = dab2_p;
    int dab2_len = o.Gen_Product_With_PreAlloc(2, aez, bd_len, bd, &dab2, 32);
    double dab3_p[32], * dab3 = dab3_p;
    int dab3_len = o.Gen_Product_With_PreAlloc(2, bez, da_len, da, &dab3, 32);
    double dab4_p[32], * dab4 = dab4_p;
    int dab4_len = o.Gen_Sum_With_PreAlloc(dab1_len, dab1, dab3_len, dab3, &dab4, 32);
    double dab_p[32], * dab = dab_p;
    int dab_len = o.Gen_Sum_With_PreAlloc(dab4_len, dab4, dab2_len, dab2, &dab, 32);
    double al1[8];
    int al1_len = o.Gen_Product(2, aex, 2, aex, al1);
    double al2[8];
    int al2_len = o.Gen_Product(2, aey, 2, aey, al2);
    double al3[8];
    int al3_len = o.Gen_Product(2, aez, 2, aez, al3);
    double al4[16];
    int al4_len = o.Gen_Sum(al1_len, al1, al2_len, al2, al4);
    double alift[24];
    int alift_len = o.Gen_Sum(al4_len, al4, al3_len, al3, alift);
    double bl1[8];
    int bl1_len = o.Gen_Product(2, bex, 2, bex, bl1);
    double bl2[8];
    int bl2_len = o.Gen_Product(2, bey, 2, bey, bl2);
    double bl3[8];
    int bl3_len = o.Gen_Product(2, bez, 2, bez, bl3);
    double bl4[16];
    int bl4_len = o.Gen_Sum(bl1_len, bl1, bl2_len, bl2, bl4);
    double blift[24];
    int blift_len = o.Gen_Sum(bl4_len, bl4, bl3_len, bl3, blift);
    double cl1[8];
    int cl1_len = o.Gen_Product(2, cex, 2, cex, cl1);
    double cl2[8];
    int cl2_len = o.Gen_Product(2, cey, 2, cey, cl2);
    double cl3[8];
    int cl3_len = o.Gen_Product(2, cez, 2, cez, cl3);
    double cl4[16];
    int cl4_len = o.Gen_Sum(cl1_len, cl1, cl2_len, cl2, cl4);
    double clift[24];
    int clift_len = o.Gen_Sum(cl4_len, cl4, cl3_len, cl3, clift);
    double dl1[8];
    int dl1_len = o.Gen_Product(2, dex, 2, dex, dl1);
    double dl2[8];
    int dl2_len = o.Gen_Product(2, dey, 2, dey, dl2);
    double dl3[8];
    int dl3_len = o.Gen_Product(2, dez, 2, dez, dl3);
    double dl4[16];
    int dl4_len = o.Gen_Sum(dl1_len, dl1, dl2_len, dl2, dl4);
    double dlift[24];
    int dlift_len = o.Gen_Sum(dl4_len, dl4, dl3_len, dl3, dlift);
    double ds1_p[32], * ds1 = ds1_p;
    int ds1_len = o.Gen_Product_With_PreAlloc(dlift_len, dlift, abc_len, abc, &ds1, 32);
    double ds2_p[32], * ds2 = ds2_p;
    int ds2_len = o.Gen_Product_With_PreAlloc(clift_len, clift, dab_len, dab, &ds2, 32);
    double dl_p[32], * dl = dl_p;
    int dl_len = o.Gen_Diff_With_PreAlloc(ds2_len, ds2, ds1_len, ds1, &dl, 32);
    double dr1_p[32], * dr1 = dr1_p;
    int dr1_len = o.Gen_Product_With_PreAlloc(blift_len, blift, cda_len, cda, &dr1, 32);
    double dr2_p[32], * dr2 = dr2_p;
    int dr2_len = o.Gen_Product_With_PreAlloc(alift_len, alift, bcd_len, bcd, &dr2, 32);
    double dr_p[32], * dr = dr_p;
    int dr_len = o.Gen_Diff_With_PreAlloc(dr2_len, dr2, dr1_len, dr1, &dr, 32);
    double det_p[32], * det = det_p;
    int det_len = o.Gen_Sum_With_PreAlloc(dl_len, dl, dr_len, dr, &det, 32);

    double return_value = det[det_len - 1];
    if (det_p != det) free(det);
    if (dr_p != dr) free(dr);
    if (dr2_p != dr2) free(dr2);
    if (dr1_p != dr1) free(dr1);
    if (dl_p != dl) free(dl);
    if (ds2_p != ds2) free(ds2);
    if (ds1_p != ds1) free(ds1);
    if (dab_p != dab) free(dab);
    if (dab4_p != dab4) free(dab4);
    if (dab3_p != dab3) free(dab3);
    if (dab2_p != dab2) free(dab2);
    if (dab1_p != dab1) free(dab1);
    if (cda_p != cda) free(cda);
    if (cda4_p != cda4) free(cda4);
    if (cda3_p != cda3) free(cda3);
    if (cda2_p != cda2) free(cda2);
    if (cda1_p != cda1) free(cda1);
    if (bcd_p != bcd) free(bcd);
    if (bcd4_p != bcd4) free(bcd4);
    if (bcd3_p != bcd3) free(bcd3);
    if (bcd2_p != bcd2) free(bcd2);
    if (bcd1_p != bcd1) free(bcd1);
    if (abc_p != abc) free(abc);
    if (abc4_p != abc4) free(abc4);
    if (abc3_p != abc3) free(abc3);
    if (abc2_p != abc2) free(abc2);
    if (abc1_p != abc1) free(abc1);

    if (return_value > 0) return IP_Sign::POSITIVE;
    if (return_value < 0) return IP_Sign::NEGATIVE;
    if (return_value == 0) return IP_Sign::ZERO;
    return IP_Sign::UNDEFINED;
}

int insphere(const double pax, const double pay, const double paz, const double pbx, const double pby, const double pbz, const double pcx, const double pcy, const double pcz, const double pdx, const double pdy, const double pdz, const double pex, const double pey, const double pez)
{
    int ret;
    ret = insphere_filtered(pax, pay, paz, pbx, pby, pbz, pcx, pcy, pcz, pdx, pdy, pdz, pex, pey, pez);
    if (ret) return ret;
    ret = insphere_interval(pax, pay, paz, pbx, pby, pbz, pcx, pcy, pcz, pdx, pdy, pdz, pex, pey, pez);
    if (ret) return ret;
    return insphere_exact(pax, pay, paz, pbx, pby, pbz, pcx, pcy, pcz, pdx, pdy, pdz, pex, pey, pez);
}
