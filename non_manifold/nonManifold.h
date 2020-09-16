/*****************************************************************************************
 *              MIT License                                                              *
 *                                                                                       *
 * Copyright (c) 2020 Gianmarco Cherchi, Marco Livesu, Riccardo Scateni e Marco Attene   *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION     *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                *
 *                                                                                       *
 * Authors:                                                                              *
 *      Gianmarco Cherchi (g.cherchi@unica.it)                                           *
 *      https://people.unica.it/gianmarcocherchi/                                        *
 *                                                                                       *
 *      Marco Livesu (marco.livesu@ge.imati.cnr.it)                                      *
 *      http://pers.ge.imati.cnr.it/livesu/                                              *
 *                                                                                       *
 *      Riccardo Scateni (riccardo@unica.it)                                             *
 *      https://people.unica.it/riccardoscateni/                                         *
 *                                                                                       *
 *      Marco Attene (marco.attene@ge.imati.cnr.it)                                      *
 *      https://www.cnr.it/en/people/marco.attene/                                       *
 *                                                                                       *
 * ***************************************************************************************/

#ifndef _NONMANIFOLD_H
#define _NONMANIFOLD_H

#include "tmesh_kernel.h"
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <assert.h>

#include "cinolib/io/read_STL.h"
#include "cinolib/io/read_OBJ.h"
#include "cinolib/vector_serialization.h"

// <V,E,T> data structure for nonmanifold meshes
// Stores complete VE and ET relations

// Macros to work with marking bits (FOR MINKOWSI)
#define MARK_VISIT(a)   ((a)->mask |= ((unsigned char)1))
#define IS_VISITED(a)   ((a)->mask &  ((unsigned char)1))
#define UNMARK_VISIT(a) ((a)->mask &= (~((unsigned char)1)))

#define MARK_VISIT2(a)   ((a)->mask |= ((unsigned char)2))
#define IS_VISITED2(a)   ((a)->mask &  ((unsigned char)2))
#define UNMARK_VISIT2(a) ((a)->mask &= (~((unsigned char)2)))

#define MARK_BIT(a,b)   ((a)->mask |= ((unsigned char)(1<<b)))
#define IS_BIT(a,b)     ((a)->mask &  ((unsigned char)(1<<b)))
#define UNMARK_BIT(a,b) ((a)->mask &= (~((unsigned char)(1<<b))))

#define TAG_SHARPEDGE(a)   (MARK_BIT((a),7))
#define IS_SHARPEDGE(a)    (IS_BIT((a),7))
#define UNTAG_SHARPEDGE(a) (UNMARK_BIT((a),7))


namespace T_MESH
{
#ifndef TAG_SHARPEDGE
#define TAG_SHARPEDGE(a)   (MARK_BIT((a),7))
#define IS_SHARPEDGE(a)    (IS_BIT((a),7))
#define UNTAG_SHARPEDGE(a) (UNMARK_BIT((a),7))
#endif

	class nmEdge;
	class nmTriangle;

	// Possibly non-manifold vertex. Complete VE relation.
	class nmVertex : public Point
	{
	public:
		std::vector<nmEdge *> ve;
		mutable unsigned char mask;

		nmVertex(const Point& p) : Point(p), ve(), mask(0) {}
		nmVertex(const Point *p) : Point(p), ve(), mask(0) {}
		nmVertex(const coord& a, const coord& b, const coord& c) : Point(a, b, c), ve(), mask(0) {}

		inline void unlink() { ve.clear(); }
		inline bool isLinked() const { return (!ve.empty()); }

		inline void addEdge(nmEdge *e) { ve.push_back(e); }
		inline void removeEdge(const nmEdge *e) { ve.erase(std::find(ve.begin(), ve.end(), e)); }
		inline bool hasIncidentEdge(const nmEdge *e) const { return (std::find(ve.begin(), ve.end(), e) != ve.end()); }
		nmEdge *getEdge(const nmVertex *ov) const;
		inline uint32_t valence() const { return (uint32_t)ve.size(); }

		//! Fills vt with incident triangles
		void getVT(std::vector<nmTriangle *> *vt) const;

		//! TRUE if any of the incident edges is on boundary
		bool isOnBoundary() const;

		//! TRUE if incident triangles form a topological disk.
		bool isTopologicalDisk() const;

		//! TRUE if incident triangles form a topological half disk. Implies isOnBoundary().
		bool isTopologicalHalfDisk() const;

		//! TRUE if incident triangles form a topological half disk. Implies isOnBoundary().
		inline bool isDiskOrHalfDisk() const { return (isTopologicalHalfDisk() || isTopologicalDisk()); }

		//! TRUE if all the incident triangles belong to a common plane. TRUE does not imply that vertex is manifold.
		//! FALSE if on boundary.
		bool isFlat() const;

		//! TRUE iff vertex neighborhood is made of either two flat halfdisks or one flat halfdisk on a rectilinear boundary.
		//! When TRUE, the two edges may be initialized with the two non-flat incident edges.
		//! If the two halfdisks are also coplanar, returns TRUE and e1 and e2 are set to NULL.
		bool isDoubleFlat(nmEdge **e1, nmEdge **e2) const;

		//! Unlinks the vertex if it is either Flat() or DoubleFlat(). On success, the function returns TRUE,
		//! the vertex neighborhood is retriangulated, and the geometric realization does not change.
		bool removeIfRedundant();

		//! Inits vt with the set of all the incident triangles
		void getIncidentTriangles(std::vector<nmTriangle *> *vt) const;

		//! Average of incident triangle normals wieghted on incidence angle.
		Point getNormal() const;

		//! Sum of incident triangle angles
		double totalAngle() const;
		
		// Static functions to be used as predicates in std algorithms
		static inline bool isUnlinkedPtr(const nmVertex *v) { return (!v->isLinked()); } // Predicate for std::remove_if
		static inline bool isIsolatedPtr(const nmVertex *v) { return (!v->isLinked()); }
		static inline bool isTopologicalDiskPtr(const nmVertex *v) { return (v->isTopologicalDisk()); }
		static inline bool isTopologicalHalfDiskPtr(const nmVertex *v) { return (v->isTopologicalHalfDisk()); }
		static inline bool isDiskOrHalfDiskPtr(const nmVertex *v) { return (v->isDiskOrHalfDisk()); }

		static inline bool lessThanPtr(const nmVertex *a, const nmVertex *b) { return ((*a)<(*b)); } // Predicate for std::sort
	};

	// Possibly non-manifold edge. Complete et relation.
	class nmEdge
	{
	public:
		nmVertex *v1, *v2;
		std::vector<nmTriangle *> et;
		mutable unsigned char mask;
		void *info;

		nmEdge(nmVertex *p1, nmVertex *p2) : v1(p1), v2(p2), mask(0), info(NULL), et() { v1->ve.push_back(this); v2->ve.push_back(this); }

		inline void unlink() { v1 = v2 = NULL; }
		inline bool isLinked() const { return (v1 != NULL); }

		inline bool hasVertex(const nmVertex *v) const { return (v == v1 || v == v2); }
		inline bool hasVertices(const nmVertex *va, const nmVertex *vb) const { return (((va == v1) && (vb == v2)) || ((va == v2) && (vb == v1))); }
		inline void invert() { nmVertex *t = v1; v1 = v2; v2 = t; }
		inline bool hasIncidentTriangle(const nmTriangle *t) const { return (std::find(et.begin(), et.end(), t) != et.end()); }
		inline Point getMidPoint() const { return ((*v1) + (*v2)) * 0.5; }
		inline Point toVector() const { return (*v2) - (*v1); }
		inline void replaceVertex(nmVertex *o, nmVertex *n) { if (v1 == o) v1 = n; else if (v2 == o) v2 = n; }
		inline nmVertex *commonVertex(const nmEdge *b) const { return ((v1 == b->v1 || v1 == b->v2) ? (v1) : ((v2 == b->v1 || v2 == b->v2) ? (v2) : (NULL))); }
		inline void addTriangle(nmTriangle *t) { et.push_back(t); }
		inline void removeTriangle(nmTriangle *t) { et.erase(std::find(et.begin(), et.end(), t)); }
		inline nmVertex *oppositeVertex(const nmVertex *v) const { return ((v1 == v) ? (v2) : ((v2 == v) ? (v1) : (NULL))); }
		inline nmTriangle *oppositeTriangle(const nmTriangle *t) const { return (et.size() == 2) ? ((et[0]==t)?(et[1]):(et[0])) : (NULL); }
		inline coord squaredLength() const { return v1->squaredDistance(v2); }
		bool isFlat() const;
		bool swap(const bool fast =false);
		nmVertex *collapseOnV1();
		inline nmVertex *collapseOnV2() { invert(); return collapseOnV1(); }
		inline nmVertex *collapse(const Point& p) { nmVertex *nv = collapseOnV1(); nv->setValue(p); }
		inline bool isIsolated() const { return (et.size() == 0); }
		inline bool isOnBoundary() const { return (et.size() == 1); }
		inline bool isTopologicalDisk() const { return (et.size() == 2); }
		inline bool isDiskOrHalfDisk() const { return (isTopologicalDisk() || isOnBoundary()); }
		inline bool isDegenerate() const { return ((*v1) == (*v2)); }
		inline nmTriangle *getBoundaryTriangle() const { return (et.size() == 1) ? (et.back()) : (NULL); }
		inline void printEdge(FILE *f = stdout) const { v1->printPoint(f); v2->printPoint(f); }

		bool hasConsistentlyOrientedNeighbors() const;

		inline double length() const { return v1->distance(v2); }
		Point toUnitVector() const;
		double dihedralAngle() const; //! -1.0 if non manifold or boundary. Actual triangle orientation matters.
		double curvature() const;

		// Static functions to be used as predicates in std algorithms
		static inline bool isFlatPtr(const nmEdge *e) { return e->isFlat(); }
		static inline bool isOnBoundaryPtr(const nmEdge *e) { return e->isOnBoundary(); }
		static inline bool isUnlinkedPtr(const nmEdge *e) { return (!e->isLinked()); }
		static inline bool isTopologicalDiskPtr(const nmEdge *e) { return (e->isTopologicalDisk()); }
		static inline bool isIsolatedPtr(const nmEdge *e) { return (e->isIsolated()); }
		static inline bool isDiskOrHalfDiskPtr(const nmEdge *e) { return (e->isDiskOrHalfDisk()); }
		static inline bool hasConsistentlyOrientedNeighborsPtr(const nmEdge *e) { return (e->hasConsistentlyOrientedNeighbors()); }

		static bool vertexPtrSortFunc(const nmEdge *a, const nmEdge *b); //! Sort based on endpoints pointers
		static bool vertexInfoSortFunc(const nmEdge *a, const nmEdge *b); //! Sort based on endpoints pointers
	};


	// Possibly non-manifold triangle. Three edges.
	class nmTriangle
	{
	public:
		nmEdge *e1, *e2, *e3;
		mutable unsigned char mask;
		void *info;

		nmTriangle(nmEdge *a, nmEdge *b, nmEdge *c) : e1(a), e2(b), e3(c), mask(0), info(NULL) { e1->et.push_back(this); e2->et.push_back(this); e3->et.push_back(this); }

		inline void unlink() { e1 = e2 = e3 = NULL; }
		inline bool isLinked() const { return (e1 != NULL); }

		inline nmVertex *v1() const { return e1->commonVertex(e2); }	//!< First vertex
		inline nmVertex *v2() const { return e2->commonVertex(e3); }	//!< Second vertex
		inline nmVertex *v3() const { return e3->commonVertex(e1); }	//!< Third vertex
		inline nmTriangle *t1() const { return e1->oppositeTriangle(this); }
		inline nmTriangle *t2() const { return e2->oppositeTriangle(this); }
		inline nmTriangle *t3() const { return e3->oppositeTriangle(this); }

		inline bool hasEdge(const nmEdge *e) const { return (e == e1 || e == e2 || e == e3); }
		inline bool hasVertex(const nmVertex *v) const { return (e1->hasVertex(v) || e2->hasVertex(v) || e3->hasVertex(v)); }
		inline bool hasCommonVertex(const nmEdge *e) const { return (hasVertex(e->v1) || hasVertex(e->v2)); }
		inline nmVertex *oppositeVertex(const nmEdge *e) const { return (e == e1) ? (v2()) : ((e == e2) ? (v3()) : ((e == e3) ? (v1()) : (NULL))); }
		inline nmEdge *oppositeEdge(const nmVertex *v) const {	return ((!e1->hasVertex(v)) ? (e1) : ((!e2->hasVertex(v)) ? (e2) : ((!e3->hasVertex(v)) ? (e3) : (NULL))));	}
		inline nmTriangle *oppositeTriangle(const nmVertex *v) const { return ((v==v1()) ? (t3()) : ((v==v2()) ? (t1()) : ((v==v3()) ? (t2()) : (NULL)))); }

		inline nmEdge *nextEdge(const nmEdge *e) const { return ((e == e1) ? (e2) : ((e == e2) ? (e3) : ((e == e3) ? (e1) : (NULL)))); }
		inline nmEdge *prevEdge(const nmEdge *e) const { return ((e == e1) ? (e3) : ((e == e2) ? (e1) : ((e == e3) ? (e2) : (NULL)))); }
		inline nmVertex *nextVertex(const nmVertex *v) const { return ((v == v1()) ? (v2()) : ((v == v2()) ? (v3()) : ((v == v3()) ? (v1()) : (NULL)))); }
		inline nmVertex *prevVertex(const nmVertex *v) const { return ((v == v1()) ? (v3()) : ((v == v2()) ? (v1()) : ((v == v3()) ? (v2()) : (NULL)))); }
		inline nmEdge *commonEdge(const nmTriangle *b) const { return (b->hasEdge(e1)) ? (e1) : ((b->hasEdge(e2)) ? (e2) : ((b->hasEdge(e3)) ? (e3) : (NULL))); }
		inline nmVertex *commonVertex(const nmTriangle *b) const { return (b->hasVertex(v1())) ? (v1()) : ((b->hasVertex(v2())) ? (v2()) : ((b->hasVertex(v3())) ? (v3()) : (NULL))); }

		inline void replaceEdge(nmEdge *o, nmEdge *n) { if (e1 == o) e1 = n; else if (e2 == o) e2 = n; else if (e3 == o) e3 = n; }
		inline void invert() { nmEdge *tmp = e2; e2 = e3; e3 = tmp; }

		nmEdge *getLongestEdge() const;
		nmEdge *getShortestEdge() const;
		bool isAcute() const;
		inline bool isNeedle() const { return (e1->isDegenerate() || e2->isDegenerate() || e3->isDegenerate()); }

		nmEdge *isCap() const;
		inline bool isDegenerate() const { return (isNeedle() || (isCap()!=NULL)); }
		bool intersects(const nmTriangle *b) const;

		Point getNormal() const;
		inline Point getCenter() const { return ((*v1()) + (*v2()) + (*v3())) / 3; }
		inline double getAngle(const nmVertex *v) const { nmEdge *e = oppositeEdge(v); return (e != NULL) ? (v->getAngle(e->v1, e->v2)) : (-1.0); }
		double getDAngle(const nmTriangle *t) const;
		inline Point getVector() const { nmVertex *vb = v2(); return (((*v1()) - (*vb))&((*vb) - (*v3()))); }
		inline double area() const { return sqrt(TMESH_TO_DOUBLE(squaredArea())); }

		inline coord squaredArea() const { return Point::squaredTriangleArea3D(*v1(), *v2(), *v3()); }
		coord squaredHeight() const { nmEdge *le = getLongestEdge(); return oppositeVertex(le)->squaredDistanceFromLine(le->v1, le->v2); }

		inline void printTriangle(FILE *f = stdout) const { v1()->printPoint(f); v2()->printPoint(f); v3()->printPoint(f); }

		// Static functions to be used as predicates in std algorithms
		static inline bool isUnlinkedPtr(const nmTriangle *t) { return (!t->isLinked()); }
		static bool edgePtrSortFunc(const nmTriangle *a, const nmTriangle *b); //! Sort based on endpoints pointers
		static bool edgeInfoSortFunc(const nmTriangle *a, const nmTriangle *b); //! Sort based on endpoints pointers
	};

	// Possibly non-manifold simplicial complex
	class nonManifoldPLC
	{
	public:
		std::vector<nmVertex *> V;
		std::vector<nmEdge *> E;
		std::vector<nmTriangle *> T;

		nonManifoldPLC() {};
		~nonManifoldPLC() { clear(); };

		//! Inits the mesh with a unit cube with diagonal (0,0,0)-(1,1,1)
		//! Possible previous elements are deleted
		void makeUnitCube();

		//! Transforms the mesh to a set of triangles that enclose its
		//! Minkowski sum with an axis-aligned cube.
		//! The resulting mesh is typically self-intersecting.
		void minkowskiWithCube(const coord& cube_size);

		//! Basic I/O functions
		bool loadOFF(const char *filename);
		bool saveOFF(const char *filename) const;

        bool loadSTL(const char *filename, const double &multiplier);
        bool loadOBJ(const char *filename, const double &multiplier);
        void toFloat(std::vector<cinolib::vec3d> &verts) const;

		//! Appends an existing mesh 'm' to this one and clears 'm'.
		void splice(nonManifoldPLC& m);

		//! Erase all the elements and release memory. Resulting mesh is empty.
		//! If 'keep_simplexes' is true, vertices, edges and triangles are not released.
		//! Use 'keep_simplexes' if same elements are referenced by other objects.
		void clear(bool keep_simplexes =false);

		////////////////////////////////////////////////////////////////////////
		//
		// Methods working on the abstract simplicial complex (connectivity only)
		//
		////////////////////////////////////////////////////////////////////////

		//! Returns TRUE if there are no isolated vertices or edges.
		bool isPureSimplicialComplex() const { return (std::none_of(V.begin(), V.end(), nmVertex::isIsolatedPtr) && std::none_of(E.begin(), E.end(), nmEdge::isIsolatedPtr)); }

		//! Returns TRUE if all the vertex STARs are topological disks
		bool isTwoManifold() const { return (std::all_of(V.begin(), V.end(), nmVertex::isTopologicalDiskPtr)); }

		//! Returns TRUE if all the edges have exactly two incident triangles
		bool isEdgeManifold() const { return (std::all_of(E.begin(), E.end(), nmEdge::isTopologicalDiskPtr)); }

		//! Returns TRUE if all the vertex STARs are either topological disks or half-disks
		bool isTwoManifoldWithBoundary() const { return (std::all_of(V.begin(), V.end(), nmVertex::isDiskOrHalfDiskPtr)); }

		//! Returns TRUE if all the edges have either one or two incident triangles
		bool isEdgeManifoldWithBoundary() const { return (std::all_of(E.begin(), E.end(), nmEdge::isDiskOrHalfDiskPtr)); }

		//! Returns TRUE if all the triangles are consistently oriented
		bool isConsistentlyOriented() const { return (std::all_of(E.begin(), E.end(), nmEdge::hasConsistentlyOrientedNeighborsPtr)); }

		//! Number of edge-connected components
		size_t getNumEdgeConnectedComponents() const;

		//! Number of vertex-connected components
		size_t getNumVertexConnectedComponents() const;

		//! Checks the complex connectivity for errors - exits if errors are found
		void checkConnectivity() const;

		//! Consistently orients all the triangles which are edge adjacent to 't0'. Returns false if component is not orientable.
		bool orientComponent(nmTriangle *t0);

		//! Consistently orients the whole simplicial complex. Returns false if not orientable.
		bool orient();

		//! Removes unlinked simplexes from the complex.
		void removeUnlinkedElements();


		////////////////////////////////////////////////////////////////////////
		//
		// Methods acting on the embedding (geometry only)
		//
		////////////////////////////////////////////////////////////////////////

		//! Translate the mesh by 't'
		void translate(const Point& t) { for (nmVertex *v : V) (*v) += t; }

		//! Uniformly scale the mesh by 's'
		void scale(const coord& s) { for (nmVertex *v : V) (*v) *= s; }

		//! Rotate mesh about 'axis' by 'angle' (in radians)
		void rotate(const Point& axis, const double& angle) { for (nmVertex *v : V) v->rotate(axis, angle); }

		////////////////////////////////////////////////////////////////////////
		//
		// Methods acting on both connectivity and geometry 
		//
		////////////////////////////////////////////////////////////////////////

		//! Splits 'e' at 'v'. Returns 'v'. 
		//! If 'copy_mask', sub-edges and triangles inherit the mask field from parents.
		//! Does not check geometry - everything is purely topological.
		nmVertex *splitEdgeByVertex(nmEdge *e, nmVertex *v, bool copy_mask = false, bool copy_info = false);

		//! Creates a new vertex 'v' at 'p' and splits 'e' at 'v'. Returns 'v'.
		//! If 'copy_mask', sub-edges and triangles inherit the mask field from parents.
		//! Does not check geometry - everything is purely topological.
		nmVertex *splitEdge(nmEdge *e, Point *p, bool copy_mask =false);

		//! Splits 't' at 'v'. Returns 'v'. 
		//! If 'copy_mask', sub-edges and triangles inherit the mask field from parents.
		//! Does not check geometry - everything is purely topological.
		nmVertex *splitTriangleByVertex(nmTriangle *t, nmVertex *v, bool copy_mask = true, bool copy_info = true);

		//! Creates a new vertex 'v' at 'p' and splits 't' at 'v'. Returns 'v'.
		//! If 'copy_mask', sub-edges and triangles inherit the mask field from parents.
		//! Does not check geometry - everything is purely topological.
		nmVertex *splitTriangle(nmTriangle *t, Point *p, bool copy_mask = true);

		//! Iteratively flips flat edges to make coplanar triangles form Delaunay triangulations.
		void delaunizeFlatAreas(bool use_sharpedges_as_constraints, bool verbose =false);

		//! Merges vertices with coincident coordinates to a single representative instance.
		//! Duplicated edges and triangles are also unified.
		void unifyCoincidentVertices();

		//! Removes exactly degenerate triangles
		void removeDegenerateTriangles();

		//! Selects all the triangles that intersect others at non-common subsimplexes
		int selectIntersectingTriangles();

		//! After having removed all the degenerate triangles, for each triangle 't' initializes its
		//! info pointer with a std::vector<int> containing the indices of all its intersecting triangles.
        void assignIntersectingTriangles();

        // moved from protected methods to here (by Giammi)
        void CreateTriangle(nmVertex *a, nmVertex *b, nmVertex *c);
        void mergeEdges();
        void mergeTriangles();

		protected:

        //void CreateTriangle(nmVertex *a, nmVertex *b, nmVertex *c);   //moved up
        //void mergeEdges();    //moved up
        //void mergeTriangles();    //moved up

		static bool delaunize_edgeToSwap(nmEdge *e);
		bool orientComponent_AssumeClearMasks(nmTriangle *t0);

		void markEdgeConnectedComponent(nmTriangle *t0) const;
		void markVertexConnectedComponent(nmTriangle *t0) const;

		void removeNeedleTriangles();

	private:
		mutable std::vector<unsigned char> _stored_tri_masks;

		void storeAndClearTriangleMasks() const;
		void restoreTriangleMasks() const;
	};
} //namespace T_MESH

#endif //_NONMANIFOLD_H

