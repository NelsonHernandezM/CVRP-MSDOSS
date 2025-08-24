#include "miReparacion.h"
#include "miCEVRP.h"

#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <climits>
 
void insertarEstacionesDeCarga(miCEVRP* problema, std::vector<int>& ruta) {
    if (problema == nullptr || ruta.empty()) return;

    std::vector<int> rutaCorregida;
    rutaCorregida.push_back(ruta[0]); // Siempre comenzamos desde el depósito

    double energiaActual = problema->getEnergyCapacity();
    int nodoAnterior = ruta[0];

    for (size_t i = 1; i < ruta.size(); ++i) {
        int nodoActual = ruta[i];
        double consumo = problema->getCostMatrix()[nodoAnterior][nodoActual] * problema->getConsumption_Rate();

        // Intentar insertar estaciones hasta tener suficiente energía
        bool segmentoReparado = false;
        while (energiaActual < consumo) {
            int mejorEstacion = -1;
            double minDetour = std::numeric_limits<double>::max();

            // Buscar la mejor estación alcanzable que minimice el desvío
            for (int s = 0; s < problema->getNumberOfVariables(); ++s) {
                if (problema->isStation(s)) {
                    double distAEstacion = problema->getCostMatrix()[nodoAnterior][s];
                    double distAlDestino = problema->getCostMatrix()[s][nodoActual];
                    double consumoAEstacion = distAEstacion * problema->getConsumption_Rate();

                    // Verificar si es alcanzable y no crea dos estaciones consecutivas
                    if (consumoAEstacion <= energiaActual &&
                        distAEstacion + distAlDestino - problema->getCostMatrix()[nodoAnterior][nodoActual] < minDetour) {

                        mejorEstacion = s;
                        minDetour = distAEstacion + distAlDestino - problema->getCostMatrix()[nodoAnterior][nodoActual];
                    }
                }
            }

            // Si no se encontró estación válida, salir
            if (mejorEstacion == -1) {
                break;
            }

            // Insertar la estación en la ruta
            rutaCorregida.push_back(mejorEstacion);
            energiaActual -= problema->getCostMatrix()[nodoAnterior][mejorEstacion] * problema->getConsumption_Rate();
            energiaActual = problema->getEnergyCapacity();  // Recargar batería
            nodoAnterior = mejorEstacion;
            consumo = problema->getCostMatrix()[mejorEstacion][nodoActual] * problema->getConsumption_Rate();  // Actualizar consumo
            segmentoReparado = true;
        }

        // Si se reparó el segmento o tenemos suficiente energía, agregar el nodo
        if (energiaActual >= consumo) {
            rutaCorregida.push_back(nodoActual);
            energiaActual -= consumo;

            // Si es estación, recargar (a menos que sea dos estaciones seguidas)
            if (problema->isStation(nodoActual) &&
                (rutaCorregida.size() < 2 || !problema->isStation(rutaCorregida[rutaCorregida.size() - 2]))) {
                energiaActual = problema->getEnergyCapacity();
            }
            nodoAnterior = nodoActual;
        }
        else {
            // No se pudo reparar el segmento - mantener nodo original
            rutaCorregida.push_back(nodoActual);
            energiaActual -= consumo;  // Aunque sea negativo, continuamos
            nodoAnterior = nodoActual;
        }
    }

    ruta = rutaCorregida;
}


// Función para reparar una solución infactible de EVRP
void repararSolucion(Solution sol) {
    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
    if (!problema) return;

    Interval* vars = sol.getDecisionVariables();
    int n = sol.getNumVariables();

    // Separar la solución en rutas
    auto rutas = problema->separarSolucionPorRutas(&sol);
    int numClientes = problema->getNumberCustomers();
    int maxVehiculos = problema->getNumVehicles();

    // --- Paso 1: Identificar clientes no incluidos originalmente ---
    std::set<int> clientesPresentes;
    for (const auto& ruta : rutas) {
        for (int nodo : ruta) {
            if (problema->isCustomer(nodo)) {
                clientesPresentes.insert(nodo);
            }
        }
    }

    std::vector<int> clientesFaltantes;
    for (int i = 1; i <= numClientes; ++i) {
        if (clientesPresentes.find(i) == clientesPresentes.end()) {
            clientesFaltantes.push_back(i);
        }
    }

    // Función auxiliar para calcular la carga de una ruta
    auto calcularCarga = [&](const std::vector<int>& ruta) {
        int carga = 0;
        for (int nodo : ruta) {
            if (problema->isCustomer(nodo)) carga += problema->getCustomerDemand()[nodo];
        }
        return carga;
        };

    // --- Paso 2: Reparar rutas infactibles quitando y reubicando clientes uno por uno ---
    std::vector<int> clientesParaReubicar;
    bool reparadoEnEstePaso = true;
    while (reparadoEnEstePaso) {
        reparadoEnEstePaso = false;
        for (size_t i = 0; i < rutas.size(); ++i) {
            if (!problema->esRutaFactible(rutas[i])) {
                int clienteARemover = -1;
                int posCliente = -1;

                // Prioridad 1: Reparar por capacidad. Extraer el cliente con mayor demanda
                if (!problema->esRutaFactiblePorCapacidad(rutas[i])) {
                    int maxDemanda = -1;
                    for (int j = 1; j < (int)rutas[i].size() - 1; ++j) {
                        int nodo = rutas[i][j];
                        if (problema->isCustomer(nodo) && problema->getCustomerDemand()[nodo] > maxDemanda) {
                            maxDemanda = problema->getCustomerDemand()[nodo];
                            clienteARemover = nodo;
                            posCliente = j;
                        }
                    }
                }
                // Prioridad 2: Reparar por energía. Extraer el cliente más lejano al depósito
                else if (!problema->esRutaFactiblePorEnergia(rutas[i])) {
                    for (int j = (int)rutas[i].size() - 2; j >= 1; --j) {
                        if (problema->isCustomer(rutas[i][j])) {
                            clienteARemover = rutas[i][j];
                            posCliente = j;
                            break;
                        }
                    }
                    // Si no se encuentra un cliente, insertar una estación de carga
                    if (clienteARemover == -1) {
                        std::vector<int> rutaCopia = rutas[i];
                        int mejorPos = -1;
                        int estacionCercana = -1;
                        for (int j = 1; j < (int)rutaCopia.size(); ++j) {
                            int nodoPrev = rutaCopia[j - 1];
                            int estacion = problema->encontrarEstacionCercana(nodoPrev);
                            if (estacion != -1) {
                                rutaCopia.insert(rutaCopia.begin() + j, estacion);
                                if (problema->esRutaFactiblePorEnergia(rutaCopia)) {
                                    mejorPos = j;
                                    estacionCercana = estacion;
                                    break;
                                }
                                rutaCopia.erase(rutaCopia.begin() + j);
                            }
                        }
                        if (mejorPos != -1) {
                            rutas[i].insert(rutas[i].begin() + mejorPos, estacionCercana);
                            reparadoEnEstePaso = true;
                            break; // Se reparó la ruta
                        }
                    }
                }

                if (clienteARemover != -1) {
                    rutas[i].erase(rutas[i].begin() + posCliente);
                    clientesParaReubicar.push_back(clienteARemover);
                    reparadoEnEstePaso = true;
                    // Salir del bucle interno para re-evaluar la ruta actual
                    break;
                }
            }
        }
    }

    // --- Paso 3: Reubicar clientes extraídos y clientes faltantes ---
    std::vector<int> todosLosClientesPendientes;
    todosLosClientesPendientes.insert(todosLosClientesPendientes.end(), clientesFaltantes.begin(), clientesFaltantes.end());
    todosLosClientesPendientes.insert(todosLosClientesPendientes.end(), clientesParaReubicar.begin(), clientesParaReubicar.end());

    for (int cliente : todosLosClientesPendientes) {
        bool insertado = false;

        // Ordenar rutas por menor carga para una inserción balanceada
        std::vector<std::pair<int, size_t>> rutasOrdenadas;
        for (size_t k = 0; k < rutas.size(); ++k) {
            rutasOrdenadas.push_back({ calcularCarga(rutas[k]), k });
        }
        std::sort(rutasOrdenadas.begin(), rutasOrdenadas.end());

        // Intentar insertar el cliente en cada ruta ordenada, en la mejor posición
        for (const auto& par : rutasOrdenadas) {
            size_t k = par.second;

            // Búsqueda de la mejor posición de inserción en la ruta 'k'
            int mejorPos = -1;

            for (int pos = 1; pos < (int)rutas[k].size(); ++pos) {
                std::vector<int> copia = rutas[k];
                copia.insert(copia.begin() + pos, cliente);

                if (problema->esRutaFactible(copia)) {
                    mejorPos = pos;
                    break;
                }
            }

            if (mejorPos != -1) {
                rutas[k].insert(rutas[k].begin() + mejorPos, cliente);
                insertado = true;
                break;
            }
        }

        if (!insertado) {
            // Si no se pudo insertar en ninguna ruta existente, crear una nueva
            if (rutas.size() < (size_t)maxVehiculos) {
                rutas.push_back({ 0, cliente, 0 });
            }
            else {
                // Si no hay más vehículos, forzar la inserción en la ruta menos cargada
                if (!rutas.empty()) {
                    size_t k_min = rutasOrdenadas.front().second;
                    rutas[k_min].insert(rutas[k_min].end() - 1, cliente);
                }
            }
        }
    }

    for (size_t i = 0; i < rutas.size(); ++i) {
        if (!problema->esRutaFactiblePorEnergia(rutas[i])) {
            insertarEstacionesDeCarga(problema, rutas[i]);
        }
    }


    // --- Paso 4: Reconstruir la solución final ---
    std::vector<int> nuevaSolucion;
    for (const auto& ruta : rutas) {
        for (int nodo : ruta) {
            nuevaSolucion.push_back(nodo);
        }
    }

    // Asignar los valores a las variables de decisión de la solución
    for (int i = 0; i < n; ++i) {
        if (i < (int)nuevaSolucion.size()) {
            sol.setVariableValue(i, nuevaSolucion[i]);
        }
        else {
            vars[i].L = -1;
        }
    }
}

void miReparacion::execute(Solution sol) {

    sol.getProblem()->evaluate(&sol);
    sol.getProblem()->evaluateConstraints(&sol);

    int violaciones=sol.getNumberOfViolatedConstraints();

    if (violaciones > 0) {
        miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
     
  repararSolucion(sol);
        sol.getProblem()->evaluate(&sol);
        sol.getProblem()->evaluateConstraints(&sol);
        int algo = sol.getNumberOfViolatedConstraints();
    }
 /*   cout << "uaaa" << endl;*/
}

void miReparacion::initialize(Requirements* config) {
    this->param = *(config->load());
}
