#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") == NULL) //check for stego .bmp
        return e_failure;

    decInfo->stego_image_fname = argv[2]; 

    static char output_file[100];

    if (argv[3] == NULL)
    {
        strcpy(output_file, "decoded.txt");
    }
    else
    {
        strcpy(output_file, argv[3]);

        char *dot = strrchr(output_file, '.');

        if (dot != NULL)
            *dot = '\0'; //at the pos of '.' put null so that we get filename irresepective of extension

        strcat(output_file, ".txt"); //concatenate the name with.txt for output
    }

    decInfo->output_fname = output_file; //store it 

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo) 
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    if (decInfo->fptr_stego_image == NULL)
        return e_failure;

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    if (decInfo->fptr_output == NULL)
        return e_failure;

    return e_success;
}

char decode_byte_from_lsb(char *buffer)
{
    char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch = ch << 1;
        ch = ch | (buffer[i] & 1);
    }

    return ch;
}

int decode_size_from_lsb(char *buffer)
{
    int size = 0;

    for (int i = 0; i < 32; i++)
    {
        size = size << 1;
        size = size | (buffer[i] & 1);
    }

    return size;
}

Status decode_magic_string(const char *magic, DecodeInfo *decInfo)
{
    char buffer[8];

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET); //skip the 54 bytes of header

    for (int i = 0; i < strlen(magic); i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);

        if (decode_byte_from_lsb(buffer) != magic[i])
            return e_failure;
    }

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, decInfo->fptr_stego_image);

    decInfo->extn_size = decode_size_from_lsb(buffer);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < decInfo->extn_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);

        decInfo->extn_secret_file[i] = decode_byte_from_lsb(buffer);
    }

    decInfo->extn_secret_file[decInfo->extn_size] = '\0';

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 1, 32, decInfo->fptr_stego_image);

    decInfo->secret_file_size = decode_size_from_lsb(buffer);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];

    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);

        char ch = decode_byte_from_lsb(buffer);

        fputc(ch, decInfo->fptr_output);
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == e_failure)
        return e_failure;

    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
        return e_failure;

    decode_secret_file_extn_size(decInfo);

    decode_secret_file_extn(decInfo);

    decode_secret_file_size(decInfo);

    decode_secret_file_data(decInfo);

    fclose(decInfo->fptr_output);
    fclose(decInfo->fptr_stego_image);

    return e_success;
}