#include "state.h"

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

constexpr std::array<Change, 5> CHANGES{ Change::CHICKEN1, Change::CHICKEN2, Change::WOLF1, Change::WOLFCHICK, Change::WOLF2 };

template<class T, class = decltype(std::begin(std::declval<T>()))>
struct GraphSearch {
    T frontier;
    std::unordered_map<State, uint32_t> explored;
    GraphSearch() {
        this->changes.reserve(5);
    }

    inline operator bool() { return !this->frontier.empty(); }

    typename T::value_type next() {
        auto change = frontier.front();
        this->frontier.erase(frontier.begin());
        return change;
    }

    std::vector<Change> collect(std::shared_ptr<StateChange> change) {
        std::vector<Change> actions{};
        while (change->prev) {
            actions.push_back(change->change);
            change = change->prev;
        }
        std::reverse(actions.begin(), actions.end());
        return std::move(actions);
    }

    template<typename S>
    bool seen(const typename T::value_type change);
    
    bool seen(const std::shared_ptr<StateChange> change) {
        if (std::find_if(frontier.begin(), frontier.end(), [&](const std::shared_ptr<StateChange> frontierChange) {
            return change->state == frontierChange->state;
        }) != frontier.end()) return true;
        if (explored.find(change->state) != explored.end()) {
            if (change->depth < explored[change->state]) {
                explored[change->state] = change->depth;
                return false;
            }
            return true;
        }
        return false;
    }

    bool seen(const std::pair<uint32_t, std::shared_ptr<StateChange>> change) {
        if (std::find_if(frontier.begin(), frontier.end(), [&](const std::pair<uint32_t, std::shared_ptr<StateChange>> frontierChange){
            return change.second->state == frontierChange.second->state;
        }) != frontier.end()) return true;
        if (explored.find(change.second->state) != explored.end()) return true;
        return false;
    }

    void expand(const std::shared_ptr<StateChange> change, void (T::*insert)(const typename T::value_type&), std::function<typename T::value_type(const std::shared_ptr<StateChange>)> convert=[](std::shared_ptr<StateChange> s){return s;}, uint32_t depthLimit = UINT32_MAX) {
        std::transform(CHANGES.begin(), CHANGES.end(), std::back_inserter(this->changes), [&](const Change action) {
            return std::make_shared<StateChange>(change->state, action, !change->changedLeft, change, change->depth + 1);
        });
        typename T::value_type val;
        for (const auto change : changes) {
            val = convert(change);
            if (this->seen(val) || !*change.get() || change->depth > depthLimit) continue;
            (this->frontier.*insert)(val);
        }
    }

private:
    std::vector<std::shared_ptr<StateChange>> changes;
};

std::pair<uint32_t, std::vector<Change>> runBFS(State& initialState, State& goalState);

std::tuple<bool, uint32_t, std::vector<Change>> runDFS(State& initialState, State& goalState, uint32_t depthLimit = UINT32_MAX);

std::pair<uint32_t, std::vector<Change>> runIDDFS(State& initialState, State& goalState);

std::pair<uint32_t, std::vector<Change>> runAStar(State& initialState, State& goalState);
