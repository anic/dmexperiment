#include "StdAfx.h"
#include "DDPMineAlgorithm.h"
#include <iostream>


DDPMineAlgorithm::DDPMineAlgorithm(void)
{
}

DDPMineAlgorithm::~DDPMineAlgorithm(void)
{
}

bool DDPMineAlgorithm::execute(const TrDB& trdb,int nSupport)
{
	//算法结果放在m_result中
	TrDB trdb1 = trdb;
	prefix = ItemSet();
	DDPMine(trdb1, nSupport);
	return true;
}

void DDPMineAlgorithm::DDPMine(TrDB& trdb, int min_sup)
{
	const ItemSet pre = ItemSet();//start prefix
	ItemSet iset;//transaction id list T(α) containing prefix

	//the process
	if (trdb.getSize() == 0)
		return;
	branch_and_bound(trdb, min_sup, pre);
	computeTa(trdb, iset);
	update_tree(trdb, iset);
	//here need to add prefix in m_result
	DDPMine(trdb, min_sup);
}

void DDPMineAlgorithm::branch_and_bound(const TrDB& trdb, int min_sup, const ItemSet& a)
{

	//从投影数据库构造fptree，记住是使用带有ex后缀这个
	FPTreeEx fptree;
	fptree.createFromTrDB(trdb);

	double maxIG = 0;

	ItemSet_Support conbest;

	if (trdb.getSize() == 0)
		return;

	this->m_result.push_back(Rule())

	const std::set<ItemNode>& node = fptree.getHeader();
	std::set<ItemNode>::const_iterator it; 
	for (it = node.begin(); it != node.end(); ++it)
	{
		std::vector<ItemSet_Support> out;
		fptree.getPotentialPrefix(it->getId(), out);
		std::vector<ItemSet_Support>::iterator vit;
		for (vit = out.begin(); vit!=out.end(); vit++)
		{
			conbest.items.insert(a.begin(),a.end());
			conbest.items.insert(vit->items.begin(), vit->items.end());
			conbest.support = vit->support;

			double IG = computeIG(trdb, conbest);
			
			if ( IG > maxIG )
			{
				maxIG = IG;
				prefix = conbest.items;
			}

			TrDB cdb;
			cdb.createConditionalDB(trdb, *prefix.begin(), min_sup);
			double IGup = computeIGup(trdb, conbest);
			if (maxIG >= IGup)
			{

			}
			else
			{
				branch_and_bound(cdb, min_sup, conbest.items);
			}		
		}

	}

}

double DDPMineAlgorithm::computeIG(const TrDB &trdb, ItemSet_Support &iset)
{
	const ClassMap& classmap = trdb.getClassTable();
	for(ClassMap::const_iterator iter = classmap.begin();iter!=classmap.end();++iter)
	{
		//iter->first //0,1
		//iter->second->size()
	}
	//compute the entroy: need the class label

	//compute conditional entroy

	//entroy - conditional entroy
	return 0;
}

void DDPMineAlgorithm::update_tree(TrDB &trdb, const ItemSet &iset)
{
	trdb.removeItem(1);
}

double DDPMineAlgorithm::computeIGup(const TrDB &trdb, ItemSet_Support &iset)
{
	const ClassMap& classmap = trdb.getClassTable();
	int p = trdb.getSupport(1);
	trdb.getSize();
	//double p = P(c = 1)
	trdb.getSupport(iset.items);
	//double thita = P(support/size)

	//if thita <= p
	//double lb = (thita -1)((p-thita)/(1-thita)*log2((p-thita)/(1-thita)) + (1-p)/(1-thita) * log2((1-p)/(1-thita)));
	//else
	//double lb = -p*log2(p/thita) - (thita - p)log2(1-p/thita);
	return 0;
}

void DDPMineAlgorithm::computeTa(const TrDB &trdb, ItemSet &iset)
{
	//std::set<int> : int is the transaction id put in iset
}

