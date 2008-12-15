/*----------------------------------------------------------------------
File    : item.h
Contents: itemset management
Author  : Bart Goethals
Update  : 4/4/2003
----------------------------------------------------------------------*/
#pragma once
#include <set>

using namespace std;
namespace FPTree
{
	class Item;

	class Item_
	{
	public:

		Item_();
		~Item_();


		int id;

		/*多少个支持的 如 a:5___b:3
		                      |_c:2
			  ab:3 ,ac:2
		*/
		int supp;	
		
                    
		set<Item> *children;

		Item_ *parent;
		Item_ *nodelink;
	};

	class Item
	{
	public:

		Item(int s, Item_ *p);
		Item::Item(const Item& i);
		~Item();

		int getId() const {return item->id;}  
		int getSupport() const {return item->supp;}

		set<Item> *getChildren() const {return item->children;}
		set<Item> *makeChildren() const;

		Item_ *getItem() const {return item;}
		Item_ *getNext() const {return item->nodelink;}
		void setNext(Item_ *i) const {item->nodelink = i;}
		bool isFrequent(int ms) const {return item->supp >= ms;}
		void Increment(int i=1) const {item->supp += i;}

		void removeChildren() const;

		bool operator< (const Item &i) const {return getId() < i.getId();}

	private:

		Item_ *item;
	};

}