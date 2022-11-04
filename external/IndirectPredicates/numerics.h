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

#ifndef NUMERICS_H
#define NUMERICS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <fenv.h>
#include <iostream>

	inline void ip_error(const char* msg)
	{
        std::cerr << msg << std::endl;
        //fprintf(stderr, msg);
		exit(0);
	}

#if INTPTR_MAX == INT64_MAX
#define	IS64BITPLATFORM
#endif

#ifdef _MSC_VER
#define	ISVISUALSTUDIO
#endif

// 64-bit
#ifdef IS64BITPLATFORM
#ifndef __arm64__ // do not use SIMD instructions on Apple ARM M1
#define USE_SIMD_INSTRUCTIONS
#endif
#endif

#ifdef ISVISUALSTUDIO

#pragma fenv_access (on)

	inline void setFPUModeToRoundUP() { _controlfp(_RC_UP, _MCW_RC); }
	inline void setFPUModeToRoundNEAR() { _controlfp(_RC_NEAR, _MCW_RC); }

#else

#pragma STDC FENV_ACCESS ON
	inline void setFPUModeToRoundUP() { fesetround(FE_UPWARD); }
	inline void setFPUModeToRoundNEAR() { fesetround(FE_TONEAREST); }
#endif

#ifdef USE_SIMD_INSTRUCTIONS
#include <emmintrin.h>

	//inline void setFPUModeToRoundUP() { _MM_SET_ROUNDING_MODE(_MM_ROUND_UP); }
	//inline void setFPUModeToRoundNEAR() { _MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST); }
	

	class interval_number
	{
		__m128d interval; // interval[1] = min_low, interval[0] = high

		static __m128d zero;
		static __m128i sign_low_mask, sign_high_mask;

	public:
		static void init();

		const double *getInterval() const { return (const double*)&interval; }

		inline interval_number() { }
		inline interval_number(const double a) : interval(_mm_set_pd(-a, a)) {}
		inline interval_number(const double minf, const double sup) : interval(_mm_set_pd(minf, sup)) {}
		inline interval_number(const __m128d& i) : interval(i) {}
		inline interval_number(const interval_number& b) : interval(b.interval) {}

		inline double inf() const { return -((double*)(&interval))[1]; }
		inline double sup() const { return ((double*)(&interval))[0]; }
		inline double width() const { return sup() - inf(); }

		inline void invert() { interval = _mm_shuffle_pd(interval, interval, 1); }

		inline bool isNegative() const { return _mm_comilt_sd(interval, zero); }
		inline bool isPositive() const { return _mm_comilt_sd(_mm_shuffle_pd(interval, interval, 1), zero); }

		inline bool signIsReliable() const { return (isNegative() || isPositive()); } // Zero is not accounted for
		inline int sign() const { return (isNegative()) ? (-1) : (1); } // Zero is not accounted for

		inline bool isNAN() const {	return sup() != sup(); }

		inline bool operator<(const double b) const { return (_mm_comilt_sd(interval, _mm_set_sd(b))); }

		inline interval_number& operator=(const interval_number& b) { interval = b.interval; return *this; }

		inline interval_number operator+(const interval_number& b) const { return interval_number(_mm_add_pd(interval, b.interval)); }

		inline interval_number operator-(const interval_number& b) const { return interval_number(_mm_add_pd(interval, _mm_shuffle_pd(b.interval, b.interval, 1))); }

		inline interval_number operator*(const interval_number& b) const
		{
			__m128i ssg;
			__m128d llhh, lhhl, ip;

			switch ((_mm_movemask_pd(interval) << 2) | _mm_movemask_pd(b.interval))
			{
			case 0:
				llhh = _mm_mul_pd(interval, b.interval);
				lhhl = _mm_mul_pd(interval, _mm_shuffle_pd(b.interval, b.interval, 1));
				return interval_number(_mm_max_pd(_mm_unpacklo_pd(llhh, lhhl), _mm_unpackhi_pd(llhh, lhhl)));
			case 1:
				return interval_number(_mm_mul_pd(_mm_shuffle_pd(b.interval, b.interval, 3), _mm_shuffle_pd(interval, interval, 1)));
			case 2:
				return interval_number(_mm_mul_pd(_mm_shuffle_pd(b.interval, b.interval, 0), interval));
			case 4:
				return interval_number(_mm_mul_pd(_mm_shuffle_pd(interval, interval, 3), _mm_shuffle_pd(b.interval, b.interval, 1)));
			case 5:
				ip = _mm_mul_pd(_mm_castsi128_pd(_mm_xor_si128(_mm_castpd_si128(interval), sign_high_mask)), b.interval);
				return interval_number(_mm_shuffle_pd(ip, ip, 1));
			case 6:
				ssg = _mm_xor_si128(_mm_castpd_si128(b.interval), sign_low_mask);
				return interval_number(_mm_mul_pd(interval, _mm_shuffle_pd(_mm_castsi128_pd(ssg), _mm_castsi128_pd(ssg), 1)));
			case 8:
				return interval_number(_mm_mul_pd(_mm_shuffle_pd(interval, interval, 0), b.interval));
			case 9:
				ssg = _mm_xor_si128(_mm_castpd_si128(interval), sign_low_mask);
				return interval_number(_mm_mul_pd(b.interval, _mm_shuffle_pd(_mm_castsi128_pd(ssg), _mm_castsi128_pd(ssg), 1)));
			case 10:
				return interval_number(_mm_mul_pd(interval, _mm_castsi128_pd(_mm_xor_si128(_mm_castpd_si128(b.interval), sign_low_mask))));
			}

			return interval_number(NAN);
		}

	};

#else // USE_SIMD_INSTRUCTIONS

	// Interval_number
	class interval_number
	{
		typedef union error_approx_type_t
		{
			double d;
			uint64_t u;

			inline error_approx_type_t() {}
			inline error_approx_type_t(double a) : d(a) {}
			inline uint64_t is_negative() const { return u >> 63; }
		} casted_double;

	public:
		double min_low, high;

		static void init() {}

		const double* getInterval() const { return (const double*)&min_low; }

		inline interval_number() { }
		inline interval_number(double a) : min_low(-a), high(a) {}
		inline interval_number(double minf, double sup) : min_low(minf), high(sup) {}
		inline interval_number(const interval_number& b) : min_low(b.min_low), high(b.high) {}

		inline double inf() const { return -min_low; }
		inline double sup() const { return high; }
		inline double width() const { return sup() - inf(); }

		inline bool isNegative() const { return (high < 0); }
		inline void invert() { double tmp = min_low; min_low = high; high = tmp; }

		inline bool signIsReliable() const { return (min_low < 0 || high < 0); }
		inline int sign() const { return (min_low < 0) ? (1) : ((high < 0) ? (-1) : (0)); }

		inline bool isNAN() const { return (high != high); }

		inline double center() const { return 0.5 * (high - min_low); }

		inline bool operator<(const double b) const { return (high < b); }

		inline interval_number& operator=(const interval_number& b) { min_low = b.min_low; high = b.high; return *this; }

		inline interval_number operator+(const interval_number& b) const { return interval_number(min_low + b.min_low, high + b.high); }

		inline interval_number operator-(const interval_number& b) const { return interval_number(b.high + min_low, high + b.min_low); }

		inline interval_number operator*(const interval_number& b) const
		{
			casted_double l1(min_low), h1(high), l2(b.min_low), h2(b.high);
			uint64_t cfg = (l1.is_negative() << 3) + (h1.is_negative() << 2) + (l2.is_negative() << 1) + (h2.is_negative());

			switch (cfg)
			{
			case 10: return interval_number(min_low * (-b.min_low), high * b.high);
			case 8: return interval_number(high * b.min_low, high * b.high);
			case 9: return interval_number(high * b.min_low, (-min_low) * b.high);
			case 2: return interval_number(min_low * b.high, high * b.high);
			case 0:
				double ll, lh, hl, hh;
				ll = min_low * b.min_low; lh = (min_low * b.high); hl = (high * b.min_low); hh = high * b.high;
				if (hl > lh) lh = hl;
				if (ll > hh) hh = ll;
				return interval_number(lh, hh);
			case 1: return interval_number(high * b.min_low, min_low * b.min_low);
			case 6: return interval_number(min_low * b.high, high * (-b.min_low));
			case 4: return interval_number(min_low * b.high, min_low * b.min_low);
			case 5: return interval_number((-high) * b.high, min_low * b.min_low);
			};

			return interval_number(NAN);
		}
	};
#endif // USE_SIMD_INSTRUCTIONS

	inline std::ostream& operator<<(std::ostream& os, const interval_number& p)
	{
		os << "[ " << p.inf() << ", " << p.sup() << " ]";
		return os;
	}


	// The following macros are fast implementations of basic expansion arithmetic due
	// to Dekker, Knuth, Priest, Shewchuk, and others.

	// See Y. Hida, X. S. Li,  D. H. Bailey "Algorithms for Quad-Double Precision Floating Point Arithmetic"

	// Sums
#define Quick_Two_Sum(a, b, x, y) x = a + b; y = b - (x - a)
#define Two_Sum(a, b, x, y) x = a + b; _bv = x - a; y = (a - (x - _bv)) + (b - _bv)
#define Two_One_Sum(a1, a0, b, x2, x1, x0) Two_Sum(a0, b , _i, x0); Two_Sum(a1, _i, x2, x1)

// Differences
#define Two_Diff(a, b, x, y) x = a - b; _bv = a - x; y = (a - (x + _bv)) + (_bv - b)
#define Two_One_Diff(a1, a0, b, x2, x1, x0) Two_Diff(a0, b , _i, x0); Two_Sum( a1, _i, x2, x1)

// Products
#define Split(a, _ah, _al) _c = 1.3421772800000003e+008 * a; _ah = _c - (_c - a); _al = a - _ah
#define Two_Prod_PreSplit(a, b, _bh, _bl, x, y) x = a * b; Split(a, _ah, _al); y = (_al * _bl) - (((x - (_ah * _bh)) - (_al * _bh)) - (_ah * _bl))
#define Two_Product_2Presplit(a, _ah, _al, b, _bh, _bl, x, y) x = a * b; y = (_al * _bl) - (((x - _ah * _bh) - (_al * _bh)) - (_ah * _bl))


// An instance of the following must be created to access functions for expansion arithmetic
	class expansionObject
	{
		// Temporary vars used in low-level arithmetic
		double _bv, _c, _ah, _al, _bh, _bl, _i, _j, _k, _l, _0, _1, _2, _u3;

	public:
		expansionObject() {}

		inline void two_Sum(const double a, const double b, double* xy) { Two_Sum(a, b, xy[1], xy[0]); }

		inline void two_Diff(const double a, const double b, double* xy) { Two_Diff(a, b, xy[1], xy[0]); }

		// [x,y] = [a]*[b]		 Multiplies two expansions [a] and [b] of length one
		inline void Two_Prod(const double a, const double b, double& x, double& y)
		{
			x = a * b;
			Split(a, _ah, _al); Split(b, _bh, _bl);
			y = ((_ah * _bh - x) + _ah * _bl + _al * _bh) + _al * _bl;
		}
		inline void Two_Prod(const double a, const double b, double* xy) { Two_Prod(a, b, xy[1], xy[0]); }


		// [x,y] = [a]^2		Squares an expansion of length one
		inline void Square(const double a, double& x, double& y)
		{
			x = a * a;
			Split(a, _ah, _al);
			y = (_al * _al) - ((x - (_ah * _ah)) - ((_ah + _ah) * _al));
		}
		inline void Square(const double a, double* xy) { Square(a, xy[1], xy[0]); }

		// [x2,x1,x0] = [a1,a0]-[b]		Subtracts an expansion [b] of length one from an expansion [a1,a0] of length two
		inline void two_One_Diff(const double a1, const double a0, const double b, double& x2, double& x1, double& x0)
		{ Two_One_Diff(a1, a0, b, x2, x1, x0); }
		inline void two_One_Diff(const double* a, const double b, double* x) { two_One_Diff(a[1], a[0], b, x[2], x[1], x[0]); }

		// [x3,x2,x1,x0] = [a1,a0]*[b]		Multiplies an expansion [a1,a0] of length two by an expansion [b] of length one
		inline void Two_One_Prod(const double a1, const double a0, const double b, double& x3, double& x2, double& x1, double& x0)
		{
			Split(b, _bh, _bl);
			Two_Prod_PreSplit(a0, b, _bh, _bl, _i, x0); Two_Prod_PreSplit(a1, b, _bh, _bl, _j, _0);
			Two_Sum(_i, _0, _k, x1); Quick_Two_Sum(_j, _k, x3, x2);
		}
		inline void Two_One_Prod(const double* a, const double b, double* x) { Two_One_Prod(a[1], a[0], b, x[3], x[2], x[1], x[0]); }

		// [x3,x2,x1,x0] = [a1,a0]+[b1,b0]		Calculates the sum of two expansions of length two
		inline void Two_Two_Sum(const double a1, const double a0, const double b1, const double b0, double& x3, double& x2, double& x1, double& x0)
		{
			Two_One_Sum(a1, a0, b0, _j, _0, x0); Two_One_Sum(_j, _0, b1, x3, x2, x1);
		}
		inline void Two_Two_Sum(const double* a, const double* b, double* xy) { Two_Two_Sum(a[1], a[0], b[1], b[0], xy[3], xy[2], xy[1], xy[0]); }

		// [x3,x2,x1,x0] = [a1,a0]-[b1,b0]		Calculates the difference between two expansions of length two
		inline void Two_Two_Diff(const double a1, const double a0, const double b1, const double b0, double& x3, double& x2, double& x1, double& x0)
		{
			Two_One_Diff(a1, a0, b0, _j, _0, x0); Two_One_Diff(_j, _0, b1, _u3, x2, x1); x3 = _u3;
		}
		inline void Two_Two_Diff(const double* a, const double* b, double* x) { Two_Two_Diff(a[1], a[0], b[1], b[0], x[3], x[2], x[1], x[0]); }

		// Calculates the second component 'y' of the expansion [x,y] = [a]-[b] when 'x' is known
		inline void Two_Diff_Back(const double a, const double b, double& x, double& y) { _bv = a - x; y = (a - (x + _bv)) + (_bv - b); }
		inline void Two_Diff_Back(const double a, const double b, double* xy) { Two_Diff_Back(a, b, xy[1], xy[0]); }

		// [h] = [a1,a0]^2		Squares an expansion of length 2
		// 'h' must be allocated by the caller with 6 components.
		void Two_Square(const double& a1, const double& a0, double* x);

		// [h7,h6,...,h0] = [a1,a0]*[b1,b0]		Calculates the product of two expansions of length two.
		// 'h' must be allocated by the caller with eight components.
		void Two_Two_Prod(const double a1, const double a0, const double b1, const double b0, double* h);
		inline void Two_Two_Prod(const double* a, const double* b, double* xy) { Two_Two_Prod(a[1], a[0], b[1], b[0], xy); }

		// [h7,h6,...,h0] = [a1,a0]*[b1,b0]		Calculates the product of two expansions of length two.
		// 'h' must be allocated by the caller with eight components.
		//void Two_Two_Prod(const double a1, const double a0, const double b1, const double b0, double *h);
		//inline void Two_Two_Prod(const double *a, const double *b, double *xy) { Two_Two_Prod(a[1], a[0], b[1], b[0], xy); }

		// [e] = -[e]		Inplace inversion
		static void Gen_Invert(const int elen, double* e) { for (int i = 0; i < elen; i++) e[i] = -e[i]; }

		// [h] = [e] + [f]		Sums two expansions and returns number of components of result
		// 'h' must be allocated by the caller with at least elen+flen components.
		int Gen_Sum(const int elen, const double* e, const int flen, const double* f, double* h);

		// Same as above, but 'h' is allocated internally. The caller must still call 'free' to release the memory.
		inline int Gen_Sum_With_Alloc(const int elen, const double* e, const int flen, const double* f, double** h)
		{
			*h = (double*)malloc((elen + flen) * sizeof(double));
			return Gen_Sum(elen, e, flen, f, *h);
		}

		// [h] = [e] + [f]		Subtracts two expansions and returns number of components of result
		// 'h' must be allocated by the caller with at least elen+flen components.
		int Gen_Diff(const int elen, const double* e, const int flen, const double* f, double* h);

		// Same as above, but 'h' is allocated internally. The caller must still call 'free' to release the memory.
		inline int Gen_Diff_With_Alloc(const int elen, const double* e, const int flen, const double* f, double** h)
		{
			*h = (double*)malloc((elen + flen) * sizeof(double));
			return Gen_Diff(elen, e, flen, f, *h);
		}

		// [h] = [e] * b		Multiplies an expansion by a scalar
		// 'h' must be allocated by the caller with at least elen*2 components.
		int Gen_Scale(const int elen, const double* e, const double& b, double* h);

		// [h] = [e] * 2		Multiplies an expansion by 2
		// 'h' must be allocated by the caller with at least elen components. This is exact up to overflows.
		inline void Double(const int elen, const double* e, double* h) const { for (int i = 0; i < elen; i++) h[i] = 2 * e[i]; }
		
		// [h] = [e] * 2		Multiplies an expansion by n
		// If 'n' is a power of two, the multiplication is exact
		inline static void ExactScale(const int elen, double* e, const double n) { for (int i = 0; i < elen; i++) e[i] *= n; }

		// [h] = [a] * [b]
		// 'h' must be allocated by the caller with at least 2*alen*blen components.
		int Sub_product(const int alen, const double* a, const int blen, const double* b, double* h);

		// [h] = [a] * [b]
		// 'h' must be allocated by the caller with at least MAX(2*alen*blen, 8) components.
		int Gen_Product(const int alen, const double* a, const int blen, const double* b, double* h);

		// Same as above, but 'h' is allocated internally. The caller must still call 'free' to release the memory.
		inline int Gen_Product_With_Alloc(const int alen, const double* a, const int blen, const double* b, double** h)
		{
			int h_len = alen * blen * 2;
			if (h_len < 8) h_len = 8;
			*h = (double*)malloc(h_len * sizeof(double));
			return Gen_Product(alen, a, blen, b, *h);
		}


		// Assume that *h is pre-allocated with hlen doubles.
		// If more elements are required, *h is re-allocated internally.
		// In any case, the function returns the size of the resulting expansion.
		// The caller must verify whether reallocation took place, and possibly call 'free' to release the memory.
		// When reallocation takes place, *h becomes different from its original value.

		inline int Double_With_PreAlloc(const int elen, const double* e, double** h, const int hlen)
		{
			int newlen = elen;
			if (hlen < newlen) *h = (double*)malloc(newlen * sizeof(double));
			//if (hlen < newlen) printf("REALLOC %d bytes\n", newlen);
			Double(elen, e, *h);
			return newlen;
		}

		inline int Gen_Scale_With_PreAlloc(const int elen, const double* e, const double& b, double** h, const int hlen)
		{
			int newlen = elen * 2;
			if (hlen < newlen) *h = (double*)malloc(newlen * sizeof(double));
			return Gen_Scale(elen, e, b, *h);
		}

		inline int Gen_Sum_With_PreAlloc(const int elen, const double* e, const int flen, const double* f, double** h, const int hlen)
		{
			int newlen = elen + flen;
			if (hlen < newlen) *h = (double*)malloc(newlen * sizeof(double));
			return Gen_Sum(elen, e, flen, f, *h);
		}

		inline int Gen_Diff_With_PreAlloc(const int elen, const double* e, const int flen, const double* f, double** h, const int hlen)
		{
			int newlen = elen + flen;
			if (hlen < newlen) *h = (double*)malloc(newlen * sizeof(double));
			return Gen_Diff(elen, e, flen, f, *h);
		}

		inline int Gen_Product_With_PreAlloc(const int alen, const double* a, const int blen, const double* b, double** h, const int hlen)
		{
			int newlen = alen * blen * 2;
			if (hlen < newlen || hlen < 8)
			{
				if (newlen < 8) newlen = 8;
				*h = (double*)malloc(newlen * sizeof(double));
			}
			return Gen_Product(alen, a, blen, b, *h);
		}

		// Approximates the expansion to a double
		static double To_Double(const int elen, const double* e);

		static void print(const int elen, const double* e) { for (int i = 0; i < elen; i++) printf("%e ", e[i]); printf("\n");}
	};

#endif //NUMERICS_H
