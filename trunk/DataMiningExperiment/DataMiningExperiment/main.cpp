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

/*
dataset minsup k output
dataset - ���ݼ�
minsup - ��С���֧�ֶ�
k - Ϊÿ��ʵ������ھ�Ĺ�����Ŀ
output - ����Ĺ����ļ�
*/
int _tmain(int argc, _TCHAR* argv[])
{

	//���������ݿ⣬��Ϊ���ԣ���ȡ10��
	int len = -1;
	unsigned int minsup = 0;
	std::string filename,rulefile;
	int type = 1;
	int bWriteLabel = 2; //0��ʾĬ��д0��1 ��ʾд��ǩ��2��ʾ��д��3��ʾ1�ĳ�-1��2�ĳ�1
	if (argc>=5)
	{
		filename = argv[1];
		minsup = atoi(argv[2]);
		rulefile = argv[4];
		if (argc >=6 )
			type = atoi(argv[5]);
		else
			type = 1;

		if (argc >= 7)
			bWriteLabel = atoi(argv[6]);
	}
	
	/*���Դ���*/
	/*minsup = 0;
	len = 10;*/
	
	
	
	DWORD start = GetTickCount();
	TrDB trdb;
	/*�������ݿ�*/
	trdb.createFromFile(filename,len);
	trdb.setMinSupport(minsup);
	DWORD end = GetTickCount();

	std::cout<<"DB created using "<<(end - start)<<"ms"<<std::endl;


	//TrDB cdb;
	//cdb.createConditionalDB(trdb,3,trdb.getMinSupport());
	//
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
	if(algorithm.execute(trdb,nMinSupport)) //�����㷨
	{
		DWORD end = GetTickCount();
		std::cout<<"Algorithm executes "<<(end - start)<<"ms"<<std::endl;
		const Result& result = algorithm.getResult();
		//�������
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
			else
			{
				//��д
			}
				

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
