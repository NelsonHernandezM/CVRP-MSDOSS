

#include "miCEVRP.h"
#include <algorithm>
#include <windows.h>
#include <string>
#include <iostream>
using namespace std;

miCEVRP::miCEVRP() {

}

// 0 = depot, 1 = customer, 2 = station
bool miCEVRP::isDepot(int nodo) {
    return this->tipoNodo[nodo] == 0;
}


bool  miCEVRP::isStation(int nodo) {
    return this->tipoNodo[nodo] == 2;
}



static int** getCostMatrix(int** coordenadas, int n) {
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
    config->addValue("#ENERGY_CAPACITY", Constantes::INT);
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
    this->energy_Capacity = this->par->get("#ENERGY_CAPACITY").getInt();
    this->consumption_Rate = this->par->get("#CONSUMPTION_RATE").getDouble();
    this->coords = (int**)this->par->get("#COORDS").getValue();
    
    this->customer_Demand = (int*)this->par->get("#DEMANDS").getValue();

    this->stations = (int*)this->par->get("#STATIONS").getValue();
    this->depot = this->par->get("#DEPOT").getInt();
    this->cost_Matrix = (int**)getCostMatrix(coords,dimension);

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
    this->numberOfVariables_ = (this->dimension+ num_Vehicles);
    this->numberOfObjectives_ = 1;
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
            /*
            if (!(cost_Matrix[nodoPrevio][nodoActual] == 100000)) {
                cout << "previo: " << nodoPrevio << "nodo ultimo: " << 0<<endl;
                cout << "sumando: " << cost_Matrix[nodoPrevio][0] << endl;
            }*/

            break;

        }

        /*cout << "Nodo " << nodoActual << " en posici�n " << i << endl;*/

        if (nodoActual == 0) {

            //if nodoActual is 0, means the end of that route, so we added the cost
            //from nodoPrevio to Depot.
            /*if (!(cost_Matrix[nodoPrevio][nodoActual] == 100000)) {
                cout << "nodo: " << nodoPrevio << "nodoActual: " << nodoActual << endl;
                cout << "sumando: " << cost_Matrix[nodoPrevio][nodoActual] << endl;
            }*/
            distanciaTotal += cost_Matrix[nodoPrevio][nodoActual];



            // reset nodoPrevio, we start from depot in the new route.
            nodoPrevio = 0;
        }
        else {
            // we add the cost travel from nodoPrevio to nodoActual 
            distanciaTotal += cost_Matrix[nodoPrevio][nodoActual];
            /* if (!(cost_Matrix[nodoPrevio][nodoActual] == 100000)) {
                 cout << "nodo: " << nodoPrevio << "nodoActual: " << nodoActual <<endl;
                 cout << "sumando: " << cost_Matrix[nodoPrevio][nodoActual] << endl;
             }*/
             /* cout << "Distancia a�adida: " << cost_Matrix[nodoPrevio][nodoActual]
                  << " (" << nodoPrevio << " -> " << nodoActual << ")" << endl;*/

                  // update nodoprevio
            nodoPrevio = nodoActual;
        }
    }
    //if (distanciaTotal < 2000) {
    //    cout << "Distancia total: " << distanciaTotal << endl;
    //} /* cout << "xxxxxxxxx" << endl;


    s->setObjective(0, distanciaTotal);
    //cout << endl;
}

void miCEVRP::evaluateConstraints(Solution* s) {

    Interval* vars = s->getDecisionVariables();
    int* cargaRutas = new int[this->num_Vehicles]();

    int numberViolatedConstraints = 0;
    int totalofVi = 0;
    int currentVehicle = 0;  // Inicializar en 0 (primera ruta inicia en 0)

    for (int i = 0; i < this->getNumberOfVariables(); i++) {
        int nodoActual = (int)vars[i].L;

        if (nodoActual == 0) {
            // Verificar capacidad del veh�culo ACTUAL ()
            if (cargaRutas[currentVehicle] > max_Capacity) {
                numberViolatedConstraints++;
                totalofVi += (cargaRutas[currentVehicle] - max_Capacity);
            }
            // Cambiar al siguiente veh�culo solo si hay dep�sito expl�cito
            currentVehicle++;
            if (currentVehicle >= this->num_Vehicles) {
                numberViolatedConstraints++;
                break;
            }
        }
        else if (nodoActual == -1) {
            // Verificar capacidad del veh�culo actual antes de terminar
            if (cargaRutas[currentVehicle] > max_Capacity) {
                numberViolatedConstraints++;
                totalofVi += (cargaRutas[currentVehicle] - max_Capacity);
            }
            break;
        }
        else if (nodoActual > 0 && currentVehicle < this->num_Vehicles) {
            // Sumar demanda al veh�culo actual (inicia en 0)
            cargaRutas[currentVehicle] += customer_Demand[nodoActual];
        }
    }

    // Verificar el �ltimo veh�culo (si no termin� en 0 o -1)
    if (currentVehicle < this->num_Vehicles && cargaRutas[currentVehicle] > max_Capacity) {
        numberViolatedConstraints++;
        totalofVi += (cargaRutas[currentVehicle] - max_Capacity);
    }




    for (int i = 0; i < this->getNumberOfVariables() - 1; i++) {
        int nodoActual = (int)vars[i].L;
        int nodoSiguiente = (int)vars[i + 1].L;

        //
        if (nodoSiguiente == -1) {
            break;
        }
        //if the arc does not exist ...
      /*  if (adj_Matrix[nodoActual][nodoSiguiente] == 0) {
            numberViolatedConstraints++;
        }*/

    }




    s->setNumberOfViolatedConstraints(numberViolatedConstraints);
    s->setOverallConstraintViolation(-totalofVi);
    /* cout << "violated: " << numberViolatedConstraints << endl;*/

    delete[] cargaRutas;
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
  int  numVehiculos = this->num_Vehicles;
    // numVehiculos = 5;
     // Convertir a cadena ancha
    std::wstring mensaje = L"Valor de n: " + std::to_wstring(numVehiculos) + L"\n";

    // Usar la versi�n wide (Unicode)
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
        }

        // create a strucutre for the solucion
        int tamSol = this->numberOfVariables_;
        int* solArray = new int[tamSol];

        // INitialize the array with -1
        memset(solArray, -1, tamSol * sizeof(int));

        solArray[0] = auxPerm[0]; // the first element is always a customer. 
        int clientesInsertados = 1;
        int cerosInsertados = 0;
        int clientesDesdeUltimoCero = 1; // Contador de clientes desde el �ltimo 0

        double currentEnergyConsumption = 0.0;

        currentEnergyConsumption += cost_Matrix[depot][auxPerm[0]] * this->consumption_Rate;

        int nodoAnteriorVisitado = auxPerm[0];

        int stationToGo = -2;

        int vehiculosUSADOS = 1;
 


        for (int i = 1; i < tamSol; i++)
        {

            if (clientesInsertados == this->num_Customers) {
                break; // Ya insertamos todos los clientes, salimos del ciclo
            }
             
            if ((currentEnergyConsumption + (cost_Matrix[nodoAnteriorVisitado][auxPerm[clientesInsertados]] * this->consumption_Rate)  ) > this->energy_Capacity) {
                
                // si la energiaconsumida, de la suma de la energia consumida actual mas la energia que consumira el ir al siguiente cliente fuera mayor que la capacidad de bateria del vehiculo es necesario agregar una estacion de recarga
                // O invalidar la solucion? 
                stationToGo = encontrarEstacionCercana(nodoAnteriorVisitado);

                if (currentEnergyConsumption + (cost_Matrix[nodoAnteriorVisitado][stationToGo] * this->consumption_Rate)  > this->energy_Capacity) {

                    //NO ES POSIBLE LLEGAR A LA ESTACION MAS CERCANA POR QUE NO ALCANZA LA BATERIA
                    // 
                    // 
                    //INSERTAR UN NUEVO VEHICULO
                    if (vehiculosUSADOS + 1 <= this->num_Vehicles) {

                        solArray[i] = 0;
                        currentEnergyConsumption = 0.0;
                        nodoAnteriorVisitado = this->depot;
                        vehiculosUSADOS++;

                    }
                    else {
                        break; //no se puede alcanzar la siguientes estacion de recarga ni iniciar una nueva ruta
                    }


                }
                else {
                    solArray[i] = stationToGo;
                    nodoAnteriorVisitado = stationToGo;
                    currentEnergyConsumption = 0.0;
                }


            }
            else {

                solArray[i] = auxPerm[clientesInsertados];
                currentEnergyConsumption += cost_Matrix[nodoAnteriorVisitado][auxPerm[clientesInsertados]] * this->consumption_Rate;
                nodoAnteriorVisitado = auxPerm[clientesInsertados];
                clientesInsertados++;

                
            }




            //cout << solArray[i] << " ";
        }


 

        //// Asegurar que el �ltimo elemento sea 0 si hay veh�culos sin usar
        //if (cerosInsertados < numVehiculos - 1) {
        //    solArray[tamSol - 1] = 0;
        //}
        
        // Guardar soluci�n en sol_new
        for (int i = 0; i < tamSol; ++i) {
            sol_new.setVariableValue(i, solArray[i]);
            cout << solArray[i] << " ";
        }
        cout << endl;
        this->evaluate(&sol_new);
        this->evaluateConstraints(&sol_new);

        //// Si hay restricciones violadas, intentar con m�s veh�culos
        //if (sol_new.getNumberOfViolatedConstraints() > 0 && numVehiculos < this->num_Vehicles) {
        //    numVehiculos++;
        //}


        delete[] conjuntoClientes;
        delete[] auxPerm;
        delete[] solArray;

    } while (sol_new.getNumberOfViolatedConstraints() > 0);

    cout << endl;
    this->evaluate(&sol_new);
    this->evaluateConstraints(&sol_new);

    return sol_new;
}

