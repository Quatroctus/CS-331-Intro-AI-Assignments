#include <fstream>
#include <memory>

struct Bank {
    bool hasBoat = false;
    int chickens = 0;
    int wolves = 0;
    Bank() = default;
    inline operator bool() { return (chickens >= wolves || chickens == 0) && (chickens >= 0 && wolves >= 0); }
    inline bool operator ==(const Bank& bank) const {
        return this->hasBoat == bank.hasBoat && this->chickens == bank.chickens && this->wolves == bank.wolves;
    }
};

struct State {
    Bank left, right;
    State() = default;
    inline bool operator ==(const State& state) const {
        return this->left == state.left && this->right == state.right;
    }
};

namespace std {
    template<> struct hash<State> {
        size_t operator()(const State& state) const {
            return (std::hash<int>()(state.left.chickens) ^ std::hash<int>()(state.left.wolves) << 32) ||
                   (std::hash<int>()(state.right.chickens) ^ std::hash<int>()(state.right.wolves));
        }
    };
}

enum class Change { CHICKEN1, CHICKEN2, WOLF1, WOLFCHICK, WOLF2 };
std::ostream& operator <<(std::ostream& stream, const Change& change);
struct StateChange {
    State state;
    Change change;
    uint32_t depth;
    bool changedLeft;
    const std::shared_ptr<StateChange> prev = NULL;
    StateChange(const State& state);
    StateChange(const State& state, Change change, bool changeLeft, std::shared_ptr<StateChange> prev, uint32_t depth=0);

    inline operator bool() { return state.left && state.right; }

    float cost(const State& goal);
};

State ReadState(std::ifstream& stateFile);