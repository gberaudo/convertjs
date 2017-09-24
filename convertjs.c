#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <magick/MagickCore.h>
#include <magick/blob.h>

#include <stdint.h>

// From https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}


    void build_decoding_table() {

    decoding_table = malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}


unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                          + (sextet_b << 2 * 6)
                          + (sextet_c << 1 * 6)
                          + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}



void base64_cleanup() {
    free(decoding_table);
}


const char* default_b64_in = "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"
        "REB//9k=";



char *convertjs(const char *b64_in)
{
    ExceptionInfo
            *exception;

    Image
            *image,
            *images,
            *resize_image,
            *thumbnails;

    ImageInfo
            *image_info;


    /*
      Initialize the image info structure and read an image.
    */
    MagickCoreGenesis("", MagickTrue);
    exception=AcquireExceptionInfo();
    image_info=CloneImageInfo((ImageInfo *) NULL);
    size_t in_blob_size;
    const char *in_blob = base64_decode(b64_in, strlen(b64_in), &in_blob_size);
    images = BlobToImage(image_info, in_blob, in_blob_size, exception);
    if (exception->severity != UndefinedException)
        CatchException(exception);

    if (images == (Image *) NULL)
        exit(1);


    /*
      Convert the image to a thumbnail.
    */
    thumbnails=NewImageList();
    while ((image=RemoveFirstImageFromList(&images)) != (Image *) NULL)
    {
        resize_image=ResizeImage(image,106,80,1.0,LanczosFilter,exception);
        if (resize_image == (Image *) NULL)
            MagickError(exception->severity,exception->reason,exception->description);
        (void) AppendImageToList(&thumbnails,resize_image);
        DestroyImage(image);
    }

    /*
      Export image thumbnail to base64.
    */
    size_t out_blob_size;
    const char *out_blob = ImageToBlob(image_info, thumbnails, &out_blob_size, exception);
    size_t b64_out_length;
    const char *b64_out = base64_encode(out_blob, out_blob_size, &b64_out_length);

    /*
      Destroy the image thumbnail and exit.
    */
    thumbnails=DestroyImageList(thumbnails);
    image_info=DestroyImageInfo(image_info);
    exception=DestroyExceptionInfo(exception);
    MagickCoreTerminus();
    return b64_out;
}


int main(int argc,char **argv)
{
    char *b64_out = convertjs(default_b64_in);
    fprintf(stdout, "%s", b64_out);

    return(0);
}
