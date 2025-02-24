
#include "tools/RandomNumber.h"
#include "solutions/SolutionSet.h"
#include "problems/Problem.h"
#include "tools/Requirements.h"
#include "../WindowsRequirements.h"


class miCVRP : public Problem {
	Requirements* config;
	Parameters* par;

	int num_Customers;
	int num_Vehicles;
	int max_Capacity;
	int** adj_Matrix;
	int** cost_Matrix;
	int* customer_Demand;




public:
	miCVRP();
	void initialize(Requirements* config) override;
	void evaluate(Solution*  s) override;
	void evaluateConstraints(Solution* s) override;
	Solution generateRandomSolution();
};
