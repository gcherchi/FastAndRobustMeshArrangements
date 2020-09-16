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
#include <iostream>
#include <chrono>
#include <ctime>

namespace T_MESH
{

	//////////////////////////////////////////////////////////////////////////////////
	//
	// B S P   TREE TO DETECT INTERSECTIONS
	//
	//////////////////////////////////////////////////////////////////////////////////

	struct generic_bbox { double mx, Mx, my, My, mz, Mz; }; // We use float in a conservative way

	class nmTriangleBB : public generic_bbox // Bounding box of a triangle
	{
	public:
		nmTriangle *t;

		nmTriangleBB(nmTriangle *t0) : t(t0)
		{
			nmVertex *v1 = t->v1(), *v2 = t->v2(), *v3 = t->v3();
			mx = TMESH_TO_LOWER_DOUBLE(MIN(v1->x, MIN(v2->x, v3->x)));
			Mx = TMESH_TO_UPPER_DOUBLE(MAX(v1->x, MAX(v2->x, v3->x)));
			my = TMESH_TO_LOWER_DOUBLE(MIN(v1->y, MIN(v2->y, v3->y)));
			My = TMESH_TO_UPPER_DOUBLE(MAX(v1->y, MAX(v2->y, v3->y)));
			mz = TMESH_TO_LOWER_DOUBLE(MIN(v1->z, MIN(v2->z, v3->z)));
			Mz = TMESH_TO_UPPER_DOUBLE(MAX(v1->z, MAX(v2->z, v3->z)));
		}

		inline bool intersects(generic_bbox *c) const
		{
			return (mx <= c->Mx && Mx >= c->mx && my <= c->My && My >= c->my && mz <= c->Mz && Mz >= c->mz);
		}

		inline bool operator<(const nmTriangleBB& b) const { return (mx < b.mx); }
		static inline bool lessThan(nmTriangleBB *a, nmTriangleBB *b) { return ((*a) < (*b)); }
		static inline bool isUnmaskedPtr(const nmTriangleBB* b) { return (!b->t->mask); }
	};


	class nmTriangleBB_Ints : public nmTriangleBB // Bounding box of a triangle
	{
	public:
		std::set<const nmTriangleBB_Ints *> its; // Triangles that intersect 't'
		std::vector<nmTriangle *> subtriangles;
		bool mask;

		nmTriangleBB_Ints(nmTriangle *t0) : nmTriangleBB(t0), mask(0) { }
		static bool isMasked(const nmTriangleBB_Ints* b) { return b->mask; }

		bool hasIntersection(const nmTriangleBB_Ints *b) const { return (its.find(b) != its.end()); }
		bool isCoplanarWith(const nmTriangleBB_Ints *b) const
		{
			nmVertex *v21 = b->t->v1(), *v22 = b->t->v2(), *v23 = b->t->v3();
			return (!(t->v1()->exactOrientation(v21, v22, v23) || t->v2()->exactOrientation(v21, v22, v23) || t->v3()->exactOrientation(v21, v22, v23)));
		}

		void removeCoplanarIts()
		{
			std::vector<const nmTriangleBB_Ints *> tmp(its.begin(), its.end());
			for (const nmTriangleBB_Ints *i : tmp) if (isCoplanarWith(i)) its.erase(i);
		}
	};

	template <class BB>
	class bspCell : public generic_bbox // Box-cell in an axis-aligned BSP pointing to the triangle bb's in it
	{
	public:
		std::list<BB *> triangles;

		bspCell() {}
		bspCell(std::vector<BB *> *tris)
		{
			mx = my = mz = DBL_MAX; Mx = My = Mz = -DBL_MAX;
			BB *t;
			for (unsigned int i = 0; i < tris->size(); i++)
			{
				t = tris->at(i);
				triangles.push_back(t);
				if (t->mx < mx) mx = t->mx;
				if (t->Mx > Mx) Mx = t->Mx;
				if (t->my < my) my = t->my;
				if (t->My > My) My = t->My;
				if (t->mz < mz) mz = t->mz;
				if (t->Mz > Mz) Mz = t->Mz;
			}
		}

		void sortTriangles() { triangles.sort(BB::lessThan); }

		bspCell *fork()
		{
			double ex = Mx - mx, ey = My - my, ez = Mz - mz;
			bspCell *nc = new bspCell;

			if (ex >= ey && ex >= ez)
			{
				nc->Mx = Mx - (ex * 0.5);	nc->My = My; nc->Mz = Mz;
				nc->mx = mx; nc->my = my; nc->mz = mz;
				mx = nc->Mx;
			}
			else if (ey >= ex && ey >= ez)
			{
				nc->Mx = Mx; nc->My = My - (ey * 0.5); nc->Mz = Mz;
				nc->mx = mx; nc->my = my; nc->mz = mz;
				my = nc->My;
			}
			else
			{
				nc->Mx = Mx; nc->My = My; nc->Mz = Mz - (ez * 0.5);
				nc->mx = mx; nc->my = my; nc->mz = mz;
				mz = nc->Mz;
			}

            typename std::list<BB *>::iterator cur, next;
			BB *t;

			for (cur = triangles.begin(); cur != triangles.end(); cur = next)
			{
				next = cur; next++;
				t = *cur;
				if (!t->intersects(this)) nc->triangles.splice(nc->triangles.end(), triangles, cur);
				else if (t->intersects(nc))	nc->triangles.push_back(t);
			}

			return nc;
		}


		void selectIntersections()
		{
			BB *t, *y;
			nmTriangle *t1, *t2;
            typename std::list<BB *>::iterator n, m;

			for (n = triangles.begin(); n != triangles.end(); n++)
			{
				t = *n;
				m = n; m++;
				for (; m != triangles.end(); m++)
				{
					y = *m;
					if (y->mx > t->Mx) break;   // Since boxes are sorted, this cuts some tests off

					t1 = t->t;
					t2 = y->t;
					if ((!t1->mask || !t2->mask) && t->intersects(y) && t1->intersects(t2)) { t1->mask = t2->mask = 1; }
				}
			}
		}

		// Fills the triangle BB vectors with other triangle BBs that intersect
		void assignIntersections()
		{
			BB *t, *y;
            typename std::list<BB *>::iterator n, m;
			
			for (n = triangles.begin(); n != triangles.end(); n++)
			{
				t = *n;
				m = n; m++;
				for (; m != triangles.end(); m++)
				{
					y = *m;
					if (y->mx > t->Mx) break;   // Since boxes are sorted, this cuts some tests off

					if (t->intersects(y))
					{
						if (t->t->intersects(y->t))
#pragma omp critical
						{
							t->its.insert(y);
							y->its.insert(t);
						}
					}
				}
			}
		}
	};

	template <class BB>	class nonManifoldBSP
	{
	public:
		std::vector<bspCell<BB> *> bsp;
		std::vector<BB *> nmTriangleBBs;

		nonManifoldBSP(nonManifoldPLC *tin)
		{
			for (nmTriangle *t : tin->T) nmTriangleBBs.push_back(new BB(t));

			bspCell<BB> *c2, *c = new bspCell<BB>(&nmTriangleBBs);

			auto cmp = [](bspCell<BB> *c1, bspCell<BB> *c2) { return (c1->triangles.size() < c2->triangles.size()); };
			std::priority_queue<bspCell<BB> *, std::vector<bspCell<BB> *>, decltype(cmp)> todo(cmp);

			int i = 0;

			const int max_num_cells = 10000;
			const int min_cell_tris = 10000;

			// Iteratively subdivide the BSP cell having most triangles
			todo.push(c);
			while (!todo.empty())
			{
				c = todo.top();
				todo.pop();
				if (i > max_num_cells || c->triangles.size() < min_cell_tris) bsp.push_back(c);
				else
				{
//					if (!(i % 1000)) TMesh::report_progress(NULL);
					i++;
					c2 = c->fork();
					todo.push(c);
					todo.push(c2);
				}
			}
            //printf("Using %zu cells out of a max of %d\n", bsp.size(), max_num_cells);
			for (size_t i = 0; i < bsp.size(); i++) bsp[i]->sortTriangles();
		}

		~nonManifoldBSP()
		{
			for (size_t i = 0; i < nmTriangleBBs.size(); i++) delete nmTriangleBBs[i];
			for (size_t i = 0; i < bsp.size(); i++) delete bsp[i];
		}


		void selectIntersections()
		{
#pragma omp parallel for TMESH_OMP_CLAUSES schedule(dynamic)
			for (int i = 0; i < (int)bsp.size(); i++)
			{
				bsp[i]->selectIntersections();
                //if (!(i % 100)) TMesh::report_progress("%d %% done   ", ((i)* 100) / bsp.size());
			}
		}

		void assignIntersections()
		{
#pragma omp parallel for TMESH_OMP_CLAUSES schedule(dynamic)
			for (int i = 0; i < (int)bsp.size(); i++)
			{
				bsp[i]->assignIntersections();
                //if (!(i % 10)) TMesh::report_progress("%d %% done   ", ((i)* 100) / bsp.size());
			}
		}
	};
	
	int nonManifoldPLC::selectIntersectingTriangles()
	{
		// Deselect everything and select only intersecting triangles
		for (nmTriangle *t : T) t->mask = 0;
//		TMesh::begin_progress();
		nonManifoldBSP<nmTriangleBB>(this).selectIntersections();
//		TMesh::end_progress();

		auto isselected = [](nmTriangle *t) { return t->mask; };
		return (int)std::count_if(T.begin(), T.end(), isselected);
	}


	typedef std::pair<nmVertex *, nmVertex *> cit_segment;

	int cit_saveVRML1(const char *filename, std::vector<cit_segment> *segments)
	{
		FILE *fp;
		unsigned int i;
		if ((fp = fopen(filename, "w")) == NULL) { TMesh::warning("Couldn't save graph!\n"); return 1; }

		fprintf(fp, "#VRML V1.0 ascii\nSeparator {\nMaterial { diffuseColor 1 0 0 }\nCoordinate3 {\npoint [\n");

		for (i = 0; i<segments->size(); i++)
		{
			segments->at(i).first->printPoint(fp);
			segments->at(i).second->printPoint(fp);
		}
		fprintf(fp, "]\n}\nLineSet {\nnumVertices [\n");
		for (i = 0; i<segments->size(); i++) fprintf(fp, "2,\n");
		fprintf(fp, "]\n}\n");

		fprintf(fp, "Material { diffuseColor 1 0 0 }\nDrawStyle { pointSize 4 }\n");
		fprintf(fp, "PointSet {}\n");

		fprintf(fp, "}\n");
		fclose(fp);

		return 0;
	}

	typedef std::vector<nmTriangle *> triStrip;

	nmTriangle *getOppositeFlatTriangle(nmEdge *e, nmTriangle *t)
	{
		nmVertex *ov = t->oppositeVertex(e);
		for (nmTriangle *y : e->et) 
		if (y != t && 
			y->oppositeVertex(e)->exactOrientation(e->v1, e->v2, ov) == 0 &&
			!y->oppositeVertex(e)->exactSameSideOnPlane(ov, e->v1, e->v2)
			) return y;
		return NULL;
	}

	void cit_insertSegment3D(cit_segment& s, nonManifoldPLC *tin)
	{
		nmVertex *v1 = s.first;
		nmVertex *v2 = s.second;
		nmEdge *e, *e0;

		std::vector<nmTriangle *> vt;
		v1->getVT(&vt);
		size_t onv = tin->V.size();
		for (nmTriangle *t : vt)
		{
			e = t->oppositeEdge(v1);
			if (Point::innerSegmentsCross(*v1, *v2, *e->v1, *e->v2))
			{
				// calc intersection
				Point it = Point::tmpExact_lineLineIntersection(*v1, *v2, *e->v1, *e->v2);

				// split e
				nmVertex *nv = tin->splitEdge(e, &it, true);

				// if e0 split e0 (with previously created vertex)
				e0 = v1->getEdge(v2);
				if (e0) tin->splitEdgeByVertex(e0, nv, true, true);
			}
		}

        for (size_t i = onv; i < tin->V.size(); i++)
        {
            cit_segment tmp = cit_segment(tin->V[i], v2);
            cit_insertSegment3D(tmp, tin);
        }
	}

	void printElapsedTime(const char *msg)
	{
		static std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		start = end;
		printf("%f seconds elapsed for %s\n", elapsed, msg);
	}

    void nonManifoldPLC::assignIntersectingTriangles()
	{
        unifyCoincidentVertices();
        removeDegenerateTriangles();

        //TMesh::begin_progress();
        nonManifoldBSP<nmTriangleBB_Ints> bsp(this);
		bsp.assignIntersections();

		// Now, bsp points to a vector of triangleBBs, each pointing to a triangle and to a set of triangleBBs of intersecting triangles
		std::vector<std::vector<uint32_t> *> intvecs(T.size());

		// Re-index triangles
		for (uint32_t i = 0; i < T.size(); i++) {
			T[i]->info = (void *)i; 
			intvecs[i] = new std::vector<uint32_t>;
		}

		for (const nmTriangleBB_Ints *b : bsp.nmTriangleBBs)
		{
            uint32_t thisTri = static_cast<uint32_t>((uintptr_t)b->t->info);
            for (const nmTriangleBB_Ints *it : b->its) intvecs[thisTri]->push_back(static_cast<uint32_t>((uintptr_t)it->t->info));
		}

		for (uint32_t i = 0; i < T.size(); i++) T[i]->info = (void *)intvecs[i];
    }

} //namespace T_MESH
