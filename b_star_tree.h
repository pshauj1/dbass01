#pragma once
#include <string>
#include "struct.h"

using namespace std;

class BStarTree {
private:
    BTreeNode* root;
    int d; 

public:

    int count;

    BStarTree(int order) {
        root = nullptr;
        d = order;
        count=0;
    }
    
    BTreeNode* getroot() {
        return root;
    }

    int getdeg() {
        return d;
    }

    void setroot(BTreeNode* rt){
        root = rt;
    } 

    void setdeg(int degree) {
        d = degree;
    }

    void recur_insert(BTreeNode* node, string key, int rid);
    int recur_search(BTreeNode* node, string key);
    void recur_remove(BTreeNode* node, string key);
    void fix_underflow(BTreeNode* node, int index);

    void insert(string key, int rid);
    int search(string key);
    void remove(string key);
    void recur_range_query(BTreeNode* node, string p, string r, vector<int>& result);
    vector<int> range_query(string p, string r);
};

void split23(BTreeNode* node, int index);