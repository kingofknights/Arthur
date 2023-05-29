#include <stdio.h>
#include <string.h>

#  if defined(__cplusplus)
extern              "C" {
#  endif


#define XCD_MAX_STREAM_SIZE 16384
#define INPUT_STREAM_TOO_LARGE 551

void xcdError(int ret);

int xcdCompress(const unsigned char *UncompressedData, int UncompressedDataLength, unsigned char *CompressedData,
                int *CompressedDataLength);

int xcdDecompress(const unsigned char *CompressedData, int CompressedDataLength, unsigned char *UncompressedData,
                  int *UncompressedDataLength);

#  if defined(__cplusplus)
}
#  endif

