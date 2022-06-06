#include "searches.h"

#include <iostream>

std::pair<uint32_t, std::vector<Change>> runBFS(State& initialState, State& goal) {
    uint32_t expanded = 0;
    GraphSearch<std::list<std::shared_ptr<StateChange>>> search;
    search.frontier.emplace_back(new StateChange{initialState});
    while (search) {
        std::shared_ptr<StateChange> current = search.next();

        // Is the current state the goal state?
        if (current->state == goal) {
            return {expanded, search.collect(current)};
        }

        // We have now explored the current state.
        search.explored.insert(std::pair<State, uint32_t>{current->state, 0});
        expanded++;

        // Expand current.
        search.expand(current, (void (std::list<std::shared_ptr<StateChange>>::*)(const std::shared_ptr<StateChange>&)) &std::list<std::shared_ptr<StateChange>>::push_back);
    }
    return {expanded, {}};
}

std::tuple<bool, uint32_t, std::vector<Change>> runDFS(State& initialState, State& goal, uint32_t depthLimit) {
    uint32_t depth = 0;
    uint32_t expanded = 0;
    GraphSearch<std::list<std::shared_ptr<StateChange>>> search;
    search.frontier.emplace_front(new StateChange(initialState));
    while (search) {
        std::shared_ptr<StateChange> current = search.next();
        depth = std::max(current->depth, depth);
        if (current->state == goal) {
            return std::tuple<bool, uint32_t, std::vector<Change>>{true, expanded, search.collect(current)};
        }

        search.explored.insert(std::pair<State, uint32_t>{current->state, depthLimit == UINT32_MAX ? 0 : current->depth});
        expanded++;

        search.expand(
            current,
            (void (std::list<std::shared_ptr<StateChange>>::*)(const std::shared_ptr<StateChange>&)) &std::list<std::shared_ptr<StateChange>>::push_front,
            [](const std::shared_ptr<StateChange> s){return s;},
            depthLimit
        );
    }
    return std::tuple<bool, uint32_t, std::vector<Change>>{depthLimit > depth, expanded, std::vector<Change>{}};
}

std::pair<uint32_t, std::vector<Change>> runIDDFS(State& initialState, State& goal) {
    bool end = false;
    uint32_t incrementingDepth = 0;
    std::pair<uint32_t, std::vector<Change>> result{0, {}};
    while (!end) {
        auto dfsResult = runDFS(initialState, goal, incrementingDepth);
        end = std::get<0>(dfsResult);
        result.first += std::get<1>(dfsResult);
        result.second = std::get<2>(dfsResult);
        incrementingDepth++;
    }
    return result;
}

std::pair<uint32_t, std::vector<Change>> runAStar(State& initialState, State& goal) {
    uint32_t expanded = 0;
    GraphSearch<std::vector<std::pair<float, std::shared_ptr<StateChange>>>> search;
    search.frontier.push_back({0, std::make_shared<StateChange>(initialState)});
    while (search) {
        std::pair<uint32_t, std::shared_ptr<StateChange>> current = search.next();
        if (current.second->state == goal) {
            return {expanded, search.collect(current.second)};
        }

        search.explored.insert(std::pair<State, uint32_t>{current.second->state, 0});
        expanded++;

        search.expand(
            current.second,
            (void (std::vector<std::pair<float, std::shared_ptr<StateChange>>>::*)(const std::pair<float, std::shared_ptr<StateChange>>&)) &std::vector<std::pair<float, std::shared_ptr<StateChange>>>::push_back,
            [&](const std::shared_ptr<StateChange> change) {
                return std::pair<float, std::shared_ptr<StateChange>>{current.second->depth + change->cost(goal), change};
            }
        );
        std::stable_sort(search.frontier.begin(), search.frontier.end(), [](const std::pair<float, std::shared_ptr<StateChange>>& one, const std::pair<float, std::shared_ptr<StateChange>>& two) {
            return one.first < two.first;
        });
    }
    return {expanded, {}};
}
