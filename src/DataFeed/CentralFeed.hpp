#pragma once

#include "EpollSocket.hpp"
#include "Structure.hpp"
#include "include/Structure.hpp"

class CentralFeed : public EpollSocket {
  public:
    explicit CentralFeed(MarketEventQueueT& queue_);

    ~CentralFeed() override = default;

    void Process(int size_) override;

  private:
    MarketEventQueueT&             _marketEventQueue;
    struct MarketWatchDataUpdateT* _marketData;
    struct IndexDataUpdate*        _index;
    Lancelot::Header*              _header;
};
