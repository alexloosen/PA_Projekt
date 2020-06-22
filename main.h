#pragma once

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <omp.h>

// Struct zur Beschreibung eines Teilchens
struct Particle {  
	float *x, *y, *z; // Koordinaten des Teilchens
	float *vx, *vy, *vz;  // Geschwindigkeiten des Teilchens
	Particle(int nrParticles)
	{
		x = (float*)_mm_malloc(sizeof(float) * nrParticles, 32);
		y = (float*)_mm_malloc(sizeof(float) * nrParticles, 32);
		z = (float*)_mm_malloc(sizeof(float) * nrParticles, 32);
		vx = (float*)_mm_malloc(sizeof(float) * nrParticles, 32);
		vy = (float*)_mm_malloc(sizeof(float) * nrParticles, 32);
		vz = (float*)_mm_malloc(sizeof(float) * nrParticles, 32);
	}
};

// Prototypen
void MoveParticles(const int nr_Particles, Particle const partikel, const float dt);