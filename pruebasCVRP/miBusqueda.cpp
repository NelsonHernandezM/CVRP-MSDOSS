#include "miBusqueda.h"
#include <vector>
#include "miBusqueda.h"
 

#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include "../WindowsRequirements.h"
#include "utility"
#include <vector>
#include <cmath>
#include "miCEVRP.h"



  miBusqueda::miBusqueda() {


}

void miBusqueda::initialize(Requirements* config) {
	// Configura la probabilidad de mutación.
	config->addValue("#Numero-iteraciones", Constantes::INT);  // Probabilidad de mutación.

	this->param = *(config->load());  // Cargar parámetros de configuración.
}



Solution perturbacionSwaps(Solution solucion) {
	RandomNumber* rnd = RandomNumber::getInstance();
 
	Problem* base = solucion.getProblem();
	miCEVRP* problema = dynamic_cast<miCEVRP*>(base);

	// Identificar elementos fijos (0, -1 y estaciones >=22)
	std::vector<std::pair<int, int>> indicesYValores;
	int contadorNodosFijos = 0;
	for (int i = 0; i < solucion.getNumVariables(); i++) {
		int value = solucion.getVariableValue(i).L;
		if (problema->isDepot(value) || value == -1 || problema->isStation(value)) {
			contadorNodosFijos++;
			indicesYValores.push_back(std::make_pair(i, value));
		}
	}

	// Crear arreglo auxiliar solo con elementos mutables
	int numMutables = solucion.getNumVariables() - contadorNodosFijos;
	if (numMutables <= 1) return solucion;  // No se puede perturbar

	int* aux = new int[numMutables];
	int j = 0;
	for (int i = 0; i < solucion.getNumVariables(); i++) {
		int value = solucion.getVariableValue(i).L;


		if (!problema->isDepot(value) && value != -1 && !problema->isStation(value)) {
			aux[j++] = value;
		}
	}

	// Cálculo dinámico del número de perturbaciones
	int numPerturbaciones = 0;
	if (numMutables <= 5) {
		numPerturbaciones = 1;  // Casos muy pequeños
	}
	else if (numMutables <= 15) {
		numPerturbaciones = 2 + rnd->nextInt(1);  // 2-3 perturbaciones
	}
	else {
		// Para soluciones grandes: 10-15% de los elementos mutables
		numPerturbaciones = std::max(3, static_cast<int>(numMutables * 0.12));
		// Límite superior para no excederse
		numPerturbaciones = std::min(numPerturbaciones, 20);
	}

	// Tipo de perturbación (swap aleatorio o inversión)
	//int tipoPerturbacion = rnd->nextInt(1);


	// SWAPS ALEATORIOS
	for (int i = 0; i < numPerturbaciones; i++) {
		int idx1 = rnd->nextInt(numMutables - 1);
		int idx2;
		do {
			idx2 = rnd->nextInt(numMutables - 1);
		} while (idx1 == idx2);

		std::swap(aux[idx1], aux[idx2]);
	}



	// Reconstruir solución
	j = 0;
	for (int i = 0; i < solucion.getNumVariables(); i++) {
		bool esNodoFijo = false;
		for (const auto& par : indicesYValores) {
			if (par.first == i) {
				solucion.setVariableValue(i, par.second);
				esNodoFijo = true;
				break;
			}
		}
		if (!esNodoFijo) {
			solucion.setVariableValue(i, aux[j++]);
		}
	}

	delete[] aux;
 
	return solucion;
}




Solution perturbacionReversiones(Solution solucion) {
	RandomNumber* rnd = RandomNumber::getInstance();

	Problem* base = solucion.getProblem();
	miCEVRP* problema = dynamic_cast<miCEVRP*>(base);
	// Identificar elementos fijos (0, -1 y estaciones >=22)
	std::vector<std::pair<int, int>> indicesYValores;
	int contadorNodosFijos = 0;
	for (int i = 0; i < solucion.getNumVariables(); i++) {
		int value = solucion.getVariableValue(i).L;


		if (problema->isDepot(value) || value == -1 || problema->isStation(value)) {
			contadorNodosFijos++;
			indicesYValores.push_back(std::make_pair(i, value));
		}
	}

	// Crear arreglo auxiliar solo con elementos mutables
	int numMutables = solucion.getNumVariables() - contadorNodosFijos;
	if (numMutables <= 1) return solucion;  // No se puede perturbar

	int* aux = new int[numMutables];
	int j = 0;
	for (int i = 0; i < solucion.getNumVariables(); i++) {
		int value = solucion.getVariableValue(i).L;




		if (!problema->isDepot(value) && value != -1 && !problema->isStation(value)) {
			aux[j++] = value;
		}
	}

	// Cálculo dinámico del número de perturbaciones
	int numPerturbaciones = 0;
	if (numMutables <= 5) {
		numPerturbaciones = 1;  // Casos muy pequeños
	}
	else if (numMutables <= 15) {
		numPerturbaciones = 2 + rnd->nextInt(1);  // 2-3 perturbaciones
	}
	else {
		// Para soluciones grandes: 10-15% de los elementos mutables
		numPerturbaciones = std::max(3, static_cast<int>(numMutables * 0.12));
		// Límite superior para no excederse
		numPerturbaciones = std::min(numPerturbaciones, 20);
	}

	//// Tipo de perturbación (swap aleatorio o inversión)
	//int tipoPerturbacion = rnd->nextInt(1);


	// INVERSIONES
	for (int i = 0; i < numPerturbaciones; i++) {
		int inicio = rnd->nextInt(numMutables - 1);
		int fin;
		int intentos = 0;
		do {
			fin = rnd->nextInt(numMutables - 1);
			intentos++;
			// Prevenir loops infinitos en casos límite
			if (intentos > 10) {
				fin = (inicio + 1) % numMutables;
				break;
			}
		} while (abs(inicio - fin) < 2);  // Mínimo 3 elementos

		if (inicio > fin) std::swap(inicio, fin);

		// Realizar la inversión
		while (inicio < fin) {
			std::swap(aux[inicio], aux[fin]);
			inicio++;
			fin--;
		}
	}


	// Reconstruir solución
	j = 0;
	for (int i = 0; i < solucion.getNumVariables(); i++) {
		bool esNodoFijo = false;
		for (const auto& par : indicesYValores) {
			if (par.first == i) {
				solucion.setVariableValue(i, par.second);
				esNodoFijo = true;
				break;
			}
		}
		if (!esNodoFijo) {
			solucion.setVariableValue(i, aux[j++]);
		}
	}

	delete[] aux;
 

	return solucion;
}


Solution hillClimbingSwaps(Solution sol) {
	miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
	if (!problema) return sol;

	std::vector<int> indicesNodosMu;
	for (int i = 0; i < sol.getNumVariables(); i++) {
		int value = sol.getVariableValue(i).L;
		if (!problema->isDepot(value) && value != -1 && !problema->isStation(value)) {
			indicesNodosMu.push_back(i);
		}
	}

	if (indicesNodosMu.size() < 2) {
		// Nada que intercambiar
		return sol;
	}

	RandomNumber* rnd = RandomNumber::getInstance();
	const bool maximization = sol.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;

	const int total_posibles_swaps = (indicesNodosMu.size() * (indicesNodosMu.size() - 1)) / 2;
	const int max_swaps_por_iteracion = std::max(10, total_posibles_swaps / 10);

	const int max_iteraciones_sin_mejora = std::max(5, int(indicesNodosMu.size() / 5));

	const int limite_total_iteraciones = std::max(100, int(indicesNodosMu.size() * 5));


	int iteraciones_sin_mejora = 0;
	int total_iteraciones = 0;

	while (iteraciones_sin_mejora < max_iteraciones_sin_mejora && total_iteraciones < limite_total_iteraciones) {
		total_iteraciones++;
		bool mejora = false;
		std::vector<int> valores_originales;
		Interval objetivo_original = sol.getObjective(0);

		for (int i = 0; i < sol.getNumVariables(); i++) {
			valores_originales.push_back(sol.getVariableValue(i).L);
		}

		int intentos = 0;
		while (intentos < max_swaps_por_iteracion) {
			int indice1 = indicesNodosMu[rnd->nextInt(indicesNodosMu.size() - 1)];
			int indice2 = indicesNodosMu[rnd->nextInt(indicesNodosMu.size() - 1)];

			intentos++;  // Siempre incrementar el contador

			if (indice1 == indice2) continue;

			// Swap
			int temp = sol.getVariableValue(indice1).L;
			sol.setVariableValue(indice1, sol.getVariableValue(indice2).L);
			sol.setVariableValue(indice2, temp);

			sol.getProblem()->evaluate(&sol);
			sol.getProblem()->evaluateConstraints(&sol);

			if (sol.getNumberOfViolatedConstraints() == 0) {
				if ((maximization && sol.getObjective(0) > objetivo_original) ||
					(!maximization && sol.getObjective(0) < objetivo_original)) {
					mejora = true;
					break;  // Primera mejora encontrada
				}
			}

			// Revertir
			sol.setVariableValue(indice1, valores_originales[indice1]);
			sol.setVariableValue(indice2, valores_originales[indice2]);
		}

		if (mejora) {
			iteraciones_sin_mejora = 0;
		}
		else {
			iteraciones_sin_mejora++;

			for (int i = 0; i < sol.getNumVariables(); i++) {
				sol.setVariableValue(i, valores_originales[i]);
			}
			sol.getProblem()->evaluate(&sol);
			sol.getProblem()->evaluateConstraints(&sol);
		}
	}

	return sol;
}

 
void miBusqueda::execute(Solution y) {
	int numeroMAX_iteraciones = this->param.get("#Numero-iteraciones").getInt();

	y = hillClimbingSwaps(y);

	y.getProblem()->evaluate(&y);
	y.getProblem()->evaluateConstraints(&y);

	for (int i = 0; i < numeroMAX_iteraciones; i++) {

		std::vector<int> copiaValores;
		for (int j = 0; j < y.getNumVariables(); j++) {
			copiaValores.push_back(y.getVariableValue(j).L);
		}

		Interval objetivoOriginal = y.getObjective(0);
		int restriccionesVioladas = y.getNumberOfViolatedConstraints();

		bool maximization = y.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;
		bool mejor = false;

		y = perturbacionReversiones(y);
		y = hillClimbingSwaps(y);

		y.getProblem()->evaluate(&y);
		y.getProblem()->evaluateConstraints(&y);

		if (y.getNumberOfViolatedConstraints() == 0) {
			if ((maximization && y.getObjective(0) > objetivoOriginal) ||
				(!maximization && y.getObjective(0) < objetivoOriginal)) {
				mejor = true;
				//  Aquí va el mensaje de depuración
				std::wstring m2 = L"MEJORO--BUSQUEDA LOCAL: " L"";
				OutputDebugStringW(m2.c_str());

			}
		}

		if (!mejor) {
			for (int j = 0; j < y.getNumVariables(); j++) {
				y.setVariableValue(j, copiaValores[j]);
			}
			y.getProblem()->evaluate(&y);
			y.getProblem()->evaluateConstraints(&y);
		}
	}
}
