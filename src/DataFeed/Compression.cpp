//
// Created by admin on 11/11/21.
//

#include "Compression.h"

#include <xcd/libxcd.h>

#include "../include/Enums.hpp"

RequestInPackT Compression::CompressData(std::string_view order, uint64_t uid, Lancelot::RequestType type_) {
	RequestInPackT requestInPack;
	requestInPack.UserIdentifier = uid;
	requestInPack.TotalSize		 = sizeof(RequestInPackT);
	requestInPack.Type			 = type_;
	int			  CompressedDataLength;
	unsigned char CompressedData[512];
	xcdCompress(reinterpret_cast<const unsigned char*>(order.data()), order.length(), CompressedData, &CompressedDataLength);
	requestInPack.CompressedMsgLen = CompressedDataLength;
	memcpy(requestInPack.Message.data(), CompressedData, CompressedDataLength);
	return requestInPack;
}

int Compression::DeCompressData(const unsigned char* data, int size, unsigned char* unCompressedData, int* unCompressedDataLength) {
	return xcdDecompress(data, size, unCompressedData, unCompressedDataLength);
}
