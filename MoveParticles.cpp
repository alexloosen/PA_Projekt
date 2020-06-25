#include "main.h"

void MoveParticles(const int nr_Particles, Particle const partikel, const float dt)
{
	int tileSize = 2048;
	const int nPrime = nr_Particles - nr_Particles % tileSize;
	// Abschwächung als zusätzlicher Abstand, um Singularität und Selbst-Interaktion zu vermeiden
	const float softening = 1e-20f;
	//	float dx = .0f, dy = .0f, dz = .0f;
	int max_num_threads = omp_get_max_threads();
	omp_set_num_threads(8);
	// Schleife über alle Partikel
#pragma omp parallel for num_threads(max_num_threads) schedule(static)
	for (int ii = 0; ii < nr_Particles; ii += tileSize) {
		// Schleife über die anderen Partikel die Kraft auf Partikel i ausüben
		//#pragma omp parallel for simd reduction(+: Fx,Fy,Fz)
#pragma omp simd
		for (int j = 0; j < nr_Particles; j++) {
			float Fx = .0f, Fy = .0f, Fz = .0f;
			for (int i = ii; i < ii + tileSize; i++) {
				// Gravitationsgesetz
				// Berechne Abstand der Partikel i und j
				const float dx = partikel.x[j] - partikel.x[i];
				const float dy = partikel.y[j] - partikel.y[i];
				const float dz = partikel.z[j] - partikel.z[i];
				const float drSquared = dx * dx + dy * dy + dz * dz + softening;
				float drPower32 = sqrt(drSquared);
				drPower32 = drPower32 * drPower32 * drPower32;

				Fx += dx / drPower32;
				Fy += dy / drPower32;
				Fz += dz / drPower32;
				// Berechne Änderung der Geschwindigkeit des Partikel i durch einwirkende Kraft 
				partikel.vx[i] += dt * Fx;
				partikel.vy[i] += dt * Fy;
				partikel.vz[i] += dt * Fz;
			}
		}
	}
	// Bewege Partikel entsprechend der aktuellen Geschwindigkeit
	for (int i = 0; i < nr_Particles; i++) {
		partikel.x[i] += partikel.vx[i] * dt;
		partikel.y[i] += partikel.vy[i] * dt;
		partikel.z[i] += partikel.vz[i] * dt;
	}
}