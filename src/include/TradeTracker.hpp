//
// Created by VIKLOD on 30-04-2023.
//
#pragma once

#include "Structure.hpp"

class TradeTracker {
  public:
    void paint(bool* show_);
    void Insert(TradeTrackerItemT& tradeTrackerItem_);

  protected:
    void DrawTracker(bool* show_);

  private:
    TradeTrackerContainerT _trackerContainer;
    PendingTrackerUpdateT  _pendingTrackerUpdate;
    ImGuiListClipper       _clipper;
};
