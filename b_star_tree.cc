#include "b_star_tree.h"

void split23(BTreeNode* node, int index){
    vector<string> keys;
    vector<int> rids;
    vector<BTreeNode*> children;

    BTreeNode* left = node->children[index];
    BTreeNode* right = node->children[index+1];

    for(int i=0; i<left->num_keys; i++){
        keys.push_back(left->keys[i]);
        rids.push_back(left->rids[i]);
    }

    keys.push_back(node->keys[index]);
    rids.push_back(node->rids[index]);

    for(int i=0; i<right->num_keys; i++){
        keys.push_back(right->keys[i]);
        rids.push_back(right->rids[i]);
    }

    if(!left->is_leaf && !right->is_leaf){
        for(int i=0; i<=left->num_keys; i++) children.push_back(left->children[i]);
        for(int i=0; i<=right->num_keys; i++) children.push_back(right->children[i]);
    }

    left->keys.clear();
    left->rids.clear();
    left->children.clear();
    right->keys.clear();
    right->rids.clear();
    right->children.clear();

    int size = keys.size();
    int index1 = size/3;
    int index2 = (2 * (size)) / 3;

    BTreeNode* new_node = new BTreeNode(node->d, left->is_leaf);
    for(int i=0; i<index1; i++){
        left->keys.push_back(keys[i]);
        left->rids.push_back(rids[i]);
    }

    for(int i=index1+1; i<index2; i++){
        right->keys.push_back(keys[i]);
        right->rids.push_back(rids[i]);
    }

    for(int i = index2+1; i<size; i++){
        new_node->keys.push_back(keys[i]);
        new_node->rids.push_back(rids[i]);
    }

    if(!left->is_leaf && !right->is_leaf){
       for(int i=0; i<=index1; i++) left->children.push_back(children[i]);
       for(int i=index1 +1; i<=index2; i++) right->children.push_back(children[i]);
       for(int i=index2 +1; i<=size; i++) new_node->children.push_back(children[i]);
    }


    node->keys[index] = keys[index1];
    node->rids[index] = rids[index1];

    node->keys.insert(node->keys.begin()+index+1, keys[index2]);
    node->rids.insert(node->rids.begin()+index+1, rids[index2]);
    node->children.insert(node->children.begin() + index + 2, new_node);

    left->num_keys = left->keys.size();
    right->num_keys = right->keys.size();
    new_node->num_keys = new_node->keys.size();
    node->num_keys++;

    
}

void BStarTree::fix_underflow(BTreeNode* node, int index) {
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

void BStarTree::recur_insert(BTreeNode* node, string key, int rid){

    int index = node->num_keys;
    for(int i=0; i<node->num_keys; i++){
        if (key < node->keys[i]){
            index=i;
            break;
        }   
    }

    if(node->is_leaf){

        node->keys.insert(node->keys.begin()+index, key);
        node->rids.insert(node->rids.begin()+index, rid);

        node->num_keys++;
        return;
    }

    recur_insert(node->children[index], key, rid);

    if(node->children[index]->num_keys == getdeg()){
        if(index>0 && node->children[index-1]->num_keys < getdeg() -1){
            BTreeNode* target = node->children[index];
            BTreeNode* left = node->children[index - 1];
            left->keys.push_back(node->keys[index-1]);
            left->rids.push_back(node->rids[index-1]);
            left->num_keys++;

            if(!target->is_leaf) {
                left->children.push_back(target->children.front());
                target->children.erase(target->children.begin());
            }

            node->keys[index-1] = target->keys.front();
            node->rids[index-1] = target->rids.front();

            target->keys.erase(target->keys.begin());
            target->rids.erase(target->rids.begin());
            target->num_keys--;

            return;

        }

        if(index<node->num_keys && node->children[index+1]->num_keys <getdeg()-1){
            BTreeNode* target = node->children[index];
            BTreeNode* right = node->children[index +1];

            right->keys.insert(right->keys.begin(), node->keys[index]);
            right->rids.insert(right->rids.begin(), node->rids[index]);
            right->num_keys++;

            if(!target->is_leaf) {
                right->children.insert(right->children.begin(), target->children.back());
                target->children.pop_back();
            }

            node->keys[index] = target->keys.back();
            node->rids[index] = target->rids.back();

            target->keys.pop_back();
            target->rids.pop_back();
            target->num_keys--;

            return;
        }

        if(index < node->num_keys) {
            count++;
            split23(node, index);
        }
        else {
            count++;
            split23(node, index-1);
        }
    }

}

int BStarTree::recur_search(BTreeNode* node, string key){
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

void BStarTree::recur_remove(BTreeNode* node, string key) {
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


void BStarTree::insert(string key, int rid){

    if(getroot()==nullptr) {
        BTreeNode* new_root = new BTreeNode(getdeg(), true);
        new_root->rids.push_back(rid);
        new_root->keys.push_back(key);
        new_root->num_keys = 1;
        setroot(new_root);
        return;
    }

    recur_insert(getroot(), key, rid);

    if(getroot()->num_keys == getdeg()){
        count++;
        BTreeNode* full_node = getroot();
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

        string key = full_node->keys[target_idx];
        int rid = full_node->rids[target_idx];

        full_node->keys.erase(full_node->keys.begin() + target_idx, full_node->keys.end());
        full_node->rids.erase(full_node->rids.begin() + target_idx, full_node->rids.end());

        if(!full_node->children.empty()) {
            full_node->children.erase(full_node->children.begin()+target_idx + 1, full_node->children.end());
        }

        full_node->num_keys = full_node->keys.size();

        BTreeNode* new_root = new BTreeNode(getdeg(), false);
        new_root->keys.push_back(key);
        new_root->rids.push_back(rid);
        new_root->children.push_back(full_node);
        new_root->children.push_back(right_node);
        new_root->num_keys=1;
        
        setroot(new_root);
    }


}

int BStarTree::search(string key)
{
    return recur_search(getroot(), key);
}

void BStarTree::remove(string key) {
    if(getroot()==nullptr) return; //base case
    recur_remove(getroot(), key);
    if(root->num_keys==0 && !root->is_leaf) {
        BTreeNode* old = getroot();
        setroot(getroot()->children[0]);
        delete old;
    }
}

void BStarTree::recur_range_query(BTreeNode* node, string p, string r, vector<int>& result){
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

vector<int> BStarTree::range_query(string p, string r)
{
    vector<int> result;
    recur_range_query(getroot(), p, r, result);
    return result;
}

