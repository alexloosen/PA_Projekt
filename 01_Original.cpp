#include "main.h"

void testRoutine(Particle ref, Particle moved, int nr_particles)
{
	int anzFehler = 0;

	float absDiff[6] = {.0f};
	float abw[6] = {.0f};

	for (int i = 0; i < nr_particles; i++) {
		absDiff[0] = fabsf(ref.x[i] - moved.x[i]);
		absDiff[1] = fabsf(ref.y[i] - moved.y[i]);
		absDiff[2] = fabsf(ref.z[i] - moved.z[i]);
		absDiff[3] = fabsf(ref.vx[i] - moved.vx[i]);
		absDiff[4] = fabsf(ref.vy[i] - moved.vy[i]);
		absDiff[5] = fabsf(ref.vz[i] - moved.vz[i]);

		abw[0] = fmaxf(0.001f, 0.001f * ref.x[i]);
		abw[1] = fmaxf(0.001f, 0.001f * ref.y[i]);
		abw[2] = fmaxf(0.001f, 0.001f * ref.z[i]);
		abw[3] = fmaxf(0.001f, 0.001f * ref.vx[i]);
		abw[4] = fmaxf(0.001f, 0.001f * ref.vy[i]);
		abw[5] = fmaxf(0.001f, 0.001f * ref.vz[i]);

		for (int j = 0; j < 6; j++) {
			if (absDiff[j] > abw[j]) {
				printf("Wert an Stelle %d,%d liegt ueber der Toleranz!\n", i, j);
				anzFehler++;
			}
		}
	}
	printf("Pruefroutine beendet!\nEs wurden %d zu grosse Abweichungen gefunden!\n", anzFehler);	
}

void initParticles(Particle const partikel, const int nr_Particles) {
	srand(0);
	for (int i = 0; i < nr_Particles; i++) {
		partikel.x[i] = float(rand()) / RAND_MAX;
		partikel.y[i] = float(rand()) / RAND_MAX;
		partikel.z[i] = float(rand()) / RAND_MAX;
		partikel.vx[i] = float(rand()) / RAND_MAX;
		partikel.vy[i] = float(rand()) / RAND_MAX;
		partikel.vz[i] = float(rand()) / RAND_MAX;
	}
}

void copyParticles(Particle const partikel_src, Particle const partikel_dst, const int nr_Particles) {
	for (int i = 0; i < nr_Particles; i++) {
		partikel_dst.x[i] = partikel_src.x[i];
		partikel_dst.y[i] = partikel_src.y[i];
		partikel_dst.z[i] = partikel_src.z[i];
		partikel_dst.vx[i] = partikel_src.vx[i];
		partikel_dst.vy[i] = partikel_src.vy[i];
		partikel_dst.vz[i] = partikel_src.vz[i];
	}
}

int main() {
	//Problemgröße und Anzahl und Größe der Zeitschritte definieren
	constexpr int nrOfParticles = 163840;
	constexpr int nrRuns = 50;  // Anzahl der Läufe und der Zeitschritte der Simulation
	constexpr int skipRuns = 30; // Anzahl der Messungen, die nicht in Mittelwert berücksichtigt werden
	constexpr float dt = 0.01f; // Länge eines Zeitschrittes

	Particle* partikel_start = new Particle(nrOfParticles);
	Particle* partikel = new Particle(nrOfParticles);
	copyParticles(*partikel_start, *partikel, nrOfParticles);

	// Initiaslisierung der Partikel mit Zufallswerten
	initParticles(*partikel_start, nrOfParticles);

	// Messen der Performance
	double runtimeStep[nrRuns] = { 0. }; // Sammlung der Laufzeiten der Steps
	double GFlopsStep[nrRuns] = { 0. };    // Sammlung der Leistungen der Steps
	double meanRuntime = 0.;
	double stdRuntime = 0.;
	double meanGFlops = 0.;
	double stdGFlops = 0.;

	// Berechnung der Anzahl an GFLOPs der Berechnung
	const float NrOfGFLOPs = 20.0 * 1e-9 * float(nrOfParticles) * float(nrOfParticles - 1);
	printf("#### Runtime Measurements Particle Simulation  ###\n");

	for (int run = 0; run < nrRuns; run++) {
		copyParticles(*partikel_start, *partikel, nrOfParticles);
		const double tStart = omp_get_wtime(); // Start der Zeitmessung
		MoveParticles(nrOfParticles, *partikel, dt); // Funktion, die optimiert werden soll
		const double tEnd = omp_get_wtime(); // Ende der Zeitmessung

		runtimeStep[run] = tEnd - tStart;
		GFlopsStep[run] = NrOfGFLOPs / runtimeStep[run];
		if (run >= skipRuns) { // Berechnung Mittelwerte
			meanRuntime += runtimeStep[run];
			meanGFlops += GFlopsStep[run];
		}

		printf("Run %d: Runtime: %f03,\t GFLOPS/s: %f01, \t %s\n",
			run, runtimeStep[run], GFlopsStep[run], (run < skipRuns ? "Not in Average" : ""));
		fflush(stdout); //Ausgabebuffer leeren
	}
	// Berechnung der Mittelwerte
	double nrRunsInStatistics = (double)(nrRuns - skipRuns);
	meanRuntime /= nrRunsInStatistics;
	meanGFlops /= nrRunsInStatistics;

	//Berechnung der Mittelwertfehler
	for (int i = skipRuns; i < nrRuns; i++)
	{
		stdRuntime += (runtimeStep[i] - meanRuntime) * (runtimeStep[i] - meanRuntime);
		stdGFlops += (GFlopsStep[i] - meanGFlops) * (GFlopsStep[i] - meanGFlops);
	}
	stdRuntime = sqrt(stdRuntime / (nrRunsInStatistics * (nrRunsInStatistics - 1)));
	stdGFlops = sqrt(stdGFlops / (nrRunsInStatistics * (nrRunsInStatistics - 1)));

	// Ausgabe der Ergebnisse
	printf("\n\n####### Average Performance #########\n");
	printf("Average Runtime: %f03 +- %f Seconds \n", meanRuntime, stdRuntime);
	printf("Average Performance: %f03 +- %f03 GFLOPS/s \n", meanGFlops, stdGFlops);
	printf("#####################################\n");

	Particle* partikelTest = new Particle(nrOfParticles);
	copyParticles(*partikel_start, *partikelTest, nrOfParticles);
	MoveParticles(nrOfParticles, *partikelTest, dt); // Funktion, die optimiert werden soll
	testRoutine(*partikelTest, *partikel, nrOfParticles);
	//float sum = .0f;
	//for (int i = 0; i < sizeof(partikel); i++) {
	//	sum += partikel->vx[i];
	//	sum += partikel->vy[i];
	//	sum += partikel->vz[i];
	//	sum += partikel->x[i];
	//	sum += partikel->y[i];
	//	sum += partikel->z[i];
	//}
	//sum /= 1208.87023903f;
	//printf("Sum: %f\n", sum);
	//delete[] partikel;
	delete partikel;
	system("PAUSE");
	return 0;
}