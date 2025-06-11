//08/02/2025


#include <iostream>
#include <tools/AlgorithmBuilder.h>
#include "miCVRP.h"
#include<tools/builders/ProblemBuilder.h>
#include <tools/builders/MutationBuilder.h>
#include "miGenetico.h"
#include "miMutacion.h"
#include <chrono>
#include <iostream>
#include <string>
#include <random>
using namespace std;
#include <cstdlib>  // rand, srand
 
int main()
{
    std::mt19937 rng(320);  // Semilla fija
    srand(320);  // Semilla fija
    RandomNumber* rnd = RandomNumber::getInstance();
           rnd->setSeed(320);

    for (int i = 0; i < 15; i++)
    {
         
        //auto start = std::chrono::high_resolution_clock::now(); // Inicio del tiempo

        cout << i << endl;

        MutationBuilder::add("miMutacion", new miMutacion());

        AlgorithmBuilder::add("miGenetico", new miGenetico());


        ProblemBuilder::add("miCVRP", new miCVRP());

//      Algorithm* alg = AlgorithmBuilder::execute("_INPUT/config_GA.txt");

        Algorithm* alg = AlgorithmBuilder::execute("_INPUT/vrpGA.txt");
       //Algorithm* alg = AlgorithmBuilder::execute("_INPUT/vrpGA2.txt");
        alg->execute();
        ofstream out("Salida-CVRP.txt");
        SolutionSet res = alg->getSolutionSet();

        out << res;
        out.close();
      
        cout << *alg->getLastB() << endl;

       

      /*  auto end = std::chrono::high_resolution_clock::now();  
        std::chrono::duration<double> duration = end - start; 
        std::cout << "Iteración " << i << " tiempo: " << duration.count() << " segundos" << std::endl;*/

       
    }

}

 