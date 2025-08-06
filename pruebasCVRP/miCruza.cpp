#include "miCruza.h"
#pragma once
#include "miCEVRP.h"
#include <cmath>

void miCruza::initialize(Requirements* config) {
    this->local_ini = false;
}

void miCruza::execute(SolutionSet parents, SolutionSet children) {
    RandomNumber* rnd = RandomNumber::getInstance();
    Problem* problema = parents.get(0).getProblem();
    miCEVRP* p = dynamic_cast<miCEVRP*>(problema);

    //Solution padre1 = parents.get(0);
    //Solution padre2 = parents.get(1);

    //Solution hijo1(problema);
    //Solution hijo2(problema);

    int n = parents.get(0).getNumVariables();

    // Extraer solo los clientes (ignorar estaciones, depósitos y -1)
    std::vector<int> clientesPadre1;
    std::vector<int> clientesPadre2;

    for (int i = 0; i < n; ++i) {
        int nodo1 = parents.get(0).getVariableValue(i).L;
        int nodo2 = parents.get(1).getVariableValue(i).L;

        if (p->isCustomer(nodo1)) clientesPadre1.push_back(nodo1);
        if (p->isCustomer(nodo2)) clientesPadre2.push_back(nodo2);
    }

    int tamClientes = clientesPadre1.size();
    int punto1 = rnd->nextInt(tamClientes - 1);
    int punto2 = rnd->nextInt(tamClientes - 1);

    if (punto1 > punto2) std::swap(punto1, punto2);

    std::vector<int> hijoClientes1(tamClientes, -1);
    std::vector<int> hijoClientes2(tamClientes, -1);

    for (int i = punto1; i <= punto2; i++) {
        hijoClientes1[i] = clientesPadre1[i];
        hijoClientes2[i] = clientesPadre2[i];
    }

    auto rellenarOX = [&](std::vector<int>& hijo, const std::vector<int>& padre) {
        int pos = (punto2 + 1) % tamClientes;
        for (int i = 0; i < tamClientes; ++i) {
            int valor = padre[(punto2 + 1 + i) % tamClientes];
            if (std::find(hijo.begin(), hijo.end(), valor) != hijo.end()) continue;
            while (hijo[pos] != -1) pos = (pos + 1) % tamClientes;
            hijo[pos] = valor;
        }
    };

    rellenarOX(hijoClientes1, clientesPadre2);
    rellenarOX(hijoClientes2, clientesPadre1);

    // Reconstruir solución completa (respetando la estructura original)
    int indiceCliente1 = 0;
    int indiceCliente2 = 0;

    for (int i = 0; i < n; ++i) {
        int nodoOriginal = parents.get(0).getVariableValue(i).L;

        if (p->isCustomer(nodoOriginal)) {
            children.get(0).setVariableValue(i, hijoClientes1[indiceCliente1++]);
            children.get(1).setVariableValue(i, hijoClientes2[indiceCliente2++]);
        }
        else {
            // Respetar depósitos, estaciones y -1
            children.get(0).setVariableValue(i, nodoOriginal);
            children.get(1).setVariableValue(i, nodoOriginal);
        }
    }

    children.set(0, children.get(0));
    children.set(1, children.get(1));
    //cout << "entrew" << endl;
}