#include <iostream>
#include <vector>
#include "DataStructures.hpp"
//when initializing netlist, will iterate though netlist, create nets, and generate gates if they don't already exist as we go. if exists will need to add to gate's netlist

Gate::Gate() {
	Gate::name = "";
	Gate::GainTot = 0;
	Gate::Part = false;
	Gate::nets = {};
}

Gate::Gate(std::string name, std::vector<Net*> nets, bool part, int gain) {
	Gate::name = name;
	Gate::GainTot = gain;
	Gate::Part = part;
	Gate::nets = nets;
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
	Gate::Part = true;
}

void Gate::unsetPart() {
	Gate::Part = false;
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

/////////////////////Bucket Structure///////////////////////////