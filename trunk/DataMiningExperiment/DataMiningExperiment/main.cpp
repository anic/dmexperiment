// DataMiningExperiment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;
#include <time.h>
#include "FPTree/data.h"

#include "TrDB.h"
#include "DDPMineAlgorithm.h"
#include "HarmonyAlgorithm.h"
#include "DMAlgorithm.h"
#include "FPTreeEx.h"

void output(DMAlgorithm& algorithm,const TrDB& trdb,int nMinSupport)
{
	if(algorithm.execute(trdb,nMinSupport)) //运行算法
	{
		const Result& result = algorithm.getResult();
		//遍历结果
		for(Result::const_iterator iter = result.begin();
			iter!=result.end();++iter)
		{
			std::cout<<(*iter).id<<std::endl;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	//std::set<int> a,b;
	//a.insert(1);
	//a.insert(2);
	//a.insert(3);

	//b.insert(1);

	////std::cout<<()<<std::endl;
	//bool r = b<a;

	//b.insert(2);
	//r = b<a;

	//测试一下修改 ,提交    
	TrDB trdb;

	//创建了数据库，作为测试，读取10行
	trdb.createFromFile("mushroom.dat",10);

	//int added=0;
	//clock_t start;

	//FPTreeEx* fpt = new FPTreeEx();
	//fpt->setMinsup(4);
	////fpt->setOutput(out);

	//start = clock();
	//int tmin=1000000, tmax=0, ttotal=0, tnr=0;
	//const TransactionSet & data = trdb.getTransaction();
	//for(TransactionSet::const_iterator iter = data.begin();
	//	iter!=data.end();
	//	++iter)
	//{

	//	fptree::Transaction *t = new fptree::Transaction(iter->items.size());

	//	int i=0;
	//	for(ItemSet::const_iterator it = iter->items.begin();
	//		it !=iter->items.end();++it,++i)
	//		t->t[i] = *it;

	//	if(t->length) {
	//		fpt->processItems(t);
	//		ttotal += t->length;
	//		if(t->length < tmin) tmin = t->length;
	//		if(t->length > tmax) tmax = t->length;
	//	}
	//	delete t;
	//	tnr++;
	//}

	//cout << "items read [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

	//start = clock();
	//fpt->ReOrder();
	//fpt->Prune();
	//cout << "items reordered and pruned [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

	//start = clock();
	//for(TransactionSet::const_iterator iter = data.begin();
	//	iter!=data.end();
	//	++iter)
	//{
	//	fptree::Transaction *t = new fptree::Transaction(iter->items.size());
	//	int i=0;
	//	for(ItemSet::const_iterator it = iter->items.begin();
	//		it !=iter->items.end();++it,++i)
	//		t->t[i] = *it;


	//	vector<int> list;
	//	for(i=0; i<t->length; i++) {
	//		set<fptree::Element>::iterator it = fpt->relist->find(fptree::Element(t->t[i],0));
	//		if(it!=fpt->relist->end()) list.push_back(it->id);
	//	}
	//	int size=list.size();
	//	sort(list.begin(), list.end());
	//	delete t;

	//	t = new fptree::Transaction(size);
	//	for(i=0; i<size; i++) t->t[i] = list[i];
	//	if(t->length) fpt->processTransaction(t);
	//	delete t;
	//}
	//cout << "FPtree constructed [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

	//start = clock();
	//int *tmp  = new int[100];
	//added = fpt->grow(tmp,1);
	//delete [] tmp;
	//delete [] fptree::FPtree::remap;
	//delete fptree::FPtree::relist;
	//cout << "Frequent sets generated [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;


	//int count = fpt->getSupport(3,0);
	//const std::set<Item>& header = fpt->getHeader();
	//const std::vector<Item>& sortedHeader = fpt->getSortedHeader();
	//delete fpt;


	//int a = trdb.getSize();
	//const Transaction& b = trdb.getTransactionByTid(2);
	//const Transaction& c = trdb.getTransactionByTid(100);

	//TrDB cdb;
	//cdb.createConditionalDB(trdb,2,2);
	//int d = cdb.getSupport(1);
	//const TransactionIndexList& dd = cdb.getTransactionsByItem(3);
	//int f = cdb.getSupport(3,2);
	//const TransactionSet& trans = trdb.getTransaction();
	//for(TransactionSet::const_iterator iter = trans.begin();
	//iter!=trans.end();++iter)
	//{
	//const Transaction& tran = (*iter);
	//std::cout<<tran.id<<std::endl;
	//}

	DDPMineAlgorithm ddp;
	HarmonyAlgorithm har;

	//output(ddp,trdb,2);
	output(har,trdb,7);

	return 0;
}

