#pragma once

#include <cstddef>

class CentralFeed {
  public:
    static void Process(const char* buffer_, size_t size_);
};
