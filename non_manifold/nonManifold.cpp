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

#include "nonManifold.h"
#include <list>
#include <queue>
#include <set>

namespace T_MESH
{
	/////////////////////////////////////////////////////////////////////////////
	//
	// V E R T I C E S
	//
	/////////////////////////////////////////////////////////////////////////////

	nmEdge *nmVertex::getEdge(const nmVertex *ov) const
	{
		for (nmEdge *e : ve) if (e->hasVertex(ov)) return e;
		return NULL;
	}

	void nmVertex::getVT(std::vector<nmTriangle *> *vt) const
	{
		size_t i, numt = 0;
		for (nmEdge *e : ve) for (nmTriangle *t : e->et) numt++;
		std::vector<char> masks(numt);
		i = 0; for (nmEdge *e : ve) for (nmTriangle *t : e->et) { masks[i]=t->mask; t->mask = 0; }

		vt->clear();
		for (nmEdge *e : ve) for (nmTriangle *t : e->et) if (!t->mask) { t->mask = 1; vt->push_back(t); }

		i = 0; for (nmEdge *e : ve) for (nmTriangle *t : e->et) { t->mask = masks[i]; }
	}

	bool nmVertex::isOnBoundary() const
	{
		return (std::any_of(ve.begin(), ve.end(), nmEdge::isOnBoundaryPtr));
	}

	bool nmVertex::isTopologicalDisk() const
	{
		nmEdge *e = ve.back();
		if (!e->isTopologicalDisk()) return false;
		nmTriangle *t = e->et.back();
		nmTriangle *t0 = t;

		for (size_t i=1; ; i++)
		{
			t = e->oppositeTriangle(t);
			if (t == NULL) return false;
			if (t == t0) return (i == ve.size());
			e = t->nextEdge(e);
			if (!e->hasVertex(this)) e = t->nextEdge(e);
		}
	}

	bool nmVertex::isTopologicalHalfDisk() const
	{
		for (nmEdge *e : ve) if (e->isOnBoundary())
		{
			nmTriangle *t = e->et.back();

			do
			{
				e = t->nextEdge(e);
				if (!e->hasVertex(this)) e = t->nextEdge(e);
				t = e->oppositeTriangle(t);
			} while (t != NULL);

			return (e->isOnBoundary());
		}

		return false;
	}


	bool nmVertex::isFlat() const
	{
		return (std::all_of(ve.begin(), ve.end(), nmEdge::isFlatPtr));
	}

	bool nmVertex::isDoubleFlat(nmEdge **e1, nmEdge **e2) const
	{
		int nne = 0;
		*e1 = *e2 = NULL;

		for (nmEdge *e : ve) if (!e->isFlat())
		{
			if (++nne > 2) return false;
			else if (nne == 1) *e1 = e;
			else *e2 = e;
		}

		if (nne == 0) return true; // This means that vertex is flat
		if (nne == 1) return false; // This should not be possible, but just in case...
		return (!((*e1)->oppositeVertex(this)->exactMisalignment(this, (*e2)->oppositeVertex(this))));
	}

	//// Unlinks the vertex if it is either Flat() or DoubleFlat(). On success, the function returns TRUE,
	//// the vertex neighborhood is retriangulated, and the geometric realization does not change.
	bool nmVertex::removeIfRedundant()
	{
		nmEdge *e, *e1, *e2;
		if (!isDoubleFlat(&e1, &e2)) return false;

		nmVertex *vo1, *vo2;

		std::vector<nmEdge *> tve(ve.size()-2);
		uint32_t i, j = 0;
		for (i = 0; i < ve.size(); i++) if (ve[i] != e1 && ve[i] != e2) tve[j++] = ve[i];

		j = 0;
		while (j<tve.size())
		{
			e = tve[j++];
			vo1 = e->et[0]->oppositeVertex(e);
			vo2 = e->et[1]->oppositeVertex(e);
			if (vo1->exactMisalignment(e->v1, vo2) && vo1->exactMisalignment(e->v2, vo2) && !e->v1->exactSameSideOnPlane(e->v2, vo1, vo2))
			{
				if (!e->swap()) return false;
				j = 0;
				std::remove(tve.begin(), tve.end(), e);
			}
		}

		if (e1 != NULL) e = e1; // means isDoubleFlat()
		else if (tve.size() == 3) e = tve[0];
		else
		{
			for (i = 0; i < tve.size(); i++)
			{
				e = tve[i];
				if (e->et[0]->oppositeVertex(e)->exactMisalignment(this, e->et[1]->oppositeVertex(e))) break;
			}
		}

		if (e->v1 == this) e->invert();
		e->collapseOnV1();
		return true;
	}


	void nmVertex::getIncidentTriangles(std::vector<nmTriangle *> *vt) const
	{
		std::vector<nmTriangle *> lvt;
		for (nmEdge *e : ve) lvt.insert(lvt.end(), e->et.begin(), e->et.end());
		size_t i, s = lvt.size();
		std::vector<unsigned char> old_masks(vt->size());
		for (i = 0; i < s; i++) old_masks[i] = lvt[i]->mask;
		for (i = 0; i < s; i++) lvt[i]->mask = 1;
		for (i = 0; i < s; i++) if (lvt[i]->mask) {
			vt->push_back(lvt[i]);
			lvt[i]->mask = 0;
		}
		for (i = 0; i < s; i++) lvt[i]->mask = old_masks[i];
	}

	Point nmVertex::getNormal() const
	{
		std::vector<nmTriangle *> vt;
		getIncidentTriangles(&vt);

		Point tnor;
		for (nmTriangle *t : vt) tnor += (t->getNormal() * t->getAngle(this));
		if (!tnor.isNull()) tnor.normalize();
				
		return tnor;
	}

	double nmVertex::totalAngle() const
	{
		std::vector<nmTriangle *> vt;
		getIncidentTriangles(&vt);
		double ta = 0.0;
		for (nmTriangle *t : vt) ta += t->getAngle(this);
		return ta;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	// E D G E S
	//
	/////////////////////////////////////////////////////////////////////////////

	bool nmEdge::vertexPtrSortFunc(const nmEdge *a, const nmEdge *b)
	{
		nmVertex *va1 = (a)->v1;
		nmVertex *va2 = (a)->v2;
		nmVertex *vb1 = (b)->v1;
		nmVertex *vb2 = (b)->v2;
		nmVertex *tmp;

		if (va2<va1) { tmp = va1; va1 = va2; va2 = tmp; }
		if (vb2<vb1) { tmp = vb1; vb1 = vb2; vb2 = tmp; }
		if (va1<vb1) return true;
		if (va1>vb1) return false;
		if (va2<vb2) return true;
		return false;
	}

	bool nmEdge::vertexInfoSortFunc(const nmEdge *a, const nmEdge *b)
	{
		void *va1 = (a)->v1->info;
		void *va2 = (a)->v2->info;
		void *vb1 = (b)->v1->info;
		void *vb2 = (b)->v2->info;
		void *tmp;

		if (va2<va1) { tmp = va1; va1 = va2; va2 = tmp; }
		if (vb2<vb1) { tmp = vb1; vb1 = vb2; vb2 = tmp; }
		if (va1<vb1) return true;
		if (va1>vb1) return false;
		if (va2<vb2) return true;
		return false;
	}

	double nmEdge::curvature() const { return (et.size() == 2) ? (et[0]->getDAngle(et[1])) : (-1.0); }

	bool nmEdge::swap(const bool fast)
	{
		if (et.size()!=2) return false;

		nmTriangle *t0 = et[0];
		nmTriangle *t1 = et[1];
		nmEdge *e01 = t0->nextEdge(this);
		nmEdge *e02 = t0->nextEdge(e01);
		nmEdge *e11 = t1->nextEdge(this);
		nmEdge *e12 = t1->nextEdge(e11);
		nmVertex *ov1 = e01->commonVertex(e02);
		nmVertex *ov2 = e11->commonVertex(e12);
		
		if (!fast && ov1->getEdge(ov2) != NULL) return false;

		v1->removeEdge(this); ov1->addEdge(this); replaceVertex(v1, ov1);
		v2->removeEdge(this); ov2->addEdge(this); replaceVertex(v2, ov2);
		
		if (e01->commonVertex(e11))
		{
			t0->replaceEdge(e01, e12); e01->removeTriangle(t0); e12->addTriangle(t0);
			t1->replaceEdge(e12, e01); e12->removeTriangle(t1); e01->addTriangle(t1);
		}
		else
		{
			t0->replaceEdge(e01, e11); e01->removeTriangle(t0); e11->addTriangle(t0);
			t1->replaceEdge(e11, e01); e11->removeTriangle(t1); e01->addTriangle(t1);
		}
		return true;
	}

	bool nmEdge::isFlat() const
	{
		return (et.size() == 2) ? ((et[0]->oppositeVertex(this)->exactOrientation(et[1]->v3(), et[1]->v2(), et[1]->v1()))==0) : (false); 
	}

	nmVertex *nmEdge::collapseOnV1()
	{
		// Remove this from both v1->VE and v2->VE
		v1->removeEdge(this);
		v2->removeEdge(this);

		// Replace v2 in v2->VE with v1
		for (nmEdge *e : v2->ve) e->replaceVertex(v2, v1);

		// v1->VE = v1->VE U v2->VE (splice ?) v2->VE = empty
		v1->ve.insert(v1->ve.end(), v2->ve.begin(), v2->ve.end());
		v2->unlink();

		// unlink triangles in et
		for (nmTriangle *t : et)
		{
			if (t->e1 != this) t->e1->removeTriangle(t);
			if (t->e2 != this) t->e2->removeTriangle(t);
			if (t->e3 != this) t->e3->removeTriangle(t);
			t->unlink();
		}
		et.clear();

		// unlink this
		nmVertex *ov1 = v1;
		unlink();

		// Remove isolated edges in v1->VE
		for (nmEdge *e : ov1->ve) if (e->isIsolated())
		{
			e->oppositeVertex(ov1)->removeEdge(e);
			e->unlink();
		}
		ov1->ve.erase(std::remove_if(ov1->ve.begin(), ov1->ve.end(), nmEdge::isUnlinkedPtr), ov1->ve.end());

		// Merge edges in v1->VE
		if (ov1->ve.size() > 1)
		{
			std::sort(ov1->ve.begin(), ov1->ve.end(), nmEdge::vertexPtrSortFunc);

			nmEdge *e, *last = ov1->ve[0];
			std::vector<nmEdge *> ces;

			for (size_t i = 1; i < ov1->ve.size(); i++)
			{
				e = ov1->ve[i];
				if (nmEdge::vertexPtrSortFunc(last, e)) last = e;
				else
				{
					ces.push_back(last);
					for (size_t j = 0; j < e->et.size(); j++)
					{
						e->et[j]->replaceEdge(e, last);
						last->et.push_back(e->et[j]);
					}
					e->et.clear();
					e->v1->removeEdge(e);
					e->v2->removeEdge(e);
					e->unlink();
					i--;
				}
			}

			for (nmEdge *e : ces)
			{
				if (e->et.size() > 1)
				{
					std::sort(e->et.begin(), e->et.end(), nmTriangle::edgePtrSortFunc);

					nmTriangle *t, *last = e->et[0];
					for (uint32_t i = 1; i < e->et.size(); i++)
					{
						t = e->et[i];
						if (nmTriangle::edgePtrSortFunc(last, t)) last = t;
						else
						{
							t->e1->removeTriangle(t);
							t->e2->removeTriangle(t);
							t->e3->removeTriangle(t);
							t->unlink();
							i--;
						}
					}
				}
			}

		}
		else if (ov1->ve.size())
		{
			nmEdge *e = ov1->ve[0];
			e->oppositeVertex(ov1)->removeEdge(e);
			e->unlink();
			ov1->ve.clear();
		}
		// Remove duplicated triangles in v1->VT

		return ov1;



		//nmTriangle *t, *ot;
		//nmEdge *e, *oe, *te;

		//for (unsigned int i = 0; i < et.size(); i++)
		//{
		//	t = et[i];
		//	e = t->nextEdge(this);
		//	oe = t->nextEdge(e);
		//	if (commonVertex(e) != v2) { te = e; e = oe; oe = te; }
		//	e->removeTriangle(t);
		//	oe->removeTriangle(t);
		//	t->unlink();
		//	for (unsigned int j = 0; j < e->et.size(); j++)
		//	{
		//		ot = e->et[j];
		//		ot->replaceEdge(e, oe);
		//		oe->addTriangle(ot);
		//	}
		//	if (oe->et.size() == 0)
		//	{
		//		oe->v1->removeEdge(oe);
		//		oe->v2->removeEdge(oe);
		//		oe->unlink();
		//	}
		//	e->v1->removeEdge(e);
		//	e->v2->removeEdge(e);
		//	e->unlink();
		//}

		//for (unsigned int i = 0; i < v2->ve.size(); i++)
		//{
		//	e = v2->ve[i];
		//	if (e != this)
		//	{
		//		e->replaceVertex(v2, v1);
		//		v1->addEdge(e);
		//	}
		//}

		//nmVertex *ov1 = v1;
		//v1->removeEdge(this);
		//v2->unlink();
		//unlink();

		//// There might exist a vertex v3 s.t. <v2,v3> and <v1,v3> do not form a triangle with this edge.
		//// In that case the collapse would create duplicated simplexes that must be removed.

		//// Local mergeEdges on v1->ve - keep track of edges that solved duplications (ces)
		//// For each edge in ces, need a local mergeTriangles() on e->et

		//if (ov1->ve.size() > 1)
		//{
		//	std::sort(ov1->ve.begin(), ov1->ve.end(), nmEdge::vertexPtrSortFunc);

		//	nmEdge *last = ov1->ve[0];
		//	std::vector<nmEdge *> ces;

		//	for (size_t i = 1; i < ov1->ve.size(); i++)
		//	{
		//		e = ov1->ve[i];
		//		if (nmEdge::vertexPtrSortFunc(last, e)) last = e;
		//		else
		//		{
		//			ces.push_back(last);
		//			for (size_t j = 0; j < e->et.size(); j++)
		//			{
		//				e->et[j]->replaceEdge(e, last);
		//				last->et.push_back(e->et[j]);
		//			}
		//			e->et.clear();
		//			e->v1->removeEdge(e);
		//			e->v2->removeEdge(e);
		//			e->unlink();
		//		}
		//	}

		//	for (nmEdge *e : ces)
		//	{
		//		std::sort(e->et.begin(), e->et.end(), nmTriangle::edgePtrSortFunc);

		//		nmTriangle *t, *last = e->et[0];
		//		for (uint32_t i = 1; i < e->et.size(); i++)
		//		{
		//			t = e->et[i];
		//			if (nmTriangle::edgePtrSortFunc(last, t)) last = t;
		//			else
		//			{
		//				t->e1->removeTriangle(t);
		//				t->e2->removeTriangle(t);
		//				t->e3->removeTriangle(t);
		//				t->unlink();
		//			}
		//		}
		//	}

		//}

		////////////////

		//return ov1;
	}

	Point nmEdge::toUnitVector() const
	{
		Point v = toVector();
		double l = v.length();

		if (l == 0) TMesh::error("Edge::toUnitVector : Degenerate Edge !\n");

		return v / l;
	}

	double nmEdge::dihedralAngle() const
	{
		if (et.size() != 2) return -1.0;
		Point nor1 = et[0]->getNormal();
		Point nor2 = et[1]->getNormal();
		if (nor1.isNull() || nor2.isNull()) return -1.0;
		double c = nor1.getAngle(&nor2);

		nmVertex *ov = et[1]->oppositeVertex(this);
		if (((*ov)*nor1) - ((*v1)*nor1) < 0) return -(c - M_PI);

		return c + M_PI;
	}

	bool nmEdge::hasConsistentlyOrientedNeighbors() const 
	{
		if (et.size() == 1) return true;
		if (et.size() != 2) return false;
		nmVertex *n1 = et[0]->nextVertex(v1);
		nmVertex *n2 = et[1]->nextVertex(v2);
		return ((n1 == v2 && n2 == v1) || (n1 != v2 && n2 != v1));
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	// T R I A N G L E S
	//
	/////////////////////////////////////////////////////////////////////////////

	bool nmTriangle::edgePtrSortFunc(const nmTriangle *a, const nmTriangle *b)
	{
		nmEdge *ea1 = (a)->e1, *ea2 = (a)->e2, *ea3 = (a)->e3;
		nmEdge *eb1 = (b)->e1, *eb2 = (b)->e2, *eb3 = (b)->e3;
		nmEdge *e11 = MIN(ea1, MIN(ea2, ea3));
		nmEdge *e13 = MAX(ea1, MAX(ea2, ea3));
		nmEdge *e12 = (e11 != ea1 && e13 != ea1) ? (ea1) : ((e11 != ea2 && e13 != ea2) ? (ea2) : (ea3));
		nmEdge *e21 = MIN(eb1, MIN(eb2, eb3));
		nmEdge *e23 = MAX(eb1, MAX(eb2, eb3));
		nmEdge *e22 = (e21 != eb1 && e23 != eb1) ? (eb1) : ((e21 != eb2 && e23 != eb2) ? (eb2) : (eb3));

		if (e11 < e21) return true;
		else if (e11>e21) return false;
		if (e12 < e22) return true;
		else if (e12>e22) return false;
		if (e13 < e23) return true;
		else return false;
	}

	bool nmTriangle::edgeInfoSortFunc(const nmTriangle *a, const nmTriangle *b)
	{
		void *ea1 = (a)->e1->info, *ea2 = (a)->e2->info, *ea3 = (a)->e3->info;
		void *eb1 = (b)->e1->info, *eb2 = (b)->e2->info, *eb3 = (b)->e3->info;
		void *e11 = MIN(ea1, MIN(ea2, ea3));
		void *e13 = MAX(ea1, MAX(ea2, ea3));
		void *e12 = (e11 != ea1 && e13 != ea1) ? (ea1) : ((e11 != ea2 && e13 != ea2) ? (ea2) : (ea3));
		void *e21 = MIN(eb1, MIN(eb2, eb3));
		void *e23 = MAX(eb1, MAX(eb2, eb3));
		void *e22 = (e21 != eb1 && e23 != eb1) ? (eb1) : ((e21 != eb2 && e23 != eb2) ? (eb2) : (eb3));

		if (e11 < e21) return true;
		else if (e11>e21) return false;
		if (e12 < e22) return true;
		else if (e12>e22) return false;
		if (e13 < e23) return true;
		else return false;
	}

	Point nmTriangle::getNormal() const
	{
		Point vd = getVector();
		double l = vd.length();
		return (l == 0) ? (Point(0, 0, 0)) : (vd / l);
	}

	double nmTriangle::getDAngle(const nmTriangle *t) const
	{
		Point thisNormal = getVector();
		Point otherNormal = t->getVector();

		if (thisNormal.isNull() || otherNormal.isNull()) return -1.0;

		return thisNormal.getAngle(otherNormal);
	}

	nmEdge *nmTriangle::getLongestEdge() const
	{
		coord l1 = e1->squaredLength();
		coord l2 = e2->squaredLength();
		coord l3 = e3->squaredLength();
		if (l1 >= l2 && l1 >= l3) return e1;
		if (l2 >= l1 && l2 >= l3) return e2;
		return e3;
	}

	nmEdge *nmTriangle::getShortestEdge() const
	{
		coord l1 = e1->squaredLength();
		coord l2 = e2->squaredLength();
		coord l3 = e3->squaredLength();
		if (l1 <= l2 && l1 <= l3) return e1;
		if (l2 <= l1 && l2 <= l3) return e2;
		return e3;
	}

	bool nmTriangle::isAcute() const
	{
		nmVertex *va = v1(), *vb = v2(), *vc = v3();
		if ((((*vb) - (*va)) * ((*vc) - (*va))) <= 0) return false;
		if ((((*va) - (*vb)) * ((*vc) - (*vb))) <= 0) return false;
		if ((((*vb) - (*vc)) * ((*va) - (*vc))) <= 0) return false;
		return true;
	}

	nmEdge *nmTriangle::isCap() const 
	{
		if (Point::pointInInnerSegment(v2(), e1->v1, e1->v2)) return e1;
		if (Point::pointInInnerSegment(v3(), e2->v1, e2->v2)) return e2;
		if (Point::pointInInnerSegment(v1(), e3->v1, e3->v2)) return e3;
		return NULL;
	}

	inline int orient2Dxy(const Point3c *A, const Point3c *B, const Point3c *C)
	{
		double a[2] = { A->x, A->y }, b[2] = { B->x, B->y }, c[2] = { C->x, C->y };
	    double det = TMesh::tri_orientation(a, b, c);
		return (det > 0) ? (1) : ((det < 0) ? (-1) : (0));
	}

	inline int orient2Dyz(const Point3c* A, const Point3c* B, const Point3c* C)
	{
		double a[2] = { A->y, A->z }, b[2] = { B->y, B->z }, c[2] = { C->y, C->z };
		double det = TMesh::tri_orientation(a, b, c);
		return (det > 0) ? (1) : ((det < 0) ? (-1) : (0));
	}

	inline int orient2Dzx(const Point3c* A, const Point3c* B, const Point3c* C)
	{
		double a[2] = { A->z, A->x }, b[2] = { B->z, B->x }, c[2] = { C->z, C->x };
		double det = TMesh::tri_orientation(a, b, c);
		return (det > 0) ? (1) : ((det < 0) ? (-1) : (0));
	}

	bool coplanarInnerSegmentsCross(const Point3c *A, const Point3c *B, const Point3c *P, const Point3c *Q)
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

	bool nmTriangle::intersects(const nmTriangle *t2) const
	{
		nmEdge *ce = commonEdge(t2);
		if (ce) // If they share an edge, intersection occurs only if t1 and t2 overlap
		{
			nmVertex *ov = t2->oppositeVertex(ce);
			return (ov->exactOrientation(v1(), v2(), v3()) == 0 && ov->exactSameSideOnPlane(oppositeVertex(ce), ce->v1, ce->v2));
		}

		nmVertex *cv = commonVertex(t2);
		nmVertex *v11, *v12, *v13, *v21, *v22, *v23;
		v11 = v1(); v12 = v2(); v13 = v3();
		v21 = t2->v1(); v22 = t2->v2(); v23 = t2->v3();
		if (cv) // If they share a vertex, intersection occurs if the opposite edge intersect the triangle
		{
			nmEdge *ee1 = oppositeEdge(cv), *ee2 = t2->oppositeEdge(cv);
			return (Point::segmentIntersectsTriangle(ee1->v1, ee1->v2, v21, v22, v23) ||
				Point::segmentIntersectsTriangle(ee2->v1, ee2->v2, v11, v12, v13));
		}

		// Calculate relative orientations
		char o11 = v11->exactOrientation(v21, v22, v23);
		char o12 = v12->exactOrientation(v21, v22, v23);
		char o13 = v13->exactOrientation(v21, v22, v23);
		if ((o11>0 && o12>0 && o13>0) || (o11<0 && o12<0 && o13<0)) return false; // t1 above/below t2

		if (o11 == 0 && o12 == 0 && o13 == 0) // t1 and t2 are coplanar
		{
			if (coplanarInnerSegmentsCross(v11, v12, v21, v22)) return true;
			if (coplanarInnerSegmentsCross(v11, v12, v22, v23)) return true;
			if (coplanarInnerSegmentsCross(v11, v12, v23, v21)) return true;
			if (coplanarInnerSegmentsCross(v12, v13, v21, v22)) return true;
			if (coplanarInnerSegmentsCross(v12, v13, v22, v23)) return true;
			if (coplanarInnerSegmentsCross(v12, v13, v23, v21)) return true;
			if (coplanarInnerSegmentsCross(v13, v11, v21, v22)) return true;
			if (coplanarInnerSegmentsCross(v13, v11, v22, v23)) return true;
			if (coplanarInnerSegmentsCross(v13, v11, v23, v21)) return true;
			return (
				Point::pointInTriangle(v11, v21, v22, v23) ||
				Point::pointInTriangle(v12, v21, v22, v23) ||
				Point::pointInTriangle(v13, v21, v22, v23) ||
				Point::pointInTriangle(v21, v11, v12, v13) ||
				Point::pointInTriangle(v22, v11, v12, v13) ||
				Point::pointInTriangle(v23, v11, v12, v13));
		} 

		char o21 = v21->exactOrientation(v11, v12, v13);
		char o22 = v22->exactOrientation(v11, v12, v13);
		char o23 = v23->exactOrientation(v11, v12, v13);
		if ((o21 > 0 && o22 > 0 && o23 > 0) || (o21 < 0 && o22 < 0 && o23 < 0)) return false; // t2 above/below t1

		return (
			Point::segmentIntersectsTriangle(v11, v12, v21, v22, v23, o11, o12) ||
			Point::segmentIntersectsTriangle(v12, v13, v21, v22, v23, o12, o13) ||
			Point::segmentIntersectsTriangle(v13, v11, v21, v22, v23, o13, o11) ||
			Point::segmentIntersectsTriangle(v21, v22, v11, v12, v13, o21, o22) ||
			Point::segmentIntersectsTriangle(v22, v23, v11, v12, v13, o22, o23) ||
			Point::segmentIntersectsTriangle(v23, v21, v11, v12, v13, o23, o21));
	}


	/////////////////////////////////////////////////////////////////////////////
	//
	// S I M P L I C I A L   C O M P L E X
	//
	/////////////////////////////////////////////////////////////////////////////


	void nonManifoldPLC::checkConnectivity() const
	{
		nmVertex *v;
		nmEdge *e;
		nmTriangle *t;
		uint32_t i, j;

		for (i = 0; i < V.size(); i++)
		{
			v = V[i];
			for (j = 0; j < v->ve.size(); j++)
				if (!v->ve[j]->hasVertex(v)) TMesh::error("Incident edge has not the vertex.\n");
			if (j == 0) TMesh::error("Empty ve relation (unlinked/isolated vertex).\n");
			if (v->ve.size() > 1)
			{
				std::vector<nmEdge *> tmpE(v->ve.begin(), v->ve.end());
				std::sort(tmpE.begin(), tmpE.end(), nmEdge::vertexPtrSortFunc);
				nmEdge *last = tmpE[0];
				for (uint32_t i = 1; i < tmpE.size(); i++)
				if (!nmEdge::vertexPtrSortFunc(last, tmpE[i])) TMesh::error("Duplicated edge in vertex VE.\n");
				else last = tmpE[i];
			}

		}

		for (i = 0; i < E.size(); i++) if ((e = E[i])->isLinked())
		{
			if (!e->v1->hasIncidentEdge(e)) TMesh::error("Missing incident edge at e->v1.\n");
			if (!e->v2->hasIncidentEdge(e)) TMesh::error("Missing incident edge at e->v2.\n");
			for (j = 0; j < e->et.size(); j++)
				if (!e->et[j]->hasEdge(e)) TMesh::error("Incident triangle has not the edge.\n");
			if (j == 0) TMesh::error("Empty et relation.\n");
			if (e->et.size() > 1)
			{
				std::vector<nmTriangle *> tmpT(e->et.begin(), e->et.end());
				std::sort(tmpT.begin(), tmpT.end(), nmTriangle::edgePtrSortFunc);
				nmTriangle *last = tmpT[0];
				for (uint32_t i = 1; i < tmpT.size(); i++)
				if (!nmTriangle::edgePtrSortFunc(last, tmpT[i])) TMesh::error("Duplicated triangle.\n");
				else last = tmpT[i];
			}

		}
		else TMesh::error("Unlinked edge.\n");

		for (i = 0; i < T.size(); i++) if ((t = T[i])->isLinked())
		{
			if (t->v1() == NULL) TMesh::error("NULL v1 at triangle.\n");
			if (t->v2() == NULL) TMesh::error("NULL v2 at triangle.\n");
			if (t->v3() == NULL) TMesh::error("NULL v3 at triangle.\n");
			if (!t->e1->hasIncidentTriangle(t)) TMesh::error("Missing incident triangle at t->e1.\n");
			if (!t->e2->hasIncidentTriangle(t)) TMesh::error("Missing incident triangle at t->e2.\n");
			if (!t->e3->hasIncidentTriangle(t)) TMesh::error("Missing incident triangle at t->e3.\n");
		}
		else TMesh::error("Unlinked triangle.\n");

		if (E.size() > 1)
		{
			std::vector<nmEdge *> tmpE(E.begin(), E.end());
			std::sort(tmpE.begin(), tmpE.end(), nmEdge::vertexPtrSortFunc);
			nmEdge *last = tmpE[0];
			for (uint32_t i = 1; i < tmpE.size(); i++)
			if (!nmEdge::vertexPtrSortFunc(last, tmpE[i])) TMesh::error("Duplicated edge.\n");
			else last = tmpE[i];
		}

		if (T.size() > 1)
		{
			std::vector<nmTriangle *> tmpT(T.begin(), T.end());
			std::sort(tmpT.begin(), tmpT.end(), nmTriangle::edgePtrSortFunc);
			nmTriangle *last = tmpT[0];
			for (uint32_t i = 1; i < tmpT.size(); i++)
			if (!nmTriangle::edgePtrSortFunc(last, tmpT[i])) TMesh::error("Duplicated triangle.\n");
			else last = tmpT[i];
		}

		TMesh::info("Check connectivty passed.\n");
	}


	void nonManifoldPLC::mergeEdges()
	{
		if (E.size() < 2) return;
		
		std::vector<void *> o_info;
		o_info.assign(V.size(), NULL);
		for (uint32_t i = 0; i<V.size(); i++) { o_info[i] = V[i]->info; V[i]->info = (void *)i; }
		
		std::sort(E.begin(), E.end(), nmEdge::vertexInfoSortFunc);

		nmEdge *e, *last = E[0];
		for (uint32_t i = 1; i < E.size(); i++)
		{
			e = E[i];
			if (nmEdge::vertexInfoSortFunc(last, e)) last = e;
			else
			{
				for (uint32_t j = 0; j < e->et.size(); j++)
				{
					e->et[j]->replaceEdge(e, last);
					last->et.push_back(e->et[j]);
				}
				e->et.clear();
				e->v1->removeEdge(e);
				e->v2->removeEdge(e);
			}
		}

		E.erase(std::remove_if(E.begin(), E.end(), nmEdge::isIsolatedPtr), E.end());

		for (uint32_t i = 0; i<V.size(); i++) V[i]->info = o_info[i];
	}

	void nonManifoldPLC::mergeTriangles()
	{
        #ifdef SORT_TRIS
        return;
        #endif

		if (T.size() < 2) return;

		std::vector<void *> o_info;
		o_info.assign(E.size(), NULL);
		for (uint32_t i = 0; i<E.size(); i++) { o_info[i] = E[i]->info; E[i]->info = (void *)i; }
		
		std::sort(T.begin(), T.end(), nmTriangle::edgeInfoSortFunc);

		nmTriangle *t, *last = T[0];
		for (uint32_t i = 1; i < T.size(); i++)
		{
			t = T[i];
			if (nmTriangle::edgeInfoSortFunc(last, t)) last = t;
			else
			{
				t->e1->removeTriangle(t);
				t->e2->removeTriangle(t);
				t->e3->removeTriangle(t);
				t->unlink();
			}
		}

		T.erase(std::remove_if(T.begin(), T.end(), nmTriangle::isUnlinkedPtr), T.end());

		for (uint32_t i = 0; i<E.size(); i++) E[i]->info = o_info[i];
	}

	void nonManifoldPLC::CreateTriangle(nmVertex *a, nmVertex *b, nmVertex *c)
	{
		nmEdge *e1 = new nmEdge(a, b);
		nmEdge *e2 = new nmEdge(b, c);
		nmEdge *e3 = new nmEdge(c, a);
		nmTriangle *t = new nmTriangle(e1, e2, e3);
		E.push_back(e1); E.push_back(e2); E.push_back(e3);
		T.push_back(t);
	}


	void nonManifoldPLC::makeUnitCube()
	{
		clear();
		const double crds[8][3] = { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 } };
		const int tris[12][3] = { { 6, 4, 2 }, { 4, 0, 2 }, { 5, 7, 1 }, { 7, 3, 1 }, { 7, 6, 3 }, { 6, 2, 3 }, { 4, 5, 0 }, { 5, 1, 0 }, { 6, 7, 4 }, { 7, 5, 4 }, { 0, 1, 2 }, { 1, 3, 2 } };
		for (int i = 0; i<8; i++) V.push_back(new nmVertex(crds[i][0], crds[i][1], crds[i][2]));
		for (int i = 0; i<12; i++) CreateTriangle(V[tris[i][0]], V[tris[i][1]], V[tris[i][2]]);
		mergeEdges();
	}

	nmVertex *nonManifoldPLC::splitEdgeByVertex(nmEdge *e, nmVertex *nv, bool copy_mask, bool copy_info)
	{
		nmVertex *v2 = e->v2;
		nmEdge *ne = nv->getEdge(v2);
		bool newedge = (ne == NULL);
		if (newedge) ne = new nmEdge(nv, v2);
		if (copy_mask) ne->mask = e->mask;

		for (uint32_t j = 0; j < e->et.size(); j++)
		{
			nmTriangle *t = e->et[j];
			nmVertex *o = t->oppositeVertex(e);
			nmEdge *net = nv->getEdge(o); if (!net) E.push_back(net = new nmEdge(nv, o));
			nmEdge *nexte = t->nextEdge(e);
			if (nexte->commonVertex(e) == e->v1) nexte = t->nextEdge(nexte);
			nmTriangle *nt = new nmTriangle(nexte, ne, net);
			if (copy_mask) nt->mask = t->mask;
			if (copy_info) nt->info = t->info;
			t->replaceEdge(nexte, net);
			nexte->removeTriangle(t);
			net->addTriangle(t);
			T.push_back(nt);
		}

		e->replaceVertex(v2, nv);
		v2->removeEdge(e);
		if (!nv->hasIncidentEdge(e)) nv->addEdge(e);
		if (newedge) E.push_back(ne);
		return nv;
	}

	nmVertex *nonManifoldPLC::splitEdge(nmEdge *e, Point *p, bool copy_mask)
	{
		nmVertex *nv = new nmVertex(p);
		V.push_back(nv);
		return splitEdgeByVertex(e, nv, copy_mask);
	}

	nmVertex *nonManifoldPLC::splitTriangleByVertex(nmTriangle *t, nmVertex *v, bool copy_mask, bool copy_info)
	{
		nmVertex *v1 = t->v1();
		nmVertex *v2 = t->v2();
		nmVertex *v3 = t->v3();

		nmEdge *ne1 = v->getEdge(v1); if (!ne1) E.push_back(ne1 = new nmEdge(v, v1));
		nmEdge *ne2 = v->getEdge(v2); if (!ne2) E.push_back(ne2 = new nmEdge(v, v2));
		nmEdge *ne3 = v->getEdge(v3); if (!ne3) E.push_back(ne3 = new nmEdge(v, v3));

		nmTriangle *nt1 = new nmTriangle(ne2, t->e3, ne3);
		nmTriangle *nt2 = new nmTriangle(ne3, t->e1, ne1);

		t->e3->removeTriangle(t);
		t->e1->removeTriangle(t);

		t->replaceEdge(t->e3, ne2);
		t->replaceEdge(t->e1, ne1);

		ne1->addTriangle(t);
		ne2->addTriangle(t);

		T.push_back(nt1);
		T.push_back(nt2);

		if (copy_mask)
		{
			nt1->mask = t->mask;
			nt2->mask = t->mask;
		}
		if (copy_info)
		{
			nt1->info = t->info;
			nt2->info = t->info;
		}

		return v;
	}

	nmVertex *nonManifoldPLC::splitTriangle(nmTriangle *t, Point *p, bool copy_mask)
	{
		nmVertex *nv = new nmVertex(p);
		V.push_back(nv);
		return splitTriangleByVertex(t, nv, copy_mask);
	}


	bool nonManifoldPLC::loadOFF(const char *fname)
	{
		FILE *fp;
		char s[256], line[1024];
		float x, y, z;
		int i, j, i1, i2, i3, i4, nv, nt, ne, triangulate = 0;
		int line_nr = 0;

		if ((fp = fopen(fname, "rb")) == NULL) return false;

		if (fgets(line, 1024, fp) != line) { TMesh::warning("\nloadOFF: Syntax error at line 0.\n"); goto close_file; }

		sscanf(line, "%255s", s);
		if (strcmp(s, "OFF") && strcmp(s, "COFF")) { TMesh::warning("\nloadOFF: Syntax error at line 0 (OFF expected).\n"); goto close_file; }

		do { 
			if (fgets(line, 1024, fp) != line) { TMesh::warning("\nloadOFF: Syntax error at line %d.\n", line_nr); goto close_file; }
			line_nr++;
		} while (line[0] == '#' || line[0] == '\0' || line[0] == '\n' || line[0] == '\r' || !sscanf(line, "%256s", s));
		if (sscanf(line, "%d %d %d", &nv, &nt, &ne) < 3) { TMesh::warning("\nloadOFF: Syntax error at line %d.\n", line_nr); goto close_file; }
		if (nv < 3) { TMesh::warning("\nloadOFF: Can't load objects with less than 3 vertices.\n"); goto close_file; }
		if (nt < 1) { TMesh::warning("\nloadOFF: Can't load objects with no faces.\n"); goto close_file; }

		for (i = 0; i<nv; i++) 
			if (fscanf(fp, "%f %f %f", &x, &y, &z) == 3) V.push_back(new nmVertex(x, y, z));
			else { TMesh::warning("\nloadOFF: Couldn't read coordinates for vertex # %d\n", i); goto clear_partial; }

		TMesh::begin_progress();
		for (i = 0; i<nt; i++)
		{
			if (fscanf(fp, "%d %d %d %d", &i4, &i1, &i2, &i3) == 4)
			{
                //if ((i % 1000) == 0) TMesh::report_progress("Loading ..%d%%", (i * 100) / nt);
				if (i1 < 0 || i2<0 || i3<0 || i4<3 || i1>(nv - 1) || i2>(nv - 1) || i3>(nv - 1)) { TMesh::warning("\nloadOFF: Invalid index at face %d!\n", i); goto break_progress; }
				for (j = 3; j <= i4; j++)
				{
					if (i1 == i2 || i2 == i3 || i3 == i1) TMesh::warning("\nloadOFF: Coincident indexes at triangle %d! Skipping.\n", i);
					else CreateTriangle(V[i1], V[i2], V[i3]);
					i2 = i3;
					if (j<i4)
					{
						if (fscanf(fp, "%d", &i3) != 1) { TMesh::warning("\nloadOFF: Couldn't read indexes for face # %d\n", i); goto break_progress; }
						else triangulate = 1;
					}
				}
			} else { TMesh::error("\nloadOFF: Couldn't read indexes for face # %d\n", i); goto break_progress; }
			fgets(line, 1024, fp); // Read all the remainder of the line
		}
		TMesh::end_progress();

		fclose(fp);
		mergeEdges();
		mergeTriangles();
		if (triangulate) TMesh::warning("Some polygonal faces needed to be triangulated.\n");
		return true;

break_progress:
		TMesh::end_progress();
clear_partial:
		clear();
close_file:
		fclose(fp);
		return false;
	}

	bool nonManifoldPLC::saveOFF(const char *filename) const
	{
		FILE *fp;
		uint32_t n;
		nmVertex *v;
		nmTriangle *t;

		if ((fp = fopen(filename, "w")) == NULL)
		{
			TMesh::warning("Can't open '%s' for output !\n", filename);
			return false;
		}

		fprintf(fp, "OFF\n");
		fprintf(fp, "%d %d 0\n", (int)V.size(), (int)T.size());

		void **oldinfo = new void *[V.size()];
		for (n = 0, v = V[n]; n < V.size(); n++, v = V[n]) fprintf(fp, "%f %f %f\n", TMESH_TO_FLOAT(v->x), TMESH_TO_FLOAT(v->y), TMESH_TO_FLOAT(v->z));
		for (n = 0, v = V[n]; n < V.size(); n++, v = V[n]) { oldinfo[n] = v->info; v->info = (void *)((int)n); }
        	
		for (n = 0, t = T[n]; n < T.size(); n++, t = T[n]) fprintf(fp, "3 %d %d %d\n", static_cast<int>((intptr_t)t->v1()->info), static_cast<int>((intptr_t)t->v2()->info), static_cast<int>((intptr_t)t->v3()->info));

		fclose(fp);
		for (n = 0, v = V[n]; n < V.size(); n++, v = V[n]) v->info = oldinfo[n];
		delete oldinfo;

        return 0;
    }

    bool nonManifoldPLC::loadSTL(const char *filename, const double &multiplier)
    {
        std::vector<cinolib::vec3d> verts;
        std::vector<unsigned int> tris;

        cinolib::read_STL(filename, verts, tris);

        for(auto &v : verts)
            V.push_back(new nmVertex(v.x() * multiplier, v.y() * multiplier, v.z() * multiplier));

        for(unsigned int t_id = 0; t_id < tris.size(); t_id += 3)
        {
            if(*V[tris[t_id]] != *V[tris[t_id+1]] &&
               *V[tris[t_id+1]] != *V[tris[t_id+2]] &&
               *V[tris[t_id]] != *V[tris[t_id+2]])
                CreateTriangle(V[tris[t_id]], V[tris[t_id+1]], V[tris[t_id+2]]);
        }

        mergeEdges();
        mergeTriangles();

        return true;
    }


    bool nonManifoldPLC::loadOBJ(const char *filename, const double &multiplier)
    {
        std::vector<cinolib::vec3d> verts;
        std::vector< std::vector<uint> > tmp_tris;

        cinolib::read_OBJ(filename, verts, tmp_tris);

        //toFloat(verts);

        for(auto &v : verts)
            V.push_back(new nmVertex(v.x() * multiplier, v.y() * multiplier, v.z() * multiplier));

        std::vector<uint> tris = cinolib::serialized_vids_from_polys(tmp_tris);

        for(unsigned int t_id = 0; t_id < tris.size(); t_id += 3)
        {
            if(*V[tris[t_id]] != *V[tris[t_id+1]] &&
               *V[tris[t_id+1]] != *V[tris[t_id+2]] &&
               *V[tris[t_id]] != *V[tris[t_id+2]])
                CreateTriangle(V[tris[t_id]], V[tris[t_id+1]], V[tris[t_id+2]]);
        }

        mergeEdges();
        mergeTriangles();

        return true;


    }


    void nonManifoldPLC::toFloat(std::vector<cinolib::vec3d> &verts) const
    {
        for(auto & v : verts)
        {
            v[0] = static_cast<double>(static_cast<float>(v[0]));
            v[1] = static_cast<double>(static_cast<float>(v[1]));
            v[2] = static_cast<double>(static_cast<float>(v[2]));
        }
    }

	void nonManifoldPLC::removeUnlinkedElements()
	{
		V.erase(std::remove_if(V.begin(), V.end(), nmVertex::isUnlinkedPtr), V.end());
		E.erase(std::remove_if(E.begin(), E.end(), nmEdge::isUnlinkedPtr), E.end());
		T.erase(std::remove_if(T.begin(), T.end(), nmTriangle::isUnlinkedPtr), T.end());
	}

	void nonManifoldPLC::splice(nonManifoldPLC& m)
	{
		V.insert(V.end(), m.V.begin(), m.V.end());
		E.insert(E.end(), m.E.begin(), m.E.end());
		T.insert(T.end(), m.T.begin(), m.T.end());
		m.clear(true);
	}

	void nonManifoldPLC::clear(bool keep_simplexes)
	{
		if (!keep_simplexes)
		{
			for (uint32_t i = 0; i < V.size(); i++) delete V[i];
			for (uint32_t i = 0; i < E.size(); i++) delete E[i];
			for (uint32_t i = 0; i < T.size(); i++) delete T[i];
		}
		V.clear(); E.clear(); T.clear();
	}

	// Internal function - assumes that all the triangles have zero mask
	bool nonManifoldPLC::orientComponent_AssumeClearMasks(nmTriangle *t0)
	{
		nmTriangle *t, *s;
		t0->mask = 1;
		std::vector<nmEdge *> front;
		front.push_back(t0->e1);
		front.push_back(t0->e2);
		front.push_back(t0->e3);

		while (!front.empty())
		{
			nmEdge *e = front.back();
			front.pop_back();
			if (e->et.size() > 2) return false;
			if (e->isOnBoundary()) continue;
			t = e->et[0]; if (!t->mask) t = e->et[1];
			s = e->oppositeTriangle(t);
			if (t->mask && s->mask)
			{
				if (!e->hasConsistentlyOrientedNeighbors()) return false;
			}
			else
			{
				if (!e->hasConsistentlyOrientedNeighbors()) s->invert();
				s->mask = 1;
				e = s->nextEdge(e); front.push_back(e);
				e = s->nextEdge(e); front.push_back(e);
			}
		}

		return true;
	}

	bool nonManifoldPLC::orientComponent(nmTriangle *t0)
	{
		storeAndClearTriangleMasks();
		bool ret = orientComponent_AssumeClearMasks(t0);
		restoreTriangleMasks();
		return ret;
	}

	bool nonManifoldPLC::orient()
	{
		storeAndClearTriangleMasks();
		bool ret = true;
		for (nmTriangle *t : T) if (!t->mask) ret &= orientComponent_AssumeClearMasks(t);
		restoreTriangleMasks();
		return ret;
	}


	void nonManifoldPLC::markEdgeConnectedComponent(nmTriangle *t0) const
	{
		nmEdge *e, *f;
		std::vector<nmEdge *> front(1, t0->e1);

		while (!front.empty())
		{
			e = front.back();
			front.pop_back();
			for (nmTriangle *t : e->et) if (!t->mask)
			{
				t->mask = 1;
				f = t->nextEdge(e); front.push_back(f);
				f = t->nextEdge(f); front.push_back(f);
			}
		}
	}

	void nonManifoldPLC::markVertexConnectedComponent(nmTriangle *t0) const
	{
		nmEdge *e, *f;
		std::vector<nmEdge *> front(1, t0->e1);

		while (!front.empty())
		{
			e = front.back();
			front.pop_back(); 
			std::vector<nmEdge *> tocheck;
			for (nmEdge *g : e->v1->ve) if (g != e) tocheck.push_back(g);
			for (nmEdge *g : e->v2->ve) if (g != e) tocheck.push_back(g);
			tocheck.push_back(e);
			for (nmEdge *g : tocheck) for (nmTriangle *t : g->et) if (!t->mask)
			{
				t->mask = 1;
				f = t->nextEdge(g); front.push_back(f);
				f = t->nextEdge(f); front.push_back(f);
			}
		}
	}

	size_t nonManifoldPLC::getNumEdgeConnectedComponents() const
	{
		storeAndClearTriangleMasks();
		size_t num_c = 0;		
		for (nmTriangle *t : T) if (!t->mask) { num_c++; markEdgeConnectedComponent(t); }
		restoreTriangleMasks();
		return num_c;
	}

	size_t nonManifoldPLC::getNumVertexConnectedComponents() const
	{
		storeAndClearTriangleMasks();
		size_t num_c = 0;
		for (nmTriangle *t : T) if (!t->mask) { num_c++; markVertexConnectedComponent(t); }
		restoreTriangleMasks();
		return num_c;
	}

	void nonManifoldPLC::unifyCoincidentVertices()
	{
		// First, collapse all needles
		for (nmEdge *e : E) if (e->isLinked() && (*e->v1 == *e->v2)) e->collapseOnV1();
		removeUnlinkedElements();

		if (V.empty()) return;
		std::sort(V.begin(), V.end(), nmVertex::lessThanPtr);
		nmVertex *cur, *last = V[0];
		for (size_t n = 1; n < V.size(); n++)
		{
			cur = V[n];
			if (*cur != *last) last = cur;
			else
			{
				for (nmEdge *e : cur->ve) {	e->replaceVertex(cur, last); last->addEdge(e); }
				cur->unlink();
			}
		}
		removeUnlinkedElements();
		mergeEdges();
		mergeTriangles();
	}


	void nonManifoldPLC::removeNeedleTriangles()
	{
		for (nmEdge *e : E) if (e->isLinked() && ((*e->v1) == (*e->v2))) e->collapseOnV1();
		removeUnlinkedElements();
	}

	void nonManifoldPLC::removeDegenerateTriangles()
	{
        struct cmp{
            bool operator()(nmTriangle *a, nmTriangle *b)
            {
                nmEdge *e1 = a->isCap(), *e2 = b->isCap();
                return (e1 != nullptr && e2 != nullptr && (*a->oppositeVertex(e1) < *b->oppositeVertex(e2)));
            }
        };

		// Collect all caps and sort them by split vertex
        //auto cmp = [](nmTriangle *a, nmTriangle *b) { nmEdge *e1 = a->isCap(), *e2 = b->isCap(); return (e1!=NULL && e2!=NULL && (*a->oppositeVertex(e1) < *b->oppositeVertex(e2))); };
        std::priority_queue<nmTriangle *, std::vector<nmTriangle *>, cmp> pq;
		for (nmTriangle *t : T) if (t->isCap()) pq.push(t);

		while (!pq.empty())
		{
			nmTriangle *t = pq.top();
			pq.pop();
			nmEdge *e = t->isCap();
			if (e!=NULL)
			{
				nmVertex *v = t->oppositeVertex(e);
				size_t prev_numt = T.size();
				splitEdge(e, v, true);
				for (nmTriangle *t : e->et) if (t->isCap()) pq.push(t);
				for (size_t i = prev_numt; i < T.size(); i++) if (T[i]->isCap()) pq.push(T[i]);
			}
		}

		removeNeedleTriangles();
	}

	void nonManifoldPLC::storeAndClearTriangleMasks() const
	{
		size_t n;
		nmTriangle *t;
		_stored_tri_masks.resize(T.size());
		for (n = 0, t = T[n]; n < T.size(); n++, t = T[n]) { _stored_tri_masks[n] = t->mask; t->mask = 0; }
	}

	void nonManifoldPLC::restoreTriangleMasks() const
	{
		size_t n;
		nmTriangle *t;
		for (n = 0, t = T[n]; n < T.size(); n++, t = T[n]) t->mask = _stored_tri_masks[n];
	}

} //namespace T_MESH
