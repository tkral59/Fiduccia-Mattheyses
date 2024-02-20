#pragma once
#include <iostream>
#include <vector>
#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

struct Net; //forward declaration

class Gate { //for now will treat partition as a boolean
private:
	std::string name;
	std::vector<Net*> nets;
	bool Part;
	int GainTot;
public:
	Gate();
	Gate(std::string name, std::vector<Net*> nets, bool part, int gain);
	~Gate();
	std::vector<Net*> getNets();
	void addNet(Net* net);
	void removeNet(Net* net);
	void setPart();
	void unsetPart();
	bool getPart();
	int getGT();
	void incGT();
	void decGT();

};

struct Net {
	std::string name;
	std::vector<Gate*> gates;
	int p1cnt;
	int p2cnt;
};

#endif // HEADER_HPP