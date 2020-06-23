#include "main.h"

void MoveParticles(const int nr_Particles, Particle* const partikel, const float dt) {
	
	// Abschwächung als zusätzlicher Abstand, um Singularität und Selbst-Interaktion zu vermeiden
	const float softening = 1e-20f;
	float dx = .0f, dy = .0f, dz = .0f;
	// Schleife über alle Partikel
	for (int i = 0; i < nr_Particles; i++) {

		// Kraftkomponenten (x,y,z) der Kraft auf aktuellen Partikel (i) 
		float Fx = .0f, Fy = .0f, Fz = .0f;

		// Schleife über die anderen Partikel die Kraft auf Partikel i ausüben
#pragma omp simd
		for (int j = 0; j < nr_Particles; j++) {
			// Gravitationsgesetz
			// Berechne Abstand der Partikel i und j
			float dx = partikel[j].x - partikel[i].x;
			float dy = partikel[j].y - partikel[i].y;
			float dz = partikel[j].z - partikel[i].z;
			const float drSquared = dx * dx + dy * dy + dz * dz + softening;
			float drPower32 = sqrt(drSquared);
			drPower32 = drPower32 * drPower32 * drPower32;
			//float drPower32 = pow(drSquared, 3.0f / 2.0f);

			// Addiere Kraftkomponenten zur Netto-Kraft
			// What is more efficient? Bottleneck by one division and three multiplication, or three divisions?
			float drPower32divided = 1.0f / drPower32;
			
			Fx += dx * drPower32divided;
			Fy += dy * drPower32divided;
			Fz += dz * drPower32divided;
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