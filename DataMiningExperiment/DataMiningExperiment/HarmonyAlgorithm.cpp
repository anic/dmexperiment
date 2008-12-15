                                                                     
                                                                     
                                                                     
                                             
#include "StdAfx.h"
#include "HarmonyAlgorithm.h"


HarmonyAlgorithm::HarmonyAlgorithm(void)
{
}

HarmonyAlgorithm::~HarmonyAlgorithm(void)
{
}

bool HarmonyAlgorithm::execute(const TrDB& trdb,int nSupport)
{
	//算法结果放在m_result中
	initHCCR(trdb);
	ruleminer(trdb,nSupport,CRA);
	return true;
}
void HarmonyAlgorithm::initHCCR(const TrDB& trdb)
{
	for(TransactionSet::const_iterator iter = trdb.getTransaction().begin(); iter != trdb.getTransaction().end(); ++iter)
	{	
		HCCR hccr((*iter).id, (*iter).label);
		_hccrSet.push_back(hccr);
	}
}
void HarmonyAlgorithm::ruleminer(const TrDB& trdb,int min_sup, ESortAlg sortAlg)
{
	if(!(trdb.getPrefix().empty()))	
	{
		//reset HCCCR of each transaction
		for(HCCRSet::iterator iter = _hccrSet.begin(); iter != _hccrSet.end(); ++iter)
		{
			//create a new rule
			Rule r(m_result.size());
			r.body = trdb.getPrefix(); //空间释放问题, to cfj:这里会拷贝一份数据
			r.head = (*iter)._cid;			
			r.support = trdb.getTransaction().size();
			if(r.support > 0)
			{
				//pending
				double conf = (trdb.getSupport(r.head) + 0.0)/r.support;
				r.confidence = conf;			
			}			
			if((*iter)._hConf < r.confidence)
			{
				m_result.push_back(r);
				(*iter)._rid = r.id;
				(*iter)._hConf = r.confidence;
			}
		}
	}
	//
	//prune support_equivalence
	int supP = trdb.getTransaction().size();
	ItemMap itTable = trdb.getItemTable();		
	//debugging
	for(ItemMap::iterator it = itTable.begin(); it!= itTable.end();)
	{
		if(it->second.size() == supP)
		{
			itTable.erase(it++);
		}
		else 
			++it;
	}
	//prune unpromising 
	for(ItemMap::iterator iter = itTable.begin(); iter!= itTable.end(); iter )
	{
		TransactionIndexList::iterator it = iter->second.begin();
		for( ; it!= iter->second.end(); ++it)
		{
			//int tid = iter->second.at(it);
			double upSupPx = trdb.getSupport(iter->first,_hccrSet[*it]._cid);
			upSupPx = (upSupPx + 0.0)/min_sup;
			if(upSupPx >1)
				upSupPx = 1;
			if(_hccrSet[*it]._hConf < upSupPx)
				break;					
		}
		if( it == iter->second.end())
		{
			itTable.erase(iter++);			
		}
		else 
			++iter;
	}
	//
	if(!(itTable.empty()))
	{
		bool prunned = true;
		for(ClassMap::const_iterator iter = trdb.getClassTable().begin(); iter != trdb.getClassTable().end(); ++iter)
		{
			//ClassLabel cl = iter->first
			double upSupP = trdb.getSupport(iter->first);
			upSupP = upSupP/min_sup;
			if(upSupP > 1)
				upSupP = 1;
			TransactionIndexList::const_iterator it = iter->second.begin();
			for(; it != iter->second.end(); ++it)
			{
				//int tid = iter->second.at(it);
				if(_hccrSet[*it]._hConf < upSupP)
				{
					prunned = false;
					break;
				}
			}
			if(prunned == false)
				break;
		}
		if(prunned == true)
			return;
		ItemCRTable itCRTable;
		switch(sortAlg)
		{
		case MCD:
			{
				setItCRTalbeMCD(trdb,itTable,itCRTable);
				break;
			}
		case EA:
			{
				setItCRTalbeEA(trdb,itTable,itCRTable);
				break;
			}
		default:// CRA:
			{
				setItCRTalbeCRA(trdb,itTable,itCRTable);
				break;
			}
		}
		/*ItemCRTable test;
		test.push_back(::make_pair(2,89));
		test.push_back(::make_pair(22,839));
		test.push_back(::make_pair(21,8));
		sort(test.begin(),test.end(), ItemCRCompare());*/

		sort(itCRTable.begin(),itCRTable.end(), ItemCRCompare());  
		for(ItemCRTable::iterator it =  itCRTable.begin(); it!= itCRTable.end(); it++)
		{
			TrDB cdb;
			cdb.createConditionalDB(trdb, it->first, min_sup);
			ruleminer(cdb, min_sup,sortAlg);
		}
	}
}
bool HarmonyAlgorithm::setItCRTalbeMCD(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable)
{
	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); iter++)
	{			
		double maxProC= 0.0;
		int supPx =iter->second.size();
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ic++)
		{			
			double supPxC = trdb.getSupport(iter->first, ic->first);
			double temp = supPxC/supPx;
			if(temp > maxProC)
				maxProC = temp;		
		}
		itCRTable.push_back(::make_pair(iter->first, -maxProC)); //按-maxProC的升序排列
	}
	return true;
}
bool HarmonyAlgorithm::setItCRTalbeEA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable)
{
	int k = trdb.getClassTable().size();
	double proPxC = 0.0;
	double entropy = 0.0;
	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); iter++)
	{		
		int supPx =iter->second.size();
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			double supPxC = trdb.getSupport(iter->first, ic->first);
			proPxC = supPxC/supPx;
			entropy +=log(proPxC)*(-proPxC);			
		}
		itCRTable.push_back(::make_pair(iter->first, entropy));
	}
	return true;

}
bool HarmonyAlgorithm::setItCRTalbeCRA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable)
{
	double mean_supP = 0.0;
	double mean_supPx = 0.0;
	int k =trdb.getClassTable().size();
	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); iter++)
	{		
		mean_supPx = (iter->second.size() +0.0)/k;
		int supPC = 0;
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			supPC += ic->second.size();			
		}
		mean_supP = (supPC+0.0)/k;

		int supPxC = 0;
		double temp = 0;
		int temp2 = 0;
		double sigmaPx = 0.0;
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			supPC = ic->second.size();
			supPxC = trdb.getSupport(iter->first, ic->first);
			temp += (supPC*supPxC - mean_supP*mean_supPx);
			temp2 += supPxC*supPxC;
		}
		sigmaPx =  (temp2+0.0)/k - mean_supPx*mean_supPx;
		if(sigmaPx > 0)
		{
			sigmaPx = sqrt(sigmaPx);
			itCRTable.push_back(::make_pair(iter->first,temp/sigmaPx));
		}
		else if(sigmaPx == 0)
		{
			itCRTable.push_back(::make_pair(iter->first,1000000));
		}
		else	
			return false;
		
	}
	return true;
}

