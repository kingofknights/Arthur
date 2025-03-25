#pragma once

#include <boost/signals2.hpp>
struct OrderInfoT;
using OrderInfoPtrT = std::shared_ptr<OrderInfoT>;

using AddContractToMarketWatchSignalT = boost::signals2::signal<void(const std::string&)>;
using AddContractToMarketWatchSlotT   = AddContractToMarketWatchSignalT::slot_type;

using OptionChainContractSignalT = AddContractToMarketWatchSignalT;
using OptionChainContractSlotT   = OptionChainContractSignalT::slot_type;

using AddContractToDemoSignalT = boost::signals2::signal<void(int)>;
using AddContractToDemoSlotT   = AddContractToDemoSignalT::slot_type;

struct OrderInfoT;
using DemoOrderInfoSignalT = boost::signals2::signal<void(OrderInfoPtrT)>;
