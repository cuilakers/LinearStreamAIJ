//
// Created by Administrator on 25-5-10.
//

#ifndef KNAPSTREAM_H
#define KNAPSTREAM_H
#include "utilityfunction.h"
#include "MultiStream.h"
#include "Offline.h"
class S_array
{
public:
    vector<S_class> sarray;
    double gamma;
    //S_array(){}
    S_array(const double g,const int h)
    {
        gamma=g;
        for(int i=0;i<h;i++)
            sarray.emplace_back();
    }
};
Result KnapsackStreaming(double B,double eps)
{
    int PassNum=1;
    cout<<"KnapsackStream ---------start--------- "<<endl;
    cout<<"B = "<<B<<endl;
    // if(PassNum==1)
    //     cout<<"OnePass !"<<endl;
    // else
    //     cout<<"TwoPass !"<<endl;

    long long int oracle_times=0;
    long long int memory=0;

    double alpha;
    int d=1;
    if(d==1)
        alpha=4.0+eps;
    if(d>1)
        alpha=4.0*(d+1.0)+eps;

    int h;
    double lambda;
    if(PassNum==1) {
        double numerator_h = (10 + 21*d) * (1 + 2*d + alpha);
        double denominator_h = (1 + alpha) * d;
        h = ceil(numerator_h / denominator_h);

        lambda = (1 + 2*d + alpha) / 2.0;
    }
    if(PassNum>1) {
        double h_numerator = 41 * (2 + alpha);
        double h_denominator = 1 + alpha;
        h = ceil(h_numerator / h_denominator);

        lambda = 2 + alpha;
    }
    eps=0.05*d/lambda;
    //eps=1.0;


    vector<S_array> Gamma;

    S_class S_best;

    double max_gamma=0.0;//max gamma in Gamma, help to update Gamma
    int min_gamma_index_in_C=-1;//-1 represent Gamma is empty, else represent min gamma index which is still available now
    int max_gamma_index_in_C=-1;

    //for(int u=node_num-1;u>=0;u--)
    for(int u=0;u<node_num;u++)
    {
        if(!budget_feasible_single(u,B))
            continue;

        //generate Gamma
        double fu_value=f_u(u);

        oracle_times++;

        if(fu_value>S_best.s_revenue)
        {
            S_best.replace_with_singleton(fu_value,u);
        }
        if(fu_value<=0) continue;
        double left_temp=S_best.s_revenue/(lambda*B*(1.0+eps));


        // double u_cost_sum=0.0;
        // for(int r=0;r<d;r++)
        //     u_cost_sum+=node_cost[r][u];

        double right_temp=fu_value/Groundset[u].cost*(1.0+eps);
        if(left_temp>=right_temp) continue;

        double left=ceil(log(left_temp)/log(1.0+eps));
        double right=floor(log(right_temp)/log(1.0+eps));

        // left=max(left,1.0);
        // right=max(right,1.0);

        if(min_gamma_index_in_C==-1)//first visit S_array
        {
            for(int t=left;t<=right;t++)
            {
                double gamma_temp=pow(1.0+eps,t);
                Gamma.push_back(S_array(gamma_temp,h));
            }
            min_gamma_index_in_C=0;//the index of min gamma which is still available now
            max_gamma=pow(1.0+eps,right);//now max gamma
            max_gamma_index_in_C=Gamma.size()-1;//max gamma index in C now
        }
        else//not first visit
        {
            double now_min_gamma_in_C=pow(1.0+eps,left);//min gamma in C now
            double now_max_gamma_in_C=pow(1.0+eps,right);//max gamma in C now
            if(now_min_gamma_in_C>max_gamma)//all old S_array should be removed
            {
                min_gamma_index_in_C=Gamma.size();//the index of min gamma which is still available now
                for(int t=left;t<=right;t++)//then add new S gamma pair
                {
                    double gamma_temp=pow(1.0+eps,t);
                    Gamma.push_back(S_array(gamma_temp,h));
                }
                max_gamma_index_in_C=Gamma.size()-1;//max gamma index in C now
                max_gamma = Gamma[Gamma.size() - 1].gamma;//the last element always is the max_gamma anyway
            }
            else//else find where is the min gamma index now, which is equivalent to remove all S whose gamma < left
            {
                bool need_update= true;//judge S_array should be updated or not
                for (int iter = min_gamma_index_in_C; iter < Gamma.size(); iter++)
                {
                    if (Gamma[iter].gamma < now_min_gamma_in_C)
                        min_gamma_index_in_C++;
                    if (Gamma[iter].gamma >= now_max_gamma_in_C)
                    {
                        max_gamma_index_in_C=iter;
                        need_update=false;
                        break;
                    }
                }
                if(need_update)
                {
                    //finally, go through all gamma now, put new pair (S,gamma) if needed
                    for (int t = left; t <= right; t++)
                    {
                        double gamma_temp = pow(1.0 + eps, t);
                        if (gamma_temp > max_gamma)
                            Gamma.push_back(S_array(gamma_temp,h));
                    }
                    max_gamma = Gamma[Gamma.size() - 1].gamma;//the last element always is the max_gamma anyway
                    max_gamma_index_in_C=Gamma.size() - 1;
                }
            }
        }
        //foreach gamma\in Gamma
        for(int iter=min_gamma_index_in_C;iter<=max_gamma_index_in_C;iter++)
        {
            int max_solution=-1;
            double max_marginal=-999999.0;
            for(int y=0;y<h;y++)
            {
                oracle_times++;
                if(Gamma[iter].sarray[y].budget_feasible(u,B))
                {
                    double temp_marginal=Gamma[iter].sarray[y].marginal_gain(u);
                    if(temp_marginal>max_marginal)
                    {
                        max_marginal=temp_marginal;
                        max_solution=y;
                    }
                }
            }
            if(max_solution!=-1)
            {
                if( ( max_marginal / Groundset[u].cost ) >= Gamma[iter].gamma)
                {
                    Gamma[iter].sarray[max_solution].add_element(max_marginal,u);
                    if(Gamma[iter].sarray[max_solution].s_revenue>S_best.s_revenue)
                    {
                        S_best=Gamma[iter].sarray[max_solution];
                    }
                }
            }

        }
    }
//*

    // cout<<"final gamma size: "<<Gamma.size()-min_gamma_index_in_C<<endl;
    double now_left=(S_best.s_revenue/(lambda*B*(1.0+eps)) );
    //*
    //post processing -1
    for (int iter = min_gamma_index_in_C; iter < Gamma.size(); iter++)
    {
        if (Gamma[iter].gamma < now_left)
            continue;
        //foreach S^gamma_h to construct ground-set for OfflineAlgorithm
        vector<int> ground_set;
        for (auto &S_h:Gamma[iter].sarray)
        {
            for(auto e:S_h.solution)
                ground_set.push_back(e);

            memory+=S_h.solution.size();
        }
        S_class S_0;
//        if(d==1) S_0=smkrandom_acc2(data,eps,B,oracle_times,ground_set);
//        if(d>1)

        if (ground_set.size()<=0)
            continue;

        S_0= Offline(B,eps,ground_set,oracle_times);
        if(S_0.s_revenue>S_best.s_revenue) {
            S_best=S_0;
        }
        //To get better solution in practice
        //Gamma[iter].sarray.push_back(S_0);
    }
//*/
    //*
    //post processing -2
    //second pass
    for(int u=0;u<node_num;u++)
    {
        if(d!=1) break;//only work for d=1

        //foreach S^gamma
        for (int iter = min_gamma_index_in_C; iter < Gamma.size(); iter++)
        {
            if (Gamma[iter].gamma < now_left)
                continue;
            //foreach S^gamma_h
            for (auto &S_h:Gamma[iter].sarray)
            {
                if (!S_h.budget_feasible(u,B))
                    continue;

                //judge if u\in S^gamma_h
                if(S_h.solution.count(u))
                    continue;

                oracle_times++;
                double marginal=S_h.marginal_gain(u);
                double f_S_g_and_e=marginal+S_h.s_revenue;
                if(f_S_g_and_e>S_best.s_revenue)
                {
                    S_best=S_h;
                    S_best.add_element(marginal,u);
                }

            }
        }
    }
//*/
    if (memory<=1) memory=1;

    // cout<<"KnapsackStream & Budget: "<<B<<endl;
    // cout<<"S*:"<<endl;
    // cout<<"  revenue: "<<S_best.s_revenue<<" cost: "<<S_best.s_cost<<" size: "<<S_best.solution.size()<<endl;
    // for(auto &i:S_best.solution)
    //     cout<<i<<" ";
    // cout<<endl;
    cout<<"Utility: "<<S_best.f_S()<<endl;
    cout<<"Query: "<<oracle_times<<endl;
    cout<<"Memory: "<<memory<<endl;

    cout<<"KnapsackStream ---------end--------- "<<endl<<endl;

    return Result(S_best.s_revenue, S_best.s_cost, S_best.solution.size(), oracle_times,memory);
}
#endif //KNAPSTREAM_H
