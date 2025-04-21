#pragma once

#include "include/Structure.hpp"

#include <cstddef>

class CentralFeed {
  public:
    explicit CentralFeed(MarketEventQueueT& queue_);

    void Process(const char* buffer_, size_t size_);

  private:
    MarketEventQueueT& _marketEventQueue;
};
