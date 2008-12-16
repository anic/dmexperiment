#pragma once

#include "DMAlgorithm.h"
#include <string>
class ResultWriter
{
public:
	ResultWriter(void);
	~ResultWriter(void);

	void WriteResult(const Result& result,std::string& output);
};
