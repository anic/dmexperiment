#pragma once
#include "FPTree/fptree.h"
#include "common.h"
#include "Trdb.h"

#include <set>
#include <vector>


typedef struct ITEM_SUPPORT
{
public:
	::Item item;
	int support;
	ITEM_SUPPORT(::Item i,int nSupport)
	{
		item = i;
		support = nSupport;
	}
}Item_Support;

class FPTreeEx:public fptree::FPtree
{
public:
	FPTreeEx(void);
	~FPTreeEx(void);

	int getMinSupport()const{ return minsup;}

	//获得Item的表，无需访问支持度
	void getHeader(std::vector<::Item>& output,bool bSortedBySupport =false) const;

	//获得Item的表，需要访问支持度，并可以选择根据支持度排降序
	void getHeader(std::vector<Item_Support>& output,bool bSortedBySupport =false) const;

	//获得Item的表，需要访问支持度，根据Item自然排序，并能快速定位
	void getHeader(std::map<::Item,int>& output) const;

	void removeItem(::Item item);

	int getSupport(::Item prefix,ClassLabel label) const;

	void createConditionalFPTree(const FPTreeEx& parent,::Item prefix,int nMinSupport);

	void createConditionalFPTree(const FPTreeEx& parent,::Item prefix);

	void createFromTrDB(const TrDB& trdb,int nMinSupport);

	void createFromTrDB(const TrDB& trdb);

	void printOnConsole() const;

protected:
	void FPTreeEx::printNode(const fptree::Item& item, int level) const;

	int getRemapIndex(::Item item) const ;
	//mutable std::set<int>
};
