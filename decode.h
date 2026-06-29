#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    /* Stego Image */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output File */
    char *output_fname;
    FILE *fptr_output;

    /* Secret file info */
    int extn_size;
    char extn_secret_file[20];

    int secret_file_size;

} DecodeInfo;


/* Read and validate decode arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode one byte */
char decode_byte_from_lsb(char *buffer);

/* Decode integer */
int decode_size_from_lsb(char *buffer);

/* Decode magic string */
Status decode_magic_string(const char *magic, DecodeInfo *decInfo);

/* Decode extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Perform decoding */
Status do_decoding(DecodeInfo *decInfo);

#endif