#include "miBusqueda.h"
#include "miCEVRP.h"
#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include <vector>
#include <cmath>
#include <utility> // Para std::swap
#include <numeric>
#include <algorithm>
#include <cassert>

// Clase principal
miBusqueda::miBusqueda() {
    // Constructor
}

void miBusqueda::initialize(Requirements* config) {
    config->addValue("#Numero-iteraciones", Constantes::INT);
    this->param = *config->load();
}

// Función auxiliar para obtener los índices de los nodos mutables y sus valores.
std::vector<int> obtenerValoresNodosMutables(Solution& solucion, miCEVRP* problema, std::vector<int>& indicesMutables) {
    indicesMutables.clear();
    std::vector<int> valoresMutables;

    indicesMutables.reserve(solucion.getNumVariables());
    valoresMutables.reserve(solucion.getNumVariables());

    for (int i = 0; i < solucion.getNumVariables(); ++i) {
        int value = solucion.getVariableValue(i).L;
        if (!problema->isDepot(value) && value != -1 && !problema->isStation(value)) {
            indicesMutables.push_back(i);
            valoresMutables.push_back(value);
        }
    }
    return valoresMutables;
}

// Función auxiliar para calcular el número de perturbaciones
int calcularNumPerturbaciones(size_t numMutables, RandomNumber* rnd) {
    if (numMutables <= 5) {
        return 1;
    }
    if (numMutables <= 15) {
        return 2 + rnd->nextInt(1);
    }
    int numPerturbaciones = std::max(3, static_cast<int>(numMutables * 0.12));
    return std::min(numPerturbaciones, 20);
}

// Reconstruir la solución a partir de un vector de valores mutables y los índices fijos.
void reconstruirSolucion(Solution& solucion, const std::vector<int>& valoresMutables, const std::vector<int>& indicesMutables) {
    for (size_t i = 0; i < indicesMutables.size(); ++i) {
        solucion.setVariableValue(indicesMutables[i], valoresMutables[i]);
    }
}

// Perturbación de Swaps, ahora sobre un vector de valores
void perturbacionSwaps(std::vector<int>& valoresMutables) {
    if (valoresMutables.size() < 2) return;

    RandomNumber* rnd = RandomNumber::getInstance();
    int numPerturbaciones = calcularNumPerturbaciones(valoresMutables.size(), rnd);

    for (int i = 0; i < numPerturbaciones; ++i) {
        int idx1 = rnd->nextInt(valoresMutables.size() - 1);
        int idx2;
        do {
            idx2 = rnd->nextInt(valoresMutables.size() - 1);
        } while (idx1 == idx2);

        std::swap(valoresMutables[idx1], valoresMutables[idx2]);
    }
}

// Perturbación de Reversiones, ahora sobre un vector de valores
void perturbacionReversiones(std::vector<int>& valoresMutables) {
    if (valoresMutables.size() < 2) return;

    RandomNumber* rnd = RandomNumber::getInstance();
    int numPerturbaciones = calcularNumPerturbaciones(valoresMutables.size(), rnd);

    for (int i = 0; i < numPerturbaciones; ++i) {
        int inicio = rnd->nextInt(valoresMutables.size() - 1);
        int fin;
        int intentos = 0;
        do {
            fin = rnd->nextInt(valoresMutables.size() - 1);
            if (intentos++ > 100) {
                fin = (inicio + 1) % valoresMutables.size();
                break;
            }
        } while (std::abs(inicio - fin) < 2);

        if (inicio > fin) std::swap(inicio, fin);

        std::reverse(valoresMutables.begin() + inicio, valoresMutables.begin() + fin + 1);
    }
}

// Hill Climbing, ahora sobre un vector de valores
bool hillClimbingSwaps(Solution& sol, std::vector<int>& valoresMutables, const std::vector<int>& indicesMutables) {
    if (valoresMutables.size() < 2) return false;

    RandomNumber* rnd = RandomNumber::getInstance();
    const bool maximization = sol.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;
    const size_t num_mutables = valoresMutables.size();

    const int max_swaps_por_iteracion = std::max(10, static_cast<int>(num_mutables * (num_mutables - 1) / 30));
    const int max_iteraciones_sin_mejora = std::max(5, static_cast<int>(num_mutables / 10));
    const int limite_total_iteraciones = std::max(100, static_cast<int>(num_mutables * 3));

    int iteraciones_sin_mejora = 0;
    int total_iteraciones = 0;
    bool mejora_global = false;

    // Evaluar la solución antes de empezar el hill climbing
    sol.getProblem()->evaluate(&sol);
    sol.getProblem()->evaluateConstraints(&sol);
    Interval objetivo_original = sol.getObjective(0);

    while (iteraciones_sin_mejora < max_iteraciones_sin_mejora && total_iteraciones < limite_total_iteraciones) {
        total_iteraciones++;
        bool mejora_local = false;

        // Creamos una copia del vector para los intentos del bucle interno
        std::vector<int> temp_valores = valoresMutables;

        for (int i = 0; i < max_swaps_por_iteracion; ++i) {
            int idx1 = rnd->nextInt(num_mutables - 1);
            int idx2;
            do {
                idx2 = rnd->nextInt(num_mutables - 1);
            } while (idx1 == idx2);

            std::swap(temp_valores[idx1], temp_valores[idx2]);

            // Reconstruir temporalmente la solución y evaluar
            reconstruirSolucion(sol, temp_valores, indicesMutables);
            sol.getProblem()->evaluate(&sol);
            sol.getProblem()->evaluateConstraints(&sol);

            if (sol.getNumberOfViolatedConstraints() == 0) {
                if ((maximization && sol.getObjective(0) > objetivo_original) ||
                    (!maximization && sol.getObjective(0) < objetivo_original)) {

                    valoresMutables = temp_valores; // Actualizamos el vector con la mejor versión
                    objetivo_original = sol.getObjective(0);
                    mejora_local = true;
                    mejora_global = true;
                    break;
                }
            }

            // Si no hay mejora, revertir el swap
            std::swap(temp_valores[idx1], temp_valores[idx2]);
        }

        if (mejora_local) {
            iteraciones_sin_mejora = 0;
        }
        else {
            iteraciones_sin_mejora++;
        }
    }
    // Reconstruir la solución final con el mejor vector encontrado
    reconstruirSolucion(sol, valoresMutables, indicesMutables);
    return mejora_global;
}


void miBusqueda::execute(Solution y) {
    int numeroMAX_iteraciones = this->param.get("#Numero-iteraciones").getInt();

    // Se extraen los valores una sola vez al principio
    miCEVRP* problema = dynamic_cast<miCEVRP*>(y.getProblem());
    std::vector<int> indicesMutables;
    std::vector<int> valoresMutables = obtenerValoresNodosMutables(y, problema, indicesMutables);

    // Asumimos que 'y' tiene los valores de inicio

    // Primer Hill Climbing
    hillClimbingSwaps(y, valoresMutables, indicesMutables);

    // Se evalúa la solución inicial (después del primer HC)
    y.getProblem()->evaluate(&y);
    y.getProblem()->evaluateConstraints(&y);
    Interval objetivoOriginal = y.getObjective(0);

    bool maximization = y.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;

    for (int i = 0; i < numeroMAX_iteraciones; i++) {
        std::vector<int> temp_valores = valoresMutables;

        perturbacionReversiones(temp_valores);

        // Reconstruimos la solución temporalmente para el hill climbing
        reconstruirSolucion(y, temp_valores, indicesMutables);
        bool mejora_hc = hillClimbingSwaps(y, temp_valores, indicesMutables);

        y.getProblem()->evaluate(&y);
        y.getProblem()->evaluateConstraints(&y);

        if (y.getNumberOfViolatedConstraints() == 0 &&
            ((maximization && y.getObjective(0) > objetivoOriginal) ||
                (!maximization && y.getObjective(0) < objetivoOriginal))) {

            objetivoOriginal = y.getObjective(0);
            valoresMutables = temp_valores; // Se guarda la mejor versión
            //OutputDebugStringW(L"MEJORO--BUSQUEDA LOCAL: \n");
        }
        else {
            // Si no mejora, restauramos la solución original con los valores que teníamos
            reconstruirSolucion(y, valoresMutables, indicesMutables);
        }
    }
}