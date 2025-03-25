#ifndef ARTHUR_INCLUDE_GREEK_BOOK_HPP
#define ARTHUR_INCLUDE_GREEK_BOOK_HPP
#pragma once

#include <boost/asio.hpp>

#include "Enums.hpp"
#include "Structure.hpp"

class Position {
  public:
    explicit Position(boost::asio::io_context& ioContext_);
    void Insert(const OrderInfoPtrT& tradeInfo_);
    void Paint(bool* show_);

  protected:
    void DrawBook(bool* show_);
    void SymbolBoolWiseBookUpdate(const OrderInfoPtrT& tradeInfo_);
    void PFWiseBookUpdate(const OrderInfoPtrT& tradeInfo_);
    void GreekBookUpdate(const OrderInfoPtrT& tradeInfo_);
    void DrawSymbolWiseNetBook();
    void DrawPFWiseNetBook();
    void DrawGreekNetBook();
    void UpdateGreekValue();
    void TimerEvent();
    auto CalculateSymbolWisePNL(const NetBookColumnPtrT& column_) -> double;

  private:
    PendingTradeUpdateT          _pendingTradeUpdate;
    SymbolWiseTradeContainerT    _symbolWiseTradeContainer;
    SymbolWiseTradeContainerVecT _symbolWiseTradeContainerVec;
    PFWiseTradeContainerT        _pFWiseTradeContainer;
    PFWiseTradeContainerVecT     _pFWiseTradeContainerVec;

    GreekBookContainerT _greekBookContainer;
    GreeksListT         _greekList;

    double           _netPNL = 0;
    ImGuiListClipper _clipper;

    NetBookCalculation          _calculation;
    boost::asio::deadline_timer _timer;
};

#endif// ARTHUR_INCLUDE_GREEK_BOOK_HPP
