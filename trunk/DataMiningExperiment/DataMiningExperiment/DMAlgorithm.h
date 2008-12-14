#ifndef _IDMALGORITHM_H
#define _IDMALOGRITHM_H
#include <vector>
#include "common.h"
#include "TrDB.h"

typedef std::vector<Rule> Result;

class DMAlgorithm
{
protected:
	Result m_result;
public:
	virtual Result::const_iterator execute(const TrDB& trdb,int nSupport) = 0;

};


#endif