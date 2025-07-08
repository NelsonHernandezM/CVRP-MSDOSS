
#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include "../WindowsRequirements.h"


class miCEVRP : public Problem {
	Requirements* config;
	Parameters* par;


	int optimal;
	int num_Vehicles;
	int dimension;
	int num_stations;
	int max_Capacity;
	 
	double energy_Capacity;
	double consumption_Rate;
	int** coords;
	int* customer_Demand;
	int* stations;
	int depot;
	int num_Customers;

	int** cost_Matrix;

	int* tipoNodo; // 0 = depot, 1 = customer, 2 = station






public:
	miCEVRP();
	 
	void initialize(Requirements* config) override;
	void evaluate(Solution* s) override;
	void evaluateConstraints(Solution* s) override;

	Solution generateRandomSolution();
	bool isStation(int nodo);
	bool isDepot(int nodo);
	bool isCustomer(int nodo);
	int encontrarEstacionCercana(int nodo);

	 
};
