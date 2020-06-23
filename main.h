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
		x = (float*)_mm_malloc(sizeof(float) * nrParticles, (int)sizeof(float));
		y = (float*)_mm_malloc(sizeof(float) * nrParticles, (int)sizeof(float));
		z = (float*)_mm_malloc(sizeof(float) * nrParticles, (int)sizeof(float));
		vx = (float*)_mm_malloc(sizeof(float) * nrParticles, (int)sizeof(float));
		vy = (float*)_mm_malloc(sizeof(float) * nrParticles, (int)sizeof(float));
		vz = (float*)_mm_malloc(sizeof(float) * nrParticles, (int)sizeof(float));
	}
};

// Prototypen
void MoveParticles(const int nr_Particles, Particle const partikel, const float dt);