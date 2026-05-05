#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>


#include "std_record.h"
#include "b_tree.h"
#include "b_star_tree.h"
#include "b_plus_tree.h" 

using namespace std;
using namespace std::chrono;

void test_degree(int degree , int data_size, vector<StudentRecord> db);
void test_randomsearch(int degree , int data_size, vector<StudentRecord> db);
void test_query(int degree , int data_size, vector<StudentRecord> db);
void test_randomdelete(int degree , int data_size, vector<StudentRecord> db, int count);

int main() {
    vector<StudentRecord> db;
    ifstream file("student.csv");

    if (!file.is_open()) {
        cout << "can not open student.csv" << endl;
        return -1;
    }

    string line;
    getline(file, line);

    while(getline(file, line)) {
        stringstream s(line);
        string token;
        StudentRecord record;

        getline(s, record.student_id, ',');
        getline(s, record.name, ',');
        getline(s, record.gender, ',');

        getline(s, token, ',');
        record.gpa = stof(token);

        getline(s, token, ',');
        record.height = stof(token);

        getline(s, token, ',');
        record.weight = stof(token);

        db.push_back(record);
    }
    file.close();
    int data_size = db.size();

    cout<<"/////////////////////////"<<"Insertion & Parameter Tuning"<<"/////////////////////////"<<endl;
    test_degree(3, data_size, db);
    test_degree(5, data_size, db);
    test_degree(10, data_size, db);
    cout<<""<<endl;
    test_randomsearch(10 ,data_size, db);
    cout<<""<<endl;
    test_query(10, data_size, db);
    cout<<""<<endl;
    test_randomdelete(10, data_size, db, 10000);
}


void count_btree(BTreeNode* node, int& num_keys, int& num_nodes){
    num_keys += node->num_keys;
    num_nodes++;
    if(!node->is_leaf){
        for(int i=0; i<=node->num_keys; i++){
            count_btree(node->children[i], num_keys, num_nodes);
        }
    }
}

void count_b_plus_tree(BPlusNode* node, int& num_keys, int& num_nodes){
    num_keys += node->num_keys;
    num_nodes++;
    if(!node->is_leaf){
        for(int i=0; i<=node->num_keys; i++){
            count_b_plus_tree(node->children[i], num_keys, num_nodes);
        }
    }
}

void test_degree(int degree , int data_size, vector<StudentRecord> db){
    time_point<high_resolution_clock> start, end;
    duration<double, milli> btree_time, bstar_time, bplus_time;
    int b_nodes = 0, b_keys = 0;
    int bs_nodes = 0, bs_keys = 0;
    int bp_nodes = 0, bp_keys = 0;

    BTree btree(degree);
    BStarTree bstartree(degree);
    BPlusTree bplustree(degree); 

    start=high_resolution_clock::now();
    for(int i=0; i<data_size; i++) btree.insert(db[i].student_id, i);
    end=high_resolution_clock::now();
    btree_time = end - start;

    start=high_resolution_clock::now();
    for(int i=0; i<data_size; i++) bstartree.insert(db[i].student_id, i);
    end=high_resolution_clock::now();
    bstar_time = end - start;

    start=high_resolution_clock::now();
    for(int i=0; i<data_size; i++) bplustree.insert(db[i].student_id, i);
    end=high_resolution_clock::now();
    bplus_time = end - start;

    count_btree(btree.getroot(), b_keys, b_nodes);
    count_btree(bstartree.getroot(), bs_keys, bs_nodes);
    count_b_plus_tree(bplustree.getroot(), bp_keys, bp_nodes);
    
    cout<<"/////////////////////////"<<"degree:"<<degree<<"/////////////////////////"<<endl;
    cout<<"[b-tree] "<<"excution time: "<<btree_time.count()<<" node_util: "<<((float)b_keys / (b_nodes * (degree - 1))) * 100.0f<<"%"<<" split: "<<btree.count<<endl;
    cout<<"[b*-tree] "<<"excution time: "<<bstar_time.count()<<" node_util: "<<((float)bs_keys / (bs_nodes * (degree - 1))) * 100.0f<<"%"<<" split: "<<bstartree.count<<endl;
    cout<<"[b+-tree] "<<"excution time: "<<bplus_time.count()<<" node_util: "<<((float)bp_keys / (bp_nodes * (degree - 1))) * 100.0f<<"%"<<" split: "<<bplustree.count<<endl;
}

void test_randomsearch(int degree , int data_size, vector<StudentRecord> db){
    time_point<high_resolution_clock> start, end;
    duration<double, milli> btree_time, bstar_time, bplus_time;
    int count=10000;

    BTree btree(degree);
    BStarTree bstartree(degree);
    BPlusTree bplustree(degree); 

    for(int i=0; i<data_size; i++){
        btree.insert(db[i].student_id, i);
        bstartree.insert(db[i].student_id, i);
        bplustree.insert(db[i].student_id, i);
    }

    vector<string> targets;
    mt19937 gen(321);
    uniform_int_distribution<int> dis(0, data_size -1);

    for(int i=0; i<count; i++) targets.push_back(db[dis(gen)].student_id);

    start=high_resolution_clock::now();
    for(const string& target : targets) btree.search(target);
    end=high_resolution_clock::now();
    btree_time = end - start;

    start=high_resolution_clock::now();
    for(const string& target : targets) bstartree.search(target);
    end=high_resolution_clock::now();
    bstar_time = end - start;

    start=high_resolution_clock::now();
    for(const string& target : targets) bplustree.search(target);
    end=high_resolution_clock::now();
    bplus_time = end - start;

    cout<<"/////////////////////////"<<"Point Search with degree "<<degree<<"/////////////////////////"<<endl;
    cout<<"[b-tree] "<<"excution time: "<<btree_time.count()<<" mean excution time "<<btree_time.count()/count<<endl;
    cout<<"[b*-tree] "<<"excution time: "<<bstar_time.count()<<" mean excution time "<<bstar_time.count()/count<<endl;
    cout<<"[b+-tree] "<<"excution time: "<<bplus_time.count()<<" mean excution time "<<bplus_time.count()/count<<endl;

}

int cal_avg_weight(vector<int> vec, vector<StudentRecord> db){
    if(vec.size() == 0) return -1;
    int count =0;
    int sum= 0;
    for(int index : vec){
        if(db[index].gender=="Female"){
            count++;
            sum+=db[index].weight;
        }
    }
    return sum/count;
}

void test_query(int degree , int data_size, vector<StudentRecord> db){
    time_point<high_resolution_clock> start, end;
    duration<double, milli> btree_time, bstar_time, bplus_time;

    BTree btree(degree);
    BStarTree bstartree(degree);
    BPlusTree bplustree(degree); 

    for(int i=0; i<data_size; i++){
        btree.insert(db[i].student_id, i);
        bstartree.insert(db[i].student_id, i);
        bplustree.insert(db[i].student_id, i);
    }

    string start_key = "202083137";
    string end_key ="202150000";
    int br, bsr, bpr;

    vector<int> btrees, bstrees, bptrees;
    start=high_resolution_clock::now();
    btrees = btree.range_query(start_key, end_key);
    br = cal_avg_weight(btrees, db);
    end=high_resolution_clock::now();
    btree_time = end - start;

    start=high_resolution_clock::now();
    bstrees = bstartree.range_query(start_key, end_key);
    bsr = cal_avg_weight(bstrees, db);
    end=high_resolution_clock::now();
    bstar_time = end - start;

    start=high_resolution_clock::now();
    bptrees = bplustree.range_query(start_key, end_key);
    bpr = cal_avg_weight(bptrees, db);
    end=high_resolution_clock::now();
    bplus_time = end - start;

    if(br==bsr && bsr == bpr && br ==bpr){
        cout<<"/////////////////////////"<<"Range Query with degree "<<degree<<"/////////////////////////"<<endl;
        cout<<"[b-tree] "<<"excution time: "<<btree_time.count()<<endl;
        cout<<"[b*-tree] "<<"excution time: "<<bstar_time.count()<<endl;
        cout<<"[b+-tree] "<<"excution time: "<<bplus_time.count()<<endl;
        cout<<"/////////////////////////"<<"query_result"<<"/////////////////////////"<<endl;
        if(br!=-1){
            cout<<"average weight of female students whose IDs are 202083137 and 202150000 : "<<br<<endl;
        }
        else{
            cout<<"there are no students whose IDs are 202083137 and 202150000"<<endl;
        }
    }
    else{
        cout<<"error: trees are differ"<<endl;
    }
    
}

bool check_integrity_bt(BTreeNode* node , int d, bool root){
    if(node==nullptr) return true;
    if(node->num_keys < (d-1) / 2 && !root) return false;
    if(!node->is_leaf){
        for (int i=0; i<=node->num_keys; i++){
            if(!check_integrity_bt(node->children[i], d, false)) return false;
        }
    }
    return true;
}

bool check_integrity_bp(BPlusNode* node , int d, bool root){
    if(node==nullptr) return true;
    if(node->num_keys < (d-1) / 2 && !root) return false;
    if(!node->is_leaf){
        for (int i=0; i<=node->num_keys; i++){
            if(!check_integrity_bp(node->children[i], d, false)) return false;
        }
    }
    return true;
}

void test_randomdelete(int degree , int data_size, vector<StudentRecord> db, int count){
    if(count>data_size) {
        cout<<"can not delete"<<endl;
        return;
    }
    time_point<high_resolution_clock> start, end;
    duration<double, milli> btree_time, bstar_time, bplus_time;

    BTree btree(degree);
    BStarTree bstartree(degree);
    BPlusTree bplustree(degree); 

    for(int i=0; i<data_size; i++){
        btree.insert(db[i].student_id, i);
        bstartree.insert(db[i].student_id, i);
        bplustree.insert(db[i].student_id, i);
    }

    vector<string> targets;
    mt19937 gen(321);
    uniform_int_distribution<int> dis(0, data_size -1);

    for(int i=0; i<count; i++) targets.push_back(db[dis(gen)].student_id);

    start=high_resolution_clock::now();
    for(const string& target : targets) btree.remove(target);
    end=high_resolution_clock::now();
    btree_time = end - start;

    start=high_resolution_clock::now();
    for(const string& target : targets) bstartree.remove(target);
    end=high_resolution_clock::now();
    bstar_time = end - start;

    start=high_resolution_clock::now();
    for(const string& target : targets) bplustree.remove(target);
    end=high_resolution_clock::now();
    bplus_time = end - start;

    cout<<"/////////////////////////"<<"Deletion & Structural Integrity with degree "<<degree<<", delete_num "<<count<<"/////////////////////////"<<endl;
    cout<<"[b-tree] "<<"excution time: "<<btree_time.count()<<endl;
    cout<<"[b*-tree] "<<"excution time: "<<bstar_time.count()<<endl;
    cout<<"[b+-tree] "<<"excution time: "<<bplus_time.count()<<endl;

    if(check_integrity_bt(btree.getroot(), degree, true))  cout<<"[b-tree] "<<"maintain structural integrity"<<endl;
    else cout<<"[b-tree] "<<"not maintain structural integrity"<<endl;

    if(check_integrity_bt(bstartree.getroot(), degree, true))  cout<<"[b*-tree] "<<"maintain structural integrity"<<endl;
    else cout<<"[b*-tree] "<<"not maintain structural integrity"<<endl;

    if(check_integrity_bp(bplustree.getroot(), degree, true))  cout<<"[b+-tree] "<<"maintain structural integrity"<<endl;
    else cout<<"[b+-tree] "<<"not maintain structural integrity"<<endl;

}