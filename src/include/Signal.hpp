#pragma once

#include <boost/signals2.hpp>
struct OrderInfoT;
using OrderInfoPtrT = std::shared_ptr<OrderInfoT>;

using AddContractToMarketWatchSignal = boost::signals2::signal<void(const std::string &)>;
using AddContractToMarketWatchSlot	 = AddContractToMarketWatchSignal::slot_type;

using OptionChainContractSignal = AddContractToMarketWatchSignal;
using OptionChainContractSlot	= OptionChainContractSignal::slot_type;

using AddContractToDemoSignalT = boost::signals2::signal<void(int)>;
using AddContractToDemoSlot	   = AddContractToDemoSignalT::slot_type;

struct OrderInfoT;
using DemoOrderInfoSignalT = boost::signals2::signal<void(OrderInfoPtrT)>;