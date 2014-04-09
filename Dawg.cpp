/*
 * Nigel Scrabble artificial intelligence and analysis.
 * Copyright (C) 2013 by Jonah and Daniel Scheinerman
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file Dawg.cpp
 * @brief Implementation of the Dawg class.
 */

#include "Dawg.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <set>

using namespace std;

using Edge = pair<int, int>;

const char Dawg::kEow = 'Z' + 1;

// Traverse the dawg represented by the given node, adding all nodes to the
// deleted nodes set.
void DeleteNode(Dawg::Node* node, set<Dawg::Node*>& deleted_nodes);

// Connects the nodes in the vector together using the edges provided.
void MakeConnections(vector<Dawg::Node*>& nodes, vector<Edge>& edges);

string Dawg::Node::GetChildren() const {
    string result;
    for(char letter = 'A'; letter <= 'Z'; letter++) {
        if(children_[Index(letter)] != nullptr) {
            result += letter;
        }
    }
    return result;
}

Dawg::Dawg() :
words_(0), nodes_(0), root_(new Node('\0', *this)), eow_(new Node(kEow, *this)) {}

Dawg::Dawg(const Dawg& other) : Dawg() {
    other.EachWord([this] (const string& word) {
        Insert(word);
    });
}

Dawg::Dawg(Dawg&& other) :
words_(other.words_), nodes_(other.nodes_), root_(other.root_),
eow_(other.eow_) {
    other.root_ = nullptr;
    other.eow_ = nullptr;
}

void MakeConnections(vector<Dawg::Node*>& nodes, vector<Edge>& edges) {
    for(Edge& e : edges) {
        Dawg::Node* parent = nodes[e.first];
        Dawg::Node* child = nodes[e.second];
        parent->PutChild(child);
    }
}

Dawg::~Dawg() {
    Clear();
    delete root_;
    delete eow_;
}

void Dawg::Clear() {
    words_ = 0;
    nodes_ = 0;
    if(root_ != nullptr) {
        set<Node*> deleted_nodes;
        DeleteNode(root_, deleted_nodes);
    }
    root_ = new Node('\0', *this);
    eow_ = new Node(kEow, *this);
}

void DeleteNode(Dawg::Node* node, set<Dawg::Node*>& deleted_nodes) {
    if(deleted_nodes.count(node) == 0) {
        for(char letter = 'A'; letter <= Dawg::kEow; letter++) {
            if(node->HasChild(letter)) {
                DeleteNode(node->GetChild(letter), deleted_nodes);
            }
        }
        deleted_nodes.insert(node);
        delete node;
    }
}

Dawg& Dawg::operator= (const Dawg& other) {
    Dawg copy(other);
    Swap(copy);
    return *this;
}

Dawg& Dawg::operator= (Dawg&& other) {
    Swap(other);
    return *this;
}

bool Dawg::Insert(const string& word) {
    Node* current = root_;
    for(char letter : word) {
        Node* child = current->GetChild(letter);
        if(!child) {
            child = new Node(letter, *this);
            current->PutChild(child);
        }
        current = child;
    }
    if(current->HasChild(kEow))
        return true;
    words_++;
    current->PutChild(eow_);
    return false;
}

bool Dawg::Contains(const string& word) const {
    Node* current = root_;
    for(char let : word) {
        Node* child = current->GetChild(let);
        if(child) {
            current = child;
        } else {
            return false;
        }
    }
    return current->HasChild(kEow);
}

const Dawg::Node* Dawg::Prefix(const string& prefix) const {
    Node* current = root_;
    for(char let : prefix) {
        Node* child = current->GetChild(let);
        if(child) {
            current = child;
        } else {
            return nullptr;
        }
    }
    return current;
}

void Dawg::Swap(Dawg& other) {
    swap(words_, other.words_);
    swap(nodes_, other.nodes_);
    swap(root_, other.root_);
    swap(eow_, other.eow_);
}
