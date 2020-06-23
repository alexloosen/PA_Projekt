#include "main.h"

void MoveParticles(const int nr_Particles, Particle const partikel, const float dt)
{

	// Abschwächung als zusätzlicher Abstand, um Singularität und Selbst-Interaktion zu vermeiden
	const float softening = 1e-20f;
	float Fx = .0f, Fy = .0f, Fz = .0f;
	float dx = .0f, dy = .0f, dz = .0f;
	// Schleife über alle Partikel
	for (int i = 0; i < nr_Particles; i++) {
		// Schleife über die anderen Partikel die Kraft auf Partikel i ausüben
#pragma omp simd
		for (int j = 0; j < nr_Particles; j++) {
			__assume_aligned(partikel.x, 32);
			__assume_aligned(partikel.y, 32);
			__assume_aligned(partikel.z, 32);
			// Gravitationsgesetz
			// Berechne Abstand der Partikel i und j
			dx = partikel.x[j] - partikel.x[i];
			dy = partikel.y[j] - partikel.y[i];
			dz = partikel.z[j] - partikel.z[i];
			const float drSquared = dx * dx + dy * dy + dz * dz + softening;

			float drPower32 = sqrt(drSquared);
			drPower32 = drPower32 * drPower32 * drPower32;

			Fx += dx / drPower32;
			Fy += dy / drPower32;
			Fz += dz / drPower32;
		}
		// Berechne Änderung der Geschwindigkeit des Partikel i durch einwirkende Kraft 
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