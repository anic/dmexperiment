// DataMiningExperiment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <time.h>
#include <fstream>
using namespace std;

#include "FPTree/data.h"

#include "TrDB.h"
#include "DDPMineAlgorithm.h"
#include "HarmonyAlgorithm.h"
#include "DMAlgorithm.h"
#include "FPTreeEx.h"

void checkResult(const Result& result,const TrDB& trdb)
{
	for(Result::const_iterator iter = result.begin();
		iter!=result.end();++iter)
	{ 
		if (iter->id >0)
		{
			int sup = 0;
			for(TransactionSet::const_iterator iterTrans = trdb.getTransaction().begin();
				iterTrans!= trdb.getTransaction().end();
				++iterTrans)
			{

				ItemSet temp;
				if ((*iterTrans)->label == iter->head)
				{
					::set_intersection(
						iter->body.begin(),
						iter->body.end(),
						(*iterTrans)->items.begin(),
						(*iterTrans)->items.end(),
						std::insert_iterator<ItemSet>(temp, temp.begin() ));
					if(temp.size() == iter->body.size())
						sup++;
				}
			}
			
			if (sup != iter->support)
				cout<<"error!"<<endl;
		} 
	}

}

void output(DMAlgorithm& algorithm,const TrDB& trdb,int nMinSupport)
{
	DWORD start = GetTickCount();
	if(algorithm.execute(trdb,nMinSupport)) //运行算法
	{
		DWORD end = GetTickCount();
		std::cout<<"Algorithm executes "<<(end - start)<<"ms"<<std::endl;
		const Result& result = algorithm.getResult();
		//遍历结果
		std::ofstream ofs("rule.txt");
		for(Result::const_iterator iter = result.begin();
			iter!=result.end();++iter)
		{ 
			if (iter->id <0 )
				continue;

			ofs<<iter->head<<" ";
			for(ItemSet::const_iterator ibody = iter->body.begin();
				ibody !=iter->body.end();
				++ibody)
				ofs<<*ibody<<":1 ";

			ofs<<"\r\n";

		}
		//checkResult(result,trdb);
		ofs.close();
	} 
}

int _tmain(int argc, _TCHAR* argv[])
{

	//创建了数据库，作为测试，读取10行
	int len = -1;
	int minsup = 50;
	std::string filename = "UKchess_train_10_1.dat";
	
	/*以下这段请保留不要删除，作为正式版本需要根据读入字符串处理*/
	//if (argc>=2 )
	//	filename = argv[1];
	//if (argc>=3)
	//	minsup = atoi(argv[2]);

	TrDB trdb;
	trdb.createFromFile(filename,len);
	trdb.setMinSupport(minsup);

	DDPMineAlgorithm ddp;
	HarmonyAlgorithm har;

	//output(ddp,trdb,2);
	output(har,trdb,minsup);

	return 0;
}

