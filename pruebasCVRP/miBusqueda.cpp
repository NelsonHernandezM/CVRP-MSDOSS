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
	Solution copia(solucion.getProblem());

	// Copiar el array original
	for (int j = 0; j < copia.getNumVariables(); j++) {
		copia.setVariableValue(j, solucion.getVariableValue(j));
	}

	// Evaluar la solución original
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	Problem* base = copia.getProblem();
	miCEVRP* problema = dynamic_cast<miCEVRP*>(base);

	// Identificar elementos fijos (0, -1 y estaciones >=22)
	std::vector<std::pair<int, int>> indicesYValores;
	int contadorCiudadesFijas = 0;
	for (int i = 0; i < copia.getNumVariables(); i++) {
		int value = copia.getVariableValue(i).L;
		if (problema->isDepot(value) || value == -1 || problema->isStation(value)) {
			contadorCiudadesFijas++;
			indicesYValores.push_back(std::make_pair(i, value));
		}
	}

	// Crear arreglo auxiliar solo con elementos mutables
	int numMutables = copia.getNumVariables() - contadorCiudadesFijas;
	if (numMutables <= 1) return solucion;  // No se puede perturbar

	int* aux = new int[numMutables];
	int j = 0;
	for (int i = 0; i < copia.getNumVariables(); i++) {
		int value = copia.getVariableValue(i).L;

		
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
	int tipoPerturbacion = rnd->nextInt(1);


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
	for (int i = 0; i < copia.getNumVariables(); i++) {
		bool esCiudadFija = false;
		for (const auto& par : indicesYValores) {
			if (par.first == i) {
				copia.setVariableValue(i, par.second);
				esCiudadFija = true;
				break;
			}
		}
		if (!esCiudadFija) {
			copia.setVariableValue(i, aux[j++]);
		}
	}

	delete[] aux;

	// Evaluar la nueva solución
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	// Verificar si es mejor solución
	bool maximization = solucion.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;
	if (copia.getNumberOfViolatedConstraints() == 0) {
		if ((maximization && copia.getObjective(0) > solucion.getObjective(0)) ||
			(!maximization && copia.getObjective(0) < solucion.getObjective(0))) {
			return copia;
		}
	}

	return solucion;
}




Solution perturbacionReversiones(Solution solucion) {
	RandomNumber* rnd = RandomNumber::getInstance();
	Solution copia(solucion.getProblem());

	// Copiar el array original
	for (int j = 0; j < copia.getNumVariables(); j++) {
		copia.setVariableValue(j, solucion.getVariableValue(j));
	}

	// Evaluar la solución original
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	Problem* base = copia.getProblem();
	miCEVRP* problema = dynamic_cast<miCEVRP*>(base);
	// Identificar elementos fijos (0, -1 y estaciones >=22)
	std::vector<std::pair<int, int>> indicesYValores;
	int contadorCiudadesFijas = 0;
	for (int i = 0; i < copia.getNumVariables(); i++) {
		int value = copia.getVariableValue(i).L;


		if (problema->isDepot(value) || value == -1 || problema->isStation(value)) {
			contadorCiudadesFijas++;
			indicesYValores.push_back(std::make_pair(i, value));
		}
	}

	// Crear arreglo auxiliar solo con elementos mutables
	int numMutables = copia.getNumVariables() - contadorCiudadesFijas;
	if (numMutables <= 1) return solucion;  // No se puede perturbar

	int* aux = new int[numMutables];
	int j = 0;
	for (int i = 0; i < copia.getNumVariables(); i++) {
		int value = copia.getVariableValue(i).L;


	 

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
	int tipoPerturbacion = rnd->nextInt(1);


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
	for (int i = 0; i < copia.getNumVariables(); i++) {
		bool esCiudadFija = false;
		for (const auto& par : indicesYValores) {
			if (par.first == i) {
				copia.setVariableValue(i, par.second);
				esCiudadFija = true;
				break;
			}
		}
		if (!esCiudadFija) {
			copia.setVariableValue(i, aux[j++]);
		}
	}

	delete[] aux;

	// Evaluar la nueva solución
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	// Verificar si es mejor solución
	bool maximization = solucion.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;
	if (copia.getNumberOfViolatedConstraints() == 0) {
		if ((maximization && copia.getObjective(0) > solucion.getObjective(0)) ||
			(!maximization && copia.getObjective(0) < solucion.getObjective(0))) {
			return copia;
		}
	}

	return solucion;
}

 

void miBusqueda::execute(Solution y) {


	int numeroMAX_iteraciones = this->param.get("#Numero-iteraciones").getInt();


	y = perturbacionSwaps(y);


	for (int i = 0; i < numeroMAX_iteraciones; i++) {

		y = perturbacionReversiones(y);
		y = perturbacionSwaps(y);

	}


}



//void busquedaLocalIterada(SolutionSet* solucionesIniciales) {
//
//	int numeroMAX_iteraciones= this->param.get("#Numero-iteraciones").getInt();
//
//	for (int i = 0; i < numeroMAX_iteraciones; i++) {
//		
//
//
//		Solution nuevaSolucion = perturbacionPermutacionVRP(solucionesIniciales->get(i));
//
//
//		solucionesIniciales->set(i, nuevaSolucion);
//
//	}
//}