#include <stdio.h>
#include <iostream>
#include <fstream>
#include <assert.h>

#include "searches.h"

template<typename T>
std::shared_ptr<T> OpenFile(const char* filepath) {
    std::shared_ptr<T> stream = std::make_shared<T>(filepath);
    if (!stream) {
        fprintf(stderr, "%s cannot be opened.\n", filepath);
        exit(1);
    }
    return stream;
}

void WriteOutput(std::ostream& output, uint32_t expandedNodes, const std::vector<Change>& actions) {
    output << "We expanded " << expandedNodes << (expandedNodes > 1 ? " nodes.\n" : " node.\n");
    
    output << "Path to goal: ";
    if (expandedNodes == 0) output << "Start state is goal state.\n";
    else if (actions.empty()) output << "No solution";
    else {
        output << actions[0];
        for (auto it = actions.cbegin()+1; it != actions.cend(); it++) output << ", " << *it;
        output << "\n";
        output << "Path is " << actions.size() << (actions.size() == 1 ? " step long." : " steps long.");
    }
    output << std::endl;
}

void Assert_Answer(const State& initial, const State& goal, const std::vector<Change>& changes) {
    std::shared_ptr<StateChange> s = std::make_shared<StateChange>(initial);
    for (const auto& change : changes) {
        s = std::make_shared<StateChange>(s->state, change, !s->changedLeft, s, s->depth+1);
    }
    assert(goal == s->state);
}

int main(int argc, char** args) {
    if (argc < 5) {
        fprintf(stderr, "Usage %s <initial_state_file> <goal_state_file> <mode> <output_file>\n", args[0]);
        return 1;
    }
    std::shared_ptr<std::ifstream> initial_state = OpenFile<std::ifstream>(args[1]);
    std::shared_ptr<std::ifstream> goal_state = OpenFile<std::ifstream>(args[2]);
    const std::string mode = args[3];
    std::shared_ptr<std::ofstream> output = OpenFile<std::ofstream>(args[4]);

    State initial = ReadState(*initial_state);
    State goal = ReadState(*goal_state);

    auto result = std::pair<uint32_t, std::vector<Change>>{};
    if (mode == "bfs") {
        result = runBFS(initial, goal);
    } else if (mode == "dfs") {
        auto dfsResult = runDFS(initial, goal);
        result.first = std::get<1>(dfsResult);
        result.second = std::get<2>(dfsResult);
    } else if (mode == "iddfs") {
        result = runIDDFS(initial, goal);
    } else if (mode == "astar") {
        result = runAStar(initial, goal);
    } else {
        fprintf(stderr, "%s is not a valid mode{bfs, dfs, iddfs, astar}.\n", mode.c_str());
        return 1;
    }
    Assert_Answer(initial, goal, result.second);
    WriteOutput(*output, result.first, result.second);
    WriteOutput(std::cout, result.first, result.second);
    return 0;
}