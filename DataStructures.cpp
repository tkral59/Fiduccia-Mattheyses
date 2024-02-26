#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "DataStructures.hpp"

Gate::Gate() {
    name = "";
    GainTot = 0;
    Part = false;
    nets = {};
    area = 0;
}

Gate::Gate(std::string name, vector<Net*> nets, bool part, int gain, int area) {
    this->name = name;
    this->GainTot = gain;
    this->Part = part;
    this->nets = nets;
    this->area = area;
}

Gate::~Gate() {
    // Destructor logic here (if needed for cleanup)
}

vector<Net*> Gate::getNets() {
    return nets;
}

void Gate::addNet(Net* net) {
    nets.push_back(net);
    // Adjust p1cnt or p2cnt based on the gate's current partition
    if (Part) {
        net->p1cnt++;
    } else {
        net->p2cnt++;
    }
    // Recalculate gains for all gates in this net
    Gate::updateGainsAfterMove(this);
}

void Gate::removeNet(Net* net) {
    for (size_t i = 0; i < nets.size(); ++i) {
        if (nets[i] == net) {
            nets.erase(nets.begin() + i);
            // Adjust p1cnt or p2cnt based on the gate's current partition
            if (Part) {
                net->p1cnt--;
            } else {
                net->p2cnt--;
            }
            // Recalculate gains for all gates in this net
            Gate::updateGainsAfterMove(this);
            break;
        }
    }
}

vvoid Gate::removeNet(Net* net) {
    for (size_t i = 0; i < nets.size(); ++i) {
        if (nets[i] == net) {
            nets.erase(nets.begin() + i);
            // Adjust p1cnt or p2cnt based on the gate's current partition
            if (Part) {
                net->p1cnt--;
            } else {
                net->p2cnt--;
            }
            // Recalculate gains for all gates in this net
            Gate::updateGainsAfterMove(this);
            break;
        }
    }
}

void Gate::setPart() {
    if (!Part) {
        Part = true;
        for (Net* net : nets) {
            net->p1cnt++;
            net->p2cnt--;
        }
        // Recalculate gains for all gates in affected nets
        Gate::updateGainsAfterMove(this);
    } else {
        cout << "Trying to setPart when already set." << endl;
    }
}

void Gate::unsetPart() {
    if (Part) {
        Part = false;
        for (Net* net : nets) {
            net->p1cnt--;
            net->p2cnt++;
        }
        // Recalculate gains for all gates in affected nets
        Gate::updateGainsAfterMove(this);
    }
}


bool Gate::getPart() {
    return Part;
}

int Gate::getGT() {
    return GainTot;
}

void Gate::incGT() {
    GainTot++;
}

void Gate::decGT() {
    GainTot--;
}

ostream& operator<<(ostream& os, Gate const& g) {
    string nst = "";
    for (Net* n : g.nets) {
        nst += n->name + " ";
    }
    os << "[" << g.name << ", " << nst << ", " << g.Part << ", " << g.GainTot << ", " << g.area << "]";
    return os;
}

string Gate::getName() {
    return name;
}

int Gate::getArea() {
    return area;
}

void Gate::setArea(int a) {
    area = a;
}

// New methods for gain calculation

void Gate::calculateInitialGain() {
    int gain = 0;
    for (Net* net : nets) {
        int p1cnt = net->p1cnt;
        int p2cnt = net->p2cnt;
        if (Part) { // Gate is in partition 1
            if (p1cnt == 1) gain++;
            if (p2cnt == 0) gain--;
        } else { // Gate is in partition 2
            if (p2cnt == 1) gain++;
            if (p1cnt == 0) gain--;
        }
    }
    GainTot = gain;
}

void Gate::updateGainsAfterMove(Gate* movedGate) {
    for (Net* net : movedGate->getNets()) {
        if (movedGate->getPart()) {
            net->p1cnt++;
            net->p2cnt--;
        } else {
            net->p1cnt--;
            net->p2cnt++;
        }
        // Update the gains of all gates on those nets
        for (Gate* gate : net->gates) {
            gate->calculateInitialGain(); // Recalculates gain
        }
    }
}
