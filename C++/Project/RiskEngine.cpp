#include "RiskEngine.h"


void RiskEngine::computeRisk(string riskType, std::shared_ptr<Trade> trade, bool singleThread) 
{
    result.clear();
    if (singleThread) {
        if(riskType == "dv01"){
            for (auto& kv: curveShocks) {
                    string market_id = kv.first;
                    auto mkt_u = kv.second.getMarketUp();
                    auto mkt_d = kv.second.getMarketDown();
                    
                    double pv_up = trade->Pv(mkt_u);
                    double pv_down = trade->Pv(mkt_d);
                    double dv01 = (pv_up - pv_down)/2.0;
                    result.emplace(market_id, dv01);
            }
        }

        if(riskType == "vega"){
            for (auto& kv: volShocks) {
                // to be added
            }

        }

        if (riskType =="price") {
            // to be added

        }
    }
    else {
        auto pv_task = [](shared_ptr<Trade> trade, string id, const Market& mkt_up, const Market& mkt_down) {
            double pv_up = trade->Pv(mkt_up);
            double pv_down = trade->Pv(mkt_down);
            double dv01 = (pv_up - pv_down)/2.0;
            return std::make_pair(id, dv01);
        };

        vector<std::future<std::pair<string, double>>> _futures;
        // calling the above function asynchronously and storing the result in future object
        for (auto& shock: curveShocks) {
            string market_id = shock.first;
            auto mkt_u = shock.second.getMarketUp();
            auto mkt_d = shock.second.getMarketDown();
            _futures.push_back(std::async(std::launch::async, pv_task, trade, market_id, mkt_u, mkt_u)); 
        }
        
        for (auto && fut: _futures) {
            auto rs = fut.get();
            result.emplace(rs);
        }     

    }
}