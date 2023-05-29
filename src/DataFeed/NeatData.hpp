//
// Created by vikram on 4/2/20.
//

#pragma once

#include <lzo/lzo1z.h>

#include "NeatStructure.hpp"

class NeatFuture {
public:
	NeatFuture();
	void Process(char *buffer, size_t size);

protected:
	enum {
		BUFFER_SIZE = 1494
	};
	MESSAGE_HEADER mHeader;
	lzo_byte	  *outFo		  = nullptr;
	lzo_byte	  *unCompressData = nullptr;
	lzo_uint	   new_len		  = 0;
};
