#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

// Forward declaration of Gate to resolve circular dependency
class Gate;

// Full definition of Net
struct Net {
    Net() = default; // Enable default construction
    std::string name; // Name of the net
    std::vector<Gate*> gates; // Pointers to Gates connected to this net
    int p1cnt = 0; // Count of gates in partition 1 connected to this net
    int p2cnt = 0; // Count of gates in partition 2 connected to this net
    bool cut = false; // Whether this net is cut (crosses partition boundary)

    Net(const std::string& name) : name(name) {}
};

struct state;

class Gate {
private:
    std::string name;
    std::vector<Net*> nets; // Now Net is fully defined above, so this is fine
    bool Part; // For now, will treat partition as a boolean
    int GainTot;
    int area;
    bool isLocked = false;

public:
    Gate();
    Gate(std::string name, std::vector<Net*> nets, bool part, int gain, int area, bool lock);
    ~Gate();
    std::vector<Net*> getNets() const;
    void addNet(Net* net);
    void removeNet(Net* net);
    void setPart(int& cutsize, std::map<std::string, Net>& netmap);
    void setPartNew(int& cutsize, state& s, std::map<std::string, Net>& netmap);
    void unsetPart(int& cutsize, state& s, std::map<std::string, Net>& netmap);
    bool getPart() const;
    int getGT() const;
    void incGT();
    void decGT();
    std::string getName() const;
    int getArea() const;
    void setArea(int a);
    void lock();    // Declaration only
    void unlock();
    void calculateInitialGain();
    static void updateGainsAfterMove(Gate* movedGate);
    void togglePartitionAndRecalculateGain(int& cutsize, state& s, std::map<std::string, Net>& netmap);
    // In DataStructures.hpp within the Gate class declaration
    friend std::ostream& operator<<(std::ostream& os, const Gate& g);
    bool getIsLocked() const;   // Declaration
    void updateGainsAfterMoveNew(Gate* movedGate, state& s);
};
struct state {
    std::vector<Gate*> P1; // Partition 1 list
    std::vector<Gate*> P2; // Partition 2 list
    int cutsize;
    int A1; // Area of partition 1
    int A2; // Area of partition 2
    std::map<int, std::set<Gate*>> buckets; // Buckets based on gain

    // Constructor for state (if necessary)
    state() : cutsize(0), A1(0), A2(0) {}
};
double getCost(const std::vector<Net>& nets, const std::map<std::string, Gate>& gates, double& cutsize, double& AreaProduct);

#endif // DATASTRUCTURES_HPP
