#include <stdlib.h>
#include "time.h"
#include "utilityfunction.h"
#include "OneStream.h"
#include "SmkStream.h"
#include "MultiStream.h"
#include "KnapStream.h"

#include <cmath>
#include <functional>

struct ResultStats {
    Result mean;
    Result stddev;

    ResultStats(Result mean_, Result stddev_)
        : mean(mean_), stddev(stddev_) {}
};

ResultStats compute_stats(const vector<Result>& results) {
    int repeat_times = results.size();

    Result total(0, 0, 0);
    for (const auto& result : results) {
        total = total + result;
    }

    Result mean = total / repeat_times;

    double var_revenue = 0.0;
    double var_oracle = 0.0;
    double var_memory = 0.0;

    if (repeat_times > 1) {
        for (const auto& result : results) {
            var_revenue += (result.revenue - mean.revenue) * (result.revenue - mean.revenue);
            var_oracle += (result.oracle - mean.oracle) * (result.oracle - mean.oracle);
            var_memory += (result.memory - mean.memory) * (result.memory - mean.memory);
        }

        var_revenue /= (repeat_times - 1);
        var_oracle /= (repeat_times - 1);
        var_memory /= (repeat_times - 1);
    }

    Result stddev(
        sqrt(var_revenue),
        sqrt(var_oracle),
        sqrt(var_memory)
    );

    return ResultStats(mean, stddev);
}

int main(int argc,char *argv[]) {
    //random_weight();
    //random_cost();
    //renum();
    //cumulate_normalize_cost(0.2);

    time_t nowtime;
    struct tm* p;;
    time(&nowtime);
    p = localtime(&nowtime);

    read_data();
    string::size_type pos1, pos2,posend;
    pos1=node_cost_text.find_last_of("/");
    pos2=node_cost_text.rfind("/",pos1-1);

    posend=node_cost_text.find_last_not_of("/");
    string name1=node_cost_text.substr(pos2+1,pos1-pos2-1);
    string name2=node_cost_text.substr(pos1+1,posend);
    string result_name=name1+"_"+name2;
    //cout<<result_name<<endl;
    string outtext="./result/result_"+result_name+"_"+to_string(p->tm_mon+1)+"."+to_string(p->tm_mday)+"_"+to_string(p->tm_hour)+"_"+to_string(p->tm_min)+"_"+to_string(p->tm_sec)+".txt";


//    double B=atof(argv[1]);
 //   double B_ratio=atof(argv[1]);
    //double eps=atof(argv[2]);
    double eps=0.1;
    cout<<"eps: "<<eps<<endl;

    vector<Result> onestream_result;
    vector<Result> multistream_result;
    vector<Result> smkstream_result;
    vector<Result> knapstream_result;

    vector<Result> onestream_std;
    vector<Result> multistream_std;

    vector<double> betas;
    double betas_start=0.02;
    double betas_end=0.20001;
    double betas_step=0.02;

    for (double b = betas_start; b <= betas_end; b += betas_step)
        betas.push_back(b);

    vector<double> budgets;
    for (double beta : betas) {
        budgets.push_back(beta * normalized_sum_cost);
    }

    int repeat_times = 10;
    for (size_t idx = 0; idx < budgets.size(); ++idx) {
        double B = budgets[idx];
        double beta = betas[idx];
        cout << "Budget ratio=" << beta << ", Budget=" << B << endl;

        vector<Result> one_repeat_results;
        one_repeat_results.reserve(repeat_times);

        for (int i = 0; i < repeat_times; ++i) {
            Result current_result = OneStream(B, eps);
            one_repeat_results.push_back(current_result);
        }

        ResultStats one_stats = compute_stats(one_repeat_results);
        onestream_result.push_back(one_stats.mean);
        onestream_std.push_back(one_stats.stddev);


        vector<Result> multi_repeat_results;
        multi_repeat_results.reserve(repeat_times);

        for (int i = 0; i < repeat_times; ++i) {
            Result current_result = MultiStream(B, eps);
            multi_repeat_results.push_back(current_result);
        }

        ResultStats multi_stats = compute_stats(multi_repeat_results);
        multistream_result.push_back(multi_stats.mean);
        multistream_std.push_back(multi_stats.stddev);


        // Deterministic baselines: run once, no error bars needed.
        smkstream_result.emplace_back(SmkStream(B, eps));
        knapstream_result.emplace_back(KnapsackStreaming(B, eps));
    }


    ofstream out(outtext);
    out<<"eps: "<<eps<<endl;
    out << "Budget ratio:" << endl;
    for (double beta : betas) {
        out << beta << "\t";
    }
    out << endl;


     out << "OneStream" << endl;

    out << "utility_mean:" << endl;
    for (auto p : onestream_result) {
        out << p.revenue << "\t";
    }
    out << endl;

    out << "utility_std:" << endl;
    for (auto p : onestream_std) {
        out << p.revenue << "\t";
    }
    out << endl;

    out << "query_mean:" << endl;
    for (auto p : onestream_result) {
        out << p.oracle << "\t";
    }
    out << endl;

    out << "query_std:" << endl;
    for (auto p : onestream_std) {
        out << p.oracle << "\t";
    }
    out << endl;

    out << "memory_mean:" << endl;
    for (auto p : onestream_result) {
        out << p.memory << "\t";
    }
    out << endl;

    out << "memory_std:" << endl;
    for (auto p : onestream_std) {
        out << p.memory << "\t";
    }
    out << endl;


    out << "MultiStream" << endl;

    out << "utility_mean:" << endl;
    for (auto p : multistream_result) {
        out << p.revenue << "\t";
    }
    out << endl;

    out << "utility_std:" << endl;
    for (auto p : multistream_std) {
        out << p.revenue << "\t";
    }
    out << endl;

    out << "query_mean:" << endl;
    for (auto p : multistream_result) {
        out << p.oracle << "\t";
    }
    out << endl;

    out << "query_std:" << endl;
    for (auto p : multistream_std) {
        out << p.oracle << "\t";
    }
    out << endl;

    out << "memory_mean:" << endl;
    for (auto p : multistream_result) {
        out << p.memory << "\t";
    }
    out << endl;

    out << "memory_std:" << endl;
    for (auto p : multistream_std) {
        out << p.memory << "\t";
    }
    out << endl;


    out << "SmkStream" << endl;

    out << "utility:" << endl;
    for (auto p : smkstream_result) {
        out << p.revenue << "\t";
    }
    out << endl;

    out << "query:" << endl;
    for (auto p : smkstream_result) {
        out << p.oracle << "\t";
    }
    out << endl;

    out << "memory:" << endl;
    for (auto p : smkstream_result) {
        out << p.memory << "\t";
    }
    out << endl;


    out << "KnapsackStream" << endl;

    out << "utility:" << endl;
    for (auto p : knapstream_result) {
        out << p.revenue << "\t";
    }
    out << endl;

    out << "query:" << endl;
    for (auto p : knapstream_result) {
        out << p.oracle << "\t";
    }
    out << endl;

    out << "memory:" << endl;
    for (auto p : knapstream_result) {
        out << p.memory << "\t";
    }
    out << endl;
    return 0;
}
