#pragma once
#include "FPTree/fptree.h"
#include "common.h"
#include "Trdb.h"

#include <set>
#include <vector>


typedef fptree::Item ItemNode;
typedef struct ItemSet_Support
{
	ItemSet items;
	int support;
	ItemSet_Support(const ItemSet& s,int sup)
	{
		items.insert(s.begin(),s.end());
		support = sup;
	}
	ItemSet_Support(){}
}ItemSet_Support;

class FPTreeEx:public fptree::FPtree
{
public:
	FPTreeEx(void);
	~FPTreeEx(void);

	int getMinSupport()const{ return minsup;}

	//效率较高，获得头表，内容是ItemNode,根据的是Item的编号排序，而非支持度
	const std::set<ItemNode>& getHeader() const { return header;};

	
	int getSupport(::Item prefix,ClassLabel label) const;

	void createFromTrDB(const TrDB& trdb);

	void printOnConsole() const;

	void getPotentialPrefix(::Item item,std::vector<ItemSet_Support>& out) const;

protected:

	int processItems(const ItemMap& itemMap);

	int processItems(const ::Transaction *t, int times = 1);

	int processTransaction(std::vector<const fptree::Item *> *t,int times = 1);

	void printNode(const fptree::Item& item, int level) const;

	/*int generateTransaction(fptree::Transaction* parent,int nParentSup,
		fptree::Item_& itemNode,std::vector<fptree::Transaction*>& result,int maxItemSize);*/


	void createFromSourceTrDB(const TrDB& trdb);

	void createFromConditionalDB(const TrDB& trdb);

	//void ReOrder(){ __super::ReOrder();}
	//int Prune(){ return __super::Prune(); }
	//void setOutput(FILE *of){ /*void*/;}
	//void print(int *itemset, int il, int *comb, int cl, int support, int spos=0, int depth=0, int *current=0){}
	//
	//int processTransaction(fptree::Transaction *t, int times=1){return __super::processTransaction(t,times);}
	//int processItems(fptree::Transaction *t, int times=1){return __super::processItems(t,times);}
	//
	//int grow(int *current, int depth){return 0;}
};
