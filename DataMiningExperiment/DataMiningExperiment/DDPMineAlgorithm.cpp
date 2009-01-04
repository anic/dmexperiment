#include "StdAfx.h"
#include "DDPMineAlgorithm.h"
#include <math.h>


DDPMineAlgorithm::DDPMineAlgorithm(void)
{
}

DDPMineAlgorithm::~DDPMineAlgorithm(void)
{
}

bool DDPMineAlgorithm::execute(const TrDB& trdb,int nSupport)
{
	//算法结果放在m_result中
	trdb_local = trdb;
	prefix = ItemSet();
	DDPMine(trdb_local, nSupport);
	return true;
}

void DDPMineAlgorithm::DDPMine(TrDB& trdb, int min_sup)
{
	const ItemSet pre = ItemSet();//start prefix
	//the process
	if (trdb.getSize() == 0)
		return;
	branch_and_bound(trdb, min_sup, pre);
	update_tree(trdb);
    //add prefix to m_result
	m_result
	//clear the prefix
	DDPMine(trdb, min_sup);
}

void DDPMineAlgorithm::branch_and_bound(const TrDB& trdb, int min_sup, const ItemSet& a)
{

	//从投影数据库构造fptree，记住是使用带有ex后缀这个
	FPTreeEx fptree;
	fptree.createFromTrDB(trdb);

	if (trdb.getSize() == 0)
		return;


	const std::set<ItemNode>& node = fptree.getHeader();
	std::set<ItemNode>::const_iterator it; 
	for (it = node.begin(); it != node.end(); ++it)
	{
		/*std::vector<ItemSet_Support> out;
		fptree.getPotentialPrefix(it->getId(), out);
		std::vector<ItemSet_Support>::iterator vit;
		for (vit = out.begin(); vit!=out.end(); vit++)
		{*/

		ItemSet conbest;
		conbest.insert(a.begin(),a.end());
		conbest.insert(it->getId());

		double IG = computeIG(trdb_local, conbest);
			
		if ( IG > maxIG )
		{
			maxIG = IG;
			prefix = conbest;
		}
		double IGup = computeIGup(trdb_local, conbest);
		if (maxIG < IGup)
		{
			TrDB cdb;
			cdb.createConditionalDB(trdb, it->getId(), min_sup);
			branch_and_bound(cdb, min_sup, conbest);
		}		
	}

}

double DDPMineAlgorithm::computeIG(const TrDB &trdb, ItemSet &iset)
{

	int trdb_size = trdb.getSize();

	int p = 0;
	const ClassMap& classmap = trdb.getClassTable();
	for(ClassMap::const_iterator iter = classmap.begin();iter!=classmap.end();++iter)
	{
		if (iter->first == 1)
		{
			p += iter->second->size();
		}
	}

	//compute the entroy: need the class label
	double probility = p * 1.0 / trdb_size;
	double entropy = 0;
	if (probility != 1 && probility != 0)
		entropy = -probility * (log(probility)/log(2.0)) - (1 - probility) * (log(1-probility)/log(2.0));
	//compute conditional entroy
	int support_p = trdb.getSupport(iset, 1);
	int support_iset = trdb.getSupport(iset);

	probility = support_p * 1.0 / support_iset;
	double conditional_entropy_1 = 0;
	if (probility != 1 && probility != 0)
		conditional_entropy_1 = (support_iset * 1.0 / trdb_size) * ( -(probility) * log(probility) / log(2.0) - (1 - probility) * log(1 - probility) / log(2.0));


	int support_notp = p - support_p;
	int support_not_iset = trdb_size - support_iset;
	probility = support_notp*1.0 / support_not_iset;
	double conditional_entropy_2 = 0;
	if (probility != 1 && probility != 0)
		conditional_entropy_2 = (support_not_iset * 1.0 / trdb_size) * ( -(probility) * log(probility) / log(2.0) - (1 - probility)*log(1 - probility) / log(2.0));

	//entroy - conditional entroy
	return (entropy - conditional_entropy_1 - conditional_entropy_2);
}

void DDPMineAlgorithm::update_tree(TrDB &trdb)
{
	std::set<Item>::iterator it; 
	
}

double DDPMineAlgorithm::computeIGup(const TrDB &trdb, ItemSet &iset)
{
	int trdb_size = trdb.getSize();
	double p = 0;
	const ClassMap& classmap = trdb.getClassTable();
	for(ClassMap::const_iterator iter = classmap.begin();iter!=classmap.end();++iter)
	{
		if (iter->first == 1)
		{
			p += iter->second->size();
		}
	}

	double probility = p * 1.0 / trdb_size;
	double entropy = 0;
	if (probility != 1 && probility != 0)
		entropy = -probility * (log(probility)/log(2.0)) - (1 - probility) * (log(1-probility)/log(2.0));

	p = p / trdb_size;
	double thita = trdb.getSupport(iset);
	thita = thita / trdb_size;
	

	double lb = 0;
	if (thita < p && thita != 1 && p != 0 && p != 1)
		lb = (thita - 1) * ((p-thita)/(1-thita) * log((p-thita)/(1-thita)) / log(2.0) + (1-p)/(1-thita) * log((1-p)/(1-thita)) / log(2.0));
	else if (thita > p && p != 0 && thita != 0)
		lb = -p * log(p/thita) / log(2.0) - (thita - p) * log(1-p/thita) / log(2.0);

	return entropy - lb;
}

