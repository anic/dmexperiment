#pragma once

#include "DMAlgorithm.h"
//#include <set>
#include <map>
#include <vector>
#include "common.h"
#include <algorithm>   
#include <math.h>
using namespace std;

typedef struct _HCCR
{
	int _tid;
	ClassLabel _cid;
	double _hConf;
	int _rid;
public:
	_HCCR(int tid, ClassLabel cid) //init
	{
		_tid = tid;
		_cid = cid;
		_hConf = 0.0;
		_rid = -1; //undefined
	}
	_HCCR(int tid, ClassLabel cid, double hConf, int rid)
	{
		_tid = tid;
		_cid = cid;
		_hConf = hConf;
		_rid = rid; 
	}
	_HCCR()
	{
		_tid = -1;
		_cid = -1;
		_hConf = 0.0;
		_rid = -1; 
	}
}HCCR;



typedef std::vector<HCCR> HCCRSet;

typedef std::pair<Item, double> ItemCR;

class ItemCRCompare
	{  //比较函数的类

private:
	bool valLess(const ItemCR::second_type &k1, //“实际”的比较函数
			const ItemCR::second_type &k2) const
	{
		return k1 < k2;
	}
public:

	bool operator()(const ItemCR& lhs, //用于排序的比较函数
		const ItemCR& rhs) const
	{	 
		return valLess(lhs.second, rhs.second); //keyLess见后面定义
	}

};
enum ESortAlg
{
	MCD = 0,//maximum confidence descending order
	EA,//entropy ascending order
	CRA//correlation coefficient ascending order
};
typedef std::vector<ItemCR> ItemCRTable;

/*!
 * \brief
 * 实现Harmony算法
 */
class HarmonyAlgorithm:
	public DMAlgorithm
{
public:
	HarmonyAlgorithm(void);
	~HarmonyAlgorithm(void);

	/*!
	 * \brief
	 * 执行算法
	 * 
	 * \param trdb
	 * 原始的数据库
	 * 
	 * \param nSupport
	 * 用户定义的支持度
	 * 
	 * \returns
	 * 算法是否执行成功，是则返回true;否则false
	 * 
	 */
	bool execute(const TrDB& trdb,int nSupport);

private:
	void ruleminer(const TrDB& trdb,int nSupport,ESortAlg sortAlg);
	bool setItCRTalbeCRA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable);
	bool setItCRTalbeEA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable);
	bool setItCRTalbeMCD(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable);
	void initHCCR(const TrDB& trdb);
private :
	HCCRSet _hccrSet; 
	
};




