#include "state.h"

#include <limits>
#include <string>

StateChange::StateChange(const State& state) : state(state), prev(nullptr), changedLeft(true), depth(0) {}

StateChange::StateChange(const State& state, Change change, bool changeLeft, const std::shared_ptr<StateChange> prev, uint32_t depth)
    : state(state), prev(prev), changedLeft(changeLeft), change(change), depth(depth) {
    auto changeState = [&](int chickens, int wolves) {
        if (changeLeft) {
            this->state.left.chickens -= chickens;
            this->state.left.wolves -= wolves;
            this->state.right.chickens += chickens;
            this->state.right.wolves += wolves;
            this->state.left.hasBoat = false;
            this->state.right.hasBoat = true;
        } else {
            this->state.left.chickens += chickens;
            this->state.left.wolves += wolves;
            this->state.right.chickens -= chickens;
            this->state.right.wolves -= wolves;
            this->state.left.hasBoat = true;
            this->state.right.hasBoat = false;
        }
    };
    switch (change) {
        case Change::CHICKEN1: changeState(1, 0); break;
        case Change::CHICKEN2: changeState(2, 0); break;
        case Change::WOLF1: changeState(0, 1); break;
        case Change::WOLFCHICK: changeState(1, 1); break;
        case Change::WOLF2: changeState(0, 2); break;
    }
}

// Optimal solutions will take 2 take 1 in a pattern.
//      Optimal solutions will have a solution cost less than 2A where A is the number of animals.
float StateChange::cost(const State& goal) {
    uint32_t A = goal.left.chickens + goal.right.chickens + goal.left.wolves + goal.right.wolves;
    //if (this->depth >= 2 * A) return std::numeric_limits<float>::infinity(); // Any node equal to or beyond 2A must be non-optimal.
    if (this->state == goal) return 0;

    // An optimal solution which must move all animals from one side to the other 
    //  has an optimal solution length/cost of 
    //      IF A is EVEN length = 2A - 1
    //      IF A is ODD  length = 2(A-1)-1
    //      WHERE A is the total number of animals.
    uint32_t optimalLength = A % 2 == 0 ? 2 * A - 1 : 2 * (A - 1) - 1;
    uint32_t remaining = std::abs(goal.left.chickens - this->state.left.chickens) + std::abs(goal.left.wolves - this->state.left.wolves);
    uint32_t prev = std::abs(goal.left.chickens - this->prev->state.left.chickens) + std::abs(goal.left.wolves - this->prev->state.left.wolves);
    if (remaining < prev) {
        // We just moved towards our goal.(This is the odd moves of the boat).
        //  Moves that move towards the goal are most optimal with 2 Chickens or 1 of Each.
        //  Thus a move that is not one of those must be less optimal likely requiring an extra move.
        switch (this->change) {
            case Change::CHICKEN1: case Change::WOLF1: case Change::WOLF2:
                return optimalLength - this->depth + 2;
            default:
                return optimalLength - this->depth;
        }
    } else {
        // We just moved away from our goal.(This is the even moves of the boat).
        //  Moves that move away from the goal are most optimal with 1 Chicken.
        //  Thus a move thata is not one of those must be less opttimal likely requiring an extra move.
        switch (this->change) {
            case Change::CHICKEN1:
                return optimalLength - this->depth;
            default:
                return optimalLength - this->depth + 2;
        }
    }
}

std::ostream& operator <<(std::ostream& stream, const Change& change) {
    switch (change) {
        case Change::CHICKEN1:  return (stream << "1 Chicken");
        case Change::CHICKEN2:  return (stream << "2 Chickens"); 
        case Change::WOLF1:     return (stream << "1 Wolf");
        case Change::WOLFCHICK: return (stream << "1 Each");
        case Change::WOLF2:     return (stream << "2 Wolves");
    }
    return (stream << "Unknown action");
}

State ReadState(std::ifstream& stateFile) {
    State state;
    std::string line;
    std::getline(stateFile, line);
    int b;
    int count = sscanf(line.c_str(), "%d,%d,%d", &state.left.chickens, &state.left.wolves, &b);
    if (count < 3) exit(2);
    state.left.hasBoat = b == 1;

    std::getline(stateFile, line);
    count = sscanf(line.c_str(), "%d,%d,%d", &state.right.chickens, &state.right.wolves, &b);
    if (count < 3) exit(2);
    state.right.hasBoat = b == 1;

    return state;
}
