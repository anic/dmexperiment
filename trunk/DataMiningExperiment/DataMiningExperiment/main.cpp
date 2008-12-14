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
	if(algorithm.execute(trdb,nMinSupport)) //�����㷨
	{
		const Result& result = algorithm.getResult();
		//�������
		for(Result::const_iterator iter = result.begin();
			iter!=result.end();++iter)
		{
			std::cout<<(*iter).id<<std::endl;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	TrDB trdb;
	//���������ݿ⣬��Ϊ���ԣ���ȡ10��
	trdb.createFromFile("mushroom.dat",10);

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

