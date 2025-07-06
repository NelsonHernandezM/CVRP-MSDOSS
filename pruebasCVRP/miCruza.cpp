#include "miCruza.h"
#pragma once

#include <cmath>

void miCruza::initialize(Requirements* config) {
    this->local_ini = false;
}

void miCruza::execute(SolutionSet parents, SolutionSet children) {
    RandomNumber* rnd = rnd->getInstance();

    Problem* p = parents.get(0).getProblem();

    int numVars = p->getNumberOfVariables();
    int corte = rnd->nextInt(numVars - 1); // corte aleatorio

    if (!this->local_ini) {
        y[0].initialize(p);
        y[1].initialize(p);
        y[2].initialize(p);
        y[3].initialize(p);

        h1_missing = new int[numVars];
        h2_missing = new int[numVars];

        this->local_ini = true;
    }

    y[0] = parents.get(0); // copia de p1
    y[1] = parents.get(1); // copia de p2

    int pos_p1, pos_p2;

    // Inicializar flags de elementos que deben insertarse
    for (int i = 0; i < numVars; ++i) {
        pos_p1 = parents.get(0).getVariableValue(i).L;
        pos_p2 = parents.get(1).getVariableValue(i).L;

        if (i > corte) {
            h1_missing[pos_p1] = 1;
            h2_missing[pos_p2] = 1;
        }
        else {
            h1_missing[pos_p1] = 0;
            h2_missing[pos_p2] = 0;
        }
    }

    int ini_p1 = corte + 1;
    int ini_p2 = corte + 1;

    for (int i = 0; i < numVars; ++i) {
        pos_p1 = parents.get(0).getVariableValue(i).L;
        pos_p2 = parents.get(1).getVariableValue(i).L;

        if (h1_missing[pos_p2] == 1) {
            y[0].setVariableValue(ini_p1, pos_p2);
            ++ini_p1;
        }

        if (h2_missing[pos_p1] == 1) {
            y[1].setVariableValue(ini_p2, pos_p1);
            ++ini_p2;
        }
    }

    children.set(0, y[0]);
    children.set(1, y[1]);
}