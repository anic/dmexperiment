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

void checkResult(const Result& result,const TrDB& trdb);
void output(DMAlgorithm& algorithm,const TrDB& trdb,unsigned int nMinSupport,const char* fOutput,int writeLabel);
void removeRedundancy(Result& result);


/*
dataset minsup k output
dataset - 数据集
minsup - 最小相对支持度，0.1
k - 为每个实例最多挖掘的规则数目，默认1，需要填写
output - 输出的规则文件
type - 1表示Harmony;其他表示DDP
label - 0表示写0；1表示正常写；2表示不写；3表示1写成-1，其他写成1（2分用）
*/
int _tmain(int argc, _TCHAR* argv[])
{

	//创建了数据库，作为测试，读取10行

	int len = -1;
	double fsup = 0.1;

	std::string filename,rulefile;
	int type = 2;			//1表示Harmony;其他表示DDP

	int bWriteLabel = 2; //0表示默认写0，1 表示写标签，2表示不写，3表示1改成-1，2改成1
	if (argc>=5)
	{
		filename = argv[1];
		
		//相对支持度
		fsup = atof(argv[2]);

		rulefile = argv[4];
		if (argc >=6 )
			type = atoi(argv[5]);

		if (argc >= 7)
			bWriteLabel = atoi(argv[6]);
	}
	
	/*测试代码*/
	//minsup = 2;
	//len = 10;	
	
	DWORD start = GetTickCount();
	TrDB trdb;
	/*创建数据库*/
	trdb.createFromFile(filename,len);
	unsigned int minsup = (unsigned int)(fsup*trdb.getSize());
	trdb.setMinSupport(minsup);
	DWORD end = GetTickCount();

	std::cout<<"DB created using "<<(end - start)<<"ms"<<std::endl;

	//TrDB cdb1 = trdb;
	////cdb1.removeItem(3);
	//ItemSet items;
	//items.insert(2);
	//items.insert(3);
	//cdb1.removeItem(items);

	//ItemSet items2;
	//items2.insert(11);
	//items2.insert(12);
	//cdb1.removeItem(items2);

	//TrDB cdb;
	//cdb.createConditionalDB(trdb,3,trdb.getMinSupport());
	
	//FPTreeEx fptree;
	//fptree.createFromTrDB(cdb);
	//fptree.printOnConsole();
	
	/*
	std::vector<ItemSet_Support> out;
	fptree.getPotentialPrefix(60,out);*/

	if (type == 1)
	{
		HarmonyAlgorithm har;
		output(har,trdb,minsup,rulefile.c_str(),bWriteLabel);
	}
	else
	{
		DDPMineAlgorithm ddp;
		output(ddp,trdb,minsup,rulefile.c_str(),bWriteLabel);
	}

	return 0;
}

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

void output(DMAlgorithm& algorithm,const TrDB& trdb,unsigned int nMinSupport,const char* fOutput,int writeLabel)
{
	DWORD start = GetTickCount();
	if(algorithm.execute(trdb,nMinSupport)) //运行算法
	{
		DWORD end = GetTickCount();
		std::cout<<"Algorithm executes "<<(end - start)<<"ms"<<std::endl;
		Result& result = algorithm.getResult();

		//不写标签的时候去除重复的
		if (writeLabel == 2)
			removeRedundancy(result);

		//遍历结果
		std::ofstream ofs(fOutput);
		for(Result::const_iterator iter = result.begin();
			iter!=result.end();++iter)
		{ 
			if (iter->id <0 )
				continue;

			if (writeLabel == 0)
				ofs<<"0 ";
			else if(writeLabel == 1)
				ofs<<trdb.getClass(iter->head)<<" ";
			else if(writeLabel == 3)
			{
				if (trdb.getClass(iter->head) == 1)
					ofs<<"-1 ";
				else
					ofs<<"1 ";
			} 
			else if (writeLabel == 2)
			{
				//不写
			}
				

			for(ItemSet::const_iterator ibody = iter->body.begin();
				ibody !=iter->body.end();
				++ibody)
				
			{
				ofs<<*ibody;
				if (writeLabel == 2)
					ofs<<" ";
				else
					ofs<<":1 ";
			}
			ofs<<"\r\n";

		}
		//checkResult(result,trdb);
		ofs.close();
	} 
}

void removeRedundancy(Result& result)
{
	for(Result::iterator iter = result.begin();
		iter!=result.end();++iter)
	{
		if (iter->id != -1) //需要写
		{
			for(Result::iterator iter2 = iter + 1;iter2!=result.end();++iter2) //从下一个开始
			{
				if (iter2->id != -1 && iter2->body.size() == iter->body.size()) //可能出现重
				{
					ItemSet::const_iterator itemIter = iter->body.begin();
					ItemSet::const_iterator itemIter2 = iter2->body.begin();
					bool same = true;
					for(;itemIter!=iter->body.end();++itemIter,++itemIter2)
					{
						if (*itemIter != *itemIter2)
						{
							same = false;
							break;
						}
					}
					if (same)
						iter2->id = -1; //表示不写这个了，不区分类标签
				}
				
			}
		}
	}
}

