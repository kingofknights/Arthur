//
// Created by vikram on 4/2/20.
//

#include "NeatData.hpp"

#include "../API/ContractInfo.hpp"
#include "../include/Logger.hpp"
#include "../include/Structure.hpp"

#if WIN32

#include <winsock.h>

#else
#include <arpa/inet.h>
#endif

extern MarketEventQueueT MarketEventQueue;
#define FUTURE	 '\x2'
#define EQUITY	 '\x4'
#define CURRENCY '\x6'
#define MAX_LEN	 (1024 * 5)

#define BROADCAST_DATA		  7208
#define TRADE_EXECUTION_RANGE 7220
#define SECURITY_UPDATE		  7305
#define BROADCAST_JRNL		  6501
#define OPEN_INTEREST		  7201
#define MARKET_CLOSED		  6521
#define MARKET_OPEN			  6511
#define MARKET_PRE_OPEN		  6531
#define MARKET_PRE_CLOSE	  6571
#define HEART_BEAT			  6541
#define MarketIndex			  7207

extern SpotInfoT   BankNifty;
extern SpotInfoT   Nifty;
extern SpotInfoT   VIX;
extern std::string StatusDisplay;

double htond(double& x) {
	int* Double_Overlay;
	int	 Holding_Buffer;
	Double_Overlay	  = (int*)&x;
	Holding_Buffer	  = Double_Overlay[0];
	Double_Overlay[0] = htonl(Double_Overlay[1]);
	Double_Overlay[1] = htonl(Holding_Buffer);

	return x;
}

std::string FormatTimeToString(time_t rawtime, std::string Format) {
	rawtime = rawtime + 315513000;
	struct tm* dt;
	dt = ::localtime(&rawtime);

	char buf[80];
	if (Format.empty()) Format = "%d-%m-%Y %H:%M:%S";
	::strftime(buf, sizeof(buf), Format.c_str(), dt);
	return std::string(buf);
}

void BroadCastData(BROADCAST_ONLY_MBP* FO, short record) {
	for (int iRec = 0; iRec < record; ++iRec) {
		int	 token	 = ntohl(FO->Data[iRec].Token);
		auto pointer = ContractInfo::GetLiveDataRef(token);
		if (not pointer) continue;

		float TopBid = pointer->Bid[0].Price;
		float TopAsk = pointer->Ask[0].Price;
		float LTP	 = pointer->LastTradePrice;
		float ATP	 = pointer->AverageTradePrice;

		for (int i = 0; i < 5; ++i) {
			pointer->Bid[i].Price	 = ntohl(FO->Data[iRec].Bid[i].Price) / 100.0f;
			pointer->Bid[i].Quantity = ntohl(FO->Data[iRec].Bid[i].Quantity);
			pointer->Bid[i].Order	 = ntohs(FO->Data[iRec].Bid[i].NumberOfOrders);

			pointer->Ask[i].Price	 = ntohl(FO->Data[iRec].Ask[i].Price) / 100.0f;
			pointer->Ask[i].Quantity = ntohl(FO->Data[iRec].Ask[i].Quantity);
			pointer->Ask[i].Order	 = ntohs(FO->Data[iRec].Ask[i].NumberOfOrders);
		}

		pointer->TotalBuyQuantity  = htond(FO->Data[iRec].TotalBuyQuantity);
		pointer->TotalSellQuantity = htond(FO->Data[iRec].TotalSellQuantity);
		pointer->VolumeTradedToday = htonl(FO->Data[iRec].VolumeTradedToday);

		pointer->OpenPrice	= ntohl(FO->Data[iRec].OpenPrice) / 100.0f;
		pointer->HighPrice	= ntohl(FO->Data[iRec].HighPrice) / 100.0f;
		pointer->LowPrice	= ntohl(FO->Data[iRec].LowPrice) / 100.0f;
		pointer->ClosePrice = ntohl(FO->Data[iRec].ClosingPrice) / 100.0f;

		pointer->AverageTradePrice = ntohl(FO->Data[iRec].AverageTradePrice) / 100.0f;
		pointer->LastTradePrice	   = ntohl(FO->Data[iRec].LastTradedPrice) / 100.0f;
		pointer->LastTradeQuantity = ntohl(FO->Data[iRec].LastTradeQuantity);

		auto time = FormatTimeToString(ntohl(FO->Data[iRec].LastTradeTime), "");
		std::memset(pointer->LastTradeTime.data(), 0, 30);
		std::memcpy(pointer->LastTradeTime.data(), time.c_str(), time.length());

		pointer->PercentageChange = ((float)(pointer->ClosePrice - pointer->LastTradePrice) / pointer->ClosePrice) * 100;

		pointer->Color.TopBid = TopBid > pointer->Bid[0].Price;
		pointer->Color.TopAsk = TopAsk > pointer->Ask[0].Price;
		pointer->Color.LTP	  = LTP > pointer->LastTradePrice;
		pointer->Color.ATP	  = ATP > pointer->AverageTradePrice;

		MarketEventQueue.push(pointer);
	}
}

NeatFuture::NeatFuture() : outFo(new lzo_byte[BUFFER_SIZE * 10]), unCompressData(new lzo_byte[BUFFER_SIZE * 10]) {}
void NeatFuture::Process(char* buffer, size_t size) {
	Data* data = reinterpret_cast<Data*>(buffer);
	//	int PacketNo = htons(data->iNoPackets);
	//	int StartPont = 4;
	// for (short i = 0; i < PacketNo; ++i)
	{
		short iCompLen = ntohs(data->data.iCompLen);
		if (iCompLen > 0) {
			int r = lzo1z_decompress(reinterpret_cast<const unsigned char*>(data->data.buffer), iCompLen, outFo, &new_len, NULL);
			if (r == LZO_E_OK) {
				unCompressData = outFo;
			}
			// StartPont = StartPont + iCompLen + 2;
		} else {
			unCompressData = reinterpret_cast<lzo_byte*>(data->data.buffer);
		}

		memcpy(&mHeader, (unCompressData + 8), sizeof(struct MESSAGE_HEADER));
		if (buffer[0] == 2) {
			switch (ntohs(mHeader.TransactionCode)) {
				case BROADCAST_DATA: {
					auto* FO	= reinterpret_cast<struct BROADCAST_ONLY_MBP*>(unCompressData + 8);
					short NoRec = ntohs(FO->NoOfRecords);
					BroadCastData(FO, NoRec);
					break;
				}
				case SECURITY_UPDATE: /* Security Update */
				{
					auto* obj	   = reinterpret_cast<MS_SECURITY_UPDATE_INFO_FO_7305*>(unCompressData + 8);
					int	  token	   = ntohl(obj->Token);
					auto  iterator = ContractInfo::GetLiveDataRef(token);
					if (iterator != nullptr) {
						iterator->LowDPR  = ntohl(obj->LowPriceRange) / 100.0f;
						iterator->HighDPR = ntohl(obj->HighPriceRange) / 100.0f;
					}

					break;
				}
				case OPEN_INTEREST: {
					auto* obj = reinterpret_cast<FoMarketData*>(unCompressData + 8);

					for (int i = 0; i < ntohs(obj->NofRecord); ++i) {
						int Token = ntohl(obj->Index[i].Token);
						int OI	  = ntohl(obj->Index[i].OpenInterest);

						auto iterator = ContractInfo::GetLiveDataRef(Token);
						if (iterator != nullptr) {
							iterator->OpenInterest = OI;
						}
					}
					break;
				}
				case 7202: {
					auto* obj = reinterpret_cast<BCAST_TICKER_AND_MKT_INDEX_7202*>(unCompressData + 8);

					for (int i = 0; i < ntohs(obj->NumberOfRecords); ++i) {
						int	 Token	  = ntohl(obj->TickerIndex[i].Token);
						int	 OI		  = ntohl(obj->TickerIndex[i].OpenInterest);
						auto iterator = ContractInfo::GetLiveDataRef(Token);
						if (iterator != nullptr) {
							iterator->OpenInterest = OI;
						}
					}
					break;
				}
				case 7130: {
					auto* obj = reinterpret_cast<MKT_MVMT_CM_OI_IN_7130*>(unCompressData + 8);

					for (int i = 0; i < ntohs(obj->NumberofRecords); ++i) {
						int	 Token	  = ntohl(obj->openIntereset[i].Token);
						int	 OI		  = ntohl(obj->openIntereset[i].OI);
						auto iterator = ContractInfo::GetLiveDataRef(Token);
						if (iterator != nullptr) {
							iterator->OpenInterest = OI;
						}
					}
					break;
				}
				case MARKET_CLOSED: {
					LOG(INFO, "{}", "Market has been Closed ..! ")
					StatusDisplay = ("Market Closed");
					break;
				}

				case MARKET_OPEN: {
					LOG(INFO, "{}", "Market has been Opened ..!")
					StatusDisplay = ("Market Opened");
					break;
				}

				case MARKET_PRE_OPEN: {
					LOG(INFO, "{}", "Pre Open Market is Started ..! ")
					StatusDisplay = ("Pre Market Opened");
					break;
				}
				case MARKET_PRE_CLOSE: {
					LOG(INFO, "{}", "Pre Open Market is Closed ..! ")
					StatusDisplay = ("Pre Market Closed");
					break;
				}
				default: break;
			}
		} else if (buffer[0] == 4) {
			switch (ntohs(mHeader.TransactionCode)) {
				case MarketIndex: {
					auto* obj = reinterpret_cast<MS_BCAST_INDICES_7207*>(unCompressData + 8);
					for (int i(0); i < ntohs(obj->NumberOfRecords); ++i) {
						std::string name	   = obj->Index[i].IndexName;
						float		IndexValue = ntohl(obj->Index[i].IndexValue);
						float		change	   = ntohl(obj->Index[i].PercentageChange);
						if (name == "Nifty 50") {
							Nifty.Value	 = IndexValue / 100.0f;
							Nifty.Change = change / 100.0f;
						} else if (name == "Nifty Bank") {
							BankNifty.Value	 = IndexValue / 100.0f;
							BankNifty.Change = change / 100.0f;
						} else if (name == "India VIX") {
							VIX.Value  = IndexValue / 100.0f;
							VIX.Change = change / 100.0f;
						}
					}
					break;
				}
			}
		}
	}
}
