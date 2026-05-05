#include "struct.h"

Target find_max_key(BTreeNode* node){
    BTreeNode* curr = node;
    while (!curr->is_leaf){
        curr = curr->children[curr->num_keys];
    }
    Target t;
    t.key = curr->keys[curr->num_keys-1];
    t.rid = curr->rids[curr->num_keys-1];
    return t;
}

void merge(BTreeNode* node, int index){
    BTreeNode* left = node->children[index];
    BTreeNode* right = node->children[index+1];

    left->keys.push_back(node->keys[index]);
    left->rids.push_back(node->rids[index]);

    for(int i =0; i<right->num_keys; i++){
        left->keys.push_back(right->keys[i]);
        left->rids.push_back(right->rids[i]);
    }

    left->num_keys += right->num_keys + 1;

    if(!left->is_leaf) {
        for(int i= 0; i<= right->num_keys; i++){
            left->children.push_back(right->children[i]);
        }
    }

    node->keys.erase(node->keys.begin()+ index);
    node->rids.erase(node->rids.begin()+ index);
    node->children.erase(node->children.begin() + index + 1);
    node->num_keys--;

    delete right;
}
