//
// Created by VIKLOD on 25-02-2023.
//

#include "../API/BaseScanner.hpp"
#include "../include/Signal.hpp"
#include "../include/Structure.hpp"

bool        BackendConnected;
int         Id;
int         UserID;
std::string StatusDisplay;

StrategyNameListT                StrategyNameList;
ScannerContainerT                ScannerContainer;
MarketWatchDatContainerT         MarketWatchDatContainer;
AllContractT                     AllContract;
ClientCodeListT                  ClientCodeList;
MarketEventQueueT                MarketEventQueue;
GlobalPortfolioScannerContainerT GlobalPortfolioScannerContainer;
SpotInfoT                        BankNifty;
SpotInfoT                        Nifty;
SpotInfoT                        VIX;

AddContractToDemoSignalT AddContractToDemoSignal;
DemoOrderInfoSignalT     DemoOrderInfoSignal;
