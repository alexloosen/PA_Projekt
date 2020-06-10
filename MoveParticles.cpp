#include "main.h"

void MoveParticles(const int nr_Particles, Particle* const partikel, const float dt) {

	// Abschwächung als zusätzlicher Abstand, um Singularität und Selbst-Interaktion zu vermeiden
	const float softening = 1e-20f;

	float* arrX = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float* arrY = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	float* arrZ = (float*)_mm_malloc(sizeof(float) * nr_Particles, 32);
	// Schleife über alle Partikel
	for (int i = 0; i < nr_Particles; i++) {

		// Kraftkomponenten (x,y,z) der Kraft auf aktuellen Partikel (i) 
		float Fx = .0f, Fy = .0f, Fz = .0f;

		// Schleife über die anderen Partikel die Kraft auf Partikel i ausüben
		for (int j = 0; j < nr_Particles; j++) {
			// Gravitationsgesetz
			// Berechne Abstand der Partikel i und j
			const float dx = partikel[j].x - partikel[i].x;
			const float dy = partikel[j].y - partikel[i].y;
			const float dz = partikel[j].z - partikel[i].z;
			const float drSquared = dx * dx + dy * dy + dz * dz + softening;

			float drPower32 = sqrt(drSquared);
			drPower32 = drPower32 * drPower32 * drPower32;
			
			//drPower32 = pow(drSquared, 3.f / 2.f);
			arrX[j] = dx * 1.f / drPower32;
			arrY[j] = dy * 1.f / drPower32;
			arrZ[j] = dz * 1.f / drPower32;

		}

#pragma omp simd
		for (int j = 0; j < nr_Particles; j++) {
			// Addiere Kraftkomponenten zur Netto-Kraft
			Fx += arrX[j];
			Fy += arrY[j];
			Fz += arrZ[j];
		}
		// Berechne Änderung der Geschwindigkeit des Partikel i durch einwirkende Kraft 
		partikel[i].vx += dt * Fx;
		partikel[i].vy += dt * Fy;
		partikel[i].vz += dt * Fz;
	}

	// Bewege Partikel entsprechend der aktuellen Geschwindigkeit
	for (int i = 0; i < nr_Particles; i++) {
		partikel[i].x += partikel[i].vx * dt;
		partikel[i].y += partikel[i].vy * dt;
		partikel[i].z += partikel[i].vz * dt;
	}
}