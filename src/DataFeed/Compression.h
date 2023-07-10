//
// Created by admin on 11/11/21.
//

#pragma once
#include "../API/Common.hpp"

namespace Lancelot {
enum RequestType : int;
}

class Compression {
public:
	static RequestInPackT CompressData(std::string_view data, uint64_t uid, Lancelot::RequestType type_);

	static int DeCompressData(const unsigned char *data, int size, unsigned char *unCompressedData, int *unCompressedDataLength);
};
