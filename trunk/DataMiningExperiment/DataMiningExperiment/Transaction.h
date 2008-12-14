#ifndef _TRANSACTION_H
#define _TRANSACTION_H
#include <set>

#include "common.h"
class Transaction
{
public:
	int id;
	ItemSet items;
	ClassLabel label;

	Transaction(int nId,ClassLabel label);
	Transaction(int nId);
	Transaction(void);

};

typedef Transaction Instance;


#endif