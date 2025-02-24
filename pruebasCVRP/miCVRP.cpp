 

#include "miCVRP.h"
#include <algorithm>
 
miCVRP::miCVRP() {}

void miCVRP::initialize(Requirements* config) {
  
    config->addValue("#NUM-CUSTOMERS", Constantes::INT);

    this->par = config->load();

    this->num_Customers = this->par->get("#NUM-CUSTOMERS").getInt();

    
    config->addValue("#NUM-VEHICLES", Constantes::INT);
    config->addValue("#MAX-CAPACITY", Constantes::INT);
    config->addMatrix ("#ADJ-MATRIX", Constantes::INT,this->num_Customers+1,this->num_Customers+1);
    config->addMatrix("#COST-MATRIX", Constantes::INT,this->num_Customers + 1,this->num_Customers + 1);
    config->addVector("#DEMANDS", Constantes::INT,this->num_Customers);

    this->par = config->load();
     
    this->num_Vehicles = this->par->get("#NUM-VEHICLES").getInt();
    this->max_Capacity = this->par->get("#MAX-CAPACITY").getInt();
    this->customer_Demand = (int*)this->par->get("#DEMANDS").getValue();
    this->adj_Matrix = (int**)this->par->get("#ADJ-MATRIX").getValue();
    this->cost_Matrix = (int**)this->par->get("#COST-MATRIX").getValue();

    //DEFINIR PAR METROS DE CONTROL
    this->numberOfVariables_ = (this->num_Customers+num_Vehicles);
    this->numberOfObjectives_ = 1;
    this->numberOfConstraints_ = 7;



    this->lowerBounds_ = new Interval[numberOfVariables_];
    this->upperBounds_ = new Interval[numberOfVariables_];

    for (int i = 0; i < this->numberOfVariables_; ++i) {
        this->lowerBounds_[i] = 0;
        this->upperBounds_[i] = num_Customers;
    }

    this->objs_type = new int[this->numberOfObjectives_];

    for (int obj = 0; obj < this->numberOfObjectives_; ++obj) {
        this->objs_type[obj] = Constantes::MINIMIZATION;
    }

     

}




 
    //aqui debo evaluar el fitness de la solucion por o que no importa si viola o no restricciones solo obtendre su fitness
void miCVRP::evaluate(Solution* s) {
    Interval* vars = s->getDecisionVariables();
    int distanciaTotal = 0;
    int nodoPrevio = 0;  // Comenzamos desde el depósito (nodo 0)

  /*  cout << "xxxxxxxxx" << endl;*/

    for (int i = 0; i < this->getNumberOfVariables(); i++) {
        int nodoActual = (int)vars[i].L;

        // Si el nodo es -1, lo ignoramos
        if (nodoActual == -1) {
            //cout << "Posición " << i << " vacía (-1), ignorando..." << endl;
            continue;
        }

        /*cout << "Nodo " << nodoActual << " en posición " << i << endl;*/

        if (nodoActual == 0) {
            // Si es un retorno al depósito, reiniciar el nodo previo
            nodoPrevio = 0;
        }
        else {
            // Agregar la distancia desde el nodo previo al nodo actual
            distanciaTotal += cost_Matrix[nodoPrevio][nodoActual];
           /* cout << "Distancia añadida: " << cost_Matrix[nodoPrevio][nodoActual]
                << " (" << nodoPrevio << " -> " << nodoActual << ")" << endl;*/

            // Actualizar nodo previo
            nodoPrevio = nodoActual;
        }
    }

  /*  cout << "Distancia total: " << distanciaTotal << endl;*/
   /* cout << "xxxxxxxxx" << endl;
    cout << "Terminé" << endl;*/

    s->setObjective(0, distanciaTotal);
}

void miCVRP::evaluateConstraints(Solution* s) {
    Interval* vars = s->getDecisionVariables();
    int* cargaRutas = new int[this->num_Vehicles](); // Initialize to 0

    int numberViolatedConstraints = 0;
    int totalofVi = 0;
    int currentVehicle = -1; // Track current vehicle

    for (int i = 0; i < this->getNumberOfVariables(); i++) {
        int nodoActual = (int)vars[i].L;

        if (nodoActual == 0) { // Depot indicates new vehicle
            if (currentVehicle >= 0) { // Check previous vehicle's capacity
                if (cargaRutas[currentVehicle] > max_Capacity) {
                    numberViolatedConstraints++;
                    totalofVi += (cargaRutas[currentVehicle] - max_Capacity);
                }
            }
            currentVehicle++;
            if (currentVehicle >= this->num_Vehicles) break; // Exceeded available vehicles
        }
        else if (nodoActual == -1) { // End of routes
            break;
        }
        else if (nodoActual > 0 && currentVehicle >= 0 && currentVehicle < this->num_Vehicles) {
            // Add customer demand to current vehicle
            cargaRutas[currentVehicle] += customer_Demand[nodoActual - 1];
        }
    }

    // Check last vehicle's capacity if not ended by -1
    if (currentVehicle >= 0 && currentVehicle < this->num_Vehicles && cargaRutas[currentVehicle] > max_Capacity) {
        numberViolatedConstraints++;
        totalofVi += (cargaRutas[currentVehicle] - max_Capacity);
    }

    for (int i = 0; i < this->getNumberOfVariables() - 1; i++) {
        int nodoActual = (int)vars[i].L;
        int nodoSiguiente = (int)vars[i + 1].L;

        if (nodoSiguiente == -1) {
            break;
        }

        if (adj_Matrix[nodoActual][nodoSiguiente] == 0) {
            numberViolatedConstraints++;

        }

    }


    s->setNumberOfViolatedConstraints(numberViolatedConstraints);
    s->setOverallConstraintViolation(-totalofVi);
   /* cout << "violated: " << numberViolatedConstraints << endl;*/

    delete[] cargaRutas; // Don't forget to free memory
}

Solution miCVRP::generateRandomSolution() {
    RandomNumber* rnd = rnd->getInstance();
    Solution sol_new(this);
    // Número de vehículos a utilizar (mínimo 1, máximo num_Vehicles)
    int numVehiculos = rnd->nextInt(this->num_Vehicles / 2) + (this->num_Vehicles / 2) + 1;
  do{
 
 
  /*  cout << "se seleccionaron " << numVehiculos<<endl;*/

    // Crear un conjunto de clientes numerados de 1 a num_Customers
    int* conjuntoClientes = new int[this->num_Customers];
    for (int i = 0; i < this->num_Customers; i++) {
        conjuntoClientes[i] = i + 1;
    }

    int* auxPerm = new int[this->num_Customers];
    int clientesRestantes = this->num_Customers;
    int clienteSeleccionado, indice;
    // Generar una permutación aleatoria de clientes
    for (int i = 0; i < this->num_Customers; i++) {
        indice = rnd->nextInt(clientesRestantes-1);
        clienteSeleccionado = conjuntoClientes[indice];
        auxPerm[i] = clienteSeleccionado;
        conjuntoClientes[indice] = conjuntoClientes[clientesRestantes - 1];
        clientesRestantes--;
        //cout << "Cliente seleccionado (auxPerm[" << i << "]): " << auxPerm[i] << endl;
    }



    // Crear el array auxiliar con clientes y ceros
    int tamSol = this->numberOfVariables_;
    int* solArray = new int[tamSol];

    // Inicializar todo el array a -1 para evitar valores basura
    memset(solArray, -1, tamSol * sizeof(int));

    solArray[0] = auxPerm[0]; // Primer cliente
    //cout << solArray[0] << "PORQUE" << endl;
    int clientesInsertados = 1;
    int cerosInsertados = 0;

    // Insertar ceros y clientes alternadamente
    for (int i = 1; i < tamSol; ++i) {
        if (cerosInsertados < numVehiculos - 1) {
            // Insertar cero si el anterior no es cero y aún hay vehículos
            if (solArray[i - 1] != 0) {
                solArray[i] = 0;
                cerosInsertados++;
            }
            else {
                // Insertar cliente si hay disponibles
                if (clientesInsertados < this->num_Customers) {
                    solArray[i] = auxPerm[clientesInsertados];
                    clientesInsertados++;
                }
            }
        }
        else {
            // Insertar clientes restantes
            if (clientesInsertados < this->num_Customers) {
                solArray[i] = auxPerm[clientesInsertados];
                clientesInsertados++;
            }
        }
        //cout << solArray[i] << "NOSE" << endl;
    }

    // Asegurar que el último elemento sea 0 si hay vehículos sin usar
    if (cerosInsertados < numVehiculos - 1) {
        solArray[tamSol - 1] = 0;
    }
    // Mostrar la solución generada
  //cout << "Solución generada: ";
    for (int i = 0; i < tamSol; ++i) {
      /* cout << solArray[i] << " ";*/
       sol_new.setVariableValue(i, solArray[i]);
    }
    this->evaluate(&sol_new); 
    this->evaluateConstraints(&sol_new);

    

  // cout << endl;
 


   
    if (sol_new.getNumberOfViolatedConstraints() > 0 && numVehiculos<this->num_Vehicles) {
        numVehiculos++;
    }
    // Liberar memoria
    delete[] conjuntoClientes;
    delete[] auxPerm;
    delete[] solArray;

  } while (sol_new.getNumberOfViolatedConstraints() > 0 && numVehiculos <= this->num_Vehicles);
  this->evaluate(&sol_new);
  this->evaluateConstraints(&sol_new);

    

 
   
    return sol_new;
}