#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;


class BPlusNode {
    public:
    int d;
    bool is_leaf;
    int num_keys;

    vector<string> keys;
    vector<BPlusNode*> children;
    vector<int> rids;
    BPlusNode* next;

    BPlusNode(int degree, bool leaf) {
        d = degree;
        is_leaf = leaf;
        num_keys=0;
        next = nullptr;
    }
};

struct BPlusTarget {
    BPlusNode* node;
    string key;
};


class BPlusTree{
private:
    BPlusNode* root;
    int d;

public:
    int count;

    BPlusTree(int degree){
        root = nullptr;
        d=degree;
        count = 0;
    }
    BPlusNode* getroot() {
        return root;
    }

    int getdeg() {
        return d;
    }

    void setroot(BPlusNode* rt){
        root = rt;
    } 

    void setdeg(int degree) {
        d = degree;
    }
    void fix_underflow(BPlusNode* node, int index);
    BPlusTarget* recur_insert(BPlusNode* node, string key, int rid);
    void recur_remove(BPlusNode* node, string key);
    void insert(string key, int rid);
    int search(string key);
    void remove(string key);
    vector<int> range_query(string p, string r);
};