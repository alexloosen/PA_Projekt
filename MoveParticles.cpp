#include "main.h"

void MoveParticles(const int nr_Particles, Particle const partikel, const float dt) {

	// Abschwächung als zusätzlicher Abstand, um Singularität und Selbst-Interaktion zu vermeiden
	const float softening = 1e-20f;
	
	float* arrX = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float* arrY = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float* arrZ = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	// Schleife über alle Partikel
	for (int i = 0; i < nr_Particles; i++) {

		// Kraftkomponenten (x,y,z) der Kraft auf aktuellen Partikel (i) 
		float Fx = .0f, Fy = .0f, Fz = .0f;
		__assume_aligned(partikel.x, 32);
		__assume_aligned(partikel.y, 32);
		__assume_aligned(partikel.z, 32);
		__assume_aligned(partikel.vx, 32);
		__assume_aligned(partikel.vy, 32);
		__assume_aligned(partikel.vz, 32);
#pragma omp simd aligned(arrX:32, arrY:32, arrZ:32)
		// Schleife über die anderen Partikel die Kraft auf Partikel i ausüben
		for (int j = 0; j < nr_Particles; j++) {
			// Gravitationsgesetz
			// Berechne Abstand der Partikel i und j
			const float dx = partikel.x[j] - partikel.x[i];
			const float dy = partikel.y[j] - partikel.y[i];
			const float dz = partikel.z[j] - partikel.z[i];
			const float drSquared = dx * dx + dy * dy + dz * dz + softening;

			float drPower32 = sqrt(drSquared);
			drPower32 = drPower32 * drPower32 * drPower32;
			
			//drPower32 = pow(drSquared, 3.f / 2.f);
			arrX[j] = dx * 1.f / drPower32;
			arrY[j] = dy * 1.f / drPower32;
			arrZ[j] = dz * 1.f / drPower32;

		}

		__assume_aligned(arrX, 32);
		__assume_aligned(arrY, 32);
		__assume_aligned(arrZ, 32);
#pragma omp simd
		for (int j = 0; j < nr_Particles; j++) {
			// Addiere Kraftkomponenten zur Netto-Kraft
			Fx += arrX[j];
			Fy += arrY[j];
			Fz += arrZ[j];
		}
		// Berechne Änderung der Geschwindigkeit des Partikel i durch einwirkende Kraft 
		partikel.vx[i] += dt * Fx;
		partikel.vy[i] += dt * Fy;
		partikel.vz[i] += dt * Fz;
	}

	// Bewege Partikel entsprechend der aktuellen Geschwindigkeit
	for (int i = 0; i < nr_Particles; i++) {
		partikel.x[i] += partikel.vx[i] * dt;
		partikel.y[i] += partikel.vy[i] * dt;
		partikel.z[i] += partikel.vz[i] * dt;
	}
}