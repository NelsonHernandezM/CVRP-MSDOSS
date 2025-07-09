#include "miGenetico.h"
#include <string>
#include <algorithm>
#include "utility"
#include <vector>
#include "miCEVRP.h"

//454 busquedalocal
miGenetico::miGenetico() :Algorithm(NULL) {

}
 


void miGenetico::initialize() {

}


void miGenetico::initialize(Requirements* req) {

	req->addValue("#N", Constantes::INT);
	req->addValue("#MAX_GENERATIONS", Constantes::INT);
	req->addValue("#Problem-Instance", Constantes::STRING);
	//req->addValue("#OPTIMO", Constantes::INT);

	this->param_ = *(req->load());

	this->MAX_GENERATIONS = this->param_.get("#MAX_GENERATIONS").getInt();
	this->N = this->param_.get("#N").getInt();
	//this->OPTIMO = this->param_.get("#OPTIMO").getInt();


	this->problem_ = ProblemBuilder::execute(this->param_.get("#Problem-Instance").getString());

	this->mo = (MutationOperator*)MutationBuilder::execute(req);
	this->co = (CrossoverOperator*)CrossoverBuilder::execute(req);
	this->so = (SelectionOperator*)SelectionBuilder::execute(req);

	pob = new SolutionSet((2 * N), this->problem_);

}

void imprimirConjuntoSoluciones(SolutionSet* soluciones) {


	cout << "Soluciones" << endl;
	for (int i = 0; i < soluciones->size(); i++)
	{
		Solution sol = soluciones->get(i);
		cout << "[";
		for (int j = 0; j < sol.getNumVariables(); j++)
		{
			cout << sol.getVariableValue(j);
			if (j < sol.getNumVariables() - 1) {
				cout << ", ";
			}

		}
		cout << "]";

		cout << " [" << sol.getObjective(0) << "]";
		cout << " [" << sol.getResourceValue(0) << "]" << endl;

	}
	cout << " " << endl;


}

void imprimirSolucion(Solution sol) {


	 
		for (int j = 0; j < sol.getNumVariables(); j++)
		{
			cout << sol.getVariableValue(j);
			if (j < sol.getNumVariables() - 1) {
				cout << ", ";
			}

	

	}
	cout << " " << endl;


}




Solution perturbacionSegmento(Solution solucion) {
	// Crear una copia de la solución original
	Solution copia(solucion.getProblem());

	// Copiar el array original para no dañarlo
	for (int j = 0; j < copia.getNumVariables(); j++) {
		copia.setVariableValue(j, solucion.getVariableValue(j));
	}

	// Evaluar la solución original antes de las perturbaciones
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	// Definir el tamaño del segmento a perturbar
	int numVariables = copia.getNumVariables();
	int inicioSegmento = rand() % numVariables;  // Selecciona un punto de inicio aleatorio
	int longitudSegmento = rand() % (numVariables - inicioSegmento);  // Longitud del segmento aleatoria

	// Perturbar el segmento
	for (int i = inicioSegmento; i < inicioSegmento + longitudSegmento; i++) {
		Interval valorOriginal = copia.getVariableValue(i);

		// Cambiar el valor de 0 a 1 (o de 1 a 0 si es el caso)
		copia.setVariableValue(i, valorOriginal == 0 ? 1 : 0);
	}

	// Evaluar la nueva solución tras el cambio en el segmento
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	// Si la nueva solución no viola restricciones y es mejor, devolverla
	if (copia.getNumberOfViolatedConstraints() == 0 && copia.getResourceValue(0) > solucion.getResourceValue(0)) {
		return copia;
	}

	// Si la nueva solución viola restricciones o no mejora, devolver la solución original
	return solucion;
}



Solution perturbacion(Solution solucion) {
	//cout << "PERTURBACION" << endl;

	Solution copia(solucion.getProblem());

	// Copiar el array original para no dañarlo
	for (int j = 0; j < copia.getNumVariables(); j++) {
		copia.setVariableValue(j, solucion.getVariableValue(j));
	}

	// Evaluar la solución original antes de las perturbaciones
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	for (int i = 0; i < copia.getNumVariables(); i++) {
		// Guardar el valor original para revertir si es necesario
		Interval valorOriginal = copia.getVariableValue(i);

		// Cambiar el valor de 0 a 1 (o de 1 a 0 si es el caso)
		copia.setVariableValue(i, valorOriginal == 0 ? 1 : 0);

		// Evaluar la nueva solución tras el cambio
		copia.getProblem()->evaluate(&copia);
		copia.getProblem()->evaluateConstraints(&copia);

		if (copia.getNumberOfViolatedConstraints() > 0) {
			// Revertir si viola restricciones
			 
			copia.setVariableValue(i, valorOriginal);
		}
		else {
			// Si no viola restricciones, evaluar si la solución es mejor
			if (copia.getResourceValue(0) > solucion.getResourceValue(0)) {
				// Si la nueva solución es mejor, devolverla
				//cout << "ENCONTRO MEJOR SOLUCION" << endl;
				return copia;
			}
			else {
				// Revertir si no mejora
				copia.setVariableValue(i, valorOriginal);
			}
		}
	}

	// Si no se encuentra una mejor solución, devolver la solución original
	return solucion;
}

//
//
//Solution perturbacionPermutacionVRP(Solution solucion) {
//	
//	//imprimirSolucion(solucion);
//	//cout << "Solucion" << endl;
//	// Obtener una copia de la solución para trabajar con ella.
//	Solution copia(solucion.getProblem());
//
//	// Copiar el array original para no dañarlo.
//	for (int j = 0; j < copia.getNumVariables(); j++) {
//		copia.setVariableValue(j, solucion.getVariableValue(j));
//
//	}
//
//	// Evaluar la solución original antes de las perturbaciones.
//	copia.getProblem()->evaluate(&copia);
//	copia.getProblem()->evaluateConstraints(&copia);
//	if (copia.getNumberOfViolatedConstraints() < 0) {
//		cout << copia.getNumberOfViolatedConstraints();
//	}
// 
//	/*imprimirSolucion(copia);*/
//	/*cout << copia.getObjective(0) << " --- " << solucion.getObjective(0) << endl;*/
//
//	// Vector para almacenar los índices  que no se deben mover (por ejemplo, valor 0 o -1).
//	std::vector<std::pair<int, int>> indicesYValores;
//
//	// Contar 0 y -1 que no deben ser modificadas (por ejemplo, 0 y -1).
//	int contadorCiudadesFijas = 0;
//	for (int i = 0; i < copia.getNumVariables(); i++) {
//		if (copia.getVariableValue(i).L == 0 || copia.getVariableValue(i).L == -1  ) {
//			contadorCiudadesFijas++;
//			indicesYValores.push_back(std::make_pair(i, copia.getVariableValue(i).L)); // Guardar el índice y valor.
//		}
//	}
//
//	// Crear un arreglo auxiliar sin las deposito o final (0 o -1).
//	int* aux = new int[(copia.getNumVariables()) - contadorCiudadesFijas];
//	int j = 0; // Índice para aux.
//
//	// Llenar el arreglo aux con los clientes que pueden ser mutadas (sin los 0 y -1).
//	for (int i = 0; i < copia.getNumVariables(); i++) {
//		if (copia.getVariableValue(i).L != 0 && copia.getVariableValue(i).L != -1) {
//			aux[j++] = copia.getVariableValue(i).L;
//		}
//	}
//
//	// Realizar la mutación en el arreglo aux sin 0 y -1
//	for (int i = 0; i < j - 1; i++) {
//		int actu = aux[i];
//		int next = aux[i + 1];
//		aux[i] = next;
//		aux[i + 1] = actu;
//	}
//
//	// Restaurar los valores mutados en la solución, respetando los índices de las ciudades fijas.
//	j = 0; // Reiniciar el índice para aux.
//	for (int i = 0; i < copia.getNumVariables(); i++) {
//		bool esCiudadFija = false;
//		for (const auto& par : indicesYValores) {
//			if (par.first == i) {
//				copia.setVariableValue(i, par.second); // Restaurar el valor original (0 o -1).
//				esCiudadFija = true;
//				break;
//			}
//		}
//
//		// Si la ciudad no es fija, aplicar la mutación (restaurar el valor mutado).
//		if (!esCiudadFija) {
//			copia.setVariableValue(i, aux[j++]);
//		}
//	}
//
//	// Liberar la memoria dinámica.
//	delete[] aux;
//
//	// Evaluar la nueva solución tras la mutación.
//	copia.getProblem()->evaluate(&copia);
//	copia.getProblem()->evaluateConstraints(&copia);
//	
//
// 
// 
//
//
//	bool maximization = solucion.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;
//	if (maximization && copia.getNumberOfViolatedConstraints() == 0 && copia.getObjective(0) > solucion.getObjective(0))
//	{
//	 
//		return copia;
//	}
//	else if (!maximization && copia.getNumberOfViolatedConstraints() == 0 && copia.getObjective(0) < solucion.getObjective(0))
//	{
//		return copia;
//
//	 
//	}
//
//
//
// 
//
//
//
//
//
//
//	// Si no mejora, devolver la solución original.
//	return solucion;
//}

Solution perturbacionPermutacionVRP(Solution solucion) {
	RandomNumber* rnd = RandomNumber::getInstance();
	Solution copia(solucion.getProblem());

	// Copiar el array original
	for (int j = 0; j < copia.getNumVariables(); j++) {
		copia.setVariableValue(j, solucion.getVariableValue(j));
	}

	// Evaluar la solución original
	copia.getProblem()->evaluate(&copia);
	copia.getProblem()->evaluateConstraints(&copia);

	// Identificar elementos fijos (0, -1 y estaciones >=22)
	std::vector<std::pair<int, int>> indicesYValores;
	int contadorCiudadesFijas = 0;
	for (int i = 0; i < copia.getNumVariables(); i++) {
		int value = copia.getVariableValue(i).L;
		if (value == 0 || value == -1 || value >= 22) {
			contadorCiudadesFijas++;
			indicesYValores.push_back(std::make_pair(i, value));
		}
	}

	// Crear arreglo auxiliar solo con elementos mutables
	int numMutables = copia.getNumVariables() - contadorCiudadesFijas;
	int* aux = new int[numMutables];
	int j = 0;
	for (int i = 0; i < copia.getNumVariables(); i++) {
		int value = copia.getVariableValue(i).L;
		if (value != 0 && value != -1 && value < 22) {
			aux[j++] = value;
		}
	}

	// Aplicar mutación con probabilidad (similar al que funciona)
	//double mutationProbability = 1.0; // Puedes ajustar este valor
	for (int i = 0; i < numMutables - 1; i++) {
		/*if (rnd->nextDouble() <= mutationProbability) {*/
			std::swap(aux[i], aux[i + 1]);
	/*	}*/
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


void busquedaLocalIterada(SolutionSet* solucionesIniciales) {
	for (int i = 0; i < solucionesIniciales->size(); i++) {
		//Solution copia(solucionesIniciales->get(i).getProblem());

		//// Copiar el array original para no dañarlo
		//for (int j = 0; j < copia.getNumVariables(); j++) {

		//	copia.setVariableValue(j, solucionesIniciales->get(i).getVariableValue(j));
		//	

		//}

		// 

		//// Evaluar la copia de la solución original
		//copia.getProblem()->evaluate(&copia);
		//copia.getProblem()->evaluateConstraints(&copia);
		//
 

		
		Solution nuevaSolucion = perturbacionPermutacionVRP(solucionesIniciales->get(i));
		
 
		solucionesIniciales->set(i, nuevaSolucion);
 
	}
}


 


void miGenetico::execute() {
	int generacionesSinMejora = 0;
	int topeGeneracioneSnMejora = 100;
	/*
	cout << "inicio" << endl;*/

	int pobSize = 2 * this->N;
	int generation = 0;
	bool first = true;
	Solution nueva(this->problem_);

	best = new SolutionSet(1, 1, this->problem_);
	SolutionSet parents(2, 2, this->problem_);
	SolutionSet childs(2, 2, this->problem_);

	SolutionSet* hijosGenerados = new SolutionSet((2 * this->N), this->problem_);

	for (int i = 0; i < pobSize; i++)
	{
		nueva = this->problem_->generateRandomSolution();

		this->problem_->evaluate(&nueva);
		this->problem_->evaluateConstraints(&nueva);
		
		 

		this->pob->add(nueva);
			

		if (first) {
			best->set(0, nueva);
			first = false;
		}
		else {
			bool maximization = this->problem_->getObjectivesType()[0] == Constantes::MAXIMIZATION;
			if (maximization && nueva.getObjective(0) > best->get(0).getObjective(0))
			{
				this->best->set(0, nueva);
			}
			else if (!maximization && nueva.getObjective(0) < best->get(0).getObjective(0))
			{
				this->best->set(0, nueva);
			}

		}

	}
 


	while (generation < this->MAX_GENERATIONS) {
		hijosGenerados = new SolutionSet((2 * this->N), this->problem_);
		for (int i = 0; i < this->N; i++)
		{


			parents.set(0, this->so->execute(*pob));
			parents.set(1, this->so->execute(*pob));

			this->co->execute(parents, childs);
 

			this->mo->execute(childs.get(0));

		 

			this->mo->execute(childs.get(1));

			hijosGenerados->add(childs.get(0));
			hijosGenerados->add(childs.get(1));


		}
 

		busquedaLocalIterada(hijosGenerados);//hijos generados


		Interval mejorHastaAhora = this->best->get(0).getObjective(0);

		bool maximization = this->problem_->getObjectivesType()[0] == Constantes::MAXIMIZATION;
		for (int i = 0; i < hijosGenerados->size(); i++)
		{

			for (int i = 0; i < 2 * this->N; i++)
			{

			

				if (maximization && hijosGenerados->get(i).getObjective(0) > best->get(0).getObjective(0) &&
					hijosGenerados->get(i).getNumberOfViolatedConstraints() == 0) {
					best->set(0, hijosGenerados->get(i));
				 
				}
				else if (!maximization && hijosGenerados->get(i).getObjective(0) < best->get(0).getObjective(0) &&
					hijosGenerados->get(i).getNumberOfViolatedConstraints() == 0) {
					best->set(0, hijosGenerados->get(i));
					std::wstring m2 = L"MEJORSO: "    L"\n";
					OutputDebugStringW(m2.c_str());
			 
				}



			}
		}
		for (int j = 0; j < pobSize; j++)
		{
			for (int i = 0; i < 2 * this->N; i++)
			{
				if (maximization && hijosGenerados->get(j).getObjective(0) > pob->get(i).getObjective(0) &&
					hijosGenerados->get(j).getNumberOfViolatedConstraints() == 0) {
					pob->set(i, hijosGenerados->get(j));
				 
					break; 
				}
				else if (!maximization && hijosGenerados->get(j).getObjective(0) < pob->get(i).getObjective(0) &&
					hijosGenerados->get(j).getNumberOfViolatedConstraints() == 0) {
					pob->set(i, hijosGenerados->get(j));
					std::wstring m = L"MEJORO: "    L"\n";
					OutputDebugStringW(m.c_str());
					break;
				}

			}


		}

 



		++generation;


	}



	this->lastB_ = this->best;
	 

 



}

