#include "main.h"

void MoveParticles(const int nr_Particles, Particle const partikel, const float dt)
{

	// Abschw�chung als zus�tzlicher Abstand, um Singularit�t und Selbst-Interaktion zu vermeiden
	const float softening = 1e-20f;
	float* arrX = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float* arrY = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float* arrZ = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float Fx = .0f, Fy = .0f, Fz = .0f;
	const int max_num_threads = omp_get_max_threads();
	float dx = .0f, dy = .0f, dz = .0f;
	// Schleife �ber alle Partikel
#pragma omp simd aligned(arrX:32, arrY:32, arrZ:32)
	for (int i = 0; i < nr_Particles; i++) {
		// Schleife �ber die anderen Partikel die Kraft auf Partikel i aus�ben
		__assume_aligned(partikel.x, 32);
		__assume_aligned(partikel.y, 32);
		__assume_aligned(partikel.z, 32);
		for (int j = 0; j < nr_Particles; j++) {
			// Gravitationsgesetz
			// Berechne Abstand der Partikel i und j
			dx = partikel.x[j] - partikel.x[i];
			dy = partikel.y[j] - partikel.y[i];
			dz = partikel.z[j] - partikel.z[i];
			const float drSquared = dx * dx + dy * dy + dz * dz + softening;

			float drPower32 = sqrt(drSquared);
			drPower32 = drPower32 * drPower32 * drPower32;

			arrX[j] = dx / drPower32;
			arrY[j] = dy / drPower32;
			arrZ[j] = dz / drPower32;
		}
#pragma omp simd aligned(arrX:32, arrY:32, arrZ:32)
		for (int j = 0; j < nr_Particles; j++)
		{
			Fx += arrX[j];
			Fy += arrY[j];
			Fz += arrZ[j];
		}

		// Berechne �nderung der Geschwindigkeit des Partikel i durch einwirkende Kraft 
		partikel.vx[i] += dt * Fx;
		partikel.vy[i] += dt * Fy;
		partikel.vz[i] += dt * Fz;
		Fx = .0f;
		Fy = .0f;
		Fz = .0f;
	}

	// Bewege Partikel entsprechend der aktuellen Geschwindigkeit
	for (int i = 0; i < nr_Particles; i++) {
		partikel.x[i] += partikel.vx[i] * dt;
		partikel.y[i] += partikel.vy[i] * dt;
		partikel.z[i] += partikel.vz[i] * dt;
	}
}