#include "miReparacion.h"
#include "miCEVRP.h"

#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <climits>

//using namespace std;
//void repararSolucion(Solution sol) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
//    if (!problema) return;
//
//    Interval* vars = sol.getDecisionVariables();
//    int n = sol.getNumVariables();
//    
//    auto rutas = problema->separarSolucionPorRutas(&sol);
//    int numClientes = problema->getNumberCustomers();
//    int maxVehiculos = problema->getNumVehicles();
//
//    vector<bool> clienteVisitado(numClientes + 1, false);
//
//    // Marcar clientes visitados
//    for (auto& ruta : rutas) {
//        for (int nodo : ruta) {
//            if (problema->isCustomer(nodo)) {
//                clienteVisitado[nodo] = true;
//            }
//        }
//    }
//
//    // Clientes faltantes
//    vector<int> clientesFaltantes;
//    for (int i = 1; i <= numClientes; ++i) {
//        if (!clienteVisitado[i]) clientesFaltantes.push_back(i);
//    }
//
//    // Reparar rutas infactibles
//    for (size_t i = 0; i < rutas.size(); ++i) {
//        while (!problema->esRutaFactible(rutas[i])) {
//            bool reparado = false;
//
//            for (int j = 1; j < (int)rutas[i].size() - 1; ++j) {
//                int cliente = rutas[i][j];
//                if (!problema->isCustomer(cliente)) continue;
//
//                bool reubicado = false;
//
//                // Intentar insertar después de una estación en la misma ruta
//                for (int pos = 1; pos < (int)rutas[i].size() - 1; ++pos) {
//                    if (problema->isStation(rutas[i][pos])) {
//                        vector<int> copiaRuta = rutas[i];
//                        copiaRuta.erase(copiaRuta.begin() + j);
//                        copiaRuta.insert(copiaRuta.begin() + pos + 1, cliente);
//
//                        if (problema->esRutaFactible(copiaRuta)) {
//                            rutas[i] = copiaRuta;
//                            reubicado = true;
//                            reparado = true;
//                            break;
//                        }
//                    }
//                }
//
//                if (reubicado) break; // Reubicado en la misma ruta
//
//                // No se pudo reubicar en la misma ruta, eliminar cliente de esta ruta
//                rutas[i].erase(rutas[i].begin() + j);
//
//                // Intentar insertar en otra ruta ordenada por menor carga
//                vector<pair<int, size_t>> rutasOrdenadas;
//                for (size_t k = 0; k < rutas.size(); ++k) {
//                    if (k == i) continue;
//                    int cargaRuta = 0;
//                    for (int nodo : rutas[k]) {
//                        if (problema->isCustomer(nodo)) {
//                            cargaRuta += problema->getCustomerDemand()[nodo];
//                        }
//                    }
//                    rutasOrdenadas.push_back({ cargaRuta, k });
//                }
//                sort(rutasOrdenadas.begin(), rutasOrdenadas.end());
//
//                bool insertadoEnOtraRuta = false;
//                for (const auto& par : rutasOrdenadas) {
//                    size_t k = par.second;
//
//                    auto copia = rutas[k];
//                    copia.insert(copia.end() - 1, cliente);
//
//                    if (problema->esRutaFactible(copia)) {
//                        rutas[k] = copia;
//                        insertadoEnOtraRuta = true;
//                        reparado = true;
//                        break;
//                    }
//                }
//
//                if (insertadoEnOtraRuta) break;
//
//                // No pudo ser reubicado en ninguna ruta, crear nueva si hay vehículos disponibles
//                if (rutas.size() < (size_t)maxVehiculos) {
//                    rutas.push_back({ 0, cliente, 0 });
//                    reparado = true;
//                    break;
//                }
//                else {
//                    // Insertar forzadamente en ruta menos cargada
//                    size_t k_min = rutasOrdenadas.front().second;
//                    rutas[k_min].insert(rutas[k_min].end() - 1, cliente);
//                    reparado = true;
//                    break;
//                }
//            }
//
//            if (!reparado) {
//                // No pudo reparar ni reubicar ningún cliente, salir para evitar loop infinito
//                break;
//            }
//        }
//    }
//
//    // Insertar clientes faltantes
//    for (int cliente : clientesFaltantes) {
//        bool insertado = false;
//
//        vector<pair<int, size_t>> rutasOrdenadas;
//        for (size_t k = 0; k < rutas.size(); ++k) {
//            int cargaRuta = 0;
//            for (int nodo : rutas[k]) {
//                if (problema->isCustomer(nodo)) {
//                    cargaRuta += problema->getCustomerDemand()[nodo];
//                }
//            }
//            rutasOrdenadas.push_back({ cargaRuta, k });
//        }
//        sort(rutasOrdenadas.begin(), rutasOrdenadas.end());
//
//        for (const auto& par : rutasOrdenadas) {
//            size_t k = par.second;
//
//            auto copia = rutas[k];
//            copia.insert(copia.end() - 1, cliente);
//
//            if (problema->esRutaFactible(copia)) {
//                rutas[k] = copia;
//                insertado = true;
//                break;
//            }
//        }
//
//        if (!insertado) {
//            // No puede insertar, forzar en la ruta menos cargada
//            size_t k_min = rutasOrdenadas.front().second;
//            rutas[k_min].insert(rutas[k_min].end() - 1, cliente);
//        }
//    }
//
//    // Reconstruir solución
//    vector<int> nuevaSol;
//    for (auto& ruta : rutas) {
//        for (int nodo : ruta) {
//            nuevaSol.push_back(nodo);
//        }
//    }
//
//    for (int i = 0; i < n; ++i) {
//        if (i < (int)nuevaSol.size())
//            /*vars[i].L = nuevaSol[i];*/
//        sol.setVariableValue(i, nuevaSol[i]);
//        else
//            vars[i].L = -1;
//    }
//}
//void repararSolucion(Solution sol) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
//    
//    if (!problema) return;
//
//    Interval* vars = sol.getDecisionVariables();
//    int n = sol.getNumVariables();
//
//    auto rutas = problema->separarSolucionPorRutas(&sol);
//    int numClientes = problema->getNumberCustomers();
//    int maxVehiculos = problema->getNumVehicles();
//
//    vector<bool> clienteVisitado(numClientes + 1, false);
//
//    // Marcar clientes visitados
//    for (auto& ruta : rutas) {
//        for (int nodo : ruta) {
//            if (problema->isCustomer(nodo)) {
//                clienteVisitado[nodo] = true;
//            }
//        }
//    }
//
//    // Clientes faltantes
//    vector<int> clientesFaltantes;
//    for (int i = 1; i <= numClientes; ++i) {
//        if (!clienteVisitado[i]) clientesFaltantes.push_back(i);
//    }
//
//    // Función para calcular carga de ruta
//    auto calcularCarga = [&](const vector<int>& ruta) {
//        int carga = 0;
//        for (int nodo : ruta) {
//            if (problema->isCustomer(nodo)) carga += problema->getCustomerDemand()[nodo];
//        }
//        return carga;
//        };
//
//    // Reparar rutas infactibles
//    for (size_t i = 0; i < rutas.size(); ++i) {
//        int intentos = 0;
//        const int maxIntentos = 10000; // Límite de seguridad
//
//        while ((!problema->esRutaFactiblePorCapacidad(rutas[i]) ||
//            (!problema->esRutaFactiblePorEnergia(rutas[i]))) &&
//            intentos < maxIntentos) {
//            intentos++;
//            bool reparado = false;
//
//            // Fase 1: Reparar capacidad (prioridad alta)
//            if (!problema->esRutaFactiblePorCapacidad(rutas[i])) {
//                // Buscar cliente con mayor demanda para remover
//                int idxMax = -1;
//                int maxDemanda = -1;
//                for (int j = 1; j < (int)rutas[i].size() - 1; ++j) {
//                    if (problema->isCustomer(rutas[i][j])) {
//                        int demanda = problema->getCustomerDemand()[rutas[i][j]];
//                        if (demanda > maxDemanda) {
//                            maxDemanda = demanda;
//                            idxMax = j;
//                        }
//                    }
//                }
//
//                if (idxMax != -1) {
//                    int clienteRemovido = rutas[i][idxMax];
//                    rutas[i].erase(rutas[i].begin() + idxMax);
//
//                    // Intentar insertar en otra ruta con capacidad disponible
//                    bool reubicado = false;
//                    vector<pair<int, size_t>> rutasCandidatas;
//                    for (size_t k = 0; k < rutas.size(); ++k) {
//                        if (k == i) continue;
//                        int cargaActual = calcularCarga(rutas[k]);
//                        if (cargaActual + problema->getCustomerDemand()[clienteRemovido] <= problema->getMaxCapacity()) {
//                            rutasCandidatas.push_back({ cargaActual, k });
//                        }
//                    }
//                    sort(rutasCandidatas.begin(), rutasCandidatas.end());
//
//                    for (const auto& par : rutasCandidatas) {
//                        size_t k = par.second;
//                        // Intentar insertar en diferentes posiciones
//                        for (int pos = 1; pos < (int)rutas[k].size(); ++pos) {
//                            vector<int> copia = rutas[k];
//                            copia.insert(copia.begin() + pos, clienteRemovido);
//
//                            if (problema->esRutaFactiblePorCapacidad(copia)) {
//                                rutas[k] = copia;
//                                reubicado = true;
//                                reparado = true;
//                                break;
//                            }
//                        }
//                        if (reubicado) break;
//                    }
//
//                    // Si no se reubicó, manejar adecuadamente
//                    if (!reubicado) {
//                        if (rutas.size() < (size_t)maxVehiculos) {
//                            rutas.push_back({ 0, clienteRemovido, 0 });
//                        }
//                        else {
//                            // Insertar en ruta con menor carga
//                            size_t mejorRuta = 0;
//                            int menorCarga = INT_MAX;
//                            for (size_t k = 0; k < rutas.size(); ++k) {
//                                int cargaRuta = calcularCarga(rutas[k]);
//                                if (cargaRuta < menorCarga) {
//                                    menorCarga = cargaRuta;
//                                    mejorRuta = k;
//                                }
//                            }
//                            rutas[mejorRuta].insert(rutas[mejorRuta].begin() + 1, clienteRemovido);
//                        }
//                        reparado = true;
//                    }
//                }
//            }
//
//            // Fase 2: Reparar energía (solo si capacidad está bien)
//            if (!reparado && problema->esRutaFactiblePorCapacidad(rutas[i]) &&
//                !problema->esRutaFactiblePorEnergia(rutas[i])) {
//
//                // Estrategia 1: Insertar estación de recarga más cercana
//                bool energiaReparada = false;
//                for (int j = 1; j < (int)rutas[i].size(); ++j) {
//                    // Insertar estación después de nodos de servicio
//                    if (problema->isCustomer(rutas[i][j - 1]) || problema->isDepot(rutas[i][j - 1])) {
//                        vector<int> rutaTest = rutas[i];
//                        // Encontrar estación más cercana al nodo anterior
//                        int nodo_anterior = rutas[i][j - 1];
//                        int estacion = problema->encontrarEstacionCercana(nodo_anterior);
//
//                        if (estacion != -1) { // Solo si se encontró estación
//                            rutaTest.insert(rutaTest.begin() + j, estacion);
//
//                            if (problema->esRutaFactiblePorEnergia(rutaTest)) {
//                                rutas[i] = rutaTest;
//                                reparado = true;
//                                energiaReparada = true;
//                                break;
//                            }
//                        }
//                    }
//                }
//
//                // Estrategia 2: Reordenar clientes cerca de estaciones
//                if (!energiaReparada) {
//                    for (int j = 1; j < (int)rutas[i].size() - 1; ++j) {
//                        if (!problema->isCustomer(rutas[i][j])) continue;
//
//                        // Buscar posición cerca de estación
//                        for (int k = 1; k < (int)rutas[i].size() - 1; ++k) {
//                            if (problema->isStation(rutas[i][k])) {
//                                vector<int> copia = rutas[i];
//                                int cliente = copia[j];
//                                copia.erase(copia.begin() + j);
//                                // Insertar después de la estación
//                                copia.insert(copia.begin() + k + 1, cliente);
//
//                                if (problema->esRutaFactiblePorEnergia(copia)) {
//                                    rutas[i] = copia;
//                                    reparado = true;
//                                    energiaReparada = true;
//                                    break;
//                                }
//                            }
//                        }
//                        if (energiaReparada) break;
//                    }
//                }
//            }
//
//            // Si no se reparó en este intento, forzar salida
//            if (!reparado) break;
//        }
//    }
//
//    // Insertar clientes faltantes con doble verificación
//    for (int cliente : clientesFaltantes) {
//        bool insertado = false;
//        int demandaCliente = problema->getCustomerDemand()[cliente];
//
//        // Intentar en rutas con capacidad disponible
//        vector<pair<int, size_t>> rutasCandidatas;
//        for (size_t k = 0; k < rutas.size(); ++k) {
//            int cargaActual = calcularCarga(rutas[k]);
//            if (cargaActual + demandaCliente <= problema->getMaxCapacity()) {
//                rutasCandidatas.push_back({ cargaActual, k });
//            }
//        }
//        sort(rutasCandidatas.begin(), rutasCandidatas.end());
//
//        // Intentar inserción en posición válida
//        for (const auto& par : rutasCandidatas) {
//            size_t k = par.second;
//            for (int pos = 1; pos < (int)rutas[k].size(); ++pos) {
//                vector<int> copia = rutas[k];
//                copia.insert(copia.begin() + pos, cliente);
//
//                if (problema->esRutaFactiblePorCapacidad(copia) &&
//                    problema->esRutaFactiblePorEnergia(copia)) {
//                    rutas[k] = copia;
//                    insertado = true;
//                    break;
//                }
//            }
//            if (insertado) break;
//        }
//
//        // Crear nueva ruta si es necesario
//        if (!insertado) {
//            if (rutas.size() < (size_t)maxVehiculos) {
//                rutas.push_back({ 0, cliente, 0 });
//            }
//            else {
//                // Insertar en ruta con menor carga
//                size_t mejorRuta = 0;
//                int menorCarga = INT_MAX;
//                for (size_t k = 0; k < rutas.size(); ++k) {
//                    int cargaRuta = calcularCarga(rutas[k]);
//                    if (cargaRuta < menorCarga) {
//                        menorCarga = cargaRuta;
//                        mejorRuta = k;
//                    }
//                }
//                rutas[mejorRuta].insert(rutas[mejorRuta].begin() + 1, cliente);
//            }
//        }
//    }
//
//    // Post-procesamiento: Verificar y forzar factibilidad
//    for (auto& ruta : rutas) {
//        // Verificar capacidad
//        int cargaTotal = 0;
//        vector<int> rutaValida = { 0 };
//        for (int j = 1; j < (int)ruta.size() - 1; ++j) {
//            int nodo = ruta[j];
//            if (problema->isCustomer(nodo)) {
//                if (cargaTotal + problema->getCustomerDemand()[nodo] <= problema->getMaxCapacity()) {
//                    rutaValida.push_back(nodo);
//                    cargaTotal += problema->getCustomerDemand()[nodo];
//                }
//            }
//            else {
//                rutaValida.push_back(nodo);
//            }
//        }
//        rutaValida.push_back(0);
//        ruta = rutaValida;
//
//        // Verificar energía
//        while (!problema->esRutaFactiblePorEnergia(ruta)) {
//            // Insertar estación de emergencia más cercana
//            bool inserted = false;
//            for (int j = 1; j < (int)ruta.size(); ++j) {
//                vector<int> test = ruta;
//                int nodo_ref = ruta[j - 1];
//                int estacion_cercana = problema->encontrarEstacionCercana(nodo_ref);
//                if (estacion_cercana != -1) {
//                    test.insert(test.begin() + j, estacion_cercana);
//                    if (problema->esRutaFactiblePorEnergia(test)) {
//                        ruta = test;
//                        inserted = true;
//                        break;
//                    }
//                }
//            }
//            if (!inserted) break; // Prevenir ciclo infinito
//        }
//    }
//
//    // Reconstruir solución
//    vector<int> nuevaSol;
//    for (auto& ruta : rutas) {
//        for (int nodo : ruta) {
//            nuevaSol.push_back(nodo);
//        }
//    }
//
//    for (int i = 0; i < n; ++i) {
//        if (i < (int)nuevaSol.size()) {
//            sol.setVariableValue(i, nuevaSol[i]);
//        }
//        else {
//            vars[i].L = -1;
//        }
//    }
//}



//void repararSolucion(Solution sol) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
//    if (!problema) return;
//
//    Interval* vars = sol.getDecisionVariables();
//    int n = sol.getNumVariables();
//
//    // 1. Recopilar todos los clientes únicos en la solución actual
//    std::vector<int> clientes;
//    std::vector<bool> visitados(problema->getNumberCustomers() + 1, false);
//
//    for (int i = 0; i < n; ++i) {
//        int nodo = vars[i].L;
//        if (nodo == -1) break;
//        if (problema->isCustomer(nodo) && !visitados[nodo]) {
//            clientes.push_back(nodo);
//            visitados[nodo] = true;
//        }
//    }
//
//    // 2. Añadir clientes faltantes
//    for (int i = 1; i <= problema->getNumberCustomers(); ++i) {
//        if (!visitados[i]) {
//            clientes.push_back(i);
//        }
//    }
//
//    // 3. Ordenar clientes (opcional: nearest neighbor, etc.)
//    // std::sort(clientes.begin(), clientes.end());
//
//    // 4. Reconstruir solución garantizando factibilidad
//    std::vector<int> nuevaRuta;
//    double energia = problema->getEnergyCapacity();
//    int capacidad = 0;
//    int vehiculos = 1;
//    int current = 0; // Depósito inicial
//    nuevaRuta.push_back(current);
//
//    for (int cliente : clientes) {
//        double consumo = problema->getCostMatrix()[current][cliente] * problema->getConsumption_Rate();
//        int demanda = problema->getCustomerDemand()[cliente];
//
//        // Verificar factibilidad
//        if (energia - consumo < 0 || capacidad + demanda > problema->getMaxCapacity()) {
//            // Insertar estación si es problema de energía
//            if (energia - consumo < 0) {
//                int estacion = problema->encontrarEstacionCercana(current);
//                if (estacion != -1) {
//                    double consumoEst = problema->getCostMatrix()[current][estacion] * problema->getConsumption_Rate();
//                    if (consumoEst <= energia) {
//                        nuevaRuta.push_back(estacion);
//                        energia -= consumoEst;
//                        current = estacion;
//                        energia = problema->getEnergyCapacity(); // Recargar
//                    }
//                }
//            }
//
//            // Si aún no es factible, cambiar de vehículo
//            if (energia - consumo < 0 || capacidad + demanda > problema->getMaxCapacity()) {
//                if (vehiculos >= problema->getNumVehicles()) {
//                    // No hay más vehículos disponibles - forzar estación
//                    int estacion = problema->encontrarEstacionCercana(current);
//                    if (estacion != -1) {
//                        nuevaRuta.push_back(estacion);
//                        current = estacion;
//                        energia = problema->getEnergyCapacity();
//                    }
//                }
//                else {
//                    // Nuevo vehículo
//                    nuevaRuta.push_back(0);
//                    vehiculos++;
//                    current = 0;
//                    energia = problema->getEnergyCapacity();
//                    capacidad = 0;
//                }
//            }
//        }
//
//        // Insertar cliente
//        nuevaRuta.push_back(cliente);
//        energia -= problema->getCostMatrix()[current][cliente] * problema->getConsumption_Rate();
//        capacidad += demanda;
//        current = cliente;
//    }
//
//    // Cerrar última ruta
//    if (current != 0) {
//        nuevaRuta.push_back(0);
//    }
//
//    // Rellenar solución
//    int idx = 0;
//    for (; idx < nuevaRuta.size() && idx < n; idx++) {
//        vars[idx].L = nuevaRuta[idx];
//    }
//    for (; idx < n; idx++) {
//        vars[idx].L = -1;
//    }
//}
//
 // Método auxiliar para insertar estaciones de carga si es necesario
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
//
//void repararEstacionesMasCercanas(Solution s) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(s.getProblem());
// 
//
//    Interval* vars = s.getDecisionVariables();
//    int nodoPrevio = 0;  // Partimos desde el depósito.
//
//    for (int i = 0; i < s.getNumVariables(); ++i) {
//        int nodoActual = vars[i].L;
//
//        if (nodoActual == -1) {
//            break;  // Fin de las rutas.
//        }
//
//        if (nodoActual == 0) {
//            nodoPrevio = 0;  // Nuevo vehículo (desde el depósito).
//            continue;
//        }
//
//        if (problema->isStation(nodoActual)) {
//            int nuevaEstacion = problema->encontrarEstacionCercana(nodoPrevio);
//
//            if (nuevaEstacion != -1 && nuevaEstacion != nodoActual) {
//                vars[i].L = nuevaEstacion;
//            }
//
//            nodoPrevio = nuevaEstacion;
//        }
//        else {
//            nodoPrevio = nodoActual;  // Cliente visitado.
//        }
//    }
//
//    // Re-evaluar la solución después del cambio.
//    //problema->evaluate(&s);
//    //problema->evaluateConstraints(&s);
//
//}
//
//
//
//void repara2(Solution sol) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
//    if (problema == nullptr) return;
//
//    Interval* vars = sol.getDecisionVariables();
//
//    double energiaRestante = problema->getEnergyCapacity();
//
//    int** cost_Matrix = problema->getCostMatrix();
//
//    double consumption_Rate = problema->getConsumption_Rate();
//    int* customer_Demand = problema->getCustomerDemand();
//
//    int cargaActual = 0;
//    int nodoAnterior = 0;
//    int vehiculosUsados = 1;
//    int pos = 0;
//
//    for (int i = 0; i < sol.getNumVariables(); ++i) {
//        int nodo = vars[i].L;
//        if (nodo == -1) break;
//
//        if (problema->isDepot(nodo)) {
//            energiaRestante = problema->getEnergyCapacity();
//            cargaActual = 0;
//            nodoAnterior = 0;
//            vehiculosUsados++;
//            vars[pos++].L = 0;
//            continue;
//        }
//
//        double consumo = cost_Matrix[nodoAnterior][nodo] * consumption_Rate;
//        int demanda = problema->isCustomer(nodo) ? customer_Demand[nodo] : 0;
//
//        if (energiaRestante - consumo < 0) {
//            int estacion = problema->encontrarEstacionCercana(nodoAnterior);
//            vars[pos++].L = estacion;
//            energiaRestante = problema->getEnergyCapacity();
//            nodoAnterior = estacion;
//        }
//
//        if (cargaActual + demanda > problema->getMaxCapacity()) {
//            vars[pos++].L = 0;
//            energiaRestante = problema->getEnergyCapacity();
//            cargaActual = 0;
//            nodoAnterior = 0;
//            vehiculosUsados++;
//        }
//
//        vars[pos++].L = nodo;
//        energiaRestante -= consumo;
//        cargaActual += demanda;
//        nodoAnterior = nodo;
//    }
//
//    if (nodoAnterior != 0) {
//        vars[pos++].L = 0;
//    }
//
//    while (pos < sol.getNumVariables()) {
//        vars[pos++].L = -1;
//    }
//
//    
//    repararEstacionesMasCercanas(sol);
//    //problema->evaluate(&sol);
//    //problema->evaluateConstraints(&sol);
//}
//
//
//
//
//void miReparacion::execute(Solution sol) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
// 
//
//        Interval* vars = sol.getDecisionVariables();
//    
//        double energiaRestante = problema->getEnergyCapacity();
//    
//        int** cost_Matrix = problema->getCostMatrix();
//    
//        double consumption_Rate = problema->getConsumption_Rate();
//        int* customer_Demand = problema->getCustomerDemand();
//
//    std::vector<int> clientesPendientes;
//    bool* clientesInsertados = new bool[problema->getNumberCustomers()]();
//
//    // Marcar clientes presentes en la solución original
//    for (int i = 0; i < sol.getNumVariables(); ++i) {
//        int nodo = vars[i].L;
//        if (problema->isCustomer(nodo)) {
//            int idx = nodo - 1;
//            if (idx >= 0 && idx < problema->getNumberCustomers())
//                clientesInsertados[idx] = true;
//        }
//    }
//
//    // Lista de todos los clientes (los que faltan y los que ya estaban)
//    for (int i = 0; i < problema->getNumberCustomers(); ++i) {
//        clientesPendientes.push_back(i + 1);
//    }
//
//    // Orden opcional (mantener original o usar nearest neighbor)
//    // std::random_shuffle(clientesPendientes.begin(), clientesPendientes.end());
//
//    int pos = 0;
//    int vehiculosUsados = 1;
// 
//    int cargaActual = 0;
//    int nodoAnterior = 0;
//
//    for (int cliente : clientesPendientes) {
//        double consumo = cost_Matrix[nodoAnterior][cliente] * consumption_Rate;
//        int demanda = customer_Demand[cliente];
//
//        bool necesitaEstacion = energiaRestante - consumo < 0;
//        bool excedeCarga = cargaActual + demanda > problema->getMaxCapacity();
//
//        if (necesitaEstacion || excedeCarga) {
//            if (vehiculosUsados < problema->getNumVehicles()) {
//                if (nodoAnterior != 0)
//                    vars[pos++].L = 0;
//                energiaRestante = problema->getEnergyCapacity();
//                cargaActual = 0;
//                nodoAnterior = 0;
//                vehiculosUsados++;
//            }
//            else if (necesitaEstacion) {
//                int estacion = problema->encontrarEstacionCercana(nodoAnterior);
//                vars[pos++].L = estacion;
//                energiaRestante = problema->getEnergyCapacity();
//                nodoAnterior = estacion;
//            }
//        }
//
//        vars[pos++].L = cliente;
//        energiaRestante -= consumo;
//        cargaActual += demanda;
//        nodoAnterior = cliente;
//    }
//
//    if (nodoAnterior != 0)
//        vars[pos++].L = 0;
//
//    while (pos < sol.getNumVariables()) {
//        vars[pos++].L = -1;
//    }
//
//    delete[] clientesInsertados;
//
//    //problema->evaluate(&sol);
//    //problema->evaluateConstraints(&sol);
//   // repararEstacionesMasCercanas(sol);
//    /*repara2(sol);*/
//
//}


//
//void miReparacion::execute(Solution sol) {
//    miCEVRP* problema = dynamic_cast<miCEVRP*>(sol.getProblem());
//    if (problema == nullptr) return;
//
//    Interval* vars = sol.getDecisionVariables();
//
//    double energiaRestante = problema->getEnergyCapacity();
//
//    int** cost_Matrix = problema->getCostMatrix();
//
//    double consumption_Rate = problema->getConsumption_Rate();
//    int* customer_Demand = problema->getCustomerDemand();
// 
//    int cargaActual = 0;
//    int nodoAnterior = 0;
//    int vehiculosUsados = 1;
//    int pos = 0;
//
//    for (int i = 0; i < sol.getNumVariables(); ++i) {
//        int nodo = vars[i].L;
//        if (nodo == -1) break;
//
//        if (problema->isDepot(nodo)) {
//            energiaRestante = problema->getEnergyCapacity();
//            cargaActual = 0;
//            nodoAnterior = 0;
//            vehiculosUsados++;
//            vars[pos++].L = 0;
//            continue;
//        }
//
//        double consumo =  cost_Matrix[nodoAnterior][nodo] * consumption_Rate;
//        int demanda = problema->isCustomer(nodo) ? customer_Demand[nodo] : 0;
//
//        if (energiaRestante - consumo < 0) {
//            int estacion = problema->encontrarEstacionCercana(nodoAnterior);
//            vars[pos++].L = estacion;
//            energiaRestante = problema->getEnergyCapacity();
//            nodoAnterior = estacion;
//        }
//
//        if (cargaActual + demanda > problema->getMaxCapacity()) {
//            vars[pos++].L = 0;
//            energiaRestante = problema->getEnergyCapacity();
//            cargaActual = 0;
//            nodoAnterior = 0;
//            vehiculosUsados++;
//        }
//
//        vars[pos++].L = nodo;
//        energiaRestante -= consumo;
//        cargaActual += demanda;
//        nodoAnterior = nodo;
//    }
//
//    if (nodoAnterior != 0) {
//        vars[pos++].L = 0;
//    }
//
//    while (pos < sol.getNumVariables()) {
//        vars[pos++].L = -1;
//    }
//
//    problema->evaluate(&sol);
//    problema->evaluateConstraints(&sol);
//
//}

//void miReparacion::initialize(Requirements* config) {
//	this->param = *(config->load());
//}