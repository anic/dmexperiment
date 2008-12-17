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

	//���Item�ı��������֧�ֶ�
	void getHeader(std::vector<::Item>& output,bool bSortedBySupport =false) const;

	//���Item�ı���Ҫ����֧�ֶȣ�������ѡ�����֧�ֶ��Ž���
	void getHeader(std::vector<Item_Support>& output,bool bSortedBySupport =false) const;

	//���Item�ı���Ҫ����֧�ֶȣ�����Item��Ȼ���򣬲��ܿ��ٶ�λ
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
