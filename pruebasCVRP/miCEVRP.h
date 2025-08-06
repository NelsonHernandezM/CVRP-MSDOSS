
#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include "../WindowsRequirements.h"
#include <vector>
#include <cmath>

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

	//double** cost_Matrix;
	int** cost_Matrix;

	int* tipoNodo; // 0 = depot, 1 = customer, 2 = station






public:
	miCEVRP();
	 
	void initialize(Requirements* config) override;
	void evaluate(Solution* s) override;
	void evaluateConstraints(Solution* s) override;

	Solution generateRandomSolution();
	  bool isStation(int nodo);
	  bool esRutaFactiblePorEnergia(const std::vector<int>& ruta);
	  bool esRutaFactiblePorCapacidad(const std::vector<int>& ruta);
	  int evaluarRutaDistancia(const std::vector<int>& ruta);
	  double evaluarRutaEnergia(const std::vector<int>& ruta);
	  bool esRutaFactible(const std::vector<int>& ruta);
	  std::vector<std::vector<int>> separarSolucionPorRutas(Solution* s);
	bool isDepot(int nodo);
	bool isCustomer(int nodo);
	int encontrarEstacionCercana(int nodo);

	double getEnergyCapacity();
	int** getCostMatrix();
	double getConsumption_Rate();
	int* getCustomerDemand();
	int getMaxCapacity();
	int getNumberCustomers();
	int getNumVehicles();
	 
};
