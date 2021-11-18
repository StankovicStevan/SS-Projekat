#pragma once
#include <iostream>

using namespace std;

class RelocationData {
private:
	int value;
	string type;
	int ordinal;
public:
	RelocationData(int value, string type, int ordinal);
	int getValue();
	void setValue(int value);
	string getType();
	void setType(string type);
	int getOrdinal();
	void setOrdinal(int ordinal);
};