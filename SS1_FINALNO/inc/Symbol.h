#pragma once

#include <iostream>

using namespace std;

class Symbol {
private:
	int ordinal;
	string name;
	int section;
	int value;
	string local;

	int size;
	string access;

public:
	Symbol(int ordinal, string name, int section, int value, string local, int size/*, string access*/);
	int getOrdinal();
	void setOrdinal(int ordinal);
	string getName();
	void setName(string name);
	int getSection();
	void setSection(int section);
	int getValue();
	void setValue(int value);
	string getLocal();
	void setLocal(string local);
	int getSize();
	void setSize(int size);
	/*string getAccess();
	void setAccess(string access);*/
	Symbol* getSymbol();
	//~Symbol();
};