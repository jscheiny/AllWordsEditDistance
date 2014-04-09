#ifndef DAWG_H
#define DAWG_H

#include <string>
#include <vector>
#include <set>
#include <unordered_set>

class Dawg {

public:
    /// The character of a node that represents the end of a word.
    static const char kEow;

    class Node;

    friend Node;

    Dawg();

    Dawg(const Dawg& other);
    Dawg(Dawg&& other);

    template<typename InputIterator>
    Dawg(InputIterator begin, InputIterator end);

    ~Dawg();

    Dawg& operator= (const Dawg& other);
    Dawg& operator= (Dawg&& other);

    int words() const {
        return words_;
    }
    int nodes() const {
        return nodes_;
    }
    const Node* root() const {
        return root_;
    }

    void Clear();

    bool Insert(const std::string& word);
    bool Contains(const std::string& word) const;
    const Node* Prefix(const std::string& prefix) const;

    template<typename Function>
    void EachWord(Function fn) const;

    void Swap(Dawg& other);

private:
    int words_;
    int nodes_;
    Node* root_;
    Node* eow_;
};

class Dawg::Node {
public:
    Node(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) = delete;

    char letter() const {
        return letter_;
    }

    void PutChild(Node* child) {
        children_[Index(child->letter_)] = child;
    }

    bool HasChild(char letter) const {
        return children_[Index(letter)] != nullptr;
    }

    const Node* GetChild(char letter) const {
        return children_[Index(letter)];
    }

    Node* GetChild(char letter) {
        return children_[Index(letter)];
    }

    std::string GetChildren() const;

    template<typename Function>
    void EachSuffix(Function fn) const;

    friend Dawg;

private:
    char letter_;
    Node* children_[27];

    Node(char letter, Dawg& dawg) : letter_(letter), children_() {
        dawg.nodes_++;
    }

    template<typename Function>
    static void EachSuffix(const Node* root, const std::string& prefix,
                           Function& fn);

    static int Index(char let) {
        return let - 'A';
    }
};

// Template function definitions

template<typename InputIterator>
Dawg::Dawg(InputIterator begin, InputIterator end) : Dawg() {
    for(auto itr = begin; itr != end; ++itr) {
        Insert(*itr);
    }
}

template<typename Function>
void Dawg::EachWord(Function fn) const {
    root_->EachSuffix(fn);
}

template<typename Function>
void Dawg::Node::EachSuffix(Function fn) const {
    EachSuffix(this, {letter_, '\0'}, fn);
}

template<typename Function>
void Dawg::Node::EachSuffix(const Node* root, const std::string& prefix,
                            Function& fn) {
    if(root->HasChild(kEow)) {
        fn(prefix);
    }
    for(char letter = 'A'; letter <= 'Z'; letter++) {
        if(root->HasChild(letter)) {
            EachSuffix(root->GetChild(letter), prefix + letter, fn);
        }
    }
}

#endif
