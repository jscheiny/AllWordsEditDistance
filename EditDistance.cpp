#include <string>
#include <set>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>

#include "Dawg.h"

using namespace std;

enum Operation {
    INS, DEL, SUB
};

unsigned StdCost(Operation op) {
    return 1;
}

using CostFn = std::function<unsigned(Operation)>;

using State = tuple<int, int, const Dawg::Node*>;

void AllWordsEditDist(const string& source, int index,
                      const string& prefix, int edits,
                      const Dawg::Node* root,
                      CostFn cost,
                      set<string>* words,
                      set<State>* visited) {
    // Don't revisit states unecessarily
    State currState{make_tuple(index, edits, root)};
    if(visited->find(currState) != visited->end()) {
        return;
    }
    // Memoize current state
    visited->insert(currState);

    if(edits < 0) {
        return;
    }
    if(index == source.size() && root->HasChild(Dawg::kEow)) {
        words->insert(prefix);
    }
    char curr = source[index];
    auto child = root->GetChild(curr);
    if(child) {
        // No edit
        AllWordsEditDist(source, index + 1, prefix + curr, edits, child,
                         cost, words, visited);
    }
    // Deletion
    AllWordsEditDist(source, index + 1, prefix, edits - cost(DEL), root,
                     cost, words, visited);
    for(char c = 'A'; c <= 'Z'; c++) {
        auto child = root->GetChild(c);
        if(child) {
            // Insertion
            AllWordsEditDist(source, index, prefix + c, edits - cost(INS), child,
                             cost, words, visited);
            // Substitution
            if(c != curr) {
                AllWordsEditDist(source, index + 1, prefix + c, edits - cost(SUB), child,
                                 cost, words, visited);
            }
        }
    }
}

set<string> AllWordsEditDist(const string& source,
                             int edits,
                             const Dawg& lexicon,
                             CostFn cost) {
    set<string> words;
    set<State> visited;
    AllWordsEditDist(source, 0, "", edits, lexicon.root(), cost, &words, &visited);
    return words;
}

int main(int argc, char const *argv[]) {
    Dawg d;
    ifstream twl("list.txt");
    string line;
    while(getline(twl, line)) {
        d.Insert(line);
    }

    auto words = AllWordsEditDist("HELLO", 2, d, StdCost);
    copy(words.begin(), words.end(), ostream_iterator<string>(cout, "\n"));
    return 0;
}
