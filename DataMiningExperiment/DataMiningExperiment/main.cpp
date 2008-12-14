// DataMiningExperiment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "TrDB.h"
#include "DDPMineAlgorithm.h"
#include "HarmonyAlgorithm.h"
#include "DMAlgorithm.h"


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

	//测试一下修改

	TrDB trdb;

	//创建了数据库，作为测试，读取10行
	trdb.createFromFile("mushroom.dat",10);

	int a = trdb.getSize();
	const Transaction& b = trdb.getTransactionByTid(2);
	const Transaction& c = trdb.getTransactionByTid(100);
	
	TrDB cdb;
	cdb.createConditionalDB(trdb,2,2);
	int d = cdb.getSupport(1);
	const TransactionIndexList& dd = cdb.getTransactionsByItem(3);
	int f = cdb.getSupport(3,2);
	const TransactionSet& trans = trdb.getTransaction();
	for(TransactionSet::const_iterator iter = trans.begin();
		iter!=trans.end();++iter)
	{
		const Transaction& tran = (*iter);
		std::cout<<tran.id<<std::endl;
	}

	DDPMineAlgorithm ddp;
	HarmonyAlgorithm har;

	output(ddp,trdb,2);
	output(har,trdb,2);
	return 0;
}

