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

typedef struct _ItemCRV
{
	Item item;
	double val;
	int sup;
} ItemCRV;
class ItemCRVCompare
{
public:

	bool operator()(const ItemCRV& lhs, //��������ıȽϺ���
		const ItemCRV& rhs) const
	{	 
		if(lhs.val < rhs.val || (lhs.val == rhs.val && lhs.sup > rhs.sup))
			return true;
		return false;
	}

};

typedef std::pair<Item, double> ItemCR;

//typedef std
class ItemCRCompare
	{  //�ȽϺ�������

private:
	bool valLess(const ItemCR::second_type &k1, //��ʵ�ʡ��ıȽϺ���
			const ItemCR::second_type &k2) const
	{
		return k1 < k2;
	}
public:

	bool operator()(const ItemCR& lhs, //��������ıȽϺ���
		const ItemCR& rhs) const
	{	 
		return valLess(lhs.second, rhs.second); //keyLess�����涨��
	}

};
enum ESortAlg
{
	MCD = 0,//maximum confidence descending order
	EA,//entropy ascending order
	CRA//correlation coefficient ascending order
};
typedef std::vector<ItemCR> ItemCRTable;
typedef std::vector<ItemCRV> ItemCRVTable;

/*!
 * \brief
 * ʵ��Harmony�㷨
 */
class HarmonyAlgorithm:
	public DMAlgorithm
{
public:
	HarmonyAlgorithm(void);
	~HarmonyAlgorithm(void);

	/*!
	 * \brief
	 * ִ���㷨
	 * 
	 * \param trdb
	 * ԭʼ�����ݿ�
	 * 
	 * \param nSupport
	 * �û������֧�ֶ�
	 * 
	 * \returns
	 * �㷨�Ƿ�ִ�гɹ������򷵻�true;����false
	 * 
	 */
	bool execute(const TrDB& trdb,int nSupport);

private:
	void ruleminer(const TrDB& trdb,int nSupport,std::map<Item,bool>& minedItemMap, ESortAlg sortAlg);
	//bool setItCRTalbeCRA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable);
	//bool setItCRTalbeEA(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable);
	//bool setItCRTalbeMCD(const TrDB& trdb, ItemMap& itTable,ItemCRTable& itCRTable);

	bool setItCRVTableMCD(const TrDB& trdb, ItemMap& itTable,ItemCRVTable& itCRVTable);
	bool setItCRVTableEA(const TrDB& trdb, ItemMap& itTable,ItemCRVTable& itCRVTable);
	bool setItCRVTableCRA(const TrDB& trdb, ItemMap& itTable,ItemCRVTable& itCRVTable);

	void initHCCR(const TrDB& trdb);
	void cleanResult();
private :
	HCCRSet _hccrSet; 
	
	

};




