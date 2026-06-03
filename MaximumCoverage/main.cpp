#include "time.h"

#include "SmkStream.h"
#include "OneStream.h"
#include "MultiStream.h"
#include "DynamicMRT.h"
int main(int argc,char *argv[]) {
    read_data();

    time_t nowtime;
    struct tm *p;;
    time(&nowtime);
    p = localtime(&nowtime);
    string::size_type pos1, pos2, posend;
    pos1 = edge_text.find_last_of("/");
    pos2 = edge_text.rfind("/", pos1 - 1);
    posend = edge_text.find_last_not_of("/");
    string name1 = edge_text.substr(pos2 + 1, pos1 - pos2 - 1);
    string name2 = edge_text.substr(pos1 + 1, posend);
    string result_name = name1 + "_" + name2;
    //cout<<result_name<<endl;
    string outtext =
            "./result/result_" + result_name + "_" + to_string(p->tm_mon + 1) + "." + to_string(p->tm_mday) + "_" +
            to_string(p->tm_hour) + "_" + to_string(p->tm_min) + "_" + to_string(p->tm_sec) + ".txt";

    vector<Result> onestream_result;
    vector<Result> multistream_result;
    vector<Result> smkstream_result;
    vector<Result> DMRT_result;


    vector<int> ground_set;
    for(int i=0;i<node_num;i++)
        ground_set.push_back(i);

    double eps=0.1;

    group_upper.resize(group_num);
    group_lower.resize(group_num);


    vector<double> betas;
    double betas_start=0.002;
    double betas_end=0.02001;
    double betas_step=0.002;

    for (double b = betas_start; b <= betas_end; b += betas_step)
        betas.push_back(b);


    vector<double> budgets;
    for (double beta : betas) {
        budgets.push_back(beta * normalized_sum_cost);
    }
    for (size_t idx = 0; idx < budgets.size(); ++idx) {
        double B = budgets[idx];
        double beta = betas[idx];
        cout << "Budget ratio=" << beta << ", Budget=" << B << endl;

        onestream_result.emplace_back(OneStream(B,eps));
        multistream_result.emplace_back(MultiStream(B,eps));
        smkstream_result.emplace_back(SmkStream(B,eps));
        DMRT_result.emplace_back(DynamicMRT(B,eps));
    }

    ofstream out(outtext);
    if(B_wrong)
        out<<"Budget is wrong !"<<endl;
    out<<"eps: "<<eps<<endl;
    // out<<"max node: "<<endl;
    out << "Budget ratio:" << endl;
    for (double beta : betas) {
        out << beta << "\t";
    }
    out << endl;

    out<<"OneStream "<<endl;
    out<<"utility: "<<endl;
    for(auto p:onestream_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"query: "<<endl;
    for(auto p:onestream_result)
    {
        out<<p.oracle<<"\t";
    }
    out<<endl;
    out<<"memory: "<<endl;
    for(auto p:onestream_result)
    {
        out<<p.memory<<"\t";
    }
    out<<endl;

    out<<"MultiStream "<<endl;
    out<<"utility: "<<endl;
    for(auto p:multistream_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"query: "<<endl;
    for(auto p:multistream_result)
    {
        out<<p.oracle<<"\t";
    }
    out<<endl;
    out<<"memory: "<<endl;
    for(auto p:multistream_result)
    {
        out<<p.memory<<"\t";
    }
    out<<endl;


    out<<"DynamicMRT "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:DMRT_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"query: "<<endl;
    for(auto p:DMRT_result)
    {
        out<<p.oracle<<"\t";
    }
    out<<endl;
    out<<"memory: "<<endl;
    for(auto p:DMRT_result)
    {
        out<<p.memory<<"\t";
    }
    out<<endl;

    out<<"SmkStream "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:smkstream_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"query: "<<endl;
    for(auto p:smkstream_result)
    {
        out<<p.oracle<<"\t";
    }
    out<<endl;
    out<<"memory: "<<endl;
    for(auto p:smkstream_result)
    {
        out<<p.memory<<"\t";
    }
    out<<endl;

    return 0;
}
