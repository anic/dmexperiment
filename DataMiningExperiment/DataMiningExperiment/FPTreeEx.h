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

typedef int ItemAlias; //��Ϊһ������

typedef fptree::Item ItemAliasNode;


class FPTreeEx:public fptree::FPtree
{
public:
	FPTreeEx(void);
	~FPTreeEx(void);

	int getMinSupport()const{ return minsup;}

	////Ч�ʽϵͣ����Item�ı��������֧�ֶȣ�����ʱ�򲻶�output�������
	//void getHeader(std::vector<::Item>& output,bool bSortedBySupport =false) const;

	////Ч�ʽϵͣ����Item�ı���Ҫ����֧�ֶȣ�������ѡ�����֧�ֶ��Ž��򣬴���ʱ�򲻶�output�������
	//void getHeader(std::vector<Item_Support>& output,bool bSortedBySupport =false) const;

	////Ч�ʽϵͣ����Item�ı���Ҫ����֧�ֶȣ�����Item��Ȼ���򣬲��ܿ��ٶ�λ������ʱ�򲻶�output�������
	//void getHeader(std::map<::Item,int>& output) const;

	//Ч�ʽϸߣ����ͷ��������ItemAlias
	const std::set<ItemAliasNode>& getHeader() const { return header;};

	void removeItemAlias(ItemAlias item);

	int getSupport(ItemAlias prefix,ClassLabel label) const;

	void createConditionalFPTree(const FPTreeEx& parent,ItemAlias prefix,int nMinSupport);

	//ʹ��parent��֧�ֶ���ͶӰ
	void createConditionalFPTree(const FPTreeEx& parent,ItemAlias prefix);

	void createFromTrDB(const TrDB& trdb,int nMinSupport);

	void createFromTrDB(const TrDB& trdb);

	void printOnConsole() const;

	//�ӻ������ԭ����Item����
	::Item getItem(ItemAlias alias)const { return remap[alias]; }

	//��Item���ֻ�û�����Ч�ʽϵ�
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
