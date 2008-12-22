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

	////效率较低，获得Item的表，无需访问支持度，传入时候不对output进行清空
	//void getHeader(std::vector<::Item>& output,bool bSortedBySupport =false) const;

	////效率较低，获得Item的表，需要访问支持度，并可以选择根据支持度排降序，传入时候不对output进行清空
	//void getHeader(std::vector<Item_Support>& output,bool bSortedBySupport =false) const;

	////效率较低，获得Item的表，需要访问支持度，根据Item自然排序，并能快速定位，传入时候不对output进行清空
	//void getHeader(std::map<::Item,int>& output) const;

	//效率较高，获得头表，内容是ItemNode,根据的是Item的编号排序，而非支持度
	const std::set<ItemNode>& getHeader() const { return header;};

	//void removeItem(::Item item);

	int getSupport(::Item prefix,ClassLabel label) const;

	//void createConditionalFPTree(const FPTreeEx& parent,Item prefix,int nMinSupport);

	//使用parent的支持度做投影
	//void createConditionalFPTree(const FPTreeEx& parent,Item prefix);

	void createFromTrDB(const TrDB& trdb,int nMinSupport);

	void createFromTrDB(const TrDB& trdb);

	void printOnConsole() const;

	void getPotentialPrefix(::Item item,std::vector<ItemSet_Support>& out) const;

protected:



	int processItems(const ::Transaction *t, int times = 1);

	int processTransaction(std::vector<const fptree::Item *> *t,int times = 1);

	void printNode(const fptree::Item& item, int level) const;

	/*int generateTransaction(fptree::Transaction* parent,int nParentSup,
		fptree::Item_& itemNode,std::vector<fptree::Transaction*>& result,int maxItemSize);*/


	void createFromSourceTrDB(const TrDB& trdb,int nMinSupport);

	void createFromConditionalDB(const TrDB& trdb,int nMinSupport);

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
