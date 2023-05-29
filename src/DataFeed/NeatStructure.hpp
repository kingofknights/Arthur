//
// Created by vikram on 5/2/20.
//
#pragma once

#include <cstring>

#pragma pack(push, 2)

struct cCompData {
	short iCompLen;
	char  buffer[1024];
};

struct Data {
	char			 cNetId[2];
	short			 iNoPackets;
	struct cCompData data;
};

struct MESSAGE_HEADER {
	short iApiTcode;
	short iApiFuncId;
	int	  LogTime;
	char  Alphachar[2];
	short TransactionCode;
	short ErrorCode;
	char  reserved1[8];
	char  TimeStamp1[8];
	char  TimeStamp2[8];
	short MessageLength;
};

struct Bcast_Header {
	int	  Reserved;
	int	  LogTime;
	char  Alphachar[2];
	short TransactionCode;
	short ErrorCode;
	int	  BcastSeqNo;
	int	  Reserved1;
	char  TimeStamp2[8];
	char  Filler[8];
	short MessageLength;
};

struct ST_INDICATOR {
	unsigned char Reserved_1 : 4;
	unsigned char Sell : 1;
	unsigned char Buy : 1;
	unsigned char Last_Trade_Less : 1;
	unsigned char Last_Trade_More : 1;
	unsigned char Reserved_2;
};

struct MBP_INFORMATION {
	int	  Quantity;
	int	  Price;
	short NumberOfOrders;
	short BbBuySellFlag;
};

struct INTERACTIVE_ONLY_MBP {
	int				Token;
	short			BookType;
	short			TradingStatus;
	int				VolumeTradedToday;
	int				LastTradedPrice;
	char			NetChangeIndicator;
	int				NetPriceChangeFromClosingPrice;
	int				LastTradeQuantity;
	int				LastTradeTime;
	int				AverageTradePrice;
	short			AuctionNumber;
	short			AuctionStatus;
	short			InitiatorType;
	int				InitiatorPrice;
	int				InitiatorQuantity;
	int				AuctionPrice;
	int				AuctionQuantity;
	MBP_INFORMATION Bid[5];	 // sizeof(MBP_INFORMATION)*10
	MBP_INFORMATION Ask[5];
	short			BbTotalBuyFlag;
	short			BbTotalSellFlag;
	double			TotalBuyQuantity;
	double			TotalSellQuantity;
	// char Indicator[2]; //for small endian machine
	struct ST_INDICATOR Indicator;
	int					ClosingPrice;
	int					OpenPrice;
	int					HighPrice;
	int					LowPrice;
};

struct BROADCAST_ONLY_MBP {
	struct MESSAGE_HEADER		mHeader;
	short						NoOfRecords;
	struct INTERACTIVE_ONLY_MBP Data[2];
};

struct SPREAD_DEPTH {
	short NoOrders;
	int	  Volume;
	int	  Price;
};

struct SPREAD_TotalOrderVolume {
	double Buy;
	double Sell;
};

struct MS_SPD_MKT_INFO {
	// Packet Length:	204	bytes
	struct MESSAGE_HEADER	mHeader;
	int						Token1;
	int						Token2;
	short					MbpBuy;
	short					MbpSell;
	int						LastActiveTime;
	int						TradedVolume;
	double					TotalTradedValue;
	SPREAD_DEPTH			Bid[5];
	SPREAD_DEPTH			Ask[5];
	SPREAD_TotalOrderVolume TotalOrderVolume;
	int						OpenPriceDifference;
	int						DayHighPriceDifference;
	int						DayLowPriceDifference;
	int						LastTradedPriceDifference;
	int						LastUpdateTime;
};

struct TRADE_EXECUTION_RANGE_DATA_FO {
	int TokenNumber;
	int HighExecBand;
	int LowExecBand;
};

struct MS_BCAST_TRADE_EXECUTION_RANGE_FO {
	MESSAGE_HEADER				  Header;
	int							  MsgCount;
	TRADE_EXECUTION_RANGE_DATA_FO DPR[25];
};

struct SEC_INFO_FO {
	char  InstrumentName[6];
	char  Symbol[10];
	char  Series[2];
	int	  ExpiryDate;
	int	  StrikePrice;
	char  OptionType[2];
	short CALevel;
};
struct ST_SEC_ELIGIBILITY_PER_MARKET_FO {
	char  Reserved : 7;
	char  Eligibility : 1;
	short Status;
};
struct ST_ELIGIBLITY_INDICATORS_FO {
	char Reserved : 5;
	char MinimumFill : 1;
	char AON : 1;
	char Participate_In_Market_Index : 1;
	char Reserved1;
};

struct ST_PURPOSE_Fo {
	char ExerciseStyle : 1;
	char Reserved : 1;
	char EGM : 1;
	char AGM : 1;
	char Interest : 1;
	char Bonus : 1;
	char Rights : 1;
	char Dividend : 1;
	char Reserved1 : 3;
	char IsCorporateAdjusted : 1;
	char IsThisAsset : 1;
	char PlAllowed : 1;
	char ExRejectionAllowed : 1;
	char ExAllowed : 1;
};

struct MS_SECURITY_UPDATE_INFO_FO_7305 {
	MESSAGE_HEADER					 Header;
	int								 Token;
	SEC_INFO_FO						 SecInfo;
	short							 PermittedToTrade;
	double							 IssuedCapital;
	int								 WarningQuantity;
	int								 FreezeQuantity;
	char							 CreditRating[12];
	ST_SEC_ELIGIBILITY_PER_MARKET_FO StSecurityEliPerMkt[4];
	short							 IssueRate;
	int								 IssueStartDate;
	int								 InterestPaymentDate;
	int								 IssueMaturityDate;
	int								 MarginPercentage;
	int								 MinimumLotQuantity;
	int								 BoardLotQuantity;
	int								 TickSize;
	char							 Name[25];
	char							 Reserved1;
	int								 ListingDate;
	int								 ExpulsionDate;
	int								 ReAdmissionDate;
	int								 RecordDate;
	int								 LowPriceRange;
	int								 HighPriceRange;
	int								 ExpiryDate;
	int								 NoDeliveryStartDate;
	int								 NoDeliveryEndDate;
	ST_ELIGIBLITY_INDICATORS_FO		 SecEligIndicator;
	int								 BookClosureStartDate;
	int								 BookClosureEndDate;
	int								 ExerciseStartDate;
	int								 ExerciseEndDate;
	int								 OldToken;
	char							 AssetInstrument[6];
	char							 AssetName[10];
	int								 AssetToken;
	int								 IntrinsicValue;
	int								 ExtrinsicValue;
	ST_PURPOSE_Fo					 StPurpose;
	int								 LocalUpdateDateTime;
	char							 DeleteFlag;
	char							 Remark[25];
	int								 BasePrice;
};

struct ST_BCAST_DESTINATION {
	char Reserved : 4;
	char JournalingRequired : 1;
	char Tandem : 1;
	char ControlWorkstation : 1;
	char TraderWorkstation : 1;
	char Reserved1 : 1;
};
struct BCAST_JRNL_VCT_MSG_FO_6501 {
	MESSAGE_HEADER		 Header;
	short				 BranchNumber;
	char				 BrokerNumber[5];
	char				 ActionCode[3];
	ST_BCAST_DESTINATION StBcastDestination;
	short				 BroadcastMessageLength;
	char				 BroadcastMessage[239];
};

struct CASH_INTERACTIVE_ONLY_MBP {
	short			Token;
	short			BookType;
	short			TradingStatus;
	int				VolumeTradedToday;
	int				LastTradedPrice;
	char			NetChangeIndicator;
	int				NetPriceChangeFromClosingPrice;
	int				LastTradeQuantity;
	int				LastTradeTime;
	int				AverageTradePrice;
	short			AuctionNumber;
	short			AuctionStatus;
	short			InitiatorType;
	int				InitiatorPrice;
	int				InitiatorQuantity;
	int				AuctionPrice;
	int				AuctionQuantity;
	MBP_INFORMATION Bid[5];
	MBP_INFORMATION Ask[5];
	short			BbTotalBuyFlag;
	short			BbTotalSellFlag;
	double			TotalBuyQuantity;
	double			TotalSellQuantity;
	// char Indicator[2]; //for small endian machine
	struct ST_INDICATOR Indicator;
	int					ClosingPrice;
	int					OpenPrice;
	int					HighPrice;
	int					LowPrice;
};

struct CASH_BROADCAST_ONLY_MBP_7208 {
	struct MESSAGE_HEADER			 cashHeader;
	short							 NoOfRecords;
	struct CASH_INTERACTIVE_ONLY_MBP Data[2];
};

struct INDUSTRY_INDICES {
	char IndustryName[15];
	int	 IndexValue;
};

struct CASH_BCAST_INDUSTRY_INDICES {
	struct MESSAGE_HEADER	cashHeader;
	short					NoOfRecords;
	struct INDUSTRY_INDICES sIndustry[20];
};

struct CASH_BROAD_INDI {
	char   IndexName[21];
	int	   IndexValue;
	int	   HighIndexValue;
	int	   LowIndexValue;
	int	   OpeningIndex;
	int	   ClosingIndex;
	int	   PercentChange;
	int	   YearlyHigh;
	int	   YearlyLow;
	int	   NoOfUpmoves;
	int	   NoOfDownmoves;
	double MarketCapitalisation;
	char   NetChangeIndicator;
	char   FILLER;
};

struct CASH_BROADCAST_INDICES {
	struct MESSAGE_HEADER  Header;
	short				   NoOfRecords;
	struct CASH_BROAD_INDI Indices[6];
};

struct BROADCAST_DESTINATION_EQ {
	char Reserved : 7;
	char TraderWs : 1;
	char Reserved1;
};

struct BROADCAST_MESSAGE_EQ_6501 {
	// Packet Length: 297 bytes

	struct MESSAGE_HEADER	 cashHeader;
	short					 BranchNumber;
	char					 BrokerNumber[5];
	char					 ActionCode[3];
	char					 Reserved[4];
	BROADCAST_DESTINATION_EQ Destination;
	short					 BroadcastMessageLength;
	char					 BroadcastMessage[239];
};

struct SECURITY_ELIGIBLE_INDICATORS_EQ {
	char Reserved : 5;
	char BooksMerged : 1;
	char MinimumFill : 1;
	char AON : 1;
	char Reserved1;
};

struct SYSTEM_INFORMATION_DATA_EQ {
	// Packet Length: 94 bytes
	struct MESSAGE_HEADER cashHeader;
	short				  Normal;
	short				  Oddlot;
	short				  Spot;
	short				  Auction;
	short				  CallAuction1;
	short				  CallAuction2;

	int								MarketIndex;
	short							DefaultSettlementPeriodNormal;
	short							DefaultSettlementPeriodSpot;
	short							DefaultSettlementPeriodAuction;
	short							CompetitorPeriod;
	short							SolicitorPeriod;
	short							WarningPercent;
	short							VolumeFreezePercent;
	char							Reserved1[2];
	short							TerminalIdleTime;
	int								BoardLotQuantity;
	int								TickSize;
	short							MaximumGtcDays;
	SECURITY_ELIGIBLE_INDICATORS_EQ SecEligibleIndicator;
	short							DisclosedQuantityPercentAllowed;
	char							Reserved2[6];
};

struct SEC_INFO {
	char Symbol[10];
	char Series[2];
};

struct SECURITY_ELIGIBILITY_PER_MARKET_EQ {
	char  Reserved : 7;
	char  Eligibility : 1;
	short Status;
};

struct ELIGIBLITY_INDICATORS_EQ {
	char Reserved : 5;
	char MinimumFill : 1;
	char AON : 1;
	char ParticipateInMarketIndex : 1;
	char reserved1;
};

struct PURPOSE_EQ {
	char Reserved : 2;
	char EGM : 1;
	char AGM : 1;
	char interest : 1;
	char Bonus : 1;
	char Rights : 1;
	char Dividend : 1;
	char Reserved1;
};

struct SECURITY_UPDATE_INFORMATION_EQ_7305 {
	// Packet Length: 256 bytes
	struct MESSAGE_HEADER			   cashHeader;
	short							   Token;
	SEC_INFO						   Security;
	short							   InstrumentType;
	short							   PermittedToTrade;
	double							   IssuedCapital;
	short							   WarningPercent;
	short							   FreezePercent;
	char							   CreditRating[17];
	SECURITY_ELIGIBILITY_PER_MARKET_EQ SecEligibilityPerMarket[6];
	short							   IssueRate;
	int								   IssueStartDate;
	int								   interestPaymentDate;
	int								   IssueMaturityDate;
	int								   BoardLotQuantity;
	int								   TickSize;
	char							   Name[25];
	char							   Reserved;
	int								   ListingDate;
	int								   ExpulsionDate;
	int								   ReAdmissionDate;
	int								   RecordDate;
	int								   ExpiryDate;
	int								   NoDeliveryStartDate;
	int								   NoDeliveryEndDate;
	ELIGIBLITY_INDICATORS_EQ		   EligibilityIndicator;
	int								   BookClosureStartDate;
	int								   BookClosureEndDate;
	PURPOSE_EQ						   Purpose;
	int								   LocalUpdateDateTime;
	char							   DeleteFlag;
	char							   Remark[25];
	int								   FaceValue;
	char							   ISINNumber[12];
	int								   MktMakerSpread;
	int								   MktMakerMinQty;
	short							   CallAuction1Flag;
};

struct PARTICIPANT_UPDATE_INFO_7306_EQ {
	// Packet Length: 83 bytes
	MESSAGE_HEADER cashHeader;
	char		   ParticipantId[12];
	char		   ParticipantName[25];
	char		   ParticipantStatus;
	int			   ParticipantUpdateDateTime;
	char		   DeleteFlag;
};

struct SECURITY_STATUS_PER_MARKET_EQ {
	short Status;
};

struct TOKEN_AND_ELIGIBILITY_EQ {
	short								 Token;
	struct SECURITY_STATUS_PER_MARKET_EQ SecStatusPerMkt[6];
};

struct SECURITY_STATUS_UPDATE_INFORMATION_7320_Eq {
	// Packet Length: 462 bytes
	struct MESSAGE_HEADER	 cashHeader;
	short					 NumberOfRecords;
	TOKEN_AND_ELIGIBILITY_EQ TokenAndEligibility[30];
};

struct BROADCAST_LIMIT_EXCEEDED_EQ {
	// Packet Length: 77 bytes
	struct MESSAGE_HEADER cashHeader;
	char				  BrokerCode[5];
	char				  CounterBrokerCode[5];
	short				  WarningType;
	SEC_INFO			  Security;
	int					  TradeNumber;
	int					  TradePrice;
	int					  TradeVolume;
	char				  Final;
};

struct ST_AUCTION_INQ_INFO_Eq {
	short Token;
	short AuctionNumber;
	short AuctionStatus;
	short InitiatorType;
	int	  TotalBuyQty;
	int	  BestBuyPrice;
	int	  TotalSellQty;
	int	  BestSellPrice;
	int	  AuctionPrice;
	int	  AuctionQty;
	short SettlementPeriod;
};
struct MS_AUCTION_INQ_DATA {
	// Packet Length: 74 bytes
	struct MESSAGE_HEADER  cashHeader;
	ST_AUCTION_INQ_INFO_Eq AuctionInfoReq;
};

struct BC_AUCTION_STATUS_CHANGE_EQ {
	// Packet Length: 301 bytes
	struct MESSAGE_HEADER	 cashHeader;
	SEC_INFO				 Security;
	short					 AuctionNumber;
	char					 AuctionStatus;
	char					 ActionCode[3];
	BROADCAST_DESTINATION_EQ BcastDestination;
	short					 BroadcastMessageLength;
	char					 BroadcastMessage[239];
};

struct BC_OPEN_MESSAGE_EQ {
	// Packet Length: 297 bytes
	struct MESSAGE_HEADER	 cashHeader;
	SEC_INFO				 Security;
	short					 MarketType;
	BROADCAST_DESTINATION_EQ BcastDestination;
	short					 BroadcastMessageLength;
	char					 BroadcastMessage[239];
};

struct TICKER_INDEX_INFORMATION_EQ {
	int	  Token;
	short MarketType;
	int	  FillPrice;
	int	  FillVolume;
	int	  OpenInterest;
	int	  DayHiOI;
	int	  DayLoOI;
};

struct BCAST_TICKER_AND_MKT_INDEX_7202 {
	// Packet Length: 490 bytes
	struct MESSAGE_HEADER		cashHeader;
	short						NumberOfRecords;
	TICKER_INDEX_INFORMATION_EQ TickerIndex[25];
};

struct MBP_INFORMATION_EQ {
	int	  Quantity;
	int	  Price;
	short NumberOfOrders;
	short BbBuySellFlag;
};

struct ST_MBO_MBP_TERMS {
	char Reserved1 : 6;
	char Aon : 1;
	char Mf : 1;
	char Reserved2;
};
struct MBO_INFORMATION_EQ {
	int				 TraderId;
	int				 Qty;
	int				 Price;
	ST_MBO_MBP_TERMS StMboMbpTerms;
	int				 MinFillQty;
};
struct MBO_MBP_INDICATOR_EQ {
	char Reserved : 4;
	char Sell : 1;
	char Buy : 1;
	char LastTradeLess : 1;
	char LastTradeMore : 1;
	char Reserved1;
};
struct intERACTIVE_MBO_DATA_EQ {
	short			   Token;
	short			   BookType;
	short			   TradingStatus;
	int				   VolumeTradedToday;
	int				   LastTradedPrice;
	char			   NetChangeIndicator;
	int				   NetPriceChangeFromClosingPrice;
	int				   LastTradeQuantity;
	int				   LastTradeTime;
	int				   AverageTradePrice;
	short			   AuctionNumber;
	short			   AuctionStatus;
	short			   InitiatorType;
	int				   InitiatorPrice;
	int				   InitiatorQuantity;
	int				   AuctionPrice;
	int				   AuctionQuantity;
	MBO_INFORMATION_EQ MBOBuffer[10];
};

struct BCAST_MBO_MBP_UPDATE_EQ_7200 {
	// Packet Length: 432 bytes
	struct MESSAGE_HEADER	cashHeader;
	intERACTIVE_MBO_DATA_EQ MBO_DATA;
	MBP_INFORMATION_EQ		MbpBuffer[10];
	short					BbTotalBuyFlag;
	short					BbTotalSellFlag;
	double					TotalBuyQuantity;
	double					TotalSellQuantity;
	MBO_MBP_INDICATOR_EQ	MboMbpIndicator;
	int						ClosingPrice;
	int						OpenPrice;
	int						HighPrice;
	int						LowPrice;
};

struct MARKET_WISE_INFORMATION_EQ {
	MBO_MBP_INDICATOR_EQ MboMbpIndicator;
	int					 BuyVolume;
	int					 BuyPrice;
	int					 SellVolume;
	int					 SellPrice;
	int					 LastTradePrice;
	int					 LastTradeTime;
};

struct MARKET_WATCH_BROADCAST_EQ {
	short					   Token;
	MARKET_WISE_INFORMATION_EQ MarketWiseInfo[3];
	int						   OpenInterest;
};

struct BROADCAST_INQUIRY_RESPONSE_EQ_7201 {
	// Packet Length: 442 bytes
	struct MESSAGE_HEADER	  cashHeader;
	short					  NumberOfRecords;
	MARKET_WATCH_BROADCAST_EQ MktWatchBcast[5];
};

struct MS_INDICES {
	char   IndexName[21];
	int	   IndexValue;
	int	   HighIndexValue;
	int	   LowIndexValue;
	int	   OpeningIndex;
	int	   ClosingIndex;
	int	   PercentageChange;
	int	   YearlyHigh;
	int	   YearlyLow;
	int	   NumofUp;
	int	   NumofDown;
	double MarketCaptial;
	char   NetChangeIndicator;
	char   Reserved;
};

struct MS_BCAST_INDICES_7207 {
	MESSAGE_HEADER MessageHeader;
	short		   NumberOfRecords;
	MS_INDICES	   Index[6];
};

struct TicketIndex {
	int	  Token;
	short MarketType;
	int	  FillPrice;
	int	  FillVolume;
	int	  OpenInterest;
	int	  DayHighOI;
	int	  DayLowOI;
};

struct FoMarketData {
	Bcast_Header header;
	short		 NofRecord;
	TicketIndex	 Index[17];
};

struct MBPDepth {
	short NumberofOrders;
	int	  Volume;
	int	  Price;
};
struct OrderVOlume {
	double Buy;
	double Sell;
};
struct BCAST_SPD_MBP_DELTA {
	Bcast_Header Header;
	int			 Token1;
	int			 Token2;
	short		 MbpBuy;
	short		 MbpSell;
	int			 LastActiveTime;
	int			 TradedVolume;
	double		 TotalTradeValue;
	MBPDepth	 Buy[5];
	MBPDepth	 Sell[5];
	OrderVOlume	 OrderVOlume_;
	int			 Open;
	int			 High;
	int			 Low;
	int			 Close;
	int			 TradedPriceDifference;
	int			 LastUpdateTime;
};

struct OPEN_INTEREST {
	int Token;
	int OI;
};

struct MKT_MVMT_CM_OI_IN_7130 {
	char		  Reserverd[4];
	int			  LogTime;
	char		  MarketType[2];
	short		  TCODE;
	short		  NumberofRecords;
	char		  Reserverd1[26];
	OPEN_INTEREST openIntereset[58];
};

#pragma pack(pop)
