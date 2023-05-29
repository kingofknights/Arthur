//
// Created by VIKLOD on 25-02-2023.
//

#include "../API/BaseScanner.hpp"
#include "../include/Signal.hpp"
#include "../include/Structure.hpp"

int			Id;
int			UserID;
std::string StatusDisplay;

StrategyNameListT				 StrategyNameList;
ScannerContainerT				 ScannerContainer;
MarketWatchDatContainerT		 MarketWatchDatContainer;
NameToTokenContainerT			 NameToTokenContainer;
ResultSetContainerT				 ResultSetContainer;
TokenToFutureTokenT				 TokenToFutureToken;
AllContractT					 AllContract;
ClientCodeListT					 ClientCodeList;
MarketEventQueueT				 MarketEventQueue;
GlobalPortfolioScannerContainerT GlobalPortfolioScannerContainer;
SpotInfoT						 BankNifty;
SpotInfoT						 Nifty;
SpotInfoT						 VIX;

AddContractToDemoSignalT AddContractToDemoSignal;
DemoOrderInfoSignalT	 DemoOrderInfoSignal;
