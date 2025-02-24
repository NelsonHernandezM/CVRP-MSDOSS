//08/02/2025


#include <iostream>
#include <tools/AlgorithmBuilder.h>
#include "miCVRP.h"
#include<tools/builders/ProblemBuilder.h>
#include "miGenetico.h"


int main()
{
    for (int i = 0; i < 10; i++)
    {


    AlgorithmBuilder::add("miGenetico", new miGenetico());


         ProblemBuilder::add("miCVRP", new miCVRP());
        Algorithm* alg = AlgorithmBuilder::execute("_INPUT/config_GA.txt");




        alg->execute();
        cout << *alg->getLastB() << endl;

    }

}

 