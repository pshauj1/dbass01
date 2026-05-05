#include "b_tree.h"

Target* split(BTreeNode* full_node){
    int target_idx = (full_node->num_keys / 2); //calculate target index
    BTreeNode* right_node = new BTreeNode(full_node->d, full_node->is_leaf);

    for(int i= target_idx+1; i < full_node->num_keys; i++) {  //move data to new right node for split
        right_node->keys.push_back(full_node->keys[i]);
        right_node->rids.push_back(full_node->rids[i]);
    }

    if(!full_node->children.empty()) {
        for(int i= target_idx+1; i <= full_node->num_keys; i++) {  //move data to new right node for split
            right_node->children.push_back(full_node->children[i]);
        }
    }

    right_node->num_keys = right_node->keys.size();

    Target* target = new Target();
    target->key = full_node->keys[target_idx];
    target->rid = full_node->rids[target_idx];
    target->node = right_node;

    full_node->keys.erase(full_node->keys.begin() + target_idx, full_node->keys.end());
    full_node->rids.erase(full_node->rids.begin() + target_idx, full_node->rids.end());

    if(!full_node->children.empty()) {
        full_node->children.erase(full_node->children.begin()+target_idx + 1, full_node->children.end());
    }

    full_node->num_keys = full_node->keys.size();

    return target;
}

void BTree::fix_underflow(BTreeNode* node, int index) {
    if(index >0 && node->children[index-1]->num_keys > (getdeg()-1)/2){
        BTreeNode* child = node->children[index];
        BTreeNode* left = node->children[index -1];

        child->keys.insert(child->keys.begin(), node->keys[index-1]);
        child->rids.insert(child->rids.begin(), node->rids[index-1]);
        child->num_keys++;

        if(!child->is_leaf) {
            child->children.insert(child->children.begin(), left->children.back());
            left->children.pop_back();
        }

        node->keys[index-1] = left->keys.back();
        node->rids[index-1] = left->rids.back();

        left->keys.pop_back();
        left->rids.pop_back();
        left->num_keys--;

        return;
    }

    if(index < node->num_keys && node->children[index+1]->num_keys > (getdeg()-1)/2){
        BTreeNode* child = node->children[index];
        BTreeNode* right = node->children[index +1];

        child->keys.push_back(node->keys[index]);
        child->rids.push_back(node->rids[index]);
        child->num_keys++;

        if(!child->is_leaf){
            child->children.push_back(right->children.front());
            right->children.erase(right->children.begin());
        }

        node->keys[index] = right->keys.front();
        node->rids[index] = right->rids.front();

        right->keys.erase(right->keys.begin());
        right->rids.erase(right->rids.begin());
        right->num_keys--;

        return;
    }

    if(index < node->num_keys) merge(node, index);
    else merge(node, index-1);
    
}

Target* BTree::recur_insert(BTreeNode* node, string key, int rid){

    if(node->is_leaf){

        int key_index = node->num_keys;

        for(int i=0; i<node->num_keys; i++){
            if (key < node->keys[i]){
                key_index=i;
                break;
            }   
        }

        node->keys.insert(node->keys.begin()+key_index, key);
        node->rids.insert(node->rids.begin()+key_index, rid);

        node->num_keys++;

        if(node->num_keys>= getdeg()) {
            count++;
            return split(node);
        }
        else return nullptr;
    }

    int ch_index = node->num_keys;

    for(int i=0; i<node->num_keys; i++){
        if (key < node->keys[i]){
            ch_index=i;
            break;
        }   
    }

    Target* result = recur_insert(node->children[ch_index], key, rid);

    if(result==nullptr) return result;

    else {
        int index = node->num_keys;

        for(int i=0; i<node->num_keys; i++){
            if (result->key < node->keys[i]){
                index=i;
                break;
            }   
        }

        node->keys.insert(node->keys.begin()+index, result->key);
        node->rids.insert(node->rids.begin()+index, result->rid);
        node->children.insert(node->children.begin() + index + 1 , result->node);

        node->num_keys++;
        delete result;

        if(node->num_keys>= getdeg()) {
            count++;
            return split(node);
        }
        else return nullptr;
    }

}

int BTree::recur_search(BTreeNode* node, string key){
    if(node == nullptr) return -1; //base case

    int index = node->num_keys;

    for(int i=0; i<node->num_keys; i++){

        if(key==node->keys[i]){
            return node->rids[i];
        }

        if(key<node->keys[i]){
            index = i;
            break;
        }
    }

    if(node->is_leaf) return -1;
    return recur_search(node->children[index], key);
}

void BTree::recur_remove(BTreeNode* node, string key) {
    int index = node->num_keys;
    bool found = false;

    for(int i=0; i< node->num_keys; i++){
        if(key==node->keys[i]){
            found = true;
            index = i;
            break;
        }
        if(key < node->keys[i]){
            index = i;
            break;
        }
    }

    if(found){
        if(node->is_leaf) {
            node->keys.erase(node->keys.begin() + index);
            node->rids.erase(node->rids.begin() + index);
            node->num_keys--;
            return;
        }
        else {
            Target result = find_max_key(node->children[index]);
            node->keys[index] = result.key;
            node->rids[index] = result.rid;
            recur_remove(node->children[index], result.key);
        }
    }
    else {
        if(node->is_leaf) return;
        recur_remove(node->children[index], key);
    }

    if(!node->is_leaf) {
        if(node->children[index]->num_keys < (getdeg()-1)/2) fix_underflow(node, index);
    }
}


void BTree::insert(string key, int rid){

    if(getroot()==nullptr) {
        root = new BTreeNode(d, true);
        root->rids.push_back(rid);
        root->keys.push_back(key);
        root->num_keys = 1;
        return;
    }

    Target* result = recur_insert(getroot(), key, rid);

    if(result!=nullptr){
        BTreeNode* new_root = new BTreeNode(d, false);

        new_root->keys.push_back(result->key);
        new_root->rids.push_back(result->rid);
        new_root->children.push_back(getroot());
        new_root->children.push_back(result->node);
        new_root->num_keys = 1;
        setroot(new_root);
        delete result;
    }


}

int BTree::search(string key)
{
    return recur_search(getroot(), key);
}

void BTree::remove(string key) {
    if(getroot()==nullptr) return; //base case
    recur_remove(getroot(), key);
    if(root->num_keys==0 && !root->is_leaf) {
        BTreeNode* old = getroot();
        setroot(getroot()->children[0]);
        delete old;
    }
}

void BTree::recur_range_query(BTreeNode* node, string p, string r, vector<int>& result){
    if(node == nullptr) return;
    int index = node->num_keys;
    for(int i=0; i<node->num_keys; i++){
        if(node->keys[i] >= p){
            index = i;
            break;
        } 
    }
    
    while(index < node->num_keys && node->keys[index] <= r){
        if(!node->is_leaf) recur_range_query(node->children[index], p, r, result);
        result.push_back(node->rids[index]);
        index++;
    }

    if(!node->is_leaf) recur_range_query(node->children[index], p, r,result);
}

vector<int> BTree::range_query(string p, string r)
{
    vector<int> result;
    recur_range_query(getroot(), p, r, result);
    return result;
}
