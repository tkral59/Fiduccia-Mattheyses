#include <iostream>
#include <vector>
#include <string>
#include "DataStructures.hpp"
using namespace std;
//when initializing netlist, will iterate though netlist, create nets, and generate gates if they don't already exist as we go. if exists will need to add to gate's netlist

Gate::Gate() {
	Gate::name = "";
	Gate::GainTot = 0;
	Gate::Part = false;
	Gate::nets = {};
}

Gate::Gate(std::string name, std::vector<Net*> nets, bool part, int gain, int area) {
	Gate::name = name;
	Gate::GainTot = gain;
	Gate::Part = part;
	Gate::nets = nets;
	Gate::area = area;
}

Gate::~Gate() {

}

std::vector<Net*> Gate::getNets() {
	return Gate::nets;
}

void Gate::addNet(Net* net) {
	Gate::nets.push_back(net);
}

void Gate::removeNet(Net* net) {
	for (int i = 0; i < nets.size(); i++) {
		if (Gate::nets[i] == net) {
			Gate::nets.erase(Gate::nets.begin()+i);
		}
	}
}

void Gate::setPart() {
	if (!Gate::Part) {
		Gate::Part = true;
		for (int i = 0; i < Gate::nets.size(); i++) {
			Gate::nets.at(i)->p1cnt++;
			Gate::nets.at(i)->p2cnt--;
		}
	}
	else
		cout << "trying to setPart when already set." << endl;
}

void Gate::unsetPart() {
	Gate::Part = false;
	for (int i = 0; i < Gate::nets.size(); i++) {
		Gate::nets.at(i)->p1cnt--;
		Gate::nets.at(i)->p2cnt++;
	}
}

bool Gate::getPart() {
	return Gate::Part;
}

int Gate::getGT() {
	return Gate::GainTot;
}

void Gate::incGT() {
	Gate::GainTot++;
}

void Gate::decGT() {
	Gate::GainTot--;
}
std::ostream& operator<<(std::ostream& os, Gate const& g){
	std::string nst = "";
	Net* n;
	for (int i = 0; i < g.nets.size(); i++) {
		n = g.nets.at(i);
		nst += n->name + " ";
	}
	os << "[" << g.name << ", " << nst << ", " << g.Part << ", " << g.GainTot << ", " << g.area << "]";
	return os;
}

std::string Gate::getName() {
	return Gate::name;
}

int Gate::getArea() {
	return Gate::area;
}

void Gate::setArea(int a) {
	Gate::area = a;
}

/////////////////////Bucket Structure///////////////////////////
