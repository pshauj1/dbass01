#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class BTreeNode {
public:
    int d;                  
    bool is_leaf;           
    int num_keys;           
    
    vector<string> keys;   
    vector<int> rids;           
    vector<BTreeNode*> children; 

    BTreeNode(int order, bool leaf) {
        d = order;
        is_leaf = leaf;
        num_keys = 0;
    }
    
    ~BTreeNode() {
       
    }
};

struct Target {
    BTreeNode* node;
    string key;
    int rid;
};

Target find_max_key(BTreeNode* node);
void merge(BTreeNode* node, int index);

