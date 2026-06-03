
#ifndef REV_UTILITY_H
#define REV_UTILITY_H

#include <iostream>
#include <fstream>
#include <vector>
#include "list"
#include "time.h"
#include "random"
#include "algorithm"
#include <vector>
#include <cmath>
#include <unordered_set>
#include "list"
#include "random"
int node_num=0;
int edge_num=0;
using namespace std;

vector<double> all_e_value;
vector<double> node_cost;
vector<vector<pair<int,double>>> edge_weight;
double min_cost=999999999;
double max_cost=0.0;
double sum_cost=0.0;
int K;
string node_cost_text="./rndata/r3/renum_cumulate_normalize_node_cost.txt";
//string node_cost_text="renum_random_node_cost.txt";
string edge_weight_text="./rndata/r3/renum_random_edge_weight.txt";
const double ave_num=1.0/2.0;//normalize the cost to 1/ave_num in average
double f_u(int node);
class Node
{
public:
    Node(){};
    Node(int num, double c)
    {
        id=num;
        // group=g;
        // weight=w;
        cost=c;
    }
    int id;
    // int group;
    // double weight;
    double cost;
};
vector<Node> Groundset;
double normalized_sum_cost=0.0;
void read_data()
{
    ifstream in1(node_cost_text);
    int temp_node;
    double temp_cost;
    node_num=0;
    //cout<<"plus one for every temp_cost to ensure they satisfy the assumption of baselines !"<<endl;
    while(!in1.eof())
    {
        in1>>temp_node>>temp_cost;
        if (in1.fail())
            break;
        node_num++;


        //temp_cost+=1.0;

        // temp_cost=2.0-exp(-0.2*temp_cost);

        Groundset.emplace_back(temp_node,temp_cost);

        if(temp_cost<min_cost)
            min_cost=temp_cost;
        if(temp_cost>max_cost)
            max_cost=temp_cost;

        node_cost.push_back(temp_cost);
        sum_cost+=temp_cost;
    }

    // cout << "before normalizing" << endl;
    // cout<<"sum cost: "<<sum_cost<<endl;
    // cout<<"max cost: "<<max_cost<<endl;
    // cout<<"min cost: "<<min_cost<<endl;



    for(int i=0;i<node_num;i++)
        edge_weight.push_back(vector<pair<int,double>>());
    ifstream in2(edge_weight_text);
    int v1,v2;
    double weight;
    edge_num=0;
    while(!in2.eof())
    {
        in2>>v1>>v2>>weight;
        if (in2.fail())
            break;
        edge_num++;
        edge_weight[v1].push_back(pair<int,double>(v2,weight));
        edge_weight[v2].push_back(pair<int,double>(v1,weight));
    }
    // cout<<"node size: "<<node_num<<" map size: "<<node_cost.size()<<endl;
    // cout<<"edge size: "<<edge_num<<endl;



    // for(int i=0;i<node_num;i++)
    //     all_e_value.push_back(f_u(i));

    // double normalize_min_cost=999999999.0;
    // double normalize_max_cost=-1.0;

    // for(auto & i : Groundset) {
    //
    //     double normalize_cost=(node_num*i.cost)/(sum_cost*ave_num);
    //     i.cost=normalize_cost;
    //
    //     if(i.cost<normalize_min_cost)
    //         normalize_min_cost=i.cost;
    //     if(i.cost>normalize_max_cost)
    //         normalize_max_cost=i.cost;
    // }


    double normalize_min_cost=999999999.0;
    double normalize_max_cost=-1.0;

    for(auto & i : Groundset) {
        double normalize_cost=i.cost/min_cost;
        i.cost=normalize_cost;

        normalized_sum_cost += i.cost;

        if(i.cost<normalize_min_cost)
            normalize_min_cost=i.cost;
        if(i.cost>normalize_max_cost)
            normalize_max_cost=i.cost;
    }

}
double f_u(int node)
{
    double sum_value=0.0;
    for(int i=0;i<node_num;i++)
    {
        if(i!=node)
        {
            double value=0.0;
            for(int j=0;j<edge_weight[i].size();j++)
            {
                if(edge_weight[i][j].first==node)
                {
                    value=edge_weight[i][j].second;
                    break;
                }
            }
            sum_value+=sqrt(value);
        }
    }
    return sum_value;
}
double f_S(const unordered_set<int> &solution) {
    double sum_value = 0.0;
    for (int i = 0; i < node_num; i++) {
        double value = 0.0;
        if(solution.count(i))
            continue;
        for (const auto &it:edge_weight[i]) {
            if(solution.count(it.first)) {
                value += it.second;
            }
        }
        sum_value += sqrt(value);
    }
    return sum_value;
}


bool budget_feasible_single(const int &node_id,const double &budget)
{
    if(Groundset[node_id].cost<=budget)
        return true;
    else
        return false;
}
class S_class {
public:
    S_class() {
        s_cost = 0.0;
        s_revenue = 0.0;
        //selected.resize(node_num, 0);

    }
    //value
    double s_cost;
    double s_revenue;
    //set
    unordered_set<int> solution;
    // vector<int> selected;
    void clear()
    {
        s_cost=0.0;
        s_revenue = 0.0;
        // for(int i=0;i<group_num;i++)
        //     group_exist[i]=0;
        solution.clear();
    }
    void replace_with_singleton(const double &marginal,const int &node)
    {
        clear();
        add_element(marginal,node);
    }
    void replace_with_singleton(const int &node)
    {
        clear();
        add_element(f_u(node),node);
    }
    void add_element(const double &marginal,const int &node_id)
    {
        s_revenue+=marginal;
        s_cost+=Groundset[node_id].cost;
        solution.emplace(node_id);
    }
    void add_element_without_cal_revenue(const int &node_id)
    {
        s_cost+=Groundset[node_id].cost;
        solution.emplace(node_id);
    }
    void add_set_element_without_cal_revenue(const vector<int> &set)
    {
        for(auto &v:set)
            add_element_without_cal_revenue(v);
    }
    void add_set_element_without_cal_revenue(const unordered_set<int> &set)
    {
        for(auto &v:set)
            add_element_without_cal_revenue(v);
    }
    bool budget_feasible(const int &node_id,const double &budget)
    {
        if(s_cost + Groundset[node_id].cost <= budget)
            return true;
        else
            return false;
    }
    double f_S() {
        double sum_value = 0.0;
        for (int i = 0; i < node_num; i++) {
            double value = 0.0;
            if(solution.count(i))
                continue;
            for (const auto &it:edge_weight[i]) {
                if(solution.count(it.first)) {
                    value += it.second;
                }
            }
            sum_value += sqrt(value);
        }
        return sum_value;
    }
    double marginal_gain(const int &node) {
        if(solution.count(node))
            return 0.0;

        double sum_value = 0.0;
        for (int i = 0; i < node_num; i++) {
            double value = 0.0;
            if(solution.count(i)||i==node)
                continue;
            for (const auto &it:edge_weight[i]) {
                if(solution.count(it.first)||it.first==node) {
                    value += it.second;
                }
            }
            sum_value += sqrt(value);
        }

        return sum_value - s_revenue;
    }
};
class Result
{
public:
    Result(){}
    Result(double rev,double cos,int siz,long long int ora)
    {
        revenue=rev;
        cost=cos;
        size=siz;
        oracle=ora;
    }
    Result(double rev,double cos,int siz,long long int ora,long long int mem)
    {
        revenue=rev;
        cost=cos;
        size=siz;
        oracle=ora;
        memory=mem;
    }
    Result(double rev,double cos,int siz,long long int ora,int vio)
    {
        revenue=rev;
        cost=cos;
        size=siz;
        oracle=ora;
        violation_num=vio;
    }
    double revenue;
    long long int oracle;
    long long int round;
    double cost;
    int size;
    long long int time=0;
    long long int max_query=0;
    long long int memory=0;
    int violation_num;

    Result(double rev, long long int orc, long long int mem = 0)
    : revenue(rev), oracle(orc), memory(mem) {}

    Result operator+(const Result& other) const {
        return Result(revenue + other.revenue,
                      oracle + other.oracle,
                      memory + other.memory);
    }

    Result operator/(double divisor) const {
        return Result(revenue / divisor,
                      oracle / divisor,
                      memory / divisor);
    }

    void print() const {
        std::cout << "Revenue: " << revenue
                  << ", Oracle: " << oracle
                  << ", Memory: " << memory << std::endl;
    }

};
#endif 
