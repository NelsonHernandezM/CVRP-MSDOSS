#pragma once
 

#include "../WindowsRequirements.h"
#include "./tools/operators/interval/ImprovementOperator.h"

class  miBusqueda : public ImprovementOperator {
	



public:
	miBusqueda();
	 
	void execute(Solution solucion);
 
	void initialize(Requirements* config) override;
};

