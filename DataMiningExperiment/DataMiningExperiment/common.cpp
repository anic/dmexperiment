#include "stdafx.h"
#include "common.h"

#include <set>
#include <algorithm>
using namespace std;

bool set_contain(const ItemSet& subSet,const ItemSet& superSet)
{ 
	/*ItemSet temp;
		::set_difference(subSet.begin(),
			subSet.end(),
			superSet.begin(),
			superSet.end(),
			std::insert_iterator<ItemSet>(temp, temp.begin() ));
	return temp.empty();*/

	return ::includes(subSet.begin(),
			subSet.end(),
			superSet.begin(),
			superSet.end());

	
}