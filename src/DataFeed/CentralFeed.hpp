#pragma once

#include "Structure.hpp"

class CentralFeed {
  public:
    explicit CentralFeed(MarketEventQueueT& queue_);

    void Process(int size_);

  protected:
    char _buffer[512];

  private:
    MarketEventQueueT&             _marketEventQueue;
    struct MarketWatchDataUpdateT* _marketData;
    struct IndexDataUpdate*        _index;
    Lancelot::Header*              _header;
};
