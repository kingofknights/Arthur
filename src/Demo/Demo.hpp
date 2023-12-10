//
// Created by VIKLOD on 06-05-2023.
//

#pragma once
#include <random>
#include <stop_token>

#include "../include/Structure.hpp"
enum OrderStatus : int;

class Demo {
  public:
    Demo();

    void Run(std::stop_token& token_);

    void startAndStop() { _startOrStop = not _startOrStop; }

  protected:
    void          GenerateMonteCarloData(const MarketWatchDataPtrT& ptr_);
    void          addContract(int token_);
    double        monteCarloEstimate(double lowBound, double upBound, int iterations);
    OrderInfoPtrT GenerateOrderInfo(const MarketWatchDataPtrT& ptr_, int gateway_, Lancelot::Side side_, OrderStatus orderStatus_);

  private:
    bool                          _startOrStop = false;
    std::set<MarketWatchDataPtrT> _liveContainer;
};
