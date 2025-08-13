

#include "miCEVRP.h"
#include <algorithm>
#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

miCEVRP::miCEVRP() {

}

void imprimirSolucion(Solution* s) {
    Interval* vars = s->getDecisionVariables();
   
    for (int i = 0; i < s->getNumVariables(); ++i) {
           cout << vars[i].L << " ";
    }
    cout << endl;
}

// 0 = depot, 1 = customer, 2 = station
 bool miCEVRP::isDepot(int nodo) {
    return this->tipoNodo[nodo] == 0;
}

bool  miCEVRP::isCustomer(int nodo) {
    return this->tipoNodo[nodo] == 1;
}

bool  miCEVRP::isStation(int nodo) {
    return this->tipoNodo[nodo] == 2;
}

double  miCEVRP::getEnergyCapacity() {
    return this->energy_Capacity;
} 

int** miCEVRP::getCostMatrix() {
    return this->cost_Matrix;
} 
double  miCEVRP::getConsumption_Rate() {
    return this->consumption_Rate;
} 

int* miCEVRP::getCustomerDemand() {
    return this->customer_Demand;
} 
int miCEVRP::getMaxCapacity() {
    return this->max_Capacity;
}

int miCEVRP::getNumberCustomers() {
    return this->num_Customers;
}
int miCEVRP::getNumVehicles() {
    return this->num_Vehicles;
}
 
//
//static double** getCostMatrix(int** coordenadas, int n) {
//    /*  cout << endl; cout << endl;*/
//    double** costMatrix = new double* [n];
//    for (int i = 0; i < n; i++) {
//        costMatrix[i] = new double[n];
//    }
//
//    // Calculamos la distancia euclidiana entre cada par de nodos
//    for (int i = 0; i < n; i++) {
//        int xi = coordenadas[i][0];
//        int yi = coordenadas[i][1];
//        for (int j = 0; j < n; j++) {
//            int xj = coordenadas[j][0];
//            int yj = coordenadas[j][1];
//            double distancia = sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));
//            costMatrix[i][j] = distancia;//static_cast<int>(round(distancia));
//            /*cout << costMatrix[i][j]<< " ";*/
//        }
//        /*  cout << endl;*/
//    }
//    /*  cout << endl;*/
//    return costMatrix;
//}


// Devuelve true si la ruta es factible energéticamente (considerando recargas).
bool miCEVRP::esRutaFactiblePorEnergia(const std::vector<int>& ruta) {
    double energiaRestante = this->energy_Capacity;

    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        int desde = ruta[i];
        int hasta = ruta[i + 1];
        double consumo = cost_Matrix[desde][hasta] * consumption_Rate;

        energiaRestante -= consumo;

        if (energiaRestante < 0) {
            return false;  // Batería agotada
        }

        if (isStation(hasta)) {
            energiaRestante = this->energy_Capacity;  // Recarga
        }
    }

    return true;  // Energía suficiente durante toda la ruta
}

// Devuelve true si la ruta es factible por capacidad del vehículo.
bool miCEVRP::esRutaFactiblePorCapacidad(const std::vector<int>& ruta) {
    int cargaAcumulada = 0;

    for (size_t i = 1; i < ruta.size(); ++i) {  // empezamos en 1 porque ruta[0] siempre es depósito
        int nodo = ruta[i];

        if (isCustomer(nodo)) {
            cargaAcumulada += customer_Demand[nodo];

            if (cargaAcumulada > max_Capacity) {
                return false;  // Capacidad del vehículo excedida
            }
        }
    }

    return true;  // Capacidad respetada
}

int miCEVRP::evaluarRutaDistancia(const std::vector<int>& ruta) {
    int distancia = 0;
    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        int desde = ruta[i];
        int hasta = ruta[i + 1];
        distancia += this->cost_Matrix[desde][hasta];
    }
    return distancia;
}

double miCEVRP::evaluarRutaEnergia(const std::vector<int>& ruta) {
    double energia = 0.0;
    double energiaRestante = this->energy_Capacity;

    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        int desde = ruta[i];
        int hasta = ruta[i + 1];
        double consumo = cost_Matrix[desde][hasta] * consumption_Rate;

        energia += consumo;

        // Si es estación, recargamos batería
        if (isStation(hasta)) {
            energiaRestante = this->energy_Capacity;
        }
        else {
            energiaRestante -= consumo;
        }
    }

    return energia;
}




bool miCEVRP::esRutaFactible(const std::vector<int>& ruta) {
    double energiaRestante = this->energy_Capacity;
    int carga = 0;

    for (size_t i = 0; i < ruta.size() - 1; ++i) {
        int desde = ruta[i];
        int hasta = ruta[i + 1];

        double consumo = cost_Matrix[desde][hasta] * consumption_Rate;

        energiaRestante -= consumo;

        // 🔴 Energía insuficiente
        if (energiaRestante < 0) {
            return false;
        }

        // 🟡 Dos estaciones seguidas  (según la literatura no es factible la visita de 2 estaciones de recarga seguidas)
        if (i >= 1 && isStation(ruta[i]) && isStation(hasta)) {
            return false;
        }

        // ⚫ Si es cliente, agregar demanda
        if (isCustomer(hasta)) {
            carga += customer_Demand[hasta];
            if (carga > max_Capacity) {
                return false;
            }
        }

        // 🟢 Si es estación, recarga
        if (isStation(hasta)) {
            energiaRestante = this->energy_Capacity;
        }
    }

    return true;
}



std::vector<std::vector<int>> miCEVRP::separarSolucionPorRutas(Solution* s) {
    std::vector<std::vector<int>> rutas;
    std::vector<int> rutaActual;

    Interval* vars = s->getDecisionVariables();

    // Siempre se empieza desde el depósito
    rutaActual.push_back(0);

    for (int i = 0; i < s->getNumVariables(); ++i) {
        int nodo = vars[i].L;

        if (nodo == -1) {
            if (rutaActual.back() != 0) {
                rutaActual.push_back(0); // aseguramos regreso a depósito
            }
            rutas.push_back(rutaActual);
            break;
        }
        else if (nodo == 0) {
            // Fin de una ruta, regreso al depósito
            if (rutaActual.back() != 0) {
                rutaActual.push_back(0);
            }
            rutas.push_back(rutaActual);
            rutaActual.clear();
            rutaActual.push_back(0); // Comenzamos una nueva ruta desde el depósito
        }
        else {
            rutaActual.push_back(nodo);
        }
    }

    return rutas;
}




static int** generateCostMatrix(int** coordenadas, int n) {
    /*  cout << endl; cout << endl;*/
    int** costMatrix = new int* [n];
    for (int i = 0; i < n; i++) {
        costMatrix[i] = new int[n];
    }

    // Calculamos la distancia euclidiana entre cada par de nodos
    for (int i = 0; i < n; i++) {
        int xi = coordenadas[i][0];
        int yi = coordenadas[i][1];
        for (int j = 0; j < n; j++) {
            int xj = coordenadas[j][0];
            int yj = coordenadas[j][1];
            double distancia = sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));
            costMatrix[i][j] = static_cast<int>(round(distancia));
            /*cout << costMatrix[i][j]<< " ";*/
        }
        /*  cout << endl;*/
    }
    /*  cout << endl;*/
    return costMatrix;
}


void miCEVRP::initialize(Requirements* config) {

  
    config->addValue("#OPTIMAL", Constantes::INT);

    config->addValue("#VEHICLES", Constantes::INT);
    config->addValue("#DIMENSION", Constantes::INT);


    this->par = config->load();

    this->dimension = this->par->get("#DIMENSION").getInt();

    config->addValue("#NUM-STATIONS", Constantes::INT);

    this->par = config->load();
    this->num_stations = this->par->get("#NUM-STATIONS").getInt();

    config->addValue("#CAPACITY", Constantes::INT);
    config->addValue("#ENERGY_CAPACITY", Constantes::DOUBLE);
    config->addValue("#CONSUMPTION_RATE", Constantes::DOUBLE);
    config->addValue("#EDGE_WEIGHT_TYPE", Constantes::STRING);
    config->addMatrix("#COORDS", Constantes::INT, this->dimension, 2);

    config->addVector("#DEMANDS", Constantes::INT, this->dimension-num_stations);

    config->addVector("#STATIONS", Constantes::INT, this->num_stations);

    config->addValue("#DEPOT", Constantes::INT);

    this->par = config->load();

    this->optimal = this -> par->get("#OPTIMAL").getInt();
    this->num_Vehicles = this->par->get("#VEHICLES").getInt();

    this->max_Capacity = this->par->get("#CAPACITY").getInt();
    this->energy_Capacity = this->par->get("#ENERGY_CAPACITY").getDouble();
    this->consumption_Rate = this->par->get("#CONSUMPTION_RATE").getDouble();
    this->coords = (int**)this->par->get("#COORDS").getValue();
    
    this->customer_Demand = (int*)this->par->get("#DEMANDS").getValue();

    this->stations = (int*)this->par->get("#STATIONS").getValue();
    this->depot = this->par->get("#DEPOT").getInt();
    this->cost_Matrix = (int**)generateCostMatrix(coords,dimension);

    this->num_Customers = this->dimension - this->num_stations - 1 ;

    this->tipoNodo = new int[this->dimension];

    // 0 = depot, 1 = customer, 2 = station
    for (int i = 0; i < this->dimension; ++i) {
        
        if (i == this->depot) {

            this->tipoNodo[i] = 0;
        }else if (i<=this->num_Customers){
        
            this->tipoNodo[i] = 1;
        
        }
        else {
            this->tipoNodo[i] =2;
        }

    }




    //DEFINIR PAR METROS DE CONTROL
    this->numberOfVariables_ = (this->dimension+ num_Vehicles)+5;
    this->numberOfObjectives_ = 1; // DISTANCIA, ..
    this->numberOfConstraints_ = 2;



    this->lowerBounds_ = new Interval[numberOfVariables_];
    this->upperBounds_ = new Interval[numberOfVariables_];

    for (int i = 0; i < this->numberOfVariables_; ++i) {
        this->lowerBounds_[i] = 0;
        this->upperBounds_[i] = dimension;
    }

    this->objs_type = new int[this->numberOfObjectives_];

    for (int obj = 0; obj < this->numberOfObjectives_; ++obj) {
        this->objs_type[obj] = Constantes::MINIMIZATION;
    }




}





// 
void miCEVRP::evaluate(Solution* s) {

    Interval* vars = s->getDecisionVariables();
    int distanciaTotal = 0;
    double totalEnergyConsumed = 0.0; //for all de routes

    int nodoPrevio = 0;  // Start from the depot to the first node (nodo 0)
    //e.g [0,1,2,3,-1] means 0 -> 1 -> 2 -> 3 -> 0
    //e.g 2 [0,1,0,3,-1] means R 0 -> 1 -> 0  R2:0 -> 3 -> 0

    //for (int i = 0; i < this->getNumberOfVariables(); i++)
    //{
    //    cout << vars[i] ;
    //}
    //cout << endl;


    for (int i = 0; i < this->getNumberOfVariables(); i++) {
        int nodoActual = (int)vars[i].L;

        // if the node is -1, it is ignored 
        if (nodoActual == -1) {
            //-1 means the end of the routes, so we need go to from actualNode to depot  ;
            distanciaTotal += cost_Matrix[0][nodoPrevio];
            
            totalEnergyConsumed += (cost_Matrix[0][nodoPrevio] * consumption_Rate)  ;


            /*
            if (!(cost_Matrix[nodoPrevio][nodoActual] == 100000)) {
                cout << "previo: " << nodoPrevio << "nodo ultimo: " << 0<<endl;
                cout << "sumando: " << cost_Matrix[nodoPrevio][0] << endl;
            }*/

            break;

        }

        /*cout << "Nodo " << nodoActual << " en posición " << i << endl;*/

        if (nodoActual == 0) {

            //if nodoActual is 0, means the end of that route, so we add the cost
            //from nodoPrevio to Depot.
            /*if (!(cost_Matrix[nodoPrevio][nodoActual] == 100000)) {
                cout << "nodo: " << nodoPrevio << "nodoActual: " << nodoActual << endl;
                cout << "sumando: " << cost_Matrix[nodoPrevio][nodoActual] << endl;
            }*/
            distanciaTotal += cost_Matrix[nodoPrevio][nodoActual];

            totalEnergyConsumed += (cost_Matrix[nodoPrevio][nodoActual] * consumption_Rate);


            // reset nodoPrevio, we start from depot in the new route.
            nodoPrevio = 0;
        }
        else {
            // we add the cost travel from nodoPrevio to nodoActual 
            distanciaTotal += cost_Matrix[nodoPrevio][nodoActual];

            totalEnergyConsumed += (cost_Matrix[nodoPrevio][nodoActual] * consumption_Rate);

            /* if (!(cost_Matrix[nodoPrevio][nodoActual] == 100000)) {
                 cout << "nodo: " << nodoPrevio << "nodoActual: " << nodoActual <<endl;
                 cout << "sumando: " << cost_Matrix[nodoPrevio][nodoActual] << endl;
             }*/
             /* cout << "Distancia añadida: " << cost_Matrix[nodoPrevio][nodoActual]
                  << " (" << nodoPrevio << " -> " << nodoActual << ")" << endl;*/

                  // update nodoprevio
            nodoPrevio = nodoActual;
        }
    }
 


    s->setObjective(0, distanciaTotal);
   // s->setObjective(1, totalEnergyConsumed);
 
}

void miCEVRP::evaluateConstraints(Solution* s) {
    Interval* vars = s->getDecisionVariables();
    int* cargaRutas = new int[this->num_Vehicles]();
    double* energiaRestante = new double[this->num_Vehicles];   // ENERGY FOR EACH VEHICLE, WHEN A VEHICLE VISITS A STATION THE LEVEL OF BATTERY RESETS
    bool* clientesVisitados = new bool[this->num_Customers]();  // <-- ALL CUSTOMERS MUST BE VISITED

    double numberViolatedConstraints = 0.0;
    double totalViolation = 0;
    int currentVehicle = 0;

    
    for (int i = 0; i < this->num_Vehicles; i++) {
        energiaRestante[i] = this->energy_Capacity;
    }

    int nodoAnterior = 0;

    for (int i = 0; i < this->getNumberOfVariables(); i++) {
        int nodoActual = (int)vars[i].L;

        // END OF ALL THE ROUTES
        if (nodoActual == -1) {
            double energiaNecesaria = cost_Matrix[nodoAnterior][0] * consumption_Rate;
            if (energiaRestante[currentVehicle] < energiaNecesaria) {
                numberViolatedConstraints++;
                totalViolation += (energiaNecesaria - energiaRestante[currentVehicle]);
            }
            break;
        }

        // CHANGE OF ROUTE (NEW VEHICLE)
        if (nodoActual == 0) {
            double energiaNecesaria = cost_Matrix[nodoAnterior][0] * consumption_Rate;
            if (energiaRestante[currentVehicle] < energiaNecesaria) {
                numberViolatedConstraints++;
                totalViolation += (energiaNecesaria - energiaRestante[currentVehicle]);
            }

            currentVehicle++;
            if (currentVehicle >= this->num_Vehicles) {
                numberViolatedConstraints++; // NUMBER OF VEHICLES EXCEEDED
                break;
            }

            nodoAnterior = 0;
            continue;
        }

        // CALCULATE THE ENERGY CONSUMPTION FROM THE LAST NODE TO THE DESTINY NODE
        double consumo = cost_Matrix[nodoAnterior][nodoActual] * consumption_Rate;
        energiaRestante[currentVehicle] -= consumo;

        if (energiaRestante[currentVehicle] < 0) {
            numberViolatedConstraints++;
            totalViolation += (-energiaRestante[currentVehicle]);
        }

        // IF THE VISITED NODE IS A CUSTOMER, WE ADD THE DEMAND AND MARK AS VISITED
        if (isCustomer(nodoActual)) {
            cargaRutas[currentVehicle] += customer_Demand[nodoActual];

            if (nodoActual >= 1 && nodoActual <= this->num_Customers) {
                clientesVisitados[nodoActual - 1] = true;
            }
        }

        // IF THE NODE IS A STATION WE RESET DE BATTERY LEVEL OF THE VEHICLE
        if (isStation(nodoActual)) {
            energiaRestante[currentVehicle] = this->energy_Capacity;
        }
        if (i >= 1) {
            if (isStation(nodoAnterior) && isStation(nodoActual) ) {
                numberViolatedConstraints++;
                totalViolation += 1;
            }
        }

        nodoAnterior = nodoActual;
    }

    // CHECK THE CAPACITY FOR ALL USED VEHICLES
    for (int v = 0; v <= std::min(currentVehicle, this->num_Vehicles - 1); v++) {
        if (cargaRutas[v] > this->max_Capacity) {
            numberViolatedConstraints++;
            totalViolation += (cargaRutas[v] - this->max_Capacity);
        }
    }

    // PENALIZE IF THERE ARE CUSTOMERS NOT VISITED
    for (int i = 0; i < this->num_Customers; ++i) {
        if (!clientesVisitados[i]) {
            std::wstring m = L"Clientes no insertados: " + std::to_wstring(i+1) + L"\n";
            OutputDebugStringW(m.c_str());
            
            numberViolatedConstraints++;
            totalViolation += 1.0;  // CUANTO??
        }
      

    }/*std::wstring m2 = L"   "   L"\n";*/
    //OutputDebugStringW(m2.c_str());

    // Guardar penalizaciones
    s->setNumberOfViolatedConstraints(numberViolatedConstraints);
    s->setOverallConstraintViolation(-totalViolation);

    // Liberar memoria
    delete[] cargaRutas;
    delete[] energiaRestante;
    delete[] clientesVisitados;
}



int miCEVRP::encontrarEstacionCercana(int nodo) {
    int costo_minimo = INT_MAX;
    int estacion_cercana = -1;

    for (int i = 0; i < this->dimension; ++i) {
        if (isStation(i) && this->cost_Matrix[nodo][i] < costo_minimo) {
            costo_minimo = this->cost_Matrix[nodo][i];
            estacion_cercana = i;
        }
    }

    return estacion_cercana;
}




Solution miCEVRP::generateRandomSolution() {
    RandomNumber* rnd = rnd->getInstance();
    Solution sol_new(this);

    // Generate random number between half and the total number of vehicle - 1
    // cambiar 
//    int numVehiculos = rnd->nextInt((this->num_Vehicles - 1) / 2) + ((this->num_Vehicles - 1) / 2) + 1;
    //En las instacias benchmark se deben usar los vehiculos indicados en la instancia(anotar cuales son)
  int  numVehiculos = this->num_Vehicles;
    // numVehiculos = 5;
  
    std::wstring mensaje = L"Valor de n: " + std::to_wstring(numVehiculos) + L"\n";

 
    OutputDebugStringW(mensaje.c_str());



    //int numVehiculos = this->num_Vehicles;
    //numVehiculos = 4;

    do {
        //  create a structure with the size of customers
        int* conjuntoClientes = new int[this->num_Customers];
        for (int i = 0; i < this->num_Customers; i++) {
            conjuntoClientes[i] = i + 1;
        }

        // create a random permutacion of customers
        int* auxPerm = new int[this->num_Customers];
        int clientesRestantes = this->num_Customers;
        for (int i = 0; i < this->num_Customers; i++) {
            int indice = rnd->nextInt(clientesRestantes - 1);
            auxPerm[i] = conjuntoClientes[indice];
            conjuntoClientes[indice] = conjuntoClientes[clientesRestantes - 1];
            clientesRestantes--;
       /*     cout << auxPerm[i]<<" ";*/
        }
       /* exit(0);*/
        // create a strucutre for the solucion
        int tamSol = this->numberOfVariables_;
        int* solArray = new int[tamSol];

        // INitialize the array with -1
        std::memset(solArray, -1, tamSol * sizeof(int));

        solArray[0] = auxPerm[0]; // the first element is always a customer. 
        int clientesInsertados = 1;
        int cerosInsertados = 0;
     //   int clientesDesdeUltimoCero = 1; // Contador de clientes desde el último 0

        double currentEnergyConsumption = 0.0;

        int currentDemandVehicle = 0;


        int nodoAnteriorVisitado = auxPerm[0];

        currentEnergyConsumption += cost_Matrix[depot][auxPerm[0]] * this->consumption_Rate;

        currentDemandVehicle += customer_Demand[auxPerm[0]];


        int stationToGo = -2;

        int vehiculosUSADOS = 1;
 
        for (int i = 1; i < tamSol; i++)
{
    if (clientesInsertados == this->num_Customers) {
        break; // Ya insertamos todos los clientes, salimos del ciclo
    }

    int clienteActual = auxPerm[clientesInsertados];
    double energiaNecesaria = cost_Matrix[nodoAnteriorVisitado][clienteActual] * this->consumption_Rate;

    // Verificamos si la energía para ir al siguiente cliente excede la capacidad
    if ((currentEnergyConsumption + energiaNecesaria) > this->energy_Capacity) {
        // Buscar estación más cercana
        stationToGo = encontrarEstacionCercana(nodoAnteriorVisitado);
        double energiaAEstacion = cost_Matrix[nodoAnteriorVisitado][stationToGo] * this->consumption_Rate;

        if ((currentEnergyConsumption + energiaAEstacion) > this->energy_Capacity) {
            // No alcanza para ir a la estación más cercana → intentamos cerrar ruta actual pero debe haber energia para cerrar la ruta
            if ((vehiculosUSADOS + 1 <= this->num_Vehicles) &&
                (currentEnergyConsumption + (cost_Matrix[nodoAnteriorVisitado][this->depot] * this->consumption_Rate)) <= this->energy_Capacity) {
                
                solArray[i] = 0; // Regresamos al depósito
                currentEnergyConsumption = 0.0;
                currentDemandVehicle = 0;
                nodoAnteriorVisitado = this->depot;
                vehiculosUSADOS++;
            }
            else {
                break; // No se puede alcanzar la estación ni iniciar nueva ruta
            }
        }
        else {
            // Insertamos estación de recarga
            solArray[i] = stationToGo;
            nodoAnteriorVisitado = stationToGo;
            currentEnergyConsumption = 0.0;
        }
    }
    else {
        int demandaCliente = this->customer_Demand[clienteActual];

        // Verificamos si la carga total excede la capacidad del vehículo
        if ((currentDemandVehicle + demandaCliente) > this->max_Capacity) {
            // Intentamos cerrar ruta actual, debe haber energia para regresar al deposito, y debe haber un vehiculo disponible para la siguiente ruta
            if ((vehiculosUSADOS + 1 <= this->num_Vehicles) &&
                (currentEnergyConsumption + (cost_Matrix[nodoAnteriorVisitado][this->depot] * this->consumption_Rate)) <= this->energy_Capacity) {
                
                solArray[i] = 0; // Insertamos regreso al depósito
                currentEnergyConsumption = 0.0;
                currentDemandVehicle = 0;
                nodoAnteriorVisitado = this->depot;
                vehiculosUSADOS++;
            }
            else {
                break; // No se puede insertar por falta de vehículos o energía
            }
        }
        else {
            // Insertamos el cliente normalmente
            solArray[i] = clienteActual;
            currentEnergyConsumption += energiaNecesaria;
            currentDemandVehicle += demandaCliente;
            nodoAnteriorVisitado = clienteActual;
            clientesInsertados++;
        }
    }
}

 

        //// Asegurar que el último elemento sea 0 si hay vehículos sin usar
        //if (cerosInsertados < numVehiculos - 1) {
        //    solArray[tamSol - 1] = 0;
        //}
        
        // Guardar solución en sol_new
        for (int i = 0; i < tamSol; ++i) {
            sol_new.setVariableValue(i, solArray[i]);
         /*   cout << solArray[i] << " ";*/
        }
        /*cout << endl;*/
        this->evaluate(&sol_new);
        this->evaluateConstraints(&sol_new);

        //// Si hay restricciones violadas, intentar con más vehículos
        //if (sol_new.getNumberOfViolatedConstraints() > 0 && numVehiculos < this->num_Vehicles) {
        //    numVehiculos++;
        //}


        delete[] conjuntoClientes;
        delete[] auxPerm;
        delete[] solArray;

    } while (sol_new.getNumberOfViolatedConstraints() > 0);

    //cout << endl;
    this->evaluate(&sol_new);
    this->evaluateConstraints(&sol_new);

    return sol_new;
}

