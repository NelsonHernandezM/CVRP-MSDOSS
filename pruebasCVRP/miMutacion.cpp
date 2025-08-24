#include "miMutacion.h" //  

#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include "../WindowsRequirements.h"
#include "utility"
#include <vector>
#include <cmath>
#include "miCEVRP.h"

void miMutacion::initialize(Requirements* config) {
    // Configura la probabilidad de mutaci�n.
    config->addValue("#miMutacion-probability", Constantes::DOUBLE);  // Probabilidad de mutaci�n.

    this->param = *(config->load());  // Cargar par�metros de configuraci�n.
}

void miMutacion::execute(Solution y) {
    RandomNumber* rnd = rnd->getInstance();

    Problem* base = y.getProblem();
    miCEVRP* problema = dynamic_cast<miCEVRP*>(base);
 

    // Contar elementos que NO deben mutarse (0, -1 y estaciones >=22)
    int contadorFijos = 0;
    for (int i = 0; i < y.getNumVariables(); i++) {
        int value = y.getVariableValue(i).L;
       
        if (value == -1 || problema->isDepot(value) || problema->isStation(value)) {
            contadorFijos++;
        }
    }

    // Crear arreglos para guardar posiciones y valores fijos
    int* indicesFijos = new int[contadorFijos];
    int* valoresFijos = new int[contadorFijos];
    int k = 0;

    // Llenar arreglos con elementos fijos
    for (int i = 0; i < y.getNumVariables(); i++) {
        int value = y.getVariableValue(i).L;
        if (problema->isDepot(value) || value == -1 || problema->isStation(value)) {
            indicesFijos[k] = i;
            valoresFijos[k] = value;
            k++;
        }
    }

    // Crear arreglo solo con elementos mutables (clientes normales)
    int numMutables = y.getNumVariables() - contadorFijos;
    int* elementosMutables = new int[numMutables];
    int j = 0;

    // Llenar arreglo con valores mutables
    for (int i = 0; i < y.getNumVariables(); i++) {
        int value = y.getVariableValue(i).L;
        if (value != 0 && value != -1 && value < 22) {
            elementosMutables[j++] = value;
        }
    }

    // Aplicar mutaci�n solo a elementos mutables
    mutationProbability_ = this->param.get("#miMutacion-probability").getDouble();
    for (int i = 0; i < numMutables; i++) {
        if (rnd->nextDouble() <= mutationProbability_) {
            int nextIdx = (i + 1) % numMutables;
            std::swap(elementosMutables[i], elementosMutables[nextIdx]);
        }
    }

    // Reconstruir soluci�n con elementos fijos y mutables
    j = 0; // �ndice para elementos mutables
    for (int i = 0; i < y.getNumVariables(); i++) {
        bool esFijo = false;
        // Verificar si es posici�n fija
        for (int idx = 0; idx < contadorFijos; idx++) {
            if (indicesFijos[idx] == i) {
                y.setVariableValue(i, valoresFijos[idx]);
                esFijo = true;
                break;
            }
        }
        // Si no es fijo, asignar valor mutable
        if (!esFijo) {
            y.setVariableValue(i, elementosMutables[j++]);
        }
    }

 
   /* y.getProblem()->evaluate(&y);
    y.getProblem()->evaluateConstraints(&y);*/
    // Liberar memoria
    delete[] indicesFijos;
    delete[] valoresFijos;
    delete[] elementosMutables;

}
