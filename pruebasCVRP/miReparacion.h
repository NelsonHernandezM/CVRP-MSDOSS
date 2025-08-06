#pragma once

#include "tools/builders/RepairBuilder.h"
#include "../WindowsRequirements.h"


class miReparacion : public RepairOperator
{


public:
	virtual void execute(Solution sol);
	virtual void initialize(Requirements* config);


};

