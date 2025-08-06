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
    // Configura la probabilidad de mutación.
    config->addValue("#miMutacion-probability", Constantes::DOUBLE);  // Probabilidad de mutación.

    this->param = *(config->load());  // Cargar parámetros de configuración.
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

    // Aplicar mutación solo a elementos mutables
    mutationProbability_ = this->param.get("#miMutacion-probability").getDouble();
    for (int i = 0; i < numMutables; i++) {
        if (rnd->nextDouble() <= mutationProbability_) {
            int nextIdx = (i + 1) % numMutables;
            std::swap(elementosMutables[i], elementosMutables[nextIdx]);
        }
    }

    // Reconstruir solución con elementos fijos y mutables
    j = 0; // Índice para elementos mutables
    for (int i = 0; i < y.getNumVariables(); i++) {
        bool esFijo = false;
        // Verificar si es posición fija
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

//void miMutacion::execute(Solution y) {
//    // Obtener instancia del generador de números aleatorios.
//    RandomNumber* rnd = rnd->getInstance();
//
//    // Contar la cantidad de 0 y -1 en la solución original.
//    int contadorCerosyMenosUnos = 0;
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        if (y.getVariableValue(i).L == 0 || y.getVariableValue(i).L == -1) {
//            contadorCerosyMenosUnos++;
//        }
//    }
//
//    // Crear un arreglo para guardar los índices y valores de 0 y -1.
//    int* indices = new int[contadorCerosyMenosUnos];
//    int* valores = new int[contadorCerosyMenosUnos];
//
//    int k = 0; // Índice para los arrays de índices y valores.
//
//    // Llenar los arrays de índices y valores.
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        if (y.getVariableValue(i).L == 0 || y.getVariableValue(i).L == -1) {
//            indices[k] = i;
//            valores[k] = y.getVariableValue(i).L;
//            k++;
//        }
//    }
//
//    // Crear un arreglo auxiliar sin los 0 y -1.
//    int* aux = new int[(y.getNumVariables()) - contadorCerosyMenosUnos];
//
//    int j = 0; // Índice para aux.
//
//    // Llenar el arreglo aux, excluyendo los 0 y -1.
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        if (y.getVariableValue(i).L != 0 && y.getVariableValue(i).L != -1) {
//            aux[j++] = y.getVariableValue(i).L; // Copiar el valor a aux.
//        }
//    }
//
//    // Obtener la probabilidad de mutación desde los parámetros.
//    mutationProbability_ = this->param.get("#miMutacion-probability").getDouble();
//
//    // Realizar la mutación en el arreglo aux.
//    for (int i = 0; i < j; i++) {
//        if (rnd->nextDouble() <= this->mutationProbability_) {
//            // Intercambiar valores en aux.
//            int actu = aux[i];
//            int next = aux[(i + 1) % j];
//            aux[i] = next;
//            aux[(i + 1) % j] = actu;
//        }
//    }
//
//    // Restaurar los valores de aux en la solución y, respetando los índices de 0 y -1.
//    j = 0; // Reiniciar el índice para aux.
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        // Verificar si el índice actual corresponde a un 0 o -1.
//        bool esCeroOMenosUno = false;
//        for (int l = 0; l < contadorCerosyMenosUnos; l++) {
//            if (indices[l] == i) {
//                y.setVariableValue(i, valores[l]); // Restaurar el valor original (0 o -1).
//                esCeroOMenosUno = true;
//                break;
//            }
//        }
//        if (!esCeroOMenosUno) {
//            y.setVariableValue(i, aux[j++]); // Restaurar el valor mutado.
//        }
//    }
//
//    // Liberar la memoria dinámica.
//    delete[] indices;
//    delete[] valores;
//    delete[] aux;
//}



//void miMutacion::execute(Solution y) {
//    // Obtener instancia del generador de números aleatorios.
//    RandomNumber* rnd = rnd->getInstance();
//
//    ;
//
//    // Vector para guardar los índices y valores de 0 y -1.
//    std::vector<std::pair<int, int>> indicesYValores;
//
//    // Contar la cantidad de 0 y -1 en la solución original.
//    int contadorCerosyMenosUnos = 0;
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        if (y.getVariableValue(i).L == 0 || y.getVariableValue(i).L == -1) {
//            contadorCerosyMenosUnos++;
//            // Guardar el índice y el valor en el vector.
//            indicesYValores.push_back(std::make_pair(i, y.getVariableValue(i).L));
//        }
//    }
//
//    // Crear un arreglo auxiliar sin los 0 y -1.
//    int* aux = new int[(y.getNumVariables()) - contadorCerosyMenosUnos];
//
//    int j = 0; // Índice para aux.
//
//    // Llenar el arreglo aux, excluyendo los 0 y -1.
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        if (y.getVariableValue(i).L != 0 && y.getVariableValue(i).L != -1) {
//            aux[j++] = y.getVariableValue(i).L; // Copiar el valor a aux.
//        }
//    }
//
//    // Obtener la probabilidad de mutación desde los parámetros.
//    mutationProbability_ = this->param.get("#miMutacion-probability").getDouble();
//
//    // Realizar la mutación en el arreglo aux.
//    for (int i = 0; i < j; i++) {
//        if (rnd->nextDouble() <= this->mutationProbability_) {
//            // Intercambiar valores en aux.
//            int actu = aux[i];
//            int next = aux[(i + 1) % j];
//            aux[i] = next;
//            aux[(i + 1) % j] = actu;
//        }
//    }
//
//    // Restaurar los valores de aux en la solución y, respetando los índices de 0 y -1.
//    j = 0; // Reiniciar el índice para aux.
//    for (int i = 0; i < y.getNumVariables(); i++) {
//        // Verificar si el índice actual corresponde a un 0 o -1.
//        bool esCeroOMenosUno = false;
//        for (const auto& par : indicesYValores) {
//            if (par.first == i) {
//                y.setVariableValue(i, par.second); // Restaurar el valor original (0 o -1).
//                esCeroOMenosUno = true;
//                break;
//            }
//        }
//        if (!esCeroOMenosUno) {
//            y.setVariableValue(i, aux[j++]); // Restaurar el valor mutado.
//        }
//    }
//
//    // Liberar la memoria dinámica.
//   /* delete[] aux;*/
//}
//





