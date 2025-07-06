#include "miCEVRP.h"
#include <algorithm>
#include <windows.h>
#include <string>
#include <cmath>  // para sqrt y pow



miCEVRP::miCEVRP() {

}




//double** getCostMatrix(int** coordenadas, int n) {
//
//    double** costMatrix = new double* [n];
//    for (int i = 0; i < n; i++) {
//        costMatrix[i] = new int[n];
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
//            costMatrix[i][j] =distancia;
//        }
//    }
//
//    return costMatrix;
//}

int** getCostMatrix(int** coordenadas, int n) {
    
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
        }
    }

    return costMatrix;
}

/////// GENERATE A METHOD FOR EVALUATE A ROUTE




void miCEVRP::initialize(Requirements* config) {
 
    config->addValue("#OPTIMAL", Constantes::INT);

    config->addValue("#VEHICLES", Constantes::INT);
    config->addValue("#DIMENSION", Constantes::INT);


    this->par = config->load();

    this->dimension = this->par->get("DIMENSION").getInt();

    config->addValue("#NUM-STATIONS", Constantes::INT);

    this->par = config->load();
    this->num_stations = this->par->get("NUM-STATIONS").getInt();

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
    this->vehicles = this->par->get("#VEHICLES").getInt();

    this->capacity = this->par->get("#CAPACITY").getInt();
    this->energy_Capacity = this->par->get("#ENERGY_CAPACITY").getInt();
    this->consumption_Rate = this->par->get("#CONSUMPTION_RATE").getDouble();
    this->coords = (int**)this->par->get("COORDS").getValue();
    this->demands = (int*)this->par->get("DEMANDS").getValue();
    this->stations = (int*)this->par->get("STATIONS").getValue();
    this->depot = this->par->get("DEPOT").getInt();
    this->cost_Matrix = (int**)getCostMatrix(coords,dimension);

    this->num_Customers = this->dimension - this->num_stations;




    //DEFINIR PAR METROS DE CONTROL
    this->numberOfVariables_ = (this->dimension+vehicles);
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




 
void miCEVRP::evaluate(Solution* s) {



 }

 


void miCEVRP::evaluateConstraints(Solution* s) {



}




Solution miCEVRP::generateRandomSolution() {
    RandomNumber* rnd = rnd->getInstance();
    Solution sol_new(this);

    return sol_new;

}