#pragma once
#include "FPTree/fptree.h"
#include "common.h"
#include "Trdb.h"

#include <set>
#include <vector>

class FPTreeEx:public fptree::FPtree
{
public:
	FPTreeEx(void);
	~FPTreeEx(void);

	int getMinSupport()const{ return minsup;}

	void getHeader(std::vector<::Item>& output,bool bSortedBySupport =false) const;

	int getSupport(::Item prefix,ClassLabel label) const;

	void createConditionalFPTree(const FPTreeEx& parent,::Item prefix,int nMinSupport);

	void createConditionalFPTree(const FPTreeEx& parent,::Item prefix);

	void createFromTrDB(const TrDB& trdb,int nMinSupport);

	void createFromTrDB(const TrDB& trdb);

	
};
