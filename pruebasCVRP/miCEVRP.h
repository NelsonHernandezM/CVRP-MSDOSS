
#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include "../WindowsRequirements.h"


class miCEVRP : public Problem {
	Requirements* config;
	Parameters* par;
	 

	int optimal;
	int vehicles;
	int dimension;
	int num_stations;
	int capacity;
	int energy_capacity;
	double energy_Capacity;
	double consumption_Rate;
	int** coords;
	int* demands;
	int* stations;
	int depot;
	int num_Customers;

	int** cost_Matrix;



public:
	miCEVRP();
	void initialize(Requirements* config) override;
	void evaluate(Solution* s) override;
	void evaluateConstraints(Solution* s) override;
	Solution generateRandomSolution();
};
