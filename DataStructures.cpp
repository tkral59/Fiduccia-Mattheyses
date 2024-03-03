#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <cmath>
#include "DataStructures.hpp"
#include <sstream>
using namespace std;

Gate::Gate() : name(""), Part(false), GainTot(0), area(0), isLocked(false) {}

Gate::Gate(string name, vector<Net*> nets, bool part, int gain, int area, bool lock)
    : name(name), nets(nets), Part(part), GainTot(gain), area(area), isLocked(lock) {}

Gate::~Gate() {
    // Cleanup if needed
}
std::vector<Net*> Gate::getNets() const {
    return nets;
}
void Gate::addNet(Net* net) {
    nets.push_back(net);

    // Adjust p1cnt or p2cnt based on the gate's current partition
    if (Part) {
        net->p1cnt++;
    }
    else {
        net->p2cnt++;
    }
    // Recalculate gains for all gates in this net
    Gate::updateGainsAfterMove(this);
}

void Gate::removeNet(Net* net) {
    nets.erase(std::remove(nets.begin(), nets.end(), net), nets.end());
    for (size_t i = 0; i < nets.size(); ++i) {
        if (nets[i] == net) {
            nets.erase(nets.begin() + i);
            // Adjust p1cnt or p2cnt based on the gate's current partition
            if (Part) {
                net->p1cnt--;
            }
            else {
                net->p2cnt--;
            }
            // Recalculate gains for all gates in this net
            Gate::updateGainsAfterMove(this);
            break;
        }
    }
}

void Gate::setPart(int& cutsize, map<string, Net>& netmap) {
    if (!Part) {
        Part = true;
        for (Net* net : nets) {
            //net->p1cnt++;
            //net->p2cnt--;
            //Net dummy = netmap[net->name];
            //netmap[net->name].p1cnt++;
            //netmap[net->name].p2cnt--;
            auto it = netmap.find(net->name);
            it->second.p1cnt++;
            it->second.p2cnt--;
            if (net->p2cnt == 0 && net->cut == true) {
                net->cut = false;
                netmap[net->name].cut = false;
                it->second.cut = false;
                cutsize--;
            }
            else if (net->p1cnt == 1 && net->cut == false) {
                net->cut = true;
                netmap[net->name].cut = true;
                it->second.cut = true;
                cutsize++;
            }
        }
        // Recalculate gains for all gates in affected nets
        Gate::updateGainsAfterMove(this);
    }
    else {
        cout << "Trying to setPart when already set." << endl;
    }
}

void Gate::setPartNew(int& cutsize, state& s, map<string, Net>& netmap) {
    if (!Part) {
        Part = true;
        for (Net* net : nets) {
            //net->p1cnt++;
            //net->p2cnt--;

            //netmap[net->name].p1cnt++;
            //netmap[net->name].p2cnt--;
            auto it = netmap.find(net->name);
            it->second.p1cnt++;
            it->second.p2cnt--;
            if (net->p2cnt == 0 && net->cut == true) {
                net->cut = false;
                netmap[net->name].cut = false;
                it->second.cut = false;
                s.cutsize--;
                if (cutsize == 0) {
                    cout << "stop" << endl;
                }
            }
            else if (net->p1cnt == 1 && net->cut == false) {
                net->cut = true;
                netmap[net->name].cut = true;
                it->second.cut = true;
                s.cutsize++;
            }
            if (cutsize == 0) {
                cout << "stop" << endl;
            }
        }
        // Recalculate gains for all gates in affected nets
        Gate::updateGainsAfterMoveNew(this, s);
    }
    else {
        cout << "Trying to setPart when already set." << endl;
    }
}

void Gate::unsetPart(int& cutsize, state& s, map<string, Net>& netmap) {
    if (Part) {
        Part = false;
        for (Net* net : nets) {
            //net->p1cnt--;
            //net->p2cnt++;
            //Net dummy = netmap[net->name];
            //netmap[net->name].p1cnt--;
            //netmap[net->name].p2cnt++;
            auto it = netmap.find(net->name);
            it->second.p1cnt--;
            it->second.p2cnt++;
            if (net->p1cnt == 0 && net->cut == true) {
                net->cut = false;
                netmap[net->name].cut = false;
                it->second.cut = false;
                s.cutsize--;
                if (cutsize == 0) {
                    cout << "stop" << endl;
                }
            }
            else if (net->p2cnt == 1 && net->p1cnt == 0) {
                net->cut = true;
                netmap[net->name].cut = true;
                it->second.cut = true;
                s.cutsize++;
                if (cutsize == 0) {
                    cout << "stop" << endl;
                }
            }
        }
        // Recalculate gains for all gates in affected nets
        Gate::updateGainsAfterMoveNew(this, s);
    }
}


bool Gate::getPart() const {
    return Part;
}

int Gate::getGT() const {
    return GainTot;
}

void Gate::incGT() {
    GainTot++;
}

void Gate::decGT() {
    GainTot--;
}

string Gate::getName() const {
    return name;
}

int Gate::getArea() const {
    return area;
}

void Gate::setArea(int a) {
    area = a;
}
bool Gate::getIsLocked() const {
    return isLocked;
}

void Gate::lock() {
    isLocked = true;
}

void Gate::unlock() {
    isLocked = false;
}


void Gate::calculateInitialGain() {
    int gain = 0;
    for (Net* net : nets) {
        int p1cnt = net->p1cnt;
        int p2cnt = net->p2cnt;
        if (Part) { // Gate is in partition 1
            if (p1cnt == 1) gain++;
            if (p2cnt == 0) {
                gain--;
                net->cut = false;
            }
            else net->cut = true;
        }
        else { // Gate is in partition 2
            if (p2cnt == 1) gain++;
            if (p1cnt == 0) {
                gain--;
                net->cut = false;
            }
            else net->cut = true;
        }
    }


    GainTot = gain;
}


void Gate::updateGainsAfterMove(Gate* movedGate) {
    for (Net* net : movedGate->getNets()) {
        // Update the gains of all gates on those nets
        for (Gate* gate : net->gates) {
            gate->calculateInitialGain(); // Recalculates gain
        }
    }
}

void Gate::updateGainsAfterMoveNew(Gate* movedGate, state& s) {
    set<Gate*> done;
    for (Net* net : movedGate->getNets()) {
        for (Gate* gate : net->gates) {
            if (done.count(gate) == 0) { // Change here
                if (gate == movedGate) {
                    s.buckets[gate->getGT()].erase(s.buckets[gate->getGT()].find(gate));
                    gate->calculateInitialGain();
                    s.buckets[gate->getGT()].insert(gate);
                    done.insert(gate);
                }
                //else if (gate->getIsLocked()) {
                //    gate->calculateInitialGain();
                //    done.insert(gate);
                //}
                else {
                    s.buckets[gate->getGT()].erase(s.buckets[gate->getGT()].find(gate));
                    gate->calculateInitialGain();
                    s.buckets[gate->getGT()].insert(gate);
                    done.insert(gate);
                }
            }
        }
    }
}

void Gate::togglePartitionAndRecalculateGain(int& cutsize, state& s, map<string, Net>& nets) {
    if (!isLocked) {
        if (Part) {
            this->unsetPart(cutsize, s, nets);
        }
        else this->setPartNew(cutsize, s, nets);
        //Part = !Part;
        //calculateInitialGain();
    }
    else {
        cout << "Gate is locked and cannot change partitions.\n";
    }
}
ostream& operator<<(ostream& os, const Gate& g) {
    os << "Gate: " << g.getName() << ", Area: " << g.getArea() << ", Part: " << (g.getPart() ? "1" : "2");
    return os;
}


double getCost(const std::vector<Net>& nets, const std::map<std::string, Gate>& gates, double& cutsize, double& AreaProduct) {
    {
        cutsize = 0;
        AreaProduct = 0;
        double P1A = 0;
        double P2A = 0;
        int totArea = 0;
        for (Net net : nets) {
            if (net.cut) cutsize++;
        }
        for (auto it = gates.begin(); it != gates.end(); it++) {
            if (it->second.getPart()) P1A += it->second.getArea();
            else P2A += it->second.getArea();
            totArea += it->second.getArea();
        }
        AreaProduct = (P1A * P2A);// *10e-12;
        //normalize:
        double cmax = nets.size();
        double num = (cutsize) / (cmax); //for now no established minimum value, set min to zero
        double hta = totArea / 2;
        double maxNormArea = (hta) * (hta);
        double den = (AreaProduct) / (maxNormArea); //again no min established, if min set at zero
        double res = (num / den);
        return res;
    }

}