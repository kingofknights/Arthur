#include "libxcd.h"
#include <time.h>
#include <stdlib.h>
#include  "zlib.h"  //" xzlib.h>

int xcdCompress(const unsigned char *UncompressedData, int UncompressedDataLength, unsigned char *CompressedData,
                int *CompressedDataLength) {
    int ret, flush;
    z_stream strm;
    unsigned char in[XCD_MAX_STREAM_SIZE];
    unsigned char out[XCD_MAX_STREAM_SIZE];

    if (UncompressedDataLength > (XCD_MAX_STREAM_SIZE - 2)) {
        return INPUT_STREAM_TOO_LARGE;
    }

    memcpy(in, UncompressedData, UncompressedDataLength);

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = UncompressedDataLength;
    flush = 1 != 0 ? Z_FINISH : Z_NO_FLUSH;
    strm.next_in = in;

    strm.avail_out = XCD_MAX_STREAM_SIZE;
    strm.next_out = out;
    ret = deflate(&strm, flush);
    if (ret == Z_STREAM_ERROR) {
        (void) deflateEnd(&strm);
        return Z_STREAM_ERROR;
    }

    if (strm.avail_in != 0 || ret != Z_STREAM_END) {
        printf("something went wrong, while compression.\n"); //study about this and then delete...
    }

    out[1] = 0;
    (*CompressedDataLength) = (XCD_MAX_STREAM_SIZE - strm.avail_out);
    out[(*CompressedDataLength) - 1] = (out[(*CompressedDataLength) - 1] + out[(*CompressedDataLength) - 4]);
    out[(*CompressedDataLength) - 4] = (out[(*CompressedDataLength) - 1] - out[(*CompressedDataLength) - 4]);
    out[(*CompressedDataLength) - 1] = (out[(*CompressedDataLength) - 1] - out[(*CompressedDataLength) - 4]);
    out[(*CompressedDataLength) - 2] = (out[(*CompressedDataLength) - 2] + out[(*CompressedDataLength) - 3]);
    out[(*CompressedDataLength) - 3] = (out[(*CompressedDataLength) - 2] - out[(*CompressedDataLength) - 3]);
    out[(*CompressedDataLength) - 2] = (out[(*CompressedDataLength) - 2] - out[(*CompressedDataLength) - 3]);
    memcpy(CompressedData, &out[1], ((*CompressedDataLength) - 1));
    (*CompressedDataLength)--;

    /* clean up and return */
    (void) deflateEnd(&strm);
    return Z_OK;
}

int xcdDecompress(const unsigned char *CompressedData, int CompressedDataLength, unsigned char *UncompressedData,
                  int *UncompressedDataLength) {
    int ret;
    z_stream strm;
    unsigned char in[XCD_MAX_STREAM_SIZE] = {'\0'};

    if (CompressedDataLength > (XCD_MAX_STREAM_SIZE - 2)) {
        return INPUT_STREAM_TOO_LARGE;
    }

    memcpy(&in[2], &CompressedData[1], CompressedDataLength);
    in[0] = 120;
    in[1] = 156;
    in[CompressedDataLength] = (in[CompressedDataLength] + in[CompressedDataLength - 3]);
    in[CompressedDataLength - 1] = (in[CompressedDataLength - 1] + in[CompressedDataLength - 2]);
    in[CompressedDataLength - 3] = (in[CompressedDataLength] - in[CompressedDataLength - 3]);
    in[CompressedDataLength - 2] = (in[CompressedDataLength - 1] - in[CompressedDataLength - 2]);
    in[CompressedDataLength] = (in[CompressedDataLength] - in[CompressedDataLength - 3]);
    in[CompressedDataLength - 1] = (in[CompressedDataLength - 1] - in[CompressedDataLength - 2]);

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    strm.next_in = in;
    strm.avail_in = (CompressedDataLength + 1);
    strm.next_out = UncompressedData;
    strm.avail_out = XCD_MAX_STREAM_SIZE;

    ret = inflate(&strm, Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR) {
        (void) inflateEnd(&strm);
        return ret;
    }
    switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            (void) inflateEnd(&strm);
            return ret;
    }

    (*UncompressedDataLength) = (XCD_MAX_STREAM_SIZE - strm.avail_out);
    if ((*UncompressedDataLength) == 0) {
        printf("Something went wrong on decompression.\n");
        return Z_DATA_ERROR;
    }

    (void) inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


void xcdError(int ret) {
    printf("Error not defined....%d\n", ret);

    /* fputs("zpipe: ", stderr); */
    /* switch (ret) { */
    /* case Z_ERRNO: */
    /*   if (ferror(stdin)) */
    /*     fputs("error reading stdin\n", stderr); */
    /*   if (ferror(stdout)) */
    /*     fputs("error writing stdout\n", stderr); */
    /*   break; */
    /* case Z_STREAM_ERROR: */
    /*   fputs("invalid compression level\n", stderr); */
    /*   break; */
    /* case Z_DATA_ERROR: */
    /*   fputs("invalid or incomplete deflate data\n", stderr); */
    /*   break; */
    /* case Z_MEM_ERROR: */
    /*   fputs("out of memory\n", stderr); */
    /*   break; */
    /* case Z_VERSION_ERROR: */
    /*   fputs("zlib version mismatch!\n", stderr); */
    /*   break; */
    /* case INPUT_STREAM_TOO_LARGE: */
    /*   fputs("Input stream is more then pre-defined!\n", stderr); */
    /* } */

}
