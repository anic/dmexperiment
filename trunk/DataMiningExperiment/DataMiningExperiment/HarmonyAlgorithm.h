#pragma once

#include "DMAlgorithm.h"


class HarmonyAlgorithm:
	public DMAlgorithm
{
public:
	HarmonyAlgorithm(void);
	~HarmonyAlgorithm(void);

	Result::const_iterator execute(const TrDB& trdb,int nSupport);
};
