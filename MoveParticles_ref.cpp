#include "main.h"

void MoveParticles(const int nr_Particles, Particle* const partikel, const float dt) {

	// Schleife über alle Partikel
	for (int i = 0; i < nr_Particles; i++) {

		// Kraftkomponenten (x,y,z) der Kraft auf aktuellen Partikel (i) 
		float Fx = 0, Fy = 0, Fz = 0;

		// Schleife über die anderen Partikel die Kraft auf Partikel i ausüben
		for (int j = 0; j < nr_Particles; j++) {

			// Abschwächung als zusätzlicher Abstand, um Singularität und Selbst-Interaktion zu vermeiden
			const float softening = 1e-20;

			// Gravitationsgesetz
			// Berechne Abstand der Partikel i und j
			const float dx = partikel->x[j] - partikel->x[j];
			const float dy = partikel->y[j] - partikel->y[j];
			const float dz = partikel->z[j] - partikel->z[j];
			const float drSquared = dx * dx + dy * dy + dz * dz + softening;
			const float drPower32 = pow(drSquared, 3.0 / 2.0);

			// Addiere Kraftkomponenten zur Netto-Kraft
			Fx += dx / drPower32;
			Fy += dy / drPower32;
			Fz += dz / drPower32;
		}

		// Berechne Änderung der Geschwindigkeit des Partikel i durch einwirkende Kraft 
		partikel->vx[i] += dt * Fx;
		partikel->vy[i] += dt * Fy;
		partikel->vz[i] += dt * Fz;
	}

	// Bewege Partikel entsprechend der aktuellen Geschwindigkeit
	for (int i = 0; i < nr_Particles; i++) {
		partikel->x[i] += partikel->vx[i] * dt;
		partikel->y[i] += partikel->vy[i] * dt;
		partikel->z[i] += partikel->vz[i] * dt;
	}
}