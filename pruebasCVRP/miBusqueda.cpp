#include "miBusqueda.h"
#include "miCEVRP.h"
#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include <vector>
#include <cmath>
#include <utility>  
#include <numeric>
#include <algorithm>
#include <cassert>

 
miBusqueda::miBusqueda() {
 
}

void miBusqueda::initialize(Requirements* config) {
    config->addValue("#Numero-iteraciones", Constantes::INT);
    this->param = *config->load();
}

 

void reconstruirSolucion(Solution& solucion, const std::vector<int>& valoresMutables, const std::vector<int>& indicesMutables) {
    for (size_t i = 0; i < indicesMutables.size(); ++i) {
        solucion.setVariableValue(indicesMutables[i], valoresMutables[i]);
    }
}

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
 
bool localSearch(Solution& sol, std::vector<int>& valoresMutables, const std::vector<int>& indicesMutables) {
    if (valoresMutables.size() < 2) return false;

    RandomNumber* rnd = RandomNumber::getInstance();
    const bool maximization = sol.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;
    bool mejora_global = false;

    reconstruirSolucion(sol, valoresMutables, indicesMutables);
    sol.getProblem()->evaluate(&sol);
    sol.getProblem()->evaluateConstraints(&sol);
    Interval mejor_objetivo = sol.getObjective(0);

    bool mejora_en_iteracion = true;
    while (mejora_en_iteracion) {
        mejora_en_iteracion = false;
        const int intentos_maximos = valoresMutables.size() * 5; // Explorar un número razonable de vecinos

        for (int i = 0; i < intentos_maximos; ++i) {
            std::vector<int> vecino_valores = valoresMutables;
            int idx1 = rnd->nextInt(vecino_valores.size() - 1);
            int idx2;
            do {
                idx2 = rnd->nextInt(vecino_valores.size() - 1);
            } while (idx1 == idx2);
            std::swap(vecino_valores[idx1], vecino_valores[idx2]);

            reconstruirSolucion(sol, vecino_valores, indicesMutables);
            sol.getProblem()->evaluate(&sol);
            sol.getProblem()->evaluateConstraints(&sol);

            if (sol.getNumberOfViolatedConstraints() == 0) {
                Interval objetivo_vecino = sol.getObjective(0);
                bool es_mejor = (maximization && objetivo_vecino > mejor_objetivo) ||
                    (!maximization && objetivo_vecino < mejor_objetivo);

                if (es_mejor) {
                    valoresMutables = vecino_valores;
                    mejor_objetivo = objetivo_vecino;
                    mejora_en_iteracion = true;
                    mejora_global = true;
                    break;
                }
            }
        }
    }

    reconstruirSolucion(sol, valoresMutables, indicesMutables);
    sol.getProblem()->evaluate(&sol);
    sol.getProblem()->evaluateConstraints(&sol);

    return mejora_global;
}

// --- Componente 2: MECANISMO DE PERTURBACIÓN (Perturbation) ---
// Aplica un número de inversiones de subsecuencias para "sacudir" la solución.
// La "fuerza" de la perturbación es clave: debe ser suficiente para escapar del
// óptimo local, pero no tanto como para reiniciar la búsqueda aleatoriamente.
void perturbarSolucion(std::vector<int>& valoresMutables) {
    if (valoresMutables.size() < 4) return; // Necesita al menos 4 elementos para una reversión con sentido.

    RandomNumber* rnd = RandomNumber::getInstance();

    // Calcular el número de perturbaciones (la "fuerza")
    int num_perturbaciones = 1;
    if (valoresMutables.size() > 10) {
        // La fuerza de la perturbación es ~15% del tamaño del problema, con un mínimo de 2.
        num_perturbaciones = std::max(2, static_cast<int>(valoresMutables.size() * 0.15));
    }

    for (int i = 0; i < num_perturbaciones; ++i) {
        int inicio = rnd->nextInt(valoresMutables.size() - 1);
        int fin;
        do {
            fin = rnd->nextInt(valoresMutables.size() - 1);
        } while (std::abs(inicio - fin) < 2); // Asegurar que el segmento a revertir tenga al menos 2 elementos.

        if (inicio > fin) std::swap(inicio, fin);

        std::reverse(valoresMutables.begin() + inicio, valoresMutables.begin() + fin + 1);
    }
}


void miBusqueda::execute(Solution y) {
    int numeroMAX_iteraciones = this->param.get("#Numero-iteraciones").getInt();
    miCEVRP* problema = dynamic_cast<miCEVRP*>(y.getProblem());
    const bool maximization = y.getProblem()->getObjectivesType()[0] == Constantes::MAXIMIZATION;

 
    std::vector<int> indicesMutables;
    std::vector<int> sol_inicial = obtenerValoresNodosMutables(y, problema, indicesMutables);
 
    localSearch(y, sol_inicial, indicesMutables);

 
    std::vector<int> valores_mejor_solucion = sol_inicial;
    y.getProblem()->evaluate(&y);
    y.getProblem()->evaluateConstraints(&y);
    Interval objetivo_mejor_solucion = y.getObjective(0);

    // ---  ILS ---
    for (int i = 0; i < numeroMAX_iteraciones; i++) {
        // Trabajar con una copia para no perder la mejor solución si no se mejora.
        std::vector<int> valores_candidatos = valores_mejor_solucion;

        
        perturbarSolucion(valores_candidatos);

        // Cargar los valores perturbados en el objeto 'y' para la siguiente búsqueda.
        reconstruirSolucion(y, valores_candidatos, indicesMutables);

        // --- Componente 1 (de nuevo): BÚSQUEDA LOCAL ---
        // Aplicar la búsqueda local a la solución recién perturbada.
        localSearch(y, valores_candidatos, indicesMutables);

        y.getProblem()->evaluate(&y);
        y.getProblem()->evaluateConstraints(&y);

        // --- Componente 3: CRITERIO DE ACEPTACIÓN ---
        // Decidir si el nuevo óptimo local reemplaza al mejor que teníamos.
        if (y.getNumberOfViolatedConstraints() == 0) {
            Interval objetivo_candidato = y.getObjective(0);
            bool es_mejor = (maximization && objetivo_candidato > objetivo_mejor_solucion) ||
                (!maximization && objetivo_candidato < objetivo_mejor_solucion);

            if (es_mejor) {
                // Aceptación: La nueva solución es mejor, la guardamos.
                valores_mejor_solucion = valores_candidatos;
                objetivo_mejor_solucion = objetivo_candidato;
            }
        }
        // Si no es mejor, no hacemos nada. En la siguiente iteración,
        // se volverá a perturbar a partir de la misma 'valores_mejor_solucion'.
    }

    // Al final del proceso, asegurar que el objeto 'y' contenga la mejor solución global encontrada.
    reconstruirSolucion(y, valores_mejor_solucion, indicesMutables);
    y.getProblem()->evaluate(&y);
    y.getProblem()->evaluateConstraints(&y);
}