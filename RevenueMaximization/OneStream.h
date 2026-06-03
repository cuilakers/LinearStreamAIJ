//
// Created by HP on 24-9-20.
//

#ifndef ONESTREAM_H
#define ONESTREAM_H
#include "utilityfunction.h"
#include <ranges>
class Si {
    public:
    double sum_cost;
    vector<int> set;
    Si() {
        sum_cost = 0;
    };
    void add_element(const int& node) {
        set.emplace_back(node);
        sum_cost+=Groundset[node].cost;
    }
};
double theta=1.0/1.0;
default_random_engine engine(2);
Result OneStream(double B,double eps) {
    bernoulli_distribution bernou(0.5);

    int h=2;

    cout<<"OneStream ---------start--------- "<<endl;
    cout<<"B = "<<B<<endl;

    long long int query=0;
    long long int memory=0;
    int e_star=-1;
    double e_star_value=-999.0;

    // cout<<"h = "<<h<<endl;
    vector<Si> S_array;
    S_array.emplace_back();
    S_array.emplace_back();
    int i=1;
    int j=1;
    S_class T;
    for(int e=0;e<node_num;e++) {
        if (!bernou(engine))
            continue;

        if(!budget_feasible_single(e,B))
            continue;

        //get e_star
        // double fe_value=f_u(e);
        // query++;
        // if(fe_value>e_star_value) {
        //     e_star_value=fe_value;
        //     e_star=e;
        // }


        //check marginal gain
        double marginal=T.marginal_gain(e);
        query++;

      //  double delta=10.0;
        if(marginal/Groundset[e].cost>=theta*T.s_revenue/B) {
            S_array[i].add_element(e);

            T.add_element(marginal,e);

            if(S_array[i].sum_cost>=B) {

                memory=max(memory,(long long int)T.solution.size());

                //delete sets
                if(i-j+1>=2*h) {
                    j=j+h;
                    //re-construct T
                    T.clear();
                    for(int it=j;it<=i;it++) {
                        T.add_set_element_without_cal_revenue(S_array[it].set);
                    }
                    T.s_revenue=T.f_S();
                }
                i++;
                S_array.emplace_back();
            }

        }
    }

    // cout<<"T cost: "<<T.s_cost<<endl;
    // cout<<"T node"<<endl;
    // for(const auto &p:T.solution)
    //     cout<<p<<" ";
    // cout<<endl;
    memory=max(memory,(long long int)T.solution.size());

    S_class Q;
    if(T.s_cost<=B) {
        // cout<<"Budget is not full !"<<endl;
        Q=T;
    }
    else {
        //we must access S_array to use vector since T store element using unorder_set
        // bool quit=false;
        // cout<<"Budget is full !"<<endl;
        Q.add_set_element_without_cal_revenue(S_array[i].set);
        Q.s_revenue=Q.f_S();
        query++;

        for(int it=i-1;it>=j;it--) {
            if (S_array[it].sum_cost<=B) {
                unordered_set<int> temp_solution(S_array[it].set.begin(),S_array[it].set.end());
                double temp_value=f_S(temp_solution);
                if (temp_value>Q.s_revenue) {
                    Q.clear();
                    Q.add_set_element_without_cal_revenue(temp_solution);
                    Q.s_revenue=Q.f_S();
                }
            }
            else {
                unordered_set<int> temp_solution(S_array[it].set.begin(),S_array[it].set.end()-1);
                double temp_value=f_S(temp_solution);
                if (temp_value>Q.s_revenue) {
                    Q.clear();
                    Q.add_set_element_without_cal_revenue(temp_solution);
                    Q.s_revenue=Q.f_S();
                }

                if (f_u(S_array[it].set.back())>Q.s_revenue) {
                    Q.replace_with_singleton(S_array[it].set.back());
                }
            }

        }
    }
    if(e_star_value>Q.s_revenue)
        Q.replace_with_singleton(e_star);


    // cout<<"S*:"<<endl;
    // cout << "  revenue: " << Q.s_revenue << " cost: " <<Q.s_cost << " size: " << Q.solution.size() << endl;
    // cout<<" node"<<endl;
    // for(const auto &p:Q.solution)
    //     cout<<p<<" ";
    // cout<<endl;
    cout<<"Utility: "<<Q.f_S()<<endl;
    cout<<"Query: "<<query<<endl;
    cout<<"Memory: "<<memory<<endl;

    cout<<"OneStream ---------end--------- "<<endl<<endl;
    return Result(Q.s_revenue, Q.s_cost, Q.solution.size(), query,memory);
}
S_class OneStreamForMulti(double B,double eps,int h, long long int &query,long long int &memory) {
    bernoulli_distribution bernou(0.5);

   // int h=log2(1.0/8.0/eps)+2;
    // cout<<"OneStreamForMultiOffline ---------start--------- "<<endl;
    // cout<<"B = "<<B<<endl;

    int e_star=-1;
    // double e_star_value=-999.0;

    // cout<<"h = "<<h<<endl;
    vector<Si> S_array;
    S_array.emplace_back();
    S_array.emplace_back();
    int i=1;
    int j=1;
    S_class T;
    for(int e=0;e<node_num;e++) {
        if (!bernou(engine))
            continue;

        //get e_star
        // double fe_value=f_u(e);
        // query++;
        // if(fe_value>e_star_value) {
        //     e_star_value=fe_value;
        //     e_star=e;
        // }


        //check marginal gain
        double marginal=T.marginal_gain(e);
        query++;

        if(marginal/Groundset[e].cost>=theta*T.s_revenue/B) {
            S_array[i].add_element(e);

            T.add_element(marginal,e);

            if(S_array[i].sum_cost>=B) {
                memory=max(memory,(long long int)T.solution.size());

                //delete sets
                if(i-j+1>=2*h) {
                    j=j+h;
                    //re-construct T
                    T.clear();
                    for(int it=j;it<=i;it++) {
                        T.add_set_element_without_cal_revenue(S_array[it].set);
                    }
                    T.s_revenue=T.f_S();
                }
                i++;
                S_array.emplace_back();
            }

        }
    }
    T.s_revenue=T.f_S();

    memory=max(memory,(long long int)T.solution.size());
    // cout<<"T revenue: "<<T.s_revenue<<endl;
    // cout<<"T cost: "<<T.s_cost<<endl;
    // cout<<"T node"<<endl;
    // for(const auto &p:T.solution)
    //     cout<<p<<" ";
    // cout<<endl;


    S_class Q;
    if(T.s_cost<=B) {
        // cout<<"Budget is not full !"<<endl;
        Q=T;
    }
    else {
        //we must access S_array to use vector since T store element using unorder_set
        // bool quit=false;
        //cout<<"Budget is full !"<<endl;
        Q.add_set_element_without_cal_revenue(S_array[i].set);
        Q.s_revenue=Q.f_S();
        query++;

        for(int it=i-1;it>=j;it--) {
            if (S_array[it].sum_cost<=B) {
                unordered_set<int> temp_solution(S_array[it].set.begin(),S_array[it].set.end());
                double temp_value=f_S(temp_solution);
                if (temp_value>Q.s_revenue) {
                    Q.clear();
                    Q.add_set_element_without_cal_revenue(temp_solution);
                    Q.s_revenue=Q.f_S();
                }
            }
            else {
                unordered_set<int> temp_solution(S_array[it].set.begin(),S_array[it].set.end()-1);
                double temp_value=f_S(temp_solution);
                if (temp_value>Q.s_revenue) {
                    Q.clear();
                    Q.add_set_element_without_cal_revenue(temp_solution);
                    Q.s_revenue=Q.f_S();
                }

                if (f_u(S_array[it].set.back())>Q.s_revenue && budget_feasible_single(S_array[it].set.back(),B) ) {
                    Q.replace_with_singleton(S_array[it].set.back());
                }
            }

        }
    }

    // if(e_star_value>Q.s_revenue)
    //     Q.replace_with_singleton(e_star);

    // cout<<"S*:"<<endl;
    // cout << "  revenue: " << Q.s_revenue << " cost: " <<Q.s_cost << " size: " << Q.solution.size() << endl;
    // cout<<" node"<<endl;
    // for(const auto &p:Q.solution)
    //     cout<<p<<" ";
    // cout<<endl;
    // cout<<"Real Revenue: "<<Q.f_S()<<endl;
    // cout<<"Query: "<<query<<endl;
    //
    // cout<<"OneStreamForMultiOffline ---------end--------- "<<endl<<endl;

    return Q;
}
#endif //ONESTREAM_H
