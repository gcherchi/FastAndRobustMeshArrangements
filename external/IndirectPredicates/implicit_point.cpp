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

#include "implicit_point.h"
#include "indirect_predicates.h"

int orient2d(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y);
int orient3d(double px, double py, double pz, double qx, double qy, double qz, double rx, double ry, double rz, double sx, double sy, double sz);

int lessThan_EE(double x1, double y1, double z1, double x2, double y2, double z2)
{
	int ret;
	if ((ret = ((x1 > x2) - (x1 < x2)))) return ret;
	if ((ret = ((y1 > y2) - (y1 < y2)))) return ret;
	return ((z1 > z2) - (z1 < z2));
}

int lessThan_LE(const implicitPoint3D_LPI& p1, double x2, double y2, double z2)
{
	int ret;
	if ((ret = lessThanOnX_LE(p1, x2))) return ret;
	if ((ret = lessThanOnY_LE(p1, y2))) return ret;
	return lessThanOnZ_LE(p1, z2);
}

int lessThan_TE(const implicitPoint3D_TPI& p1, double x2, double y2, double z2)
{
	int ret;
	if ((ret = lessThanOnX_TE(p1, x2))) return ret;
	if ((ret = lessThanOnY_TE(p1, y2))) return ret;
	return lessThanOnZ_TE(p1, z2);
}

int lessThan_LL(const implicitPoint3D_LPI& p1, const implicitPoint3D_LPI& p2)
{
	int ret;
	if ((ret = lessThanOnX_LL(p1, p2))) return ret;
	if ((ret = lessThanOnY_LL(p1, p2))) return ret;
	return lessThanOnZ_LL(p1, p2);
}

int lessThan_TT(const implicitPoint3D_TPI& p1, const implicitPoint3D_TPI& p2)
{
	int ret;
	if ((ret = lessThanOnX_TT(p1, p2))) return ret;
	if ((ret = lessThanOnY_TT(p1, p2))) return ret;
	return lessThanOnZ_TT(p1, p2);
}

int lessThan_LT(const implicitPoint3D_LPI& p1, const implicitPoint3D_TPI& p2)
{
	int ret;
	if ((ret = lessThanOnX_LT(p1, p2))) return ret;
	if ((ret = lessThanOnY_LT(p1, p2))) return ret;
	return lessThanOnZ_LT(p1, p2);
}

inline int lessThan_EE(const genericPoint& a, const genericPoint& b) { return lessThan_EE(a.toExplicit3D().X(), a.toExplicit3D().Y(), a.toExplicit3D().Z(), b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z()); }
inline int lessThan_LE(const genericPoint& a, const genericPoint& b) { return lessThan_LE(a.toLPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z()); }
inline int lessThan_TE(const genericPoint& a, const genericPoint& b) { return lessThan_TE(a.toTPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z()); }
inline int lessThan_LL(const genericPoint& a, const genericPoint& b) { return lessThan_LL(a.toLPI(), b.toLPI()); }
inline int lessThan_LT(const genericPoint& a, const genericPoint& b) { return lessThan_LT(a.toLPI(), b.toTPI()); }
inline int lessThan_TT(const genericPoint& a, const genericPoint& b) { return lessThan_TT(a.toTPI(), b.toTPI()); }

int genericPoint::lessThan(const genericPoint& a, const genericPoint& b)
{
		if (a.isExplicit3D() && b.isExplicit3D()) return lessThan_EE(a, b);
		if (a.isLPI() && b.isExplicit3D()) return lessThan_LE(a, b);
		if (a.isTPI() && b.isExplicit3D()) return lessThan_TE(a, b);
		if (a.isExplicit3D() && b.isLPI()) return -lessThan_LE(b, a);
		if (a.isLPI() && b.isLPI()) return lessThan_LL(a, b);
		if (a.isTPI() && b.isLPI())	return -lessThan_LT(b, a);
		if (a.isExplicit3D() && b.isTPI()) return -lessThan_TE(b, a);
		if (a.isLPI() && b.isTPI())	return lessThan_LT(a, b);
		if (a.isTPI() && b.isTPI())	return lessThan_TT(a, b);

		ip_error("genericPoint::lessThan - points must be 3D\n");
        return 0; // warning killer
}


int genericPoint::lessThanOnX(const genericPoint& a, const genericPoint& b)
{
		if (a.isExplicit3D() && b.isExplicit3D())
		{
			double av = a.toExplicit3D().X(), bv = b.toExplicit3D().X();
			return ((av > bv) - (av < bv));
		}
		if (a.isLPI() && b.isExplicit3D()) return lessThanOnX_LE(a.toLPI(), b.toExplicit3D().X());
		if (a.isTPI() && b.isExplicit3D()) return lessThanOnX_TE(a.toTPI(), b.toExplicit3D().X());
		if (a.isExplicit3D() && b.isLPI()) return -lessThanOnX_LE(b.toLPI(), a.toExplicit3D().X());
		if (a.isLPI() && b.isLPI()) return lessThanOnX_LL(a.toLPI(), b.toLPI());
		if (a.isTPI() && b.isLPI())	return -lessThanOnX_LT(b.toLPI(), a.toTPI());
		if (a.isExplicit3D() && b.isTPI()) return -lessThanOnX_TE(b.toTPI(), a.toExplicit3D().X());
		if (a.isLPI() && b.isTPI())	return lessThanOnX_LT(a.toLPI(), b.toTPI());
		if (a.isTPI() && b.isTPI())	return lessThanOnX_TT(a.toTPI(), b.toTPI());

        ip_error("genericPoint::lessThanOnX - points must be 3D\n");
        return 0; // warning killer
}

int genericPoint::lessThanOnY(const genericPoint& a, const genericPoint& b)
{
    if (a.isExplicit3D() && b.isExplicit3D())
    {
        double av = a.toExplicit3D().Y(), bv = b.toExplicit3D().Y();
        return ((av > bv) - (av < bv));
    }
    if (a.isLPI() && b.isExplicit3D()) return lessThanOnY_LE(a.toLPI(), b.toExplicit3D().Y());
    if (a.isTPI() && b.isExplicit3D()) return lessThanOnY_TE(a.toTPI(), b.toExplicit3D().Y());
    if (a.isExplicit3D() && b.isLPI()) return -lessThanOnY_LE(b.toLPI(), a.toExplicit3D().Y());
    if (a.isLPI() && b.isLPI()) return lessThanOnY_LL(a.toLPI(), b.toLPI());
    if (a.isTPI() && b.isLPI())	return -lessThanOnY_LT(b.toLPI(), a.toTPI());
    if (a.isExplicit3D() && b.isTPI()) return -lessThanOnY_TE(b.toTPI(), a.toExplicit3D().Y());
    if (a.isLPI() && b.isTPI())	return lessThanOnY_LT(a.toLPI(), b.toTPI());
    if (a.isTPI() && b.isTPI())	return lessThanOnY_TT(a.toTPI(), b.toTPI());

    ip_error("genericPoint::lessThanOnY - points must be 3D\n");
    return 0; // warning killer
}

int genericPoint::lessThanOnZ(const genericPoint& a, const genericPoint& b)
{
    if (a.isExplicit3D() && b.isExplicit3D())
    {
        double av = a.toExplicit3D().Z(), bv = b.toExplicit3D().Z();
        return ((av > bv) - (av < bv));
    }
    if (a.isLPI() && b.isExplicit3D()) return lessThanOnZ_LE(a.toLPI(), b.toExplicit3D().Z());
    if (a.isTPI() && b.isExplicit3D()) return lessThanOnZ_TE(a.toTPI(), b.toExplicit3D().Z());
    if (a.isExplicit3D() && b.isLPI()) return -lessThanOnZ_LE(b.toLPI(), a.toExplicit3D().Z());
    if (a.isLPI() && b.isLPI()) return lessThanOnZ_LL(a.toLPI(), b.toLPI());
    if (a.isTPI() && b.isLPI())	return -lessThanOnZ_LT(b.toLPI(), a.toTPI());
    if (a.isExplicit3D() && b.isTPI()) return -lessThanOnZ_TE(b.toTPI(), a.toExplicit3D().Z());
    if (a.isLPI() && b.isTPI())	return lessThanOnZ_LT(a.toLPI(), b.toTPI());
    if (a.isTPI() && b.isTPI())	return lessThanOnZ_TT(a.toTPI(), b.toTPI());

    ip_error("genericPoint::lessThanOnZ - points must be 3D\n");
    return 0; // warning killer
}

inline int orient2d_EEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d(a.toExplicit2D().X(), a.toExplicit2D().Y(), b.toExplicit2D().X(), b.toExplicit2D().Y(), c.toExplicit2D().X(), c.toExplicit2D().Y()); }
inline int orient2d_SEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d_indirect_SEE(a.toSSI(), b.toExplicit2D().X(), b.toExplicit2D().Y(), c.toExplicit2D().X(), c.toExplicit2D().Y()); }
inline int orient2d_SSE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d_indirect_SSE(a.toSSI(), b.toSSI(), c.toExplicit2D().X(), c.toExplicit2D().Y()); }
inline int orient2d_SSS(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d_indirect_SSS(a.toSSI(), b.toSSI(), c.toSSI()); }

int genericPoint::orient2D(const genericPoint& a, const genericPoint& b, const genericPoint& c)
{
    // Here we implicitly assume that points are 2D. Do not check.

    if (a.isExplicit2D() && b.isExplicit2D() && c.isExplicit2D()) return orient2d_EEE(a, b, c);
    if (a.isSSI() && b.isExplicit2D() && c.isExplicit2D()) return orient2d_SEE(a, b, c);
    if (a.isExplicit2D() && b.isSSI() && c.isExplicit2D()) return orient2d_SEE(b, c, a);
    if (a.isExplicit2D() && b.isExplicit2D() && c.isSSI()) return orient2d_SEE(c, a, b);
    if (a.isSSI() && b.isSSI() && c.isExplicit2D()) return orient2d_SSE(a, b, c);
    if (a.isExplicit2D() && b.isSSI() && c.isSSI())	return orient2d_SSE(b, c, a);
    if (a.isSSI() && b.isExplicit2D() && c.isSSI())	return orient2d_SSE(c, a, b);
    if (a.isSSI() && b.isSSI() && c.isSSI()) return orient2d_SSS(a, b, c);

    ip_error("genericPoint::orient2d - should not happen\n");
    return 0; // warning killer
}


inline int orient2dxy_EEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d(a.toExplicit3D().X(), a.toExplicit3D().Y(), b.toExplicit3D().X(), b.toExplicit3D().Y(), c.toExplicit3D().X(), c.toExplicit3D().Y()); }
inline int orient2dxy_LEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_LEE(a.toLPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), c.toExplicit3D().X(), c.toExplicit3D().Y()); }
inline int orient2dxy_LLE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_LLE(a.toLPI(), b.toLPI(), c.toExplicit3D().X(), c.toExplicit3D().Y()); }
inline int orient2dxy_LLL(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_LLL(a.toLPI(), b.toLPI(), c.toLPI()); }
inline int orient2dxy_TEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_TEE(a.toTPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), c.toExplicit3D().X(), c.toExplicit3D().Y()); }
inline int orient2dxy_TTE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_TTE(a.toTPI(), b.toTPI(), c.toExplicit3D().X(), c.toExplicit3D().Y()); }
inline int orient2dxy_TTT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_TTT(a.toTPI(), b.toTPI(), c.toTPI()); }
inline int orient2dxy_LTE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_LTE(a.toLPI(), b.toTPI(), c.toExplicit3D().X(), c.toExplicit3D().Y()); }
inline int orient2dxy_LLT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_LLT(a.toLPI(), b.toLPI(), c.toTPI()); }
inline int orient2dxy_LTT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dxy_indirect_LTT(a.toLPI(), b.toTPI(), c.toTPI()); }

int genericPoint::orient2Dxy(const genericPoint& a, const genericPoint& b, const genericPoint& c)
{
    if (a.isExplicit3D() && b.isExplicit3D() && c.isExplicit3D()) return orient2dxy_EEE(a, b, c);

    if (a.isLPI() && b.isExplicit3D() && c.isExplicit3D()) return orient2dxy_LEE(a, b, c);
    if (a.isExplicit3D() && b.isLPI() && c.isExplicit3D()) return orient2dxy_LEE(b, c, a);
    if (a.isExplicit3D() && b.isExplicit3D() && c.isLPI()) return orient2dxy_LEE(c, a, b);
    if (a.isTPI() && b.isExplicit3D() && c.isExplicit3D()) return orient2dxy_TEE(a, b, c);
    if (a.isExplicit3D() && b.isTPI() && c.isExplicit3D()) return orient2dxy_TEE(b, c, a);
    if (a.isExplicit3D() && b.isExplicit3D() && c.isTPI()) return orient2dxy_TEE(c, a, b);

    if (a.isLPI() && b.isLPI() && c.isExplicit3D()) return orient2dxy_LLE(a, b, c);
    if (a.isExplicit3D() && b.isLPI() && c.isLPI()) return orient2dxy_LLE(b, c, a);
    if (a.isLPI() && b.isExplicit3D() && c.isLPI()) return orient2dxy_LLE(c, a, b);
    if (a.isTPI() && b.isTPI() && c.isExplicit3D()) return orient2dxy_TTE(a, b, c);
    if (a.isExplicit3D() && b.isTPI() && c.isTPI()) return orient2dxy_TTE(b, c, a);
    if (a.isTPI() && b.isExplicit3D() && c.isTPI()) return orient2dxy_TTE(c, a, b);
    if (a.isLPI() && b.isTPI() && c.isExplicit3D()) return orient2dxy_LTE(a, b, c);
    if (a.isExplicit3D() && b.isLPI() && c.isTPI()) return orient2dxy_LTE(b, c, a);
    if (a.isExplicit3D() && b.isTPI() && c.isLPI()) return -orient2dxy_LTE(c, b, a);
    if (a.isLPI() && b.isExplicit3D() && c.isTPI()) return -orient2dxy_LTE(a, c, b);
    if (a.isTPI() && b.isExplicit3D() && c.isLPI()) return orient2dxy_LTE(c, a, b);
    if (a.isTPI() && b.isLPI() && c.isExplicit3D()) return -orient2dxy_LTE(b, a, c);

    if (a.isLPI() && b.isLPI() && c.isTPI()) return orient2dxy_LLT(a, b, c);
    if (a.isLPI() && b.isTPI() && c.isTPI()) return orient2dxy_LTT(a, b, c);
    if (a.isLPI() && b.isTPI() && c.isLPI()) return orient2dxy_LLT(c, a, b);
    if (a.isTPI() && b.isTPI() && c.isLPI()) return orient2dxy_LTT(c, a, b);
    if (a.isTPI() && b.isLPI() && c.isLPI()) return orient2dxy_LLT(b, c, a);
    if (a.isTPI() && b.isLPI() && c.isTPI()) return orient2dxy_LTT(b, c, a);
    if (a.isLPI() && b.isLPI() && c.isLPI()) return orient2dxy_LLL(a, b, c);
    if (a.isTPI() && b.isTPI() && c.isTPI()) return orient2dxy_TTT(a, b, c);

    ip_error("genericPoint::orient2dxy - should not happen\n");
    return 0; // warning killer
}

inline int orient2dyz_EEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d(a.toExplicit3D().Y(), a.toExplicit3D().Z(), b.toExplicit3D().Y(), b.toExplicit3D().Z(), c.toExplicit3D().Y(), c.toExplicit3D().Z()); }
inline int orient2dyz_LEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_LEE(a.toLPI(), b.toExplicit3D().Y(), b.toExplicit3D().Z(), c.toExplicit3D().Y(), c.toExplicit3D().Z()); }
inline int orient2dyz_LLE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_LLE(a.toLPI(), b.toLPI(), c.toExplicit3D().Y(), c.toExplicit3D().Z()); }
inline int orient2dyz_LLL(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_LLL(a.toLPI(), b.toLPI(), c.toLPI()); }
inline int orient2dyz_TEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_TEE(a.toTPI(), b.toExplicit3D().Y(), b.toExplicit3D().Z(), c.toExplicit3D().Y(), c.toExplicit3D().Z()); }
inline int orient2dyz_TTE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_TTE(a.toTPI(), b.toTPI(), c.toExplicit3D().Y(), c.toExplicit3D().Z()); }
inline int orient2dyz_TTT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_TTT(a.toTPI(), b.toTPI(), c.toTPI()); }
inline int orient2dyz_LTE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_LTE(a.toLPI(), b.toTPI(), c.toExplicit3D().Y(), c.toExplicit3D().Z()); }
inline int orient2dyz_LLT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_LLT(a.toLPI(), b.toLPI(), c.toTPI()); }
inline int orient2dyz_LTT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dyz_indirect_LTT(a.toLPI(), b.toTPI(), c.toTPI()); }

int genericPoint::orient2Dyz(const genericPoint& a, const genericPoint& b, const genericPoint& c)
{
	if (a.isExplicit3D() && b.isExplicit3D() && c.isExplicit3D()) return orient2dyz_EEE(a, b, c);

	if (a.isLPI() && b.isExplicit3D() && c.isExplicit3D()) return orient2dyz_LEE(a, b, c);
	if (a.isExplicit3D() && b.isLPI() && c.isExplicit3D()) return orient2dyz_LEE(b, c, a);
	if (a.isExplicit3D() && b.isExplicit3D() && c.isLPI()) return orient2dyz_LEE(c, a, b);
	if (a.isTPI() && b.isExplicit3D() && c.isExplicit3D()) return orient2dyz_TEE(a, b, c);
	if (a.isExplicit3D() && b.isTPI() && c.isExplicit3D()) return orient2dyz_TEE(b, c, a);
	if (a.isExplicit3D() && b.isExplicit3D() && c.isTPI()) return orient2dyz_TEE(c, a, b);

	if (a.isLPI() && b.isLPI() && c.isExplicit3D()) return orient2dyz_LLE(a, b, c);
	if (a.isExplicit3D() && b.isLPI() && c.isLPI()) return orient2dyz_LLE(b, c, a);
	if (a.isLPI() && b.isExplicit3D() && c.isLPI()) return orient2dyz_LLE(c, a, b);
	if (a.isTPI() && b.isTPI() && c.isExplicit3D()) return orient2dyz_TTE(a, b, c);
	if (a.isExplicit3D() && b.isTPI() && c.isTPI()) return orient2dyz_TTE(b, c, a);
	if (a.isTPI() && b.isExplicit3D() && c.isTPI()) return orient2dyz_TTE(c, a, b);
	if (a.isLPI() && b.isTPI() && c.isExplicit3D()) return orient2dyz_LTE(a, b, c);
	if (a.isExplicit3D() && b.isLPI() && c.isTPI()) return orient2dyz_LTE(b, c, a);
	if (a.isExplicit3D() && b.isTPI() && c.isLPI()) return -orient2dyz_LTE(c, b, a);
	if (a.isLPI() && b.isExplicit3D() && c.isTPI()) return -orient2dyz_LTE(a, c, b);
	if (a.isTPI() && b.isExplicit3D() && c.isLPI()) return orient2dyz_LTE(c, a, b);
	if (a.isTPI() && b.isLPI() && c.isExplicit3D()) return -orient2dyz_LTE(b, a, c);

	if (a.isLPI() && b.isLPI() && c.isTPI()) return orient2dyz_LLT(a, b, c);
	if (a.isLPI() && b.isTPI() && c.isTPI()) return orient2dyz_LTT(a, b, c);
	if (a.isLPI() && b.isTPI() && c.isLPI()) return orient2dyz_LLT(c, a, b);
	if (a.isTPI() && b.isTPI() && c.isLPI()) return orient2dyz_LTT(c, a, b);
	if (a.isTPI() && b.isLPI() && c.isLPI()) return orient2dyz_LLT(b, c, a);
	if (a.isTPI() && b.isLPI() && c.isTPI()) return orient2dyz_LTT(b, c, a);
	if (a.isLPI() && b.isLPI() && c.isLPI()) return orient2dyz_LLL(a, b, c);
	if (a.isTPI() && b.isTPI() && c.isTPI()) return orient2dyz_TTT(a, b, c);

	ip_error("genericPoint::orient2dyz - should not happen\n");
    return 0; // warning killer
}

inline int orient2dzx_EEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2d(a.toExplicit3D().Z(), a.toExplicit3D().X(), b.toExplicit3D().Z(), b.toExplicit3D().X(), c.toExplicit3D().Z(), c.toExplicit3D().X()); }
inline int orient2dzx_LEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_LEE(a.toLPI(), b.toExplicit3D().Z(), b.toExplicit3D().X(), c.toExplicit3D().Z(), c.toExplicit3D().X()); }
inline int orient2dzx_LLE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_LLE(a.toLPI(), b.toLPI(), c.toExplicit3D().Z(), c.toExplicit3D().X()); }
inline int orient2dzx_LLL(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_LLL(a.toLPI(), b.toLPI(), c.toLPI()); }
inline int orient2dzx_TEE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_TEE(a.toTPI(), b.toExplicit3D().Z(), b.toExplicit3D().X(), c.toExplicit3D().Z(), c.toExplicit3D().X()); }
inline int orient2dzx_TTE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_TTE(a.toTPI(), b.toTPI(), c.toExplicit3D().Z(), c.toExplicit3D().X()); }
inline int orient2dzx_TTT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_TTT(a.toTPI(), b.toTPI(), c.toTPI()); }
inline int orient2dzx_LTE(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_LTE(a.toLPI(), b.toTPI(), c.toExplicit3D().Z(), c.toExplicit3D().X()); }
inline int orient2dzx_LLT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_LLT(a.toLPI(), b.toLPI(), c.toTPI()); }
inline int orient2dzx_LTT(const genericPoint& a, const genericPoint& b, const genericPoint& c) { return orient2dzx_indirect_LTT(a.toLPI(), b.toTPI(), c.toTPI()); }

int genericPoint::orient2Dzx(const genericPoint& a, const genericPoint& b, const genericPoint& c)
{
	if (a.isExplicit3D() && b.isExplicit3D() && c.isExplicit3D()) return orient2dzx_EEE(a, b, c);

	if (a.isLPI() && b.isExplicit3D() && c.isExplicit3D()) return orient2dzx_LEE(a, b, c);
	if (a.isExplicit3D() && b.isLPI() && c.isExplicit3D()) return orient2dzx_LEE(b, c, a);
	if (a.isExplicit3D() && b.isExplicit3D() && c.isLPI()) return orient2dzx_LEE(c, a, b);
	if (a.isTPI() && b.isExplicit3D() && c.isExplicit3D()) return orient2dzx_TEE(a, b, c);
	if (a.isExplicit3D() && b.isTPI() && c.isExplicit3D()) return orient2dzx_TEE(b, c, a);
	if (a.isExplicit3D() && b.isExplicit3D() && c.isTPI()) return orient2dzx_TEE(c, a, b);

	if (a.isLPI() && b.isLPI() && c.isExplicit3D()) return orient2dzx_LLE(a, b, c);
	if (a.isExplicit3D() && b.isLPI() && c.isLPI()) return orient2dzx_LLE(b, c, a);
	if (a.isLPI() && b.isExplicit3D() && c.isLPI()) return orient2dzx_LLE(c, a, b);
	if (a.isTPI() && b.isTPI() && c.isExplicit3D()) return orient2dzx_TTE(a, b, c);
	if (a.isExplicit3D() && b.isTPI() && c.isTPI()) return orient2dzx_TTE(b, c, a);
	if (a.isTPI() && b.isExplicit3D() && c.isTPI()) return orient2dzx_TTE(c, a, b);
	if (a.isLPI() && b.isTPI() && c.isExplicit3D()) return orient2dzx_LTE(a, b, c);
	if (a.isExplicit3D() && b.isLPI() && c.isTPI()) return orient2dzx_LTE(b, c, a);
	if (a.isExplicit3D() && b.isTPI() && c.isLPI()) return -orient2dzx_LTE(c, b, a);
	if (a.isLPI() && b.isExplicit3D() && c.isTPI()) return -orient2dzx_LTE(a, c, b);
	if (a.isTPI() && b.isExplicit3D() && c.isLPI()) return orient2dzx_LTE(c, a, b);
	if (a.isTPI() && b.isLPI() && c.isExplicit3D()) return -orient2dzx_LTE(b, a, c);

	if (a.isLPI() && b.isLPI() && c.isTPI()) return orient2dzx_LLT(a, b, c);
	if (a.isLPI() && b.isTPI() && c.isTPI()) return orient2dzx_LTT(a, b, c);
	if (a.isLPI() && b.isTPI() && c.isLPI()) return orient2dzx_LLT(c, a, b);
	if (a.isTPI() && b.isTPI() && c.isLPI()) return orient2dzx_LTT(c, a, b);
	if (a.isTPI() && b.isLPI() && c.isLPI()) return orient2dzx_LLT(b, c, a);
	if (a.isTPI() && b.isLPI() && c.isTPI()) return orient2dzx_LTT(b, c, a);
	if (a.isLPI() && b.isLPI() && c.isLPI()) return orient2dzx_LLL(a, b, c);
	if (a.isTPI() && b.isTPI() && c.isTPI()) return orient2dzx_TTT(a, b, c);

	ip_error("genericPoint::orient2dzx - should not happen\n");
    return 0; // warning killer
}


inline int orient3d_EEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d) 
{ 
	return orient3d(a.toExplicit3D().X(), a.toExplicit3D().Y(), a.toExplicit3D().Z(), b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z(),
	c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LEEE(a.toLPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z(),
		c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LLEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LLEE(a.toLPI(), b.toLPI(),
		c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LLLE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LLLE(a.toLPI(), b.toLPI(), c.toLPI(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LLLL(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LLLL(a.toLPI(), b.toLPI(), c.toLPI(), d.toLPI());
}

inline int orient3d_TEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_TEEE(a.toTPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z(),
		c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_TTEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_TTEE(a.toTPI(), b.toTPI(),
		c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_TTTE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_TTTE(a.toTPI(), b.toTPI(), c.toTPI(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_TTTT(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_TTTT(a.toTPI(), b.toTPI(), c.toTPI(), d.toTPI());
}

inline int orient3d_LTEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LTEE(a.toLPI(), b.toTPI(),
		c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LLTE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LLTE(a.toLPI(), b.toLPI(), c.toTPI(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LTTE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LTTE(a.toLPI(), b.toTPI(), c.toTPI(), d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z());
}

inline int orient3d_LTTT(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LTTT(a.toLPI(), b.toTPI(), c.toTPI(), d.toTPI());
}

inline int orient3d_LLTT(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LLTT(a.toLPI(), b.toLPI(), c.toTPI(), d.toTPI());
}

inline int orient3d_LLLT(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return orient3d_indirect_LLLT(a.toLPI(), b.toLPI(), c.toLPI(), d.toTPI());
}


inline int orient3d_IIEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	if (a.isLPI() && b.isLPI()) return orient3d_LLEE(a, b, c, d);
	if (a.isLPI() && b.isTPI()) return orient3d_LTEE(a, b, c, d);
	if (a.isTPI() && b.isTPI()) return orient3d_TTEE(a, b, c, d);
	return orient3d_LTEE(b, a, d, c);
}

inline int orient3d_IIIE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	if (a.isLPI() && b.isLPI() && c.isLPI()) return orient3d_LLLE(a, b, c, d);
	if (a.isLPI() && b.isLPI() && c.isTPI()) return orient3d_LLTE(a, b, c, d);
	if (a.isLPI() && b.isTPI() && c.isTPI()) return orient3d_LTTE(a, b, c, d);
	if (a.isTPI() && b.isTPI() && c.isTPI()) return orient3d_TTTE(a, b, c, d);
	if (a.isLPI() && b.isTPI() && c.isLPI()) return orient3d_LLTE(c, a, b, d);
	if (a.isTPI() && b.isLPI() && c.isLPI()) return orient3d_LLTE(b, c, a, d);
	if (a.isTPI() && b.isLPI() && c.isTPI()) return orient3d_LTTE(b, c, a, d);
        return orient3d_LTTE(c, a, b, d);
}

int genericPoint::orient3D(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	// Here we implicitly assume that points are 3D. Do not check.

	int i = a.isExplicit3D() + b.isExplicit3D() + c.isExplicit3D() + d.isExplicit3D();

	if (i == 4) return orient3d_EEEE(a, b, c, d);
	
	if (i == 3)
	{
		if (a.isLPI()) return orient3d_LEEE(a, b, c, d);
		if (a.isTPI()) return orient3d_TEEE(a, b, c, d);
		if (b.isLPI()) return orient3d_LEEE(b, c, a, d);
		if (b.isTPI()) return orient3d_TEEE(b, c, a, d);
		if (c.isLPI()) return orient3d_LEEE(c, d, a, b);
		if (c.isTPI()) return orient3d_TEEE(c, d, a, b);
		if (d.isLPI()) return orient3d_LEEE(d, a, c, b);
		if (d.isTPI()) return orient3d_TEEE(d, a, c, b);

		ip_error("genericPoint::orient3d - should not happen (3)\n");
	}

	if (i == 2)
	{
		if (c.isExplicit3D() && d.isExplicit3D()) return orient3d_IIEE(a, b, c, d);
		if (b.isExplicit3D() && d.isExplicit3D()) return orient3d_IIEE(a, c, d, b);
		if (a.isExplicit3D() && d.isExplicit3D()) return orient3d_IIEE(b, c, a, d);
		if (b.isExplicit3D() && c.isExplicit3D()) return orient3d_IIEE(d, a, c, b);
		if (a.isExplicit3D() && c.isExplicit3D()) return orient3d_IIEE(d, b, a, c);
		if (a.isExplicit3D() && b.isExplicit3D()) return orient3d_IIEE(c, d, a, b);

		ip_error("genericPoint::orient3d - should not happen (2)\n");
	}

	if (i == 1)
	{
		if (d.isExplicit3D()) return orient3d_IIIE(a, b, c, d);
		if (c.isExplicit3D()) return orient3d_IIIE(d, b, a, c);
		if (b.isExplicit3D()) return orient3d_IIIE(a, c, d, b);
		if (a.isExplicit3D()) return orient3d_IIIE(b, d, c, a);

		ip_error("genericPoint::orient3d - should not happen (1)\n");
	}

	if (i == 0)
	{
		if (a.isLPI() && b.isLPI() && c.isLPI() && d.isLPI()) return orient3d_LLLL(a, b, c, d); // LLLL
		if (a.isLPI() && b.isLPI() && c.isLPI() && d.isTPI()) return orient3d_LLLT(a, b, c, d); // LLLT
		if (a.isLPI() && b.isLPI() && c.isTPI() && d.isTPI()) return orient3d_LLTT(a, b, c, d); // LLTT
		if (a.isLPI() && b.isTPI() && c.isTPI() && d.isTPI()) return orient3d_LTTT(a, b, c, d); // LTTT
		if (a.isTPI() && b.isTPI() && c.isTPI() && d.isTPI()) return orient3d_TTTT(a, b, c, d); // TTTT
        if (a.isLPI() && b.isLPI() && c.isTPI() && d.isLPI()) return orient3d_LLLT(b, a, d, c); // LLTL
		if (a.isLPI() && b.isTPI() && c.isLPI() && d.isLPI()) return orient3d_LLLT(c, d, a, b); // LTLL
        if (a.isTPI() && b.isLPI() && c.isLPI() && d.isLPI()) return orient3d_LLLT(d, c, b, a); // TLLL
		if (a.isLPI() && b.isTPI() && c.isLPI() && d.isTPI()) return orient3d_LLTT(c, a, b, d); // LTLT
        if (a.isTPI() && b.isLPI() && c.isLPI() && d.isTPI()) return orient3d_LLTT(b, c, a, d); // TLLT
        if (a.isTPI() && b.isLPI() && c.isTPI() && d.isLPI()) return orient3d_LLTT(b, d, c, a); // TLTL
		if (a.isTPI() && b.isTPI() && c.isLPI() && d.isLPI()) return orient3d_LLTT(c, d, a, b); // TTLL
        if (a.isLPI() && b.isTPI() && c.isTPI() && d.isLPI()) return orient3d_LLTT(a, d, b, c); // LTTL
        if (a.isTPI() && b.isLPI() && c.isTPI() && d.isTPI()) return orient3d_LTTT(b, a, d, c); // TLTT
		if (a.isTPI() && b.isTPI() && c.isLPI() && d.isTPI()) return orient3d_LTTT(c, d, a, b); // TTLT
        if (a.isTPI() && b.isTPI() && c.isTPI() && d.isLPI()) return orient3d_LTTT(d, c, b, a); // TTTL

		ip_error("genericPoint::orient3d - should not happen (0)\n");
	}

	ip_error("genericPoint::orient3d - should not happen/unsupported\n");
    return 0; // warning killer
}


inline int incircle2d_EEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incircle(a.toExplicit2D().X(), a.toExplicit2D().Y(), b.toExplicit2D().X(), b.toExplicit2D().Y(), c.toExplicit2D().X(), c.toExplicit2D().Y(), d.toExplicit2D().X(), d.toExplicit2D().Y());
}

inline int incircle2d_SEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incircle_indirect_SEEE(a.toSSI(), b.toExplicit2D().X(), b.toExplicit2D().Y(), c.toExplicit2D().X(), c.toExplicit2D().Y(), d.toExplicit2D().X(), d.toExplicit2D().Y());
}

inline int incircle2d_SSEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incircle_indirect_SSEE(a.toSSI(), b.toSSI(), c.toExplicit2D().X(), c.toExplicit2D().Y(), d.toExplicit2D().X(), d.toExplicit2D().Y());
}

inline int incircle2d_SSSE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incircle_indirect_SSSE(a.toSSI(), b.toSSI(), c.toSSI(), d.toExplicit2D().X(), d.toExplicit2D().Y());
}

inline int incircle2d_SSSS(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incircle_indirect_SSSS(a.toSSI(), b.toSSI(), c.toSSI(), d.toSSI());
}

int genericPoint::incircle(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
		if (a.isExplicit2D() && b.isExplicit2D() && c.isExplicit2D() && d.isExplicit2D()) return incircle2d_EEEE(a, b, c, d);

		if (a.isSSI() && b.isExplicit2D() && c.isExplicit2D() && d.isExplicit2D()) return incircle2d_SEEE(a, b, c, d);
		if (a.isExplicit2D() && b.isSSI() && c.isExplicit2D() && d.isExplicit2D()) return incircle2d_SEEE(b, a, d, c);
		if (a.isExplicit2D() && b.isExplicit2D() && c.isSSI() && d.isExplicit2D()) return incircle2d_SEEE(c, a, b, d);
		if (a.isExplicit2D() && b.isExplicit2D() && c.isExplicit2D() && d.isSSI()) return incircle2d_SEEE(d, a, c, b);

		if (a.isSSI() && b.isSSI() && c.isExplicit2D() && d.isExplicit2D()) return incircle2d_SSEE(a, b, c, d);
		if (a.isExplicit2D() && b.isExplicit2D() && c.isSSI() && d.isSSI()) return incircle2d_SSEE(c, d, a, b);
		if (a.isExplicit2D() && b.isSSI() && c.isExplicit2D() && d.isSSI()) return incircle2d_SSEE(b, d, c, a);
		if (a.isExplicit2D() && b.isSSI() && c.isSSI() && d.isExplicit2D()) return incircle2d_SSEE(b, c, a, d);
		if (a.isSSI() && b.isExplicit2D() && c.isExplicit2D() && d.isSSI()) return incircle2d_SSEE(a, d, b, c);
		if (a.isSSI() && b.isExplicit2D() && c.isSSI() && d.isExplicit2D()) return incircle2d_SSEE(a, c, d, b);

		if (a.isSSI() && b.isSSI() && c.isSSI() && d.isExplicit2D()) return incircle2d_SSSE(a, b, c, d);
		if (a.isExplicit2D() && b.isSSI() && c.isSSI() && d.isSSI()) return incircle2d_SSSE(c, b, d, a);
		if (a.isSSI() && b.isExplicit2D() && c.isSSI() && d.isSSI()) return incircle2d_SSSE(a, c, d, b);
		if (a.isSSI() && b.isSSI() && c.isExplicit2D() && d.isSSI()) return incircle2d_SSSE(b, a, d, c);

		if (a.isSSI() && b.isSSI() && c.isSSI() && d.isSSI()) return incircle2d_SSSS(a, b, c, d);

		ip_error("genericPoint::incircle - should not happen (2D)\n");
        return 0; // warning killer
}

inline int incircle2dxy_EEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incircle(a.toExplicit3D().X(), a.toExplicit3D().Y(), b.toExplicit3D().X(), b.toExplicit3D().Y(), c.toExplicit3D().X(), c.toExplicit3D().Y(), d.toExplicit3D().X(), d.toExplicit3D().Y());
}

inline int incircle2dxy_LEEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incirclexy_indirect_LEEE(a.toLPI(), b.toExplicit3D().X(), b.toExplicit3D().Y(), c.toExplicit3D().X(), c.toExplicit3D().Y(), d.toExplicit3D().X(), d.toExplicit3D().Y());
}

inline int incircle2dxy_LLEE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incirclexy_indirect_LLEE(a.toLPI(), b.toLPI(), c.toExplicit3D().X(), c.toExplicit3D().Y(), d.toExplicit3D().X(), d.toExplicit3D().Y());
}

inline int incircle2dxy_LLLE(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incirclexy_indirect_LLLE(a.toLPI(), b.toLPI(), c.toLPI(), d.toExplicit3D().X(), d.toExplicit3D().Y());
}

inline int incircle2dxy_LLLL(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
	return incirclexy_indirect_LLLL(a.toLPI(), b.toLPI(), c.toLPI(), d.toLPI());
}

int genericPoint::incirclexy(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d)
{
		if (a.isExplicit3D() && b.isExplicit3D() && c.isExplicit3D() && d.isExplicit3D()) return incircle2dxy_EEEE(a, b, c, d);

		if (a.isLPI() && b.isExplicit3D() && c.isExplicit3D() && d.isExplicit3D()) return incircle2dxy_LEEE(a, b, c, d);
		if (a.isExplicit3D() && b.isLPI() && c.isExplicit3D() && d.isExplicit3D()) return incircle2dxy_LEEE(b, a, d, c);
		if (a.isExplicit3D() && b.isExplicit3D() && c.isLPI() && d.isExplicit3D()) return incircle2dxy_LEEE(c, a, b, d);
		if (a.isExplicit3D() && b.isExplicit3D() && c.isExplicit3D() && d.isLPI()) return incircle2dxy_LEEE(d, a, c, b);

		if (a.isLPI() && b.isLPI() && c.isExplicit3D() && d.isExplicit3D()) return incircle2dxy_LLEE(a, b, c, d);
		if (a.isExplicit3D() && b.isExplicit3D() && c.isLPI() && d.isLPI()) return incircle2dxy_LLEE(c, d, a, b);
		if (a.isExplicit3D() && b.isLPI() && c.isExplicit3D() && d.isLPI()) return incircle2dxy_LLEE(b, d, c, a);
		if (a.isExplicit3D() && b.isLPI() && c.isLPI() && d.isExplicit3D()) return incircle2dxy_LLEE(b, c, a, d);
		if (a.isLPI() && b.isExplicit3D() && c.isExplicit3D() && d.isLPI()) return incircle2dxy_LLEE(a, d, b, c);
		if (a.isLPI() && b.isExplicit3D() && c.isLPI() && d.isExplicit3D()) return incircle2dxy_LLEE(a, c, d, b);

		if (a.isLPI() && b.isLPI() && c.isLPI() && d.isExplicit3D()) return incircle2dxy_LLLE(a, b, c, d);
		if (a.isExplicit3D() && b.isLPI() && c.isLPI() && d.isLPI()) return incircle2dxy_LLLE(c, b, d, a);
		if (a.isLPI() && b.isExplicit3D() && c.isLPI() && d.isLPI()) return incircle2dxy_LLLE(a, c, d, b);
		if (a.isLPI() && b.isLPI() && c.isExplicit3D() && d.isLPI()) return incircle2dxy_LLLE(b, a, d, c);

		if (a.isLPI() && b.isLPI() && c.isLPI() && d.isLPI()) return incircle2dxy_LLLL(a, b, c, d);

		ip_error("genericPoint::incirclexy - unsupported configuration (TPIs not implemented yet)\n");
        return 0; // warning killer
}


int genericPoint::inSphere(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d, const genericPoint& e)
{
	if (a.isExplicit3D() && b.isExplicit3D() && c.isExplicit3D() && d.isExplicit3D() && e.isExplicit3D())
		return ::inSphere(a.toExplicit3D().X(), a.toExplicit3D().Y(), a.toExplicit3D().Z(),
			b.toExplicit3D().X(), b.toExplicit3D().Y(), b.toExplicit3D().Z(),
			c.toExplicit3D().X(), c.toExplicit3D().Y(), c.toExplicit3D().Z(),
			d.toExplicit3D().X(), d.toExplicit3D().Y(), d.toExplicit3D().Z(),
			e.toExplicit3D().X(), e.toExplicit3D().Y(), e.toExplicit3D().Z());

	ip_error("genericPoint::inSphere - Only explicit 3D points supported\n");
    return 0; // warning killer
}


#ifdef USE_CACHED_VALUES

bool implicitPoint2D_SSI::getFilteredLambda(double& lx, double& ly, double &d, double& mv) const
{
	if (needsFilteredLambda())
	{
		ssfilter_max_val = 0;
		if (!lambda2d_SSI_filtered(l1_1.X(), l1_1.Y(), l1_2.X(), l1_2.Y(), l2_1.X(), l2_1.Y(), l2_2.X(), l2_2.Y(), ssfilter_lambda_x, ssfilter_lambda_y, ssfilter_denominator, ssfilter_max_val))
			ssfilter_denominator = 0;

		if (ssfilter_denominator < 0) {
			ssfilter_lambda_x = -ssfilter_lambda_x; 
			ssfilter_lambda_y = -ssfilter_lambda_y; 
			ssfilter_denominator = -ssfilter_denominator;
		}
	}

	lx = ssfilter_lambda_x;
	ly = ssfilter_lambda_y;
	d = ssfilter_denominator;
	if (ssfilter_denominator != 0 && ssfilter_max_val > mv) mv = ssfilter_max_val;
	return (ssfilter_denominator != 0);
}

bool implicitPoint2D_SSI::getIntervalLambda(interval_number& lx, interval_number& ly, interval_number &d) const
{
	if (needsIntervalLambda())
	{
		lambda2d_SSI_interval(l1_1.X(), l1_1.Y(), l1_2.X(), l1_2.Y(), l2_1.X(), l2_1.Y(), l2_2.X(), l2_2.Y(), dfilter_lambda_x, dfilter_lambda_y, dfilter_denominator);
		if (dfilter_denominator.isNegative()) {
			dfilter_lambda_x.invert();
			dfilter_lambda_y.invert();
			dfilter_denominator.invert();
		}
	}

	lx = dfilter_lambda_x;
	ly = dfilter_lambda_y;
	d = dfilter_denominator;
	return (dfilter_denominator.signIsReliable());
}

bool implicitPoint3D_LPI::getFilteredLambda(double& lx, double& ly, double& lz, double &d, double& mv) const
{
	if (needsFilteredLambda())
	{
		ssfilter_max_val = 0;
		if (!lambda3d_LPI_filtered(P().X(), P().Y(), P().Z(), Q().X(), Q().Y(), Q().Z(), R().X(), R().Y(), R().Z(), S().X(), S().Y(), S().Z(), T().X(), T().Y(), T().Z(), ssfilter_denominator, ssfilter_lambda_x, ssfilter_lambda_y, ssfilter_lambda_z, ssfilter_max_val))
			ssfilter_denominator = 0;

		if (ssfilter_denominator < 0) {
			ssfilter_lambda_x = -ssfilter_lambda_x;
			ssfilter_lambda_y = -ssfilter_lambda_y;
			ssfilter_lambda_z = -ssfilter_lambda_z;
			ssfilter_denominator = -ssfilter_denominator;
		}
	}

	lx = ssfilter_lambda_x;
	ly = ssfilter_lambda_y;
	lz = ssfilter_lambda_z;
	d = ssfilter_denominator;
	if (ssfilter_denominator != 0 && ssfilter_max_val > mv) mv = ssfilter_max_val;
	return (ssfilter_denominator != 0);
}

bool implicitPoint3D_LPI::getIntervalLambda(interval_number& lx, interval_number& ly, interval_number& lz, interval_number &d) const
{
	if (needsIntervalLambda())
	{
		lambda3d_LPI_interval(P().X(), P().Y(), P().Z(), Q().X(), Q().Y(), Q().Z(), R().X(), R().Y(), R().Z(), S().X(), S().Y(), S().Z(), T().X(), T().Y(), T().Z(), dfilter_denominator, dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z);
		if (dfilter_denominator.isNegative()) {
			dfilter_lambda_x.invert();
			dfilter_lambda_y.invert();
			dfilter_lambda_z.invert();
			dfilter_denominator.invert();
		}
	}
	lx = dfilter_lambda_x;
	ly = dfilter_lambda_y;
	lz = dfilter_lambda_z;
	d = dfilter_denominator;
	return (dfilter_denominator.signIsReliable());
}

bool implicitPoint3D_TPI::getFilteredLambda(double& lx, double& ly, double& lz, double &d, double& mv) const
{
	if (needsFilteredLambda())
	{
		ssfilter_max_val = 0;
		if (!lambda3d_TPI_filtered(
			V1().X(), V1().Y(), V1().Z(), V2().X(), V2().Y(), V2().Z(), V3().X(), V3().Y(), V3().Z(),
			W1().X(), W1().Y(), W1().Z(), W2().X(), W2().Y(), W2().Z(), W3().X(), W3().Y(), W3().Z(),
			U1().X(), U1().Y(), U1().Z(), U2().X(), U2().Y(), U2().Z(), U3().X(), U3().Y(), U3().Z(),
			ssfilter_lambda_x, ssfilter_lambda_y, ssfilter_lambda_z, ssfilter_denominator, ssfilter_max_val))
				ssfilter_denominator = 0;

		if (ssfilter_denominator < 0) {
			ssfilter_lambda_x = -ssfilter_lambda_x;
			ssfilter_lambda_y = -ssfilter_lambda_y;
			ssfilter_lambda_z = -ssfilter_lambda_z;
			ssfilter_denominator = -ssfilter_denominator;
		}
	}

	lx = ssfilter_lambda_x;
	ly = ssfilter_lambda_y;
	lz = ssfilter_lambda_z;
	d = ssfilter_denominator;
	if (ssfilter_denominator != 0 && ssfilter_max_val > mv) mv = ssfilter_max_val;
	return (ssfilter_denominator != 0);
}

bool implicitPoint3D_TPI::getIntervalLambda(interval_number& lx, interval_number& ly, interval_number& lz, interval_number& d) const
{
	if (needsIntervalLambda())
	{
	 lambda3d_TPI_interval(
		V1().X(), V1().Y(), V1().Z(), V2().X(), V2().Y(), V2().Z(), V3().X(), V3().Y(), V3().Z(),
		W1().X(), W1().Y(), W1().Z(), W2().X(), W2().Y(), W2().Z(), W3().X(), W3().Y(), W3().Z(),
		U1().X(), U1().Y(), U1().Z(), U2().X(), U2().Y(), U2().Z(), U3().X(), U3().Y(), U3().Z(),
		dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z, dfilter_denominator);
	 if (dfilter_denominator.isNegative()) {
		dfilter_lambda_x.invert();
		dfilter_lambda_y.invert();
		dfilter_lambda_z.invert();
		dfilter_denominator.invert();
	 }
	}

	lx = dfilter_lambda_x;
	ly = dfilter_lambda_y;
	lz = dfilter_lambda_z;
	d = dfilter_denominator;
	return (dfilter_denominator.signIsReliable());
}

#else
bool implicitPoint2D_SSI::getFilteredLambda(double& lx, double& ly, double &d, double& mv) const
{
	bool ret = lambda2d_SSI_filtered(l1_1.X(), l1_1.Y(), l1_2.X(), l1_2.Y(), l2_1.X(), l2_1.Y(), l2_2.X(), l2_2.Y(), lx, ly, d, mv);
	if (ret && d < 0) {
		lx = -lx;
		ly = -ly;
		d = -d;
	}
	return ret;
}

bool implicitPoint2D_SSI::getIntervalLambda(interval_number& lx, interval_number& ly, interval_number &d) const
{
	bool ret = lambda2d_SSI_interval(l1_1.X(), l1_1.Y(), l1_2.X(), l1_2.Y(), l2_1.X(), l2_1.Y(), l2_2.X(), l2_2.Y(), lx, ly, d);
	if (d.isNegative()) {
		lx.invert();
		ly.invert();
		d.invert();
	}
	return ret;
}

bool implicitPoint3D_LPI::getFilteredLambda(double& lx, double& ly, double& lz, double& d, double& mv) const
{
	bool ret = lambda3d_LPI_filtered(P().X(), P().Y(), P().Z(), Q().X(), Q().Y(), Q().Z(), R().X(), R().Y(), R().Z(), S().X(), S().Y(), S().Z(), T().X(), T().Y(), T().Z(), d, lx, ly, lz, mv);
	if (ret && d < 0) {
		lx = -lx;
		ly = -ly;
		lz = -lz;
		d = -d;
	}
	return ret;
}

bool implicitPoint3D_LPI::getIntervalLambda(interval_number& lx, interval_number& ly, interval_number& lz, interval_number& d) const
{
	bool ret = lambda3d_LPI_interval(P().X(), P().Y(), P().Z(), Q().X(), Q().Y(), Q().Z(), R().X(), R().Y(), R().Z(), S().X(), S().Y(), S().Z(), T().X(), T().Y(), T().Z(), d, lx, ly, lz);
	if (d.isNegative()) {
		lx.invert();
		ly.invert();
		lz.invert();
		d.invert();
	}
	return ret;
}

bool implicitPoint3D_TPI::getFilteredLambda(double& lx, double& ly, double& lz, double& d, double& mv) const
{
	bool ret = lambda3d_TPI_filtered(
		V1().X(), V1().Y(), V1().Z(), V2().X(), V2().Y(), V2().Z(), V3().X(), V3().Y(), V3().Z(),
		W1().X(), W1().Y(), W1().Z(), W2().X(), W2().Y(), W2().Z(), W3().X(), W3().Y(), W3().Z(),
		U1().X(), U1().Y(), U1().Z(), U2().X(), U2().Y(), U2().Z(), U3().X(), U3().Y(), U3().Z(),
		lx, ly, lz, d, mv);
	if (ret && d < 0) {
		lx = -lx;
		ly = -ly;
		lz = -lz;
		d = -d;
	}
	return ret;
}

bool implicitPoint3D_TPI::getIntervalLambda(interval_number& lx, interval_number& ly, interval_number& lz, interval_number& d) const
{
	bool ret = lambda3d_TPI_interval(
		V1().X(), V1().Y(), V1().Z(), V2().X(), V2().Y(), V2().Z(), V3().X(), V3().Y(), V3().Z(),
		W1().X(), W1().Y(), W1().Z(), W2().X(), W2().Y(), W2().Z(), W3().X(), W3().Y(), W3().Z(),
		U1().X(), U1().Y(), U1().Z(), U2().X(), U2().Y(), U2().Z(), U3().X(), U3().Y(), U3().Z(),
		lx, ly, lz, d);
	if (d.isNegative()) {
		lx.invert();
		ly.invert();
		lz.invert();
		d.invert();
	}
	return ret;
}

#endif

void implicitPoint2D_SSI::getExactLambda(double *lx, int& lxl, double *ly, int& lyl, double *d, int& dl) const
{
	lambda2d_SSI_exact(l1_1.X(), l1_1.Y(), l1_2.X(), l1_2.Y(), l2_1.X(), l2_1.Y(), l2_2.X(), l2_2.Y(), lx, lxl, ly, lyl, d, dl);
	if (d[dl - 1] < 0)
	{
		expansionObject::Gen_Invert(lxl, lx);
		expansionObject::Gen_Invert(lyl, ly);
		expansionObject::Gen_Invert(dl, d);
	}
}

// Keeps lambda/d pairs as close to one as possible to avoid under/overflows
void normalizeLambda3D(double* lx, int& lxl, double* ly, int& lyl, double* lz, int& lzl, double* d, int& dl)
{
	double maxd = -DBL_MAX, maxsd, ad, aad;
	if ((aad = fabs((ad = expansionObject::To_Double(lxl, lx)))) > maxd) { maxd = aad; maxsd = ad; }
	if ((aad = fabs((ad = expansionObject::To_Double(lyl, ly)))) > maxd) { maxd = aad; maxsd = ad; }
	if ((aad = fabs((ad = expansionObject::To_Double(lzl, lz)))) > maxd) { maxd = aad; maxsd = ad; }
	if ((aad = fabs((ad = expansionObject::To_Double(dl, d)))) > maxd) { maxd = aad; maxsd = ad; }

	int e;
	frexp(maxsd, &e);
	double m = ldexp(2, -e);

	expansionObject::ExactScale(lxl, lx, m);
	expansionObject::ExactScale(lyl, ly, m);
	expansionObject::ExactScale(lzl, lz, m);
	expansionObject::ExactScale(dl, d, m);
}

void implicitPoint3D_LPI::getExactLambda(double *lx, int& lxl, double *ly, int& lyl, double *lz, int& lzl, double *d, int& dl) const
{
	lambda3d_LPI_exact(P().X(), P().Y(), P().Z(), Q().X(), Q().Y(), Q().Z(), R().X(), R().Y(), R().Z(), S().X(), S().Y(), S().Z(), T().X(), T().Y(), T().Z(), d, dl, lx, lxl, ly, lyl, lz, lzl);
	if (d[dl - 1] < 0)
	{
		expansionObject::Gen_Invert(lxl, lx);
		expansionObject::Gen_Invert(lyl, ly);
		expansionObject::Gen_Invert(lzl, lz);
		expansionObject::Gen_Invert(dl, d);
	}
	normalizeLambda3D(lx, lxl, ly, lyl, lz, lzl, d, dl);
}

void implicitPoint3D_TPI::getExactLambda(double *lx, int& lxl, double *ly, int& lyl, double *lz, int& lzl, double *d, int& dl) const
{
	lambda3d_TPI_exact(V1().X(), V1().Y(), V1().Z(), V2().X(), V2().Y(), V2().Z(), V3().X(), V3().Y(), V3().Z(),
		W1().X(), W1().Y(), W1().Z(), W2().X(), W2().Y(), W2().Z(), W3().X(), W3().Y(), W3().Z(),
		U1().X(), U1().Y(), U1().Z(), U2().X(), U2().Y(), U2().Z(), U3().X(), U3().Y(), U3().Z(), lx, lxl, ly, lyl, lz, lzl, d, dl);
	if (d[dl - 1] < 0)
	{
		expansionObject::Gen_Invert(lxl, lx);
		expansionObject::Gen_Invert(lyl, ly);
		expansionObject::Gen_Invert(lzl, lz);
		expansionObject::Gen_Invert(dl, d);
	}
	normalizeLambda3D(lx, lxl, ly, lyl, lz, lzl, d, dl);
}


bool implicitPoint2D_SSI::apapExplicit(explicitPoint2D& e) const
{
	double l1x_p[128], * l1x = l1x_p, l1y_p[128], * l1y = l1y_p, d1_p[128], * d1 = d1_p;
	int l1x_len, l1y_len, d1_len;
	getExactLambda(l1x, l1x_len, l1y, l1y_len, d1, d1_len);
	const double lambda_x = expansionObject::To_Double(l1x_len, l1x);
	const double lambda_y = expansionObject::To_Double(l1y_len, l1y);
	const double lambda_d = expansionObject::To_Double(d1_len, d1);
	if (l1x_p != l1x) free(l1x);
	if (l1y_p != l1y) free(l1y);
	if (d1_p != d1) free(d1);
	if (lambda_d == 0) return false;

	e = explicitPoint2D(lambda_x / lambda_d, lambda_y / lambda_d);
	return true;
}

bool implicitPoint2D_SSI::approxExplicit(explicitPoint2D& e) const
{
	double lambda_x, lambda_y, lambda_d, max_var = 0;
	if (!getFilteredLambda(lambda_x, lambda_y, lambda_d, max_var))
	{
		interval_number ilx, ily, id;
		if (!getIntervalLambda(ilx, ily, id)) return apapExplicit(e);
		else
		{
			lambda_x = ilx.sup() + ilx.inf();
			lambda_y = ily.sup() + ily.inf();
			lambda_d = id.sup() + id.inf();
		}
	}
	e = explicitPoint2D(lambda_x / lambda_d, lambda_y / lambda_d);
	return true;
}

bool implicitPoint3D_LPI::apapExplicit(explicitPoint3D& e) const
{
	double l1z_p[128], * l1z = l1z_p, l1x_p[128], * l1x = l1x_p, l1y_p[128], * l1y = l1y_p, d1_p[128], * d1 = d1_p;
	int l1z_len, l1x_len, l1y_len, d1_len;
	getExactLambda(l1x, l1x_len, l1y, l1y_len, l1z, l1z_len, d1, d1_len);
	const double lambda_x = expansionObject::To_Double(l1x_len, l1x);
	const double lambda_y = expansionObject::To_Double(l1y_len, l1y);
	const double lambda_z = expansionObject::To_Double(l1z_len, l1z);
	const double lambda_d = expansionObject::To_Double(d1_len, d1);
	if (l1z_p != l1z) free(l1z);
	if (l1x_p != l1x) free(l1x);
	if (l1y_p != l1y) free(l1y);
	if (d1_p != d1) free(d1);
	if (lambda_d == 0) return false;
	e = explicitPoint3D(lambda_x / lambda_d, lambda_y / lambda_d, lambda_z / lambda_d);
	return true;
}


bool implicitPoint3D_LPI::approxExplicit(explicitPoint3D& e) const
{
	double lambda_x, lambda_y, lambda_z, lambda_d, max_var = 0;
	if (!getFilteredLambda(lambda_x, lambda_y, lambda_z, lambda_d, max_var))
	{
		interval_number ilx, ily, ilz, id;
		if (!getIntervalLambda(ilx, ily, ilz, id)) return apapExplicit(e);
		else
		{
			lambda_x = ilx.sup() + ilx.inf();
			lambda_y = ily.sup() + ily.inf();
			lambda_z = ilz.sup() + ilz.inf();
			lambda_d = id.sup() + id.inf();
		}
	}
	e = explicitPoint3D(lambda_x / lambda_d, lambda_y / lambda_d, lambda_z / lambda_d);
	return true;
}

bool implicitPoint3D_TPI::apapExplicit(explicitPoint3D& e) const
{
	double l1z_p[128], * l1z = l1z_p, l1x_p[128], * l1x = l1x_p, l1y_p[128], * l1y = l1y_p, d1_p[128], * d1 = d1_p;
	int l1z_len, l1x_len, l1y_len, d1_len;
	getExactLambda(l1x, l1x_len, l1y, l1y_len, l1z, l1z_len, d1, d1_len);
	const double lambda_x = expansionObject::To_Double(l1x_len, l1x);
	const double lambda_y = expansionObject::To_Double(l1y_len, l1y);
	const double lambda_z = expansionObject::To_Double(l1z_len, l1z);
	const double lambda_d = expansionObject::To_Double(d1_len, d1);
	if (l1z_p != l1z) free(l1z);
	if (l1x_p != l1x) free(l1x);
	if (l1y_p != l1y) free(l1y);
	if (d1_p != d1) free(d1);
	if (lambda_d == 0) return false;
	e = explicitPoint3D(lambda_x / lambda_d, lambda_y / lambda_d, lambda_z / lambda_d);
	return true;
}

bool implicitPoint3D_TPI::approxExplicit(explicitPoint3D& e) const
{
	double lambda_x, lambda_y, lambda_z, lambda_d, max_var = 0;
	if (!getFilteredLambda(lambda_x, lambda_y, lambda_z, lambda_d, max_var))
	{
		interval_number ilx, ily, ilz, id;
		if (!getIntervalLambda(ilx, ily, ilz, id)) return apapExplicit(e);
		else
		{
			lambda_x = ilx.sup() + ilx.inf();
			lambda_y = ily.sup() + ily.inf();
			lambda_z = ilz.sup() + ilz.inf();
			lambda_d = id.sup() + id.inf();
		}
	}
	e = explicitPoint3D(lambda_x / lambda_d, lambda_y / lambda_d, lambda_z / lambda_d);
	return true;
}

bool genericPoint::getApproxXYCoordinates(double& x, double& y, bool apap) const
{
	if (is2D())
	{
		explicitPoint2D op;
		const explicitPoint2D* p = &op;
		if (isExplicit2D()) p = &toExplicit2D();
		else if (isSSI()) 
		{
			if (apap)
			{
				if (!toSSI().apapExplicit(op)) return false;
			}
			else
			{
				if (!toSSI().approxExplicit(op)) return false;
			}
		}
		x = p->X(); y = p->Y();
		return true;
	}
	if (is3D())
	{
		explicitPoint3D op;
		const explicitPoint3D* p = &op;
		if (isExplicit3D()) p = &toExplicit3D();
		else if (isLPI()) 
		{
			if (apap)
			{
				if (!toLPI().apapExplicit(op)) return false;
			}
			else
			{
				if (!toLPI().approxExplicit(op)) return false;
			}
		}
		else if (isTPI()) 
		{
			if (apap)
			{
				if (!toTPI().apapExplicit(op)) return false;
			}
			else
			{
				if (!toTPI().approxExplicit(op)) return false;
			}
		}
		x = p->X(); y = p->Y();
		return true;
	}
	ip_error("genericPoint::getApproxXYCoordinates - should not happen\n");
	return false;
}

bool genericPoint::getApproxXYZCoordinates(double& x, double& y, double& z, bool apap) const
{
	if (is3D())
	{
		explicitPoint3D op;
		const explicitPoint3D* p = &op;
		if (isExplicit3D()) p = &toExplicit3D();
		else if (isLPI())
		{
			if (apap)
			{
				if (!toLPI().apapExplicit(op)) return false;
			}
			else
			{
				if (!toLPI().approxExplicit(op)) return false;
			}
		}
		else if (isTPI())
		{
			if (apap)
			{
				if (!toTPI().apapExplicit(op)) return false;
			}
			else
			{
				if (!toTPI().approxExplicit(op)) return false;
			}
		}
		x = p->X(); y = p->Y(); z = p->Z();
		return true;
	}
	ip_error("genericPoint::getApproxXYZCoordinates - should not happen\n");
	return false;
}

ostream& operator<<(ostream& os, const genericPoint& p)
{
	if (p.isExplicit2D()) return os << p.toExplicit2D();
	else if (p.isExplicit3D()) return os << p.toExplicit3D();
	else if (p.isSSI()) return os << p.toSSI();
	else if (p.isLPI()) return os << p.toLPI();
	else if (p.isTPI()) return os << p.toTPI();
	else ip_error("genericPoint::operator<< - should not happen\n");
	return os;
}

int maxComponentInTriangleNormal_filtered(double ov1x, double ov1y, double ov1z, double ov2x, double ov2y, double ov2z, double ov3x, double ov3y, double ov3z)
{
	double v3x = ov3x - ov2x;
	double v3y = ov3y - ov2y;
	double v3z = ov3z - ov2z;
	double v2x = ov2x - ov1x;
	double v2y = ov2y - ov1y;
	double v2z = ov2z - ov1z;
	double nvx1 = v2y * v3z;
	double nvx2 = v2z * v3y;
	double nvx = nvx1 - nvx2;
	double nvy1 = v3x * v2z;
	double nvy2 = v3z * v2x;
	double nvy = nvy1 - nvy2;
	double nvz1 = v2x * v3y;
	double nvz2 = v2y * v3x;
	double nvz = nvz1 - nvz2;

	double _tmp_fabs, max_var = 0;
	if ((_tmp_fabs = fabs(v3x)) > max_var) max_var = _tmp_fabs;
	if ((_tmp_fabs = fabs(v3y)) > max_var) max_var = _tmp_fabs;
	if ((_tmp_fabs = fabs(v3z)) > max_var) max_var = _tmp_fabs;
	if ((_tmp_fabs = fabs(v2x)) > max_var) max_var = _tmp_fabs;
	if ((_tmp_fabs = fabs(v2y)) > max_var) max_var = _tmp_fabs;
	if ((_tmp_fabs = fabs(v2z)) > max_var) max_var = _tmp_fabs;
	double epsilon = 8.88395e-016 * max_var * max_var;

	double nvxc = fabs(nvx);
	double nvyc = fabs(nvy);
	double nvzc = fabs(nvz);
	double nv = nvxc;
	if (nvyc > nv) nv = nvyc;
	if (nvzc > nv) nv = nvzc;

	if (nv > epsilon)
	{
		if (nv == nvx) return 0;
		if (nv == nvy) return 1;
		if (nv == nvz) return 2;
	}
	return -1;
}

int maxComponentInTriangleNormal_exact(double ov1x, double ov1y, double ov1z, double ov2x, double ov2y, double ov2z, double ov3x, double ov3y, double ov3z)
{
	expansionObject o;
	double v3x[2];
	o.two_Diff(ov3x, ov2x, v3x);
	double v3y[2];
	o.two_Diff(ov3y, ov2y, v3y);
	double v3z[2];
	o.two_Diff(ov3z, ov2z, v3z);
	double v2x[2];
	o.two_Diff(ov2x, ov1x, v2x);
	double v2y[2];
	o.two_Diff(ov2y, ov1y, v2y);
	double v2z[2];
	o.two_Diff(ov2z, ov1z, v2z);
	double nvx1[8];
	o.Two_Two_Prod(v2y, v3z, nvx1);
	double nvx2[8];
	o.Two_Two_Prod(v2z, v3y, nvx2);
	double nvx[16];
	int nvx_len = o.Gen_Diff(8, nvx1, 8, nvx2, nvx);
	double nvy1[8];
	o.Two_Two_Prod(v3x, v2z, nvy1);
	double nvy2[8];
	o.Two_Two_Prod(v3z, v2x, nvy2);
	double nvy[16];
	int nvy_len = o.Gen_Diff(8, nvy1, 8, nvy2, nvy);
	double nvz1[8];
	o.Two_Two_Prod(v2x, v3y, nvz1);
	double nvz2[8];
	o.Two_Two_Prod(v2y, v3x, nvz2);
	double nvz[16];
	int nvz_len = o.Gen_Diff(8, nvz1, 8, nvz2, nvz);

	double nvxc = fabs(nvx[nvx_len - 1]);
	double nvyc = fabs(nvy[nvy_len - 1]);
	double nvzc = fabs(nvz[nvz_len - 1]);
	double nv = nvxc;
	if (nvyc > nv) nv = nvyc;
	if (nvzc > nv) return 2;
	if (nv == nvxc) return 0;
	return 1;
}

int genericPoint::maxComponentInTriangleNormal(double ov1x, double ov1y, double ov1z, double ov2x, double ov2y, double ov2z, double ov3x, double ov3y, double ov3z)
{
	int ret;
	if ((ret = maxComponentInTriangleNormal_filtered(ov1x, ov1y, ov1z, ov2x, ov2y, ov2z, ov3x, ov3y, ov3z)) >= 0) return ret;
	return maxComponentInTriangleNormal_exact(ov1x, ov1y, ov1z, ov2x, ov2y, ov2z, ov3x, ov3y, ov3z);
}


/////////////////////////////////////////////
//
// Derived predicates
//
/////////////////////////////////////////////

bool genericPoint::innerSegmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q)
{
	int o11, o12, o21, o22;

	o11 = orient2Dxy(P, A, B);
	o12 = orient2Dxy(Q, B, A);
	o21 = orient2Dxy(A, P, Q);
	o22 = orient2Dxy(B, Q, P);
	if (o11 || o21 || o12 || o22) return (o11 == o12 && o21 == o22);

	o11 = orient2Dyz(P, A, B);
	o12 = orient2Dyz(Q, B, A);
	o21 = orient2Dyz(A, P, Q);
	o22 = orient2Dyz(B, Q, P);
	if (o11 || o21 || o12 || o22) return (o11 == o12 && o21 == o22);

	o11 = orient2Dzx(P, A, B);
	o12 = orient2Dzx(Q, B, A);
	o21 = orient2Dzx(A, P, Q);
	o22 = orient2Dzx(B, Q, P);
	if (o11 || o21 || o12 || o22) return (o11 == o12 && o21 == o22);

	return false;
}

bool genericPoint::segmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q)
{
	int o11, o12, o21, o22;

	o11 = orient2Dxy(P, A, B);
	o12 = orient2Dxy(Q, B, A);
	o21 = orient2Dxy(A, P, Q);
	o22 = orient2Dxy(B, Q, P);
	if ((o11 || o12) && (o11 * o12 >= 0) && (o21 || o22) && (o21 * o22 >= 0)) return true;

	o11 = orient2Dyz(P, A, B);
	o12 = orient2Dyz(Q, B, A);
	o21 = orient2Dyz(A, P, Q);
	o22 = orient2Dyz(B, Q, P);
	if ((o11 || o12) && (o11 * o12 >= 0) && (o21 || o22) && (o21 * o22 >= 0)) return true;

	o11 = orient2Dzx(P, A, B);
	o12 = orient2Dzx(Q, B, A);
	o21 = orient2Dzx(A, P, Q);
	o22 = orient2Dzx(B, Q, P);
	if ((o11 || o12) && (o11 * o12 >= 0) && (o21 || o22) && (o21 * o22 >= 0)) return true;

	return false;
}

bool genericPoint::innerSegmentCrossesInnerTriangle(const genericPoint& s1, const genericPoint& s2, const genericPoint& v1, const genericPoint& v2, const genericPoint& v3)
{
	int o1 = orient3D(s1, v1, v2, v3); if (o1 == 0) return false;
	int o2 = orient3D(s2, v1, v2, v3); if (o2 == 0) return false;

	if ((o1 > 0 && o2 > 0) || (o1 < 0 && o2 < 0)) return false;
	o1 = orient3D(s1, s2, v1, v2);
	o2 = orient3D(s1, s2, v2, v3);
	if ((o1 >= 0 && o2 <= 0) || (o1 <= 0 && o2 >= 0)) return false;
	int o3 = orient3D(s1, s2, v3, v1);
	if ((o1 >= 0 && o3 <= 0) || (o1 <= 0 && o3 >= 0)) return false;
	if ((o2 >= 0 && o3 <= 0) || (o2 <= 0 && o3 >= 0)) return false;
	return true;
}

bool genericPoint::pointInInnerSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2)
{
	if (misaligned(p, v1, v2)) return false;

	int lt2, lt3;
	lt2 = lessThanOnX(v1, p);
	lt3 = lessThanOnX(p, v2);
	if (lt2) return (lt2 == lt3);
	lt2 = lessThanOnY(v1, p);
	lt3 = lessThanOnY(p, v2);
	if (lt2) return (lt2 == lt3);
	lt2 = lessThanOnZ(v1, p);
	lt3 = lessThanOnZ(p, v2);
	if (lt2) return (lt2 == lt3);
	return false;
}

bool genericPoint::pointInSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2)
{
	if (misaligned(p, v1, v2)) return false;

	int lt2x = lessThanOnX(v1, p);
	int lt3x = lessThanOnX(p, v2);
	if (lt2x && lt3x) return (lt2x == lt3x);
	int lt2y = lessThanOnY(v1, p);
	int lt3y = lessThanOnY(p, v2);
	if (lt2y && lt3y) return (lt2y == lt3y);
	int lt2z = lessThanOnZ(v1, p);
	int lt3z = lessThanOnZ(p, v2);
	if (lt2z && lt3z) return (lt2z == lt3z);

	return ((lt2x == 0 && lt2y == 0 && lt2z == 0) || (lt3x == 0 && lt3y == 0 && lt3z == 0));
}

bool genericPoint::pointInTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C)
{
	int o1, o2, o3;
	o1 = orient2Dxy(P, A, B);
	o2 = orient2Dxy(P, B, C);
	o3 = orient2Dxy(P, C, A);
	if (o1 || o2 || o3) return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
	o1 = orient2Dyz(P, A, B);
	o2 = orient2Dyz(P, B, C);
	o3 = orient2Dyz(P, C, A);
	if (o1 || o2 || o3) return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
	o1 = orient2Dzx(P, A, B);
	o2 = orient2Dzx(P, B, C);
	o3 = orient2Dzx(P, C, A);
	return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
}


bool genericPoint::pointInTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C, int& o1, int& o2, int& o3)
{
	o1 = orient2Dxy(P, A, B);
	o2 = orient2Dxy(P, B, C);
	o3 = orient2Dxy(P, C, A);
	if (o1 || o2 || o3) return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
	o1 = orient2Dyz(P, A, B);
	o2 = orient2Dyz(P, B, C);
	o3 = orient2Dyz(P, C, A);
	if (o1 || o2 || o3) return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
	o1 = orient2Dzx(P, A, B);
	o2 = orient2Dzx(P, B, C);
	o3 = orient2Dzx(P, C, A);
	return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
}

bool genericPoint::pointInInnerTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C)
{
	int o1, o2, o3;
	o1 = orient2Dxy(P, A, B);
	o2 = orient2Dxy(P, B, C);
	o3 = orient2Dxy(P, C, A);
	if (o1 || o2 || o3) return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
	o1 = orient2Dyz(P, A, B);
	o2 = orient2Dyz(P, B, C);
	o3 = orient2Dyz(P, C, A);
	if (o1 || o2 || o3) return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
	o1 = orient2Dzx(P, A, B);
	o2 = orient2Dzx(P, B, C);
	o3 = orient2Dzx(P, C, A);
	return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
}




bool genericPoint::pointInInnerSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2, int xyz)
{
	int lt2, lt3;
	if (xyz == 0)
	{
		if (orient2Dyz(p, v1, v2)) return false;
		lt2 = lessThanOnY(v1, p);
		lt3 = lessThanOnY(p, v2);
		if (lt2) return (lt2 == lt3);
		lt2 = lessThanOnZ(v1, p);
		lt3 = lessThanOnZ(p, v2);
	}
	else if (xyz == 1)
	{
		if (orient2Dzx(p, v1, v2)) return false;
		lt2 = lessThanOnX(v1, p);
		lt3 = lessThanOnX(p, v2);
		if (lt2) return (lt2 == lt3);
		lt2 = lessThanOnZ(v1, p);
		lt3 = lessThanOnZ(p, v2);
	}
	else
	{
		if (orient2Dxy(p, v1, v2)) return false;
		lt2 = lessThanOnX(v1, p);
		lt3 = lessThanOnX(p, v2);
		if (lt2) return (lt2 == lt3);
		lt2 = lessThanOnY(v1, p);
		lt3 = lessThanOnY(p, v2);
	}
	return (lt2 && (lt2 == lt3));
}

bool genericPoint::pointInSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2, int xyz)
{
	int lt2, lt3, lt4, lt5;
	if (xyz == 0)
	{
		if (orient2Dyz(p, v1, v2)) return false;
		lt2 = lessThanOnY(v1, p);
		lt3 = lessThanOnY(p, v2);
		if (lt2 && lt3) return (lt2 == lt3);
		lt4 = lessThanOnZ(v1, p);
		lt5 = lessThanOnZ(p, v2);
	}
	else if (xyz == 1)
	{
		if (orient2Dzx(p, v1, v2)) return false;
		lt2 = lessThanOnX(v1, p);
		lt3 = lessThanOnX(p, v2);
		if (lt2 && lt3) return (lt2 == lt3);
		lt4 = lessThanOnZ(v1, p);
		lt5 = lessThanOnZ(p, v2);
	}
	else
	{
		if (orient2Dxy(p, v1, v2)) return false;
		lt2 = lessThanOnX(v1, p);
		lt3 = lessThanOnX(p, v2);
		if (lt2 && lt3) return (lt2 == lt3);
		lt4 = lessThanOnY(v1, p);
		lt5 = lessThanOnY(p, v2);
	}
	return ((lt2 == 0 && lt4 == 0) || (lt3 == 0 && lt5 == 0));
}

bool genericPoint::pointInInnerTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C, int xyz)
{
	int o1, o2, o3;
	if (xyz == 2)
	{
		o1 = genericPoint::orient2Dxy(P, A, B);
		o2 = genericPoint::orient2Dxy(P, B, C);
		o3 = genericPoint::orient2Dxy(P, C, A);
	}
	else if (xyz == 0)
	{
		o1 = genericPoint::orient2Dyz(P, A, B);
		o2 = genericPoint::orient2Dyz(P, B, C);
		o3 = genericPoint::orient2Dyz(P, C, A);
	}
	else
	{
		o1 = genericPoint::orient2Dzx(P, A, B);
		o2 = genericPoint::orient2Dzx(P, B, C);
		o3 = genericPoint::orient2Dzx(P, C, A);
	}
	return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
}

bool genericPoint::pointInTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C, int xyz)
{
	int o1, o2, o3;
	if (xyz == 2)
	{
		o1 = genericPoint::orient2Dxy(P, A, B);
		o2 = genericPoint::orient2Dxy(P, B, C);
		o3 = genericPoint::orient2Dxy(P, C, A);
	}
	else if (xyz == 0)
	{
		o1 = genericPoint::orient2Dyz(P, A, B);
		o2 = genericPoint::orient2Dyz(P, B, C);
		o3 = genericPoint::orient2Dyz(P, C, A);
	}
	else
	{
		o1 = genericPoint::orient2Dzx(P, A, B);
		o2 = genericPoint::orient2Dzx(P, B, C);
		o3 = genericPoint::orient2Dzx(P, C, A);
	}
	return ((o1 >= 0 && o2 >= 0 && o3 >= 0) || (o1 <= 0 && o2 <= 0 && o3 <= 0));
}

bool genericPoint::innerSegmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q, int xyz)
{
	int o11, o12, o21, o22;

	if (xyz == 2)
	{
		o11 = orient2Dxy(P, A, B);
		o12 = orient2Dxy(Q, B, A);
		o21 = orient2Dxy(A, P, Q);
		o22 = orient2Dxy(B, Q, P);
	}
	else if (xyz == 0)
	{
		o11 = orient2Dyz(P, A, B);
		o12 = orient2Dyz(Q, B, A);
		o21 = orient2Dyz(A, P, Q);
		o22 = orient2Dyz(B, Q, P);
	}
	else
	{
		o11 = orient2Dzx(P, A, B);
		o12 = orient2Dzx(Q, B, A);
		o21 = orient2Dzx(A, P, Q);
		o22 = orient2Dzx(B, Q, P);
	}

	return ((o11 || o21 || o12 || o22) && o11 == o12 && o21 == o22);
}

bool genericPoint::segmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q, int xyz)
{
	int o11, o12, o21, o22;

	if (xyz == 2)
	{
		o11 = orient2Dxy(P, A, B);
		o12 = orient2Dxy(Q, B, A);
		o21 = orient2Dxy(A, P, Q);
		o22 = orient2Dxy(B, Q, P);
	}
	else if (xyz == 0)
	{
		o11 = orient2Dyz(P, A, B);
		o12 = orient2Dyz(Q, B, A);
		o21 = orient2Dyz(A, P, Q);
		o22 = orient2Dyz(B, Q, P);
	}
	else
	{
		o11 = orient2Dzx(P, A, B);
		o12 = orient2Dzx(Q, B, A);
		o21 = orient2Dzx(A, P, Q);
		o22 = orient2Dzx(B, Q, P);
	}

	return ((o11 || o12) && (o11 * o12 >= 0) && (o21 || o22) && (o21 * o22 >= 0));
}
