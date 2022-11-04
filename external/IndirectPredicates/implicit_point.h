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

#ifndef IMPLICIT_POINT_H
#define IMPLICIT_POINT_H

#include "numerics.h"
#include <iostream>

// Always call this (once per thread) before using indirect predicates
void initFPU();

// An indirect predicate can assume one of the following values.
// UNDEFINED means that input parameters are degenerate and do not define an
// implicit point.
enum IP_Sign {
	ZERO = 0,
	POSITIVE = 1,
	NEGATIVE = -1,
	UNDEFINED = 2
};

// A filtered indirect predicate can assume one of the following values.
// UNCERTAIN means that precision is not enough to reach a conclusion.
enum Filtered_Sign {
	UNCERTAIN = 0
};

// Indirect predicates operate on points of the following types
enum Point_Type {
	UNDEF = 0,
	EXPLICIT2D = 1,
	SSI = 2, // This must be the last 2D config in this ordered list
	EXPLICIT3D = 3,
	LPI = 4,
	TPI = 5
};

// This is a generic point. It can be extended as either explicit or implicit point
class genericPoint {
protected:
	Point_Type type;

public:
	inline genericPoint(const Point_Type& t) : type(t) {}

	inline Point_Type getType() const { return type; }
	inline bool is2D() const { return type <= SSI; }
	inline bool is3D() const { return type > SSI; }
	inline bool isExplicit2D() const { return (type == EXPLICIT2D); }
	inline bool isExplicit3D() const { return (type == EXPLICIT3D); }
	inline bool isSSI() const { return (type == SSI); }
	inline bool isLPI() const { return (type == LPI); }
	inline bool isTPI() const { return (type == TPI); }

	// The following functions convert to explicit points.
	// Use only after having verified the correct type through getType()
	//
	// Note that these violate strict aliasing rules, which has two consequences:
	// 1) Many compilers issue annoying warnings
	// 2) The optimizer may do wrong assumptions
	//
	// I do not see how an optimizer may spoil these simple functions or code around them,
	// but if you experience strange behaviour you may try replacing the type casts
	// with memcpy to an object and hope that the optimizer recognizes that an actual
	// copy is not necessary.
	inline class explicitPoint2D& toExplicit2D() { return (explicitPoint2D&)(*this); }
	inline class implicitPoint2D_SSI& toSSI() { return (implicitPoint2D_SSI&)(*this); }
	inline class explicitPoint3D& toExplicit3D() { return (explicitPoint3D&)(*this); }
	inline class implicitPoint3D_LPI& toLPI() { return (implicitPoint3D_LPI&)(*this); }
	inline class implicitPoint3D_TPI& toTPI() { return (implicitPoint3D_TPI&)(*this); }

	inline const class explicitPoint2D& toExplicit2D() const { return (explicitPoint2D&)(*this); }
	inline const class implicitPoint2D_SSI& toSSI() const { return (implicitPoint2D_SSI&)(*this); }
	inline const class explicitPoint3D& toExplicit3D() const { return (explicitPoint3D&)(*this); }
	inline const class implicitPoint3D_LPI& toLPI() const { return (implicitPoint3D_LPI&)(*this); }
	inline const class implicitPoint3D_TPI& toTPI() const { return (implicitPoint3D_TPI&)(*this); }

	// Calculates the first two cartesian coordinates. If the point is implicit, these
	// coordinates are approximated due to floating point roundoff.
	// If apap==true, the approximation is as precise as possible (slightly slower).
	// Returns 0 if the implicit point is undefined.
	bool getApproxXYCoordinates(double& x, double& y, bool apap =false) const;

	// Calculates the three cartesian coordinates. If the point is implicit, these
	// coordinates are approximated due to floating point roundoff.
	// If apap==true, the approximation is as precise as possible (slightly slower).
	// Returns 0 if the point is not 3D or if the implicit point is undefined.
	bool getApproxXYZCoordinates(double& x, double& y, double& z, bool apap = false) const;

	// These are the indirect predicates supported up to now

	// Orient2D - fully supported
	// Input points can be any combination of 2D points (explicit or SSI)
	static int orient2D(const genericPoint& a, const genericPoint& b, const genericPoint& c);

	// Orient2Dxy (resp. Orient2Dyz, Orient2Dzx) - fully supported
	// Input points can be any combination of 3D points (explicit, LPI or TPI). 
	// Orientation is computed on XY (resp. YZ, ZX).
	static int orient2Dxy(const genericPoint& a, const genericPoint& b, const genericPoint& c);
	static int orient2Dyz(const genericPoint& a, const genericPoint& b, const genericPoint& c);
	static int orient2Dzx(const genericPoint& a, const genericPoint& b, const genericPoint& c);

	// Orient3D - fully supported
	// Input can be:
	// 1) any combination of 3D points (explicit, LPI or TPI)
	static int orient3D(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d);

	// incircle - fully supported
	// Input points can be any combination of 2D points (explicit or SSI)
	static int incircle(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d);

	// incircle - partly supported
	// Input points can be any combination of either explicit or LPI 3D points. Incircle is computed on XY.
	static int incirclexy(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d);

	// inSphere - partly supported
	// Input points can be only explicit
	static int inSphere(const genericPoint& a, const genericPoint& b, const genericPoint& c, const genericPoint& d, const genericPoint& e);

	// lessThanOnX (resp. Y, Z) - partly supported (only 3D)
	// Input points can be any combination of 3D points
	// lessThanOnX(a,b) =
	// -1 - if a.X < b.X
	// 0  - if a.X == b.X
	// 1  - if a.X > b.X
	static int lessThanOnX(const genericPoint& a, const genericPoint& b);
	static int lessThanOnY(const genericPoint& a, const genericPoint& b);
	static int lessThanOnZ(const genericPoint& a, const genericPoint& b);

	// lessThan - partly supported (only 3D)
	// Input points can be any combination of 3D points
	// lessThan(a,b) =
	// -1 - if a < b
	// 0  - if a == b
	// 1  - if a > b
	// in lexicographical order
	static int lessThan(const genericPoint& a, const genericPoint& b);

	// TRUE if the two points are coincident
	static bool coincident(const genericPoint& a, const genericPoint& b) { return lessThan(a, b) == 0; }

	// Let n = (x,y,z) be the normal of the triangle <v1,v2,v3>
	// and let m be the absolute value of its largest component.
	// That is, m = max(|x|, |y|, |z|).
	// maxComponentInTriangleNormal(v1,v2,v3) returns:
	// 0 - if m == |x|
	// 1 - if m == |y|
	// 2 - if m == |z|
	//
	// Warning: this function assumes that the triangle is not exactly degenerate. It may crash otherwise.
	static int maxComponentInTriangleNormal(double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z);

	// TRUE if A-B-C are not collinear
	static bool misaligned(const genericPoint& A, const genericPoint& B, const genericPoint& C) {
		return (orient2Dxy(A, B, C) || orient2Dyz(A, B, C) || orient2Dzx(A, B, C));
	}

	// TRUE if 'p' is in the interior of v1-v2
	static bool pointInInnerSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2);

	// TRUE if 'p' is in the closure of v1-v2
	static bool pointInSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2);

	// TRUE if P is in the interior of <A,B,C>
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool pointInInnerTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C);

	// TRUE if P is in the closure of <A,B,C>
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool pointInTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C);
	// Same as above, but this version initializes oAB, oAC and oCA with the orientation of P wrt one of the edges (0 = on edge)
	static bool pointInTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C, int& oAB, int& oBC, int& oCA);

	// TRUE if the interior of A-B intersects the interior of P-Q at a single point
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool innerSegmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q);

	// TRUE if the closure of A-B intersects the closure of P-Q at a single point
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool segmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q);

	// TRUE if interior of s1-s2 intersects interior of <v1,v2,v3> at a single point
	static bool innerSegmentCrossesInnerTriangle(const genericPoint& s1, const genericPoint& s2, const genericPoint& v1, const genericPoint& v2, const genericPoint& v3);


    // The following methods are equivalent to the corresponding functions hereabove,
	// but faster. They assume that points are coplanar and the dominant normal component 
	// is n_max (see maxComponentInTriangleNormal()).
	static int orient2D(const genericPoint& a, const genericPoint& b, const genericPoint& c, int n_max)
	{
		if (n_max == 0) return orient2Dyz(a, b, c);
		else if (n_max == 1) return orient2Dzx(a, b, c);
		else return orient2Dxy(a, b, c);
	}

	static bool misaligned(const genericPoint& A, const genericPoint& B, const genericPoint& C, int n_max)
	{
		return ((n_max == 2 && orient2Dxy(A, B, C)) || (n_max == 0 && orient2Dyz(A, B, C)) || (n_max == 1 && orient2Dzx(A, B, C)));
	}

	static bool pointInInnerSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2, int n_max);
	static bool pointInSegment(const genericPoint& p, const genericPoint& v1, const genericPoint& v2, int n_max);
	static bool pointInInnerTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C, int n_max);
	static bool pointInTriangle(const genericPoint& P, const genericPoint& A, const genericPoint& B, const genericPoint& C, int n_max);
	static bool innerSegmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q, int n_max);
	static bool segmentsCross(const genericPoint& A, const genericPoint& B, const genericPoint& P, const genericPoint& Q, int n_max);
};


///////////////////////////////////////////////////////////////////////////////////
//
// 2 D   P O I N T S
//
///////////////////////////////////////////////////////////////////////////////////

class explicitPoint2D : public genericPoint {
	double x, y;

public:
	inline explicitPoint2D() : genericPoint(Point_Type::EXPLICIT2D) {}
	inline explicitPoint2D(double _x, double _y) : genericPoint(Point_Type::EXPLICIT2D), x(_x), y(_y) {}
	inline explicitPoint2D(const explicitPoint2D& b) : genericPoint(Point_Type::EXPLICIT2D), x(b.x), y(b.y) {}

	inline void operator=(const explicitPoint2D& b) { type = Point_Type::EXPLICIT2D; x = b.x; y = b.y; }
	inline void set(double a, double b) { x = a; y = b; }

	inline double X() const { return x; }
	inline double Y() const { return y; }

	inline const double* ptr() const { return &x; }
};

// Comment the following to favour low memory usage wrt speed
#define USE_CACHED_VALUES

// Implicit 2D point defined by the intersection of two lines l1 and l2
class implicitPoint2D_SSI : public genericPoint{
	const explicitPoint2D &l1_1, &l1_2, &l2_1, &l2_2;

public:
	implicitPoint2D_SSI(const explicitPoint2D& l11, const explicitPoint2D& l12, 
						const explicitPoint2D& l21, const explicitPoint2D& l22)
		: genericPoint(Point_Type::SSI), l1_1(l11), l1_2(l12), l2_1(l21), l2_2(l22)
#ifdef USE_CACHED_VALUES
		, ssfilter_denominator(NAN), dfilter_denominator(NAN)
#endif
	{}

	inline const explicitPoint2D& L1_1() const { return l1_1; }
	inline const explicitPoint2D& L1_2() const { return l1_2; }
	inline const explicitPoint2D& L2_1() const { return l2_1; }
	inline const explicitPoint2D& L2_2() const { return l2_2; }

	// Calculates an explicit approximation of the implicit point.
	// Returns false if point is undefined
	bool approxExplicit(explicitPoint2D&) const;

	// Same as above, but the approximation is as precise as possible.
	// Slightly slower.
	bool apapExplicit(explicitPoint2D&) const;

#ifdef USE_CACHED_VALUES
private: // Cached values
	mutable double ssfilter_lambda_x, ssfilter_lambda_y, ssfilter_denominator, ssfilter_max_val;
	mutable interval_number dfilter_lambda_x, dfilter_lambda_y, dfilter_denominator;

	inline bool needsFilteredLambda() const { return (ssfilter_denominator != ssfilter_denominator); } // TRUE if NAN
	inline bool needsIntervalLambda() const { return (dfilter_denominator.isNAN()); } // TRUE if NAN
#endif

public:
	bool getFilteredLambda(double& lx, double& ly, double &d, double& mv) const;
	bool getIntervalLambda(interval_number& lx, interval_number& ly, interval_number &d) const;
	void getExactLambda(double *lx, int& lxl, double *ly, int& lyl, double *d, int& dl) const;
};


///////////////////////////////////////////////////////////////////////////////////
//
// 3 D   P O I N T S
//
///////////////////////////////////////////////////////////////////////////////////

class explicitPoint3D : public genericPoint {
	double x, y, z;

public:
	inline explicitPoint3D() : genericPoint(Point_Type::EXPLICIT3D) {}
	inline explicitPoint3D(double _x, double _y, double _z) : genericPoint(Point_Type::EXPLICIT3D), x(_x), y(_y), z(_z) {}
	inline explicitPoint3D(const explicitPoint3D& b) : genericPoint(Point_Type::EXPLICIT3D), x(b.x), y(b.y), z(b.z) {}

	inline void operator=(const explicitPoint3D& b) { type = Point_Type::EXPLICIT3D; x = b.x; y = b.y; z = b.z; }
	inline void set(double a, double b, double c) { x = a; y = b; z = c; }

	inline double X() const { return x; }
	inline double Y() const { return y; }
	inline double Z() const { return z; }

	inline const double* ptr() const { return &x; }
};

// Implicit point defined by the intersection of a line and a plane
class implicitPoint3D_LPI : public genericPoint{
	const explicitPoint3D &ip, &iq; // The line
	const explicitPoint3D &ir, &is, &it; // The plane

public:
	implicitPoint3D_LPI(const explicitPoint3D& _p, const explicitPoint3D& _q, 
		const explicitPoint3D& _r, const explicitPoint3D& _s, const explicitPoint3D& _t)
		: genericPoint(Point_Type::LPI), ip(_p), iq(_q), ir(_r), is(_s), it(_t)
#ifdef USE_CACHED_VALUES
		, ssfilter_denominator(NAN), dfilter_denominator(NAN)
#endif
	{}

	inline const explicitPoint3D& P() const { return ip; }
	inline const explicitPoint3D& Q() const { return iq; }
	inline const explicitPoint3D& R() const { return ir; }
	inline const explicitPoint3D& S() const { return is; }
	inline const explicitPoint3D& T() const { return it; }

	// Calculates an explicit approximation of the implicit point.
	// Returns false if point is undefined
	bool approxExplicit(explicitPoint3D&) const;

	// Same as above, but the approximation is as precise as possible.
	// Slightly slower.
	bool apapExplicit(explicitPoint3D&) const;

#ifdef USE_CACHED_VALUES
private: // Cached values
	mutable double ssfilter_lambda_x, ssfilter_lambda_y, ssfilter_lambda_z, ssfilter_denominator, ssfilter_max_val;
	mutable interval_number dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z, dfilter_denominator;

	inline bool needsFilteredLambda() const { return (ssfilter_denominator != ssfilter_denominator); } // TRUE if NAN
	inline bool needsIntervalLambda() const { return (dfilter_denominator.isNAN()); } // TRUE if NAN
#endif

public:
	bool getFilteredLambda(double& lx, double& ly, double& lz, double &d, double& mv) const;
	bool getIntervalLambda(interval_number& lx, interval_number& ly, interval_number& lz, interval_number &d) const;
	void getExactLambda(double *lx, int& lxl, double *ly, int& lyl, double *lz, int& lzl, double *d, int& dl) const;
};


// Implicit point defined by the intersection of three planes
class implicitPoint3D_TPI : public genericPoint{
	const explicitPoint3D &iv1, &iv2, &iv3; // Plane 1
	const explicitPoint3D &iw1, &iw2, &iw3; // Plane 2
	const explicitPoint3D &iu1, &iu2, &iu3; // Plane 3

public:
	implicitPoint3D_TPI(const explicitPoint3D& _v1, const explicitPoint3D& _v2, const explicitPoint3D& _v3,
						const explicitPoint3D& _w1, const explicitPoint3D& _w2, const explicitPoint3D& _w3,
						const explicitPoint3D& _u1, const explicitPoint3D& _u2, const explicitPoint3D& _u3)
						: genericPoint(Point_Type::TPI), iv1(_v1), iv2(_v2), iv3(_v3), iw1(_w1), iw2(_w2), iw3(_w3), iu1(_u1), iu2(_u2), iu3(_u3)
#ifdef USE_CACHED_VALUES
		, ssfilter_denominator(NAN), dfilter_denominator(NAN)
#endif
	{}

	inline const explicitPoint3D& V1() const { return iv1; }
	inline const explicitPoint3D& V2() const { return iv2; }
	inline const explicitPoint3D& V3() const { return iv3; }
	inline const explicitPoint3D& W1() const { return iw1; }
	inline const explicitPoint3D& W2() const { return iw2; }
	inline const explicitPoint3D& W3() const { return iw3; }
	inline const explicitPoint3D& U1() const { return iu1; }
	inline const explicitPoint3D& U2() const { return iu2; }
	inline const explicitPoint3D& U3() const { return iu3; }

	// Calculates an explicit approximation of the implicit point.
	// Returns false if point is undefined
	bool approxExplicit(explicitPoint3D&) const;

	// Same as above, but the approximation is as precise as possible.
	// Slightly slower.
	bool apapExplicit(explicitPoint3D&) const;

#ifdef USE_CACHED_VALUES
private: // Cached values
	mutable double ssfilter_lambda_x, ssfilter_lambda_y, ssfilter_lambda_z, ssfilter_denominator, ssfilter_max_val;
	mutable interval_number dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z, dfilter_denominator;

	inline bool needsFilteredLambda() const { return (ssfilter_denominator != ssfilter_denominator); } // TRUE if NAN
	inline bool needsIntervalLambda() const { return (dfilter_denominator.isNAN()); } // TRUE if NAN
#endif

public:
	bool getFilteredLambda(double& lx, double& ly, double& lz, double &d, double& mv) const;
	bool getIntervalLambda(interval_number& lx, interval_number& ly, interval_number& lz, interval_number &d) const;
	void getExactLambda(double *lx, int& lxl, double *ly, int& lyl, double *lz, int& lzl, double *d, int& dl) const;
};

//////////////////////////////////////////////////////////////////////////////////////
//
// OUTPUT TO STD STREAMS
//
//////////////////////////////////////////////////////////////////////////////////////

using namespace ::std;

ostream& operator<<(ostream& os, const genericPoint& p);

inline ostream& operator<<(ostream& os, const explicitPoint2D& p)
{
	return os << p.X() << " " << p.Y() << " 0";
}

inline ostream& operator<<(ostream& os, const implicitPoint2D_SSI& p)
{
	explicitPoint2D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEF_SSI";
}

inline ostream& operator<<(ostream& os, const explicitPoint3D& p)
{
	return os << p.X() << " " << p.Y() << " " << p.Z();
}

inline ostream& operator<<(ostream& os, const implicitPoint3D_LPI& p)
{
	explicitPoint3D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEF_LPI";
}

inline ostream& operator<<(ostream& os, const implicitPoint3D_TPI& p)
{
	explicitPoint3D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEF_TPI";
}

#endif // IMPLICIT_POINT_H
