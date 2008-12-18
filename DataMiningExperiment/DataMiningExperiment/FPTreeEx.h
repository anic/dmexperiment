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

typedef int ItemAlias; //作为一个化名

typedef fptree::Item ItemAliasNode;


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

	//效率较高，获得头表，内容是ItemAlias
	const std::set<ItemAliasNode>& getHeader() const { return header;};

	void removeItemAlias(ItemAlias item);

	int getSupport(ItemAlias prefix,ClassLabel label) const;

	void createConditionalFPTree(const FPTreeEx& parent,ItemAlias prefix,int nMinSupport);

	//使用parent的支持度做投影
	void createConditionalFPTree(const FPTreeEx& parent,ItemAlias prefix);

	void createFromTrDB(const TrDB& trdb,int nMinSupport);

	void createFromTrDB(const TrDB& trdb);

	void printOnConsole() const;

	//从化名获得原来的Item名字
	::Item getItem(ItemAlias alias)const { return remap[alias]; }

	//从Item名字获得化名，效率较低
	ItemAlias getItemAlias(::Item item)const { return getRemapIndex(item);}

protected:
	void printNode(const fptree::Item& item, int level) const;

	int getRemapIndex(::Item item) const ;
	
	int generateTransaction(fptree::Transaction* parent,int nParentSup,
		const fptree::Item& itemalias,std::vector<fptree::Transaction*>& result);

	void ReOrder(){ __super::ReOrder();}
	int Prune(){ return __super::Prune(); }
	void setOutput(FILE *of){ /*void*/;}
	void print(int *itemset, int il, int *comb, int cl, int support, int spos=0, int depth=0, int *current=0){}
	
	int processTransaction(fptree::Transaction *t, int times=1){return __super::processTransaction(t,times);}
	int processItems(fptree::Transaction *t, int times=1){return __super::processItems(t,times);}
	
	int grow(int *current, int depth){return 0;}
};
