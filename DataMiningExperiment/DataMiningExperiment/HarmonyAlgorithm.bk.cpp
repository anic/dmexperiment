                                                                     
                                                                     
                                                                     
                                             
#include "StdAfx.h"
#include "HarmonyAlgorithm.h"
#include <iostream>

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
	std::map<Item,bool> minedItemMap;
	ruleminer(trdb,nSupport,minedItemMap,CRA);
	cleanResult();
	return true;
}
void HarmonyAlgorithm::cleanResult()
{
	//to cya: 帮忙看看这个删除有什么问题，想把ir->numUsed <=0都删除。似乎ir会在erase之后连跳两个	
	Result::iterator ir = m_result.begin();
	while(ir!=m_result.end())
	{
		if(ir->numUsed <= 0)
		{
			ir->id = -1;
			++ir;
			//m_result.erase(ir++);
		}
		else
			++ir;
	}
}
void HarmonyAlgorithm::initHCCR(const TrDB& trdb)
{
	_hccrSet.reserve(trdb.getSize());
	for(TransactionSet::const_iterator iter = trdb.getTransaction().begin(); iter != trdb.getTransaction().end(); ++iter)
	{	
		HCCR hccr((*iter)->id, (*iter)->label);
		_hccrSet.push_back(hccr);
	}
}
void HarmonyAlgorithm::ruleminer(const TrDB& trdb,int min_sup, std::map<Item,bool>& minedItemMap, ESortAlg sortAlg)
{
	if(!(trdb.getPrefix().empty()))	
	{
		//reset HCCCR of each transaction
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic!=trdb.getClassTable().end();++ic)
		{
			//create a new rule
			Rule r(m_result.size());
			r.body = trdb.getPrefix(); //空间释放问题, to cfj:这里会拷贝一份数据
			r.head = (*ic).first;
			r.support = trdb.getSupport(r.head); //修改为get size
			if(r.support > 0)
			{				
				double conf = (r.support + 0.0)/trdb.getSize();
				r.confidence = conf;	
			}			
			bool isNewRule = true;
			for(TransactionIndexList::const_iterator tid = (*ic).second->begin(); tid != (*ic).second->end(); ++tid)
			{
				if( _hccrSet[*tid]._hConf < r.confidence )
				{					
					int oldRid = _hccrSet[*tid]._rid;
					if( oldRid >=0 && oldRid < m_result.size())
					{
						m_result.at(oldRid).numUsed --;
					}
					_hccrSet[*tid]._hConf = r.confidence;
					_hccrSet[*tid]._rid = r.id;					
					if(isNewRule)
					{
						r.numUsed++;
						m_result.push_back(r);
						isNewRule = false;
					}
					else
					{
						m_result.at(r.id).numUsed ++;
					}
				}
			}
				///*int oldRid = _hccrSet[*tid]._rid;
				//if(oldRid == -1 && _hccrSet[*tid]._hConf < r.confidence)
				//{
				//	_hccrSet[*tid]._hConf = r.confidence;
				//	_hccrSet[*tid]._rid = r.id;					
				//	if(isNewRule)
				//	{
				//		r.numUsed++;
				//		m_result.push_back(r);
				//		isNewRule = false;
				//	}
				//}
				//else if(_hccrSet[*tid]._hConf < r.confidence ||(_hccrSet[*tid]._hConf == r.confidence && m_result.at(oldRid).body.size() > r.body.size()))
				//{					
				//	if( oldRid >=0 && oldRid < m_result.size())
				//	{
				//		m_result.at(oldRid).numUsed --;
				//	}
				//	_hccrSet[*tid]._hConf = r.confidence;
				//	_hccrSet[*tid]._rid = r.id;					
				//	if(isNewRule)
				//	{
				//		r.numUsed++;
				//		m_result.push_back(r);
				//		isNewRule = false;
				//	}
				//	else
				//	{
				//		m_result.at(r.id).numUsed ++;
				//	}
				//}*/
			
		}
	}
	
	
	ItemMap itTable = trdb.getItemTable();	//效率比较低


	//prune already mined items
	//std::map<Item,bool> minedItemMap;
	if(!minedItemMap.empty())
	{
		for(ItemMap::iterator iter = itTable.begin(); iter!= itTable.end();)
		{
			std::map<Item,bool>::const_iterator  it= minedItemMap.find(iter->first);
			if( it != minedItemMap.end())
			{
				itTable.erase(iter++);
			}
			else 
				++iter;
		}
	}
	

	//prune support_equivalence
	int supP = trdb.getSize(); //修改为get size
	for(ItemMap::iterator iter = itTable.begin(); iter!= itTable.end();)
	{
		if(iter->second->size() == supP)
		{
			itTable.erase(iter++);
		}
		else 
			++iter;
	}
	if(!(trdb.getPrefix().empty()))	
	{
		//prune unpromising 
		for(ItemMap::iterator iter = itTable.begin(); iter!= itTable.end(); )
		{
			TransactionIndexList::iterator it = iter->second->begin();
			for( ; it!= iter->second->end(); ++it)
			{
				//int tid = iter->second.at(it);
				double upSupPx = trdb.getSupport(iter->first,_hccrSet[*it]._cid);
				if(upSupPx == 0)
					continue;
				if(min_sup == 0 && upSupPx != 0)
					upSupPx = 1;
				else
				{
					upSupPx = (upSupPx + 0.0)/min_sup;
					if(upSupPx >1)
						upSupPx = 1;
				}			
				if(_hccrSet[*it]._hConf < upSupPx)
					break;					
			}
			if( it == iter->second->end())
			{
				itTable.erase(iter++);			
			}
			else 
				++iter;
		}
	}
	//
	if(!(itTable.empty()))
	{
		if(!(trdb.getPrefix().empty()))	
		{
			bool prunned = true;
			for(ClassMap::const_iterator iter = trdb.getClassTable().begin(); iter != trdb.getClassTable().end(); ++iter)
			{
				//ClassLabel cl = iter->first
				double upSupP = trdb.getSupport(iter->first);
				upSupP = upSupP/min_sup;
				if(upSupP > 1)
					upSupP = 1;
				
				for(TransactionIndexList::const_iterator it = iter->second->begin()
					; it != iter->second->end(); ++it)
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
		}
		ItemCRVTable itCRVTable;
		switch(sortAlg)
		{
		case MCD:
			{
				setItCRVTableMCD(trdb,itTable,itCRVTable);
				break;
			}
		case EA:
			{
				setItCRVTableEA(trdb,itTable,itCRVTable);
				break;
			}
		default:
			{
				setItCRVTableCRA(trdb,itTable,itCRVTable);
				break;
			}
		}
		sort(itCRVTable.begin(),itCRVTable.end(), ItemCRVCompare());  
		std::map<Item,bool> newMinedItemMap;
		for(ItemCRVTable::iterator it =  itCRVTable.begin(); it!= itCRVTable.end(); ++it) //用++it比it++快
		{			
			
			TrDB cdb;
			cdb.createConditionalDB(trdb, it->item, min_sup);			
			ruleminer(cdb, min_sup, newMinedItemMap, sortAlg);	
			newMinedItemMap.insert(::make_pair(it->item, true));
		}
	
//
//		ItemCRTable itCRTable;
//		switch(sortAlg)
//		{
//		case MCD:
//			{
//				setItCRTalbeMCD(trdb,itTable,itCRTable);
//				break;
//			}
//		case EA:
//			{
//				setItCRTalbeEA(trdb,itTable,itCRTable);
//				break;
//			}
//		default:// CRA:
//			{
//				setItCRTalbeCRA(trdb,itTable,itCRTable);
//				break;
//			}
//		}
//		sort(itCRTable.begin(),itCRTable.end(), ItemCRCompare());  
//		std::map<Item,bool> newMinedItemMap;
//		for(ItemCRTable::iterator it =  itCRTable.begin(); it!= itCRTable.end(); ++it) //用++it比it++快
//		{			
//			
//			TrDB cdb;
//			cdb.createConditionalDB(trdb, it->first, min_sup);			
//			ruleminer(cdb, min_sup, newMinedItemMap, sortAlg);	
//			newMinedItemMap.insert(::make_pair(it->first, true));
////#if _DEBUG
////			std::cout<<"projecting db ";
////			for(ItemSet::const_iterator ip = cdb.getPrefix().begin(); ip != cdb.getPrefix().end(); ++ip)
////				std::cout<<*ip<<" ";
////			std::cout<<std::endl;
////#endif
//			
//		}
	}
}

bool HarmonyAlgorithm::setItCRVTableMCD(const TrDB& trdb, ItemMap& itTable,ItemCRVTable& itCRVTable)
{
	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); ++iter)
	{			
		double maxProC= 0.0;
		int supPx =iter->second->size();
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			double supPxC = trdb.getSupport(iter->first, ic->first);
			double temp = supPxC/supPx;
			if(temp > maxProC)
				maxProC = temp;		
		}
		ItemCRV it;
		it.item = iter->first;
		it.sup = supPx;
		it.val = -maxProC;
		itCRVTable.push_back(it);
		//itCRTable.push_back(::make_pair(iter->first, -maxProC)); //按-maxProC的升序排列
	}
	return true;
}
bool HarmonyAlgorithm::setItCRVTableEA(const TrDB& trdb, ItemMap& itTable,ItemCRVTable& itCRVTable)
{
	int k = trdb.getClassTable().size();
	double proPxC = 0.0;
	double entropy = 0.0;
	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); ++iter)
	{		
		int supPx =iter->second->size();
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			double supPxC = trdb.getSupport(iter->first, ic->first);
			proPxC = supPxC/supPx;
			entropy +=log(proPxC)*(-proPxC);			
		}
		ItemCRV it;
		it.item = iter->first;
		it.sup = supPx;
		it.val = entropy;
		itCRVTable.push_back(it);

		//itCRTable.push_back(::make_pair(iter->first, entropy));
	}
	return true;

}
bool HarmonyAlgorithm::setItCRVTableCRA(const TrDB& trdb, ItemMap& itTable,ItemCRVTable& itCRVTable)
{
	double mean_supP = 0.0;
	double mean_supPx = 0.0;
	int k =trdb.getClassTable().size();
	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); ++iter)
	{		
		mean_supPx = (iter->second->size() +0.0)/k;
		int supPC = 0;
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			supPC += ic->second->size();			
		}
		mean_supP = (supPC+0.0)/k;

		int supPxC = 0;
		double temp = 0;
		int temp2 = 0;
		double sigmaPx = 0.0;
		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
		{			
			supPC = ic->second->size();
			supPxC = trdb.getSupport(iter->first, ic->first);
			temp += (supPC*supPxC - mean_supP*mean_supPx);
			temp2 += supPxC*supPxC;
		}
		sigmaPx =  (temp2+0.0)/k - mean_supPx*mean_supPx;

		ItemCRV it;
		it.item = iter->first;
		it.sup = iter->second->size();
		
		if(sigmaPx > 0)
		{
			sigmaPx = sqrt(sigmaPx);
			it.val = temp/sigmaPx;
			itCRVTable.push_back(it);			
		}
		else if(sigmaPx == 0)
		{
			it.val = 1000000;
			itCRVTable.push_back(it);
			//itCRTable.push_back(::make_pair(iter->first,1000000));
		}
		else	
			return false;
		
	}
	return true;
}

//bool HarmonyAlgorithm::setItCRTalbeMCD(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable)
//{
//	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); ++iter)
//	{			
//		double maxProC= 0.0;
//		int supPx =iter->second->size();
//		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
//		{			
//			double supPxC = trdb.getSupport(iter->first, ic->first);
//			double temp = supPxC/supPx;
//			if(temp > maxProC)
//				maxProC = temp;		
//		}
//		itCRTable.push_back(::make_pair(iter->first, -maxProC)); //按-maxProC的升序排列
//	}
//	return true;
//}
//bool HarmonyAlgorithm::setItCRTalbeEA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable)
//{
//	int k = trdb.getClassTable().size();
//	double proPxC = 0.0;
//	double entropy = 0.0;
//	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); ++iter)
//	{		
//		int supPx =iter->second->size();
//		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
//		{			
//			double supPxC = trdb.getSupport(iter->first, ic->first);
//			proPxC = supPxC/supPx;
//			entropy +=log(proPxC)*(-proPxC);			
//		}
//		itCRTable.push_back(::make_pair(iter->first, entropy));
//	}
//	return true;
//
//}
//bool HarmonyAlgorithm::setItCRTalbeCRA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable)
//{
//	double mean_supP = 0.0;
//	double mean_supPx = 0.0;
//	int k =trdb.getClassTable().size();
//	for(ItemMap::iterator iter = itTable.begin(); iter!=itTable.end(); ++iter)
//	{		
//		mean_supPx = (iter->second->size() +0.0)/k;
//		int supPC = 0;
//		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
//		{			
//			supPC += ic->second->size();			
//		}
//		mean_supP = (supPC+0.0)/k;
//
//		int supPxC = 0;
//		double temp = 0;
//		int temp2 = 0;
//		double sigmaPx = 0.0;
//		for(ClassMap::const_iterator ic = trdb.getClassTable().begin(); ic != trdb.getClassTable().end(); ++ic)
//		{			
//			supPC = ic->second->size();
//			supPxC = trdb.getSupport(iter->first, ic->first);
//			temp += (supPC*supPxC - mean_supP*mean_supPx);
//			temp2 += supPxC*supPxC;
//		}
//		sigmaPx =  (temp2+0.0)/k - mean_supPx*mean_supPx;
//		if(sigmaPx > 0)
//		{
//			sigmaPx = sqrt(sigmaPx);
//			itCRTable.push_back(::make_pair(iter->first,temp/sigmaPx));
//		}
//		else if(sigmaPx == 0)
//		{
//			itCRTable.push_back(::make_pair(iter->first,1000000));
//		}
//		else	
//			return false;
//		
//	}
//	return true;
//}

