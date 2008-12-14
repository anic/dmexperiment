#ifndef _COMMON_H
#define _COMMON_H

#include <set>


typedef int Item;
typedef std::set<Item> ItemSet;
typedef int ClassLabel;

typedef struct RULE
{
	int id;
	ItemSet body;
	ClassLabel head;
	double support;
	double confidence;

public:
	RULE(int nId){
		id = nId;
		head = 0;
		support = 0.0;
		confidence = 0.0;

	}
}Rule;



#endif