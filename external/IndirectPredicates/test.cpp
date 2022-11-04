#include "implicit_point.h"

//#define TEST_PLAIN_PREDICATES

#ifdef TEST_PLAIN_PREDICATES
#include <vector>
#include <chrono>

extern "C" double orient2d(double* pa, double* pb, double* pc);
extern "C" double orient3d(double* pa, double* pb, double* pc, double* pd);

int orient2d(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y);
int orient3d(double px, double py, double pz, double qx, double qy, double qz, double rx, double ry, double rz, double sx, double sy, double sz);

inline double randomUnitDouble() { return ((double)rand()) / RAND_MAX; }

void testPlainPredicates2D()
{
	int num_all_triplets = 1000000;
	const double perc_degn = 0.05;
	const int num_random_triplets = num_all_triplets * (1.0 - perc_degn);
	const int num_degn_triplets = num_all_triplets * perc_degn;
	num_all_triplets = num_random_triplets + num_degn_triplets;

	// Create vector of all the point triplets
	double* atp = new double[(num_random_triplets + num_degn_triplets) * 6];

	// Create vector of random 2D point triplets
	double* random_triplets = atp;
	for (int i = 0; i < num_random_triplets * 6; i++) random_triplets[i] = randomUnitDouble();

	// Create vector of 2D point aligned triplets
	double* degn_triplets = atp + num_random_triplets * 6;
	for (int i = 0; i < num_degn_triplets; i++)
	{
		const double p0x = randomUnitDouble();
		const double p0y = randomUnitDouble();
		const double p1x = randomUnitDouble();
		const double p1y = randomUnitDouble();
		const double delta = randomUnitDouble();
		const double p2x = p1x + (p1x - p0x) * delta;
		const double p2y = p1y + (p1y - p0y) * delta;
		degn_triplets[i * 6] = p0x;
		degn_triplets[i * 6 + 1] = p0y;
		degn_triplets[i * 6 + 2] = p1x;
		degn_triplets[i * 6 + 3] = p1y;
		degn_triplets[i * 6 + 4] = p2x;
		degn_triplets[i * 6 + 5] = p2y;
	}

	std::chrono::time_point<std::chrono::system_clock> c_start;
	int dummy; // We sum the results to avoid too smart compiler optimizations

	// Calculate predicates on them
	dummy = 0;
	c_start = std::chrono::system_clock::now();
	for (int i = 0; i < num_all_triplets * 6; i += 6)
		dummy += orient2d(atp[i], atp[i + 1], atp[i + 2], atp[i + 3], atp[i + 4], atp[i + 5]);
	std::cout << "IPreds elapsed time: " << (std::chrono::system_clock::now() - c_start).count() / 10000000.0 << "s\n";
	std::cout << "Dummy sum: " << dummy << "\n";

	// Calculate predicates on them
	dummy = 0;
	c_start = std::chrono::system_clock::now();
	for (int i = 0; i < num_all_triplets * 6; i += 6)
	{
		const double r = orient2d(atp + i, atp + i + 2, atp + i + 4);
		dummy += ((r > 0) - (r < 0));
	}
	std::cout << "Shewchuk elapsed time: " << (std::chrono::system_clock::now() - c_start).count() / 10000000.0 << "s\n";
	std::cout << "Dummy sum: " << dummy << "\n";

}


void testPlainPredicates3D()
{
	int num_all_tets = 1000000;
	const double perc_degn = 0.05;
	const int num_random_tets = num_all_tets * (1.0 - perc_degn);
	const int num_degn_tets = num_all_tets * perc_degn;
	num_all_tets = num_random_tets + num_degn_tets;

	// Create vector of all the point tets
	double* atp = new double[(num_random_tets + num_degn_tets) * 12];

	// Create vector of random 3D point tets
	double* random_tets = atp;
	for (int i = 0; i < num_random_tets * 12; i++) random_tets[i] = randomUnitDouble();

	// Create vector of 3D point aligned tets
	double tet[12];
	double* degn_tets = atp + num_random_tets * 12;
	for (int i = 0; i < num_degn_tets; i++)
	{
		tet[0] = randomUnitDouble(); tet[1] = randomUnitDouble(); tet[2] = randomUnitDouble();
		tet[3] = randomUnitDouble(); tet[4] = randomUnitDouble(); tet[5] = randomUnitDouble();
		tet[6] = randomUnitDouble(); tet[7] = randomUnitDouble(); tet[8] = randomUnitDouble();
		const double delta = randomUnitDouble();
		tet[9] = tet[3] + (tet[3] - tet[0]) * delta;
		tet[10] = tet[4] + (tet[4] - tet[1]) * delta;
		tet[11] = tet[5] + (tet[5] - tet[2]) * delta;
		for (int j=0; j<12; j++) degn_tets[i * 12 + j] = tet[j];
	}

	std::chrono::time_point<std::chrono::system_clock> c_start;
	int dummy; // We sum the results to avoid too smart compiler optimizations

	// Calculate predicates on them
	dummy = 0;
	c_start = std::chrono::system_clock::now();
	for (int i = 0; i < num_all_tets * 12; i += 12)
		dummy += orient3d(atp[i], atp[i + 1], atp[i + 2], atp[i + 3], atp[i + 4], atp[i + 5], atp[i + 6], atp[i + 7], atp[i + 8], atp[i + 9], atp[i + 10], atp[i + 11]);
	std::cout << "IPreds elapsed time: " << (std::chrono::system_clock::now() - c_start).count() / 10000000.0 << "s\n";
	std::cout << "Dummy sum: " << dummy << "\n";

	// Calculate predicates on them
	dummy = 0;
	c_start = std::chrono::system_clock::now();
	for (int i = 0; i < num_all_tets * 12; i += 12)
	{
		const double r = orient3d(atp + i, atp + i + 3, atp + i + 6, atp + i + 9);
		dummy += ((r > 0) - (r < 0));
	}
	std::cout << "Shewchuk elapsed time: " << (std::chrono::system_clock::now() - c_start).count() / 10000000.0 << "s\n";
	std::cout << "Dummy sum: " << dummy << "\n";
}

int main(int argc, char* argv[])
{
	initFPU();

	testPlainPredicates3D();

	return 0;
}

#else

int main(int argc, char *argv[])
{
	initFPU();

	explicitPoint2D a(1, 1), b(3, 3), c(3, 1), d(1, 3);
	implicitPoint2D_SSI i(a, b, c, d);

	if (genericPoint::orient2D(a, i, b) == 0) std::cout << "Collinear\n";
	else std::cout << "Not collinear\n";
	
	return 0;
}
#endif