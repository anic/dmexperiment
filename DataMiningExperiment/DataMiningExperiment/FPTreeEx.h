#pragma once
#include "FPTree/fptree.h"
#include "common.h"
#include "Trdb.h"

#include <set>
#include <vector>


typedef fptree::Item ItemNode;


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

	//Ч�ʽϸߣ����ͷ��������ItemNode,���ݵ���Item�ı�����򣬶���֧�ֶ�
	const std::set<ItemNode>& getHeader() const { return header;};

	void removeItem(::Item item);

	int getSupport(::Item prefix,ClassLabel label) const;

	void createConditionalFPTree(const FPTreeEx& parent,Item prefix,int nMinSupport);

	//ʹ��parent��֧�ֶ���ͶӰ
	void createConditionalFPTree(const FPTreeEx& parent,Item prefix);

	void createFromTrDB(const TrDB& trdb,int nMinSupport);

	void createFromTrDB(const TrDB& trdb);

	void printOnConsole() const;

protected:
	void printNode(const fptree::Item& item, int level) const;

	int generateTransaction(fptree::Transaction* parent,int nParentSup,
		fptree::Item_& itemNode,std::vector<fptree::Transaction*>& result,int maxItemSize);

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
