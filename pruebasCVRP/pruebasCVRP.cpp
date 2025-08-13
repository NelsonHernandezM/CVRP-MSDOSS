//08/02/2025


#include <iostream>
#include <tools/AlgorithmBuilder.h>
#include "miCVRP.h"
#include "miCEVRP.h"
#include<tools/builders/ProblemBuilder.h>
#include <tools/builders/MutationBuilder.h>
#include <tools/builders/ImprovementBuilder.h>
#include <tools/builders/RepairBuilder.h>
#include "miReparacion.h"
#include "miBusqueda.h"
#include "miGenetico.h"
#include "miMutacion.h"
#include "miCruza.h"
#include "CVRP.h"
#include <chrono>
#include <iostream>
#include <string>
#include <random>
using namespace std;
#include <cstdlib>  // rand, srand
 
int main()
{
    std::mt19937 rng(310);  // Semilla fija
    srand(310);  // Semilla fija
    RandomNumber* rnd = RandomNumber::getInstance();
    rnd->setSeed(310);

    for (int i = 0; i < 1; i++)
    {
      
         
        auto start = std::chrono::high_resolution_clock::now(); // Inicio del tiempo

        cout << i << endl;
    
        
        CrossoverBuilder::add("miCruza", new miCruza());

        RepairBuilder::add("miReparacion", new miReparacion());

        MutationBuilder::add("miMutacion", new miMutacion());

        AlgorithmBuilder::add("miGenetico", new miGenetico());

        ImprovementBuilder::add("miBusqueda", new miBusqueda());

        ProblemBuilder::add("miCEVRP", new miCEVRP());

     Algorithm* alg = AlgorithmBuilder::execute("_INPUT-EVRP/config_GA.txt");

       // Algorithm* alg = AlgorithmBuilder::execute("_INPUT/vrpGA.txt");
       //Algorithm* alg = AlgorithmBuilder::execute("_INPUT/vrpGA2.txt");
        alg->execute();
        ofstream out("Salida-CVRP.txt");
        SolutionSet res = alg->getSolutionSet();

        out << res;
        out.close();
      
        cout << *alg->getLastB() << endl;
        cout << alg->getSolutionSet() <<endl;
        
        // Liberar memoria manualmente
       /* delete alg;*/
        

        auto end = std::chrono::high_resolution_clock::now();  
        std::chrono::duration<double> duration = end - start; 
        std::cout << "Iteración " << i << " tiempo: " << duration.count() << " segundos" << std::endl;

       
    }

}

 