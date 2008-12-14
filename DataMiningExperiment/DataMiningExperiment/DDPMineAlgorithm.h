#pragma once

#include "DMAlgorithm.h"


class DDPMineAlgorithm :
	public DMAlgorithm
{
public:
	DDPMineAlgorithm(void);
	~DDPMineAlgorithm(void);

	Result::const_iterator execute(const TrDB& trdb,int nSupport);
};

