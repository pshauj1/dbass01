
#include "b_plus_tree.h"

static void merge(BPlusNode* node, int index){
    BPlusNode* left = node->children[index];
    BPlusNode* right = node->children[index+1];

    if(left->is_leaf){
        for(int i =0; i<right->num_keys; i++){
            left->keys.push_back(right->keys[i]);
            left->rids.push_back(right->rids[i]);
        }
        left->num_keys += right->num_keys;
        left->next = right->next;

    }
    else{
        left->keys.push_back(node->keys[index]);
        for(int i =0; i<right->num_keys; i++){
            left->keys.push_back(right->keys[i]);
            left->children.push_back(right->children[i]);
        }
        left->children.push_back(right->children[right->num_keys]);
        left->num_keys+=right->num_keys+1;
    }

    node->keys.erase(node->keys.begin()+ index);
    node->children.erase(node->children.begin() + index + 1);
    node->num_keys--;

    delete right;
}

void BPlusTree::fix_underflow(BPlusNode* node, int index) {
    if(index >0 && node->children[index-1]->num_keys > (getdeg()-1)/2){
        BPlusNode* child = node->children[index];
        BPlusNode* left = node->children[index -1];

        if(child->is_leaf){
            child->keys.insert(child->keys.begin(), left->keys.back());
            child->rids.insert(child->rids.begin(), left->rids.back());
            node->keys[index-1] = child->keys.front();
            left->rids.pop_back();
        }
        else{
            child->keys.insert(child->keys.begin(), node->keys[index -1]);
            child->children.insert(child->children.begin(), left->children.back());
            node->keys[index-1] = left->keys.back();
            left->children.pop_back();
        }
        left->keys.pop_back();
        child->num_keys++;
        left->num_keys--;
        return;
    }

    if(index < node->num_keys && node->children[index+1]->num_keys > (getdeg()-1)/2){
        BPlusNode* child = node->children[index];
        BPlusNode* right = node->children[index +1];

        if(child->is_leaf){
            child->keys.push_back(right->keys.front());
            child->rids.push_back(right->rids.front());
            right->rids.erase(right->rids.begin());
            node->keys[index] = right->keys[1];
        }
        else{
            child->keys.push_back(node->keys[index]);
            child->children.push_back(right->children.front());
            node->keys[index] = right->keys.front();
            right->children.erase(right->children.begin());
        }
        right->keys.erase(right->keys.begin());
        child->num_keys++;
        right->num_keys--;
        return;
    }

    if(index < node->num_keys) merge(node, index);
    else merge(node, index-1);
    
}

BPlusTarget* BPlusTree::recur_insert(BPlusNode* node, string key, int rid){
    int index = node->num_keys;
    for(int i=0; i<node->num_keys; i++){
        if(key < node->keys[i]){
            index =i;
            break;
        }
    }

    if(node->is_leaf){
        node->keys.insert(node->keys.begin() + index, key);
        node->rids.insert(node->rids.begin() + index, rid);
        node->num_keys++;

        if(node->num_keys < getdeg()) return nullptr;

        count++;
        BPlusNode* right = new BPlusNode(getdeg(), true);
        int mid = node->num_keys/2;

        for(int i =mid; i<node->num_keys; i++){
            right->keys.push_back(node->keys[i]);
            right->rids.push_back(node->rids[i]);
        }
        node->keys.erase(node->keys.begin() + mid, node->keys.end());
        node->rids.erase(node->rids.begin() + mid, node->rids.end());

        right->num_keys = right->keys.size();
        node->num_keys = node->keys.size();

        right->next = node->next;
        node->next = right;

        BPlusTarget* target = new BPlusTarget;
        target->node = right;
        target->key = right->keys[0];
        return target;
    }

    BPlusTarget* result = recur_insert(node->children[index], key, rid);

    if(result == nullptr) return result;

    node->keys.insert(node->keys.begin() + index, result->key);
    node->children.insert(node->children.begin() + index + 1, result->node);
    node->num_keys++;

    delete result;

    if(node->num_keys < getdeg()) return nullptr;

    count++;
    BPlusNode* right = new BPlusNode(getdeg(), false);
    int mid = node->num_keys/2;
    string target_key = node->keys[mid];

    for(int i = mid + 1; i<node->num_keys; i++){
        right->keys.push_back(node->keys[i]);
        right->children.push_back(node->children[i]);
    }
    right->children.push_back(node->children[node->num_keys]);

    node->keys.erase(node->keys.begin() + mid, node->keys.end());
    node->children.erase(node->children.begin() + mid + 1, node->children.end());

    right->num_keys=right->keys.size();
    node->num_keys = node->keys.size();

    BPlusTarget* target = new BPlusTarget;
    target->node = right;
    target->key = target_key;
    return target;

}

void BPlusTree::recur_remove(BPlusNode *node, string key)
{
    int index = node->num_keys;
    for(int i=0; i<node->num_keys; i++){
        if(key < node->keys[i]){
            index =i;
            break;
        }
    }
    if(node->is_leaf){
        for(int i=0; i< node->num_keys; i++){
            if(node->keys[i] == key){
                node->keys.erase(node->keys.begin() + i);
                node->rids.erase(node->rids.begin() + i);
                node->num_keys--;
                return;
            }
        }
        return;
    }

    recur_remove(node->children[index], key);
    
    if(node->children[index]->num_keys < (getdeg()-1)/2) fix_underflow(node, index);

}


void BPlusTree::insert(string key, int rid)
{
    if(getroot()==nullptr) {
        BPlusNode* new_root = new BPlusNode(getdeg(), true);
        new_root->rids.push_back(rid);
        new_root->keys.push_back(key);
        new_root->num_keys = 1;
        setroot(new_root);
        return;
    }

    BPlusTarget* result = recur_insert(getroot(), key, rid);

    if(result != nullptr) {
        BPlusNode* new_root = new BPlusNode(getdeg(), false);
        new_root->keys.push_back(result->key);
        new_root->children.push_back(getroot());
        new_root->children.push_back(result->node);
        new_root->num_keys =1;
        setroot(new_root);

        delete result;
    }

    return;
}

void BPlusTree::remove(string key)
{
    if (getroot() == nullptr) return;

    recur_remove(getroot(), key);

    if(getroot()->num_keys == 0 && !getroot()->is_leaf){
        BPlusNode* old_root = getroot();
        setroot(getroot()->children[0]);
        delete old_root;
    }
}

int BPlusTree::search(string key){
    if(getroot()==nullptr) return -1;
    BPlusNode* cur = getroot();

    while(!cur->is_leaf){
        int index = cur->num_keys;
        for(int i=0; i<cur->num_keys; i++){
            if(key<cur->keys[i]){
                index = i;
                break;
            }
        }
        cur = cur->children[index];
    }

    for(int i=0; i<cur->num_keys; i++){
        if(cur->keys[i]==key) return cur->rids[i];
    }

    return -1;
}

vector<int> BPlusTree::range_query(string p, string r){
    vector<int> result;
    if(getroot()==nullptr) return result;
    BPlusNode* cur = getroot();

    while(!cur->is_leaf){
        int index = cur->num_keys;
        for(int i=0; i<cur->num_keys; i++){
            if(p<cur->keys[i]){
                index = i;
                break;
            }
        }
        cur = cur->children[index];
    }

    while(cur!=nullptr){
        for(int i=0; i<cur->num_keys; i++){
            if(cur->keys[i]>=p && cur->keys[i]<=r){
                result.push_back(cur->rids[i]);
            }
            else if(cur->keys[i]>r) {
                return result;
            }
    }
    cur=cur->next;
    }
    return result;
}
