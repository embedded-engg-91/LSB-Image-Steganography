#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3; // beacuse 1 pixel = 3bytes each of r,g,b
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    // make fptr set to eof
    fseek(fptr, 0, SEEK_END); // if it is at end of function only then can we know the size
    return ftell(fptr);       // return secret file size
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // step 1 -> check the source file having .bmp or not
    // no -> return e_failure
    // yes -> encInfo -> src_image_fname = src;

    // step 2 -> check secret file having (.)extn is there or not
    // no -> return e_failure
    // yes -> encInfo -> secret_fname = secret and store extn also

    // step 3 -> encInfo ->stego_image_fname = "stego.bmp"

    // return e_success;
    char *che1 = strstr(argv[2], ".bmp"); //validate from last
    if (che1 == NULL)
        return e_failure;
    else
        encInfo->src_image_fname = argv[2];

    char *che2 = strstr(argv[3], "."); //use strrchr 
    if (che2 == NULL)
        return e_failure;
    else
        encInfo->secret_fname = argv[3];

    if (argv[4] != NULL)
    {
        char *ptr3 = strstr(argv[4], ".bmp"); // user enters his optional name
        if (ptr3 == NULL)                     // if it doesnt contain .bmp then its invalid
            return e_failure;
        else
            encInfo->stego_image_fname = argv[4]; // no need to use stcpy as stego_image_fname is a charcter pointer so just make it point to addrs of argv[4]
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // see if have enough rgb data to encode
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // first get the image size ans store it inside strucutre
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);           // find the secrect file size and store it inside the structure
    if (encInfo->image_capacity > (16 + 32 + 32 + 32 + ((encInfo->size_secret_file) * 8) + 54))
    { // check for capacity
        return e_success;
    }
    printf("The original file doesnt have enough data to encode the secret message.\n");
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image); // rewind src file pointer to 0th pos
    char str[54];
    fread(str, 1, 54, fptr_src_image);   // read 54 bytes form src file
    fwrite(str, 1, 54, fptr_dest_image); // write 54 bytes to dest image
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(magic_string); i++) // do not hardcode better to run the loop for magic str len times
    {
        fread(buffer, 1, 8, encInfo->fptr_src_image); // reading 8 bytes from 54th position
        encode_byte_to_lsb(magic_string[i], buffer);  // magic string is charcters so we need to call byte_to_lsb
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer); // file extension size is integer so we can encode 32 bytes at once using size to lsb
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    for (int i = 0; i < strlen(file_extn); i++) // extension number of times
    {
        char buffer[8];
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer); // file extn in terms of character so we call byte to lsb foe length of the extension
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer); // secret file size is integer so can encode 32 bits at once hence call size to lsb
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // copy secret data to src img using byte to lsb fun
    // for each byte we need to encode
    // copy byte,encode, then write onto file
    rewind(encInfo->fptr_secret);

    for (int i = 0; i < strlen(encInfo->secret_data); i++)
    {
        char buffer[8];
        char ch = fgetc(encInfo->fptr_secret); // read one charcter from the secret file
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, buffer); // send that char for encoding
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // just copy from ptr src to ddest
    int ch;                               // it returns -1 at the end na
    while ((ch = fgetc(fptr_src)) != EOF) // fegtc and fputc both use a file pointer as parameter
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // 1 byte  8 bytes encode using buffer
    // run loop 8 times
    // get lsb bit
    // and replace
    // from msb to lsb

    // get the msb of data
    // clear the lsb of image buffer
    // add that msb data to lsb
    for (int i = 0; i < 8; i++)
    {
        // get =num& mask
        int bit = (data >> (7 - i)) & 1;         // we need to start encoding from msb to lsb of data
        image_buffer[i] = image_buffer[i] & ~1;  // get the image buffers lsb and clear it
        image_buffer[i] = image_buffer[i] | bit; // set it to the bit of data
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // at once 32 bits loop for 0 to 32
    // 1 bit 8 vytes encide using buffer
    // run loop 32 times
    // get lsb bit
    // and replace
    // from msb to lsb

    for (int i = 0; i < 32; i++)
    {
        // get =num& mask
        int bit = (size >> (31 - i)) & 1;      // we need to start encoding from msb to lsb of data
        imageBuffer[i] = imageBuffer[i] & ~1;  // get the image buffers lsb and clear it
        imageBuffer[i] = imageBuffer[i] | bit; // set it to the bit of data
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("All files opened successfully.\n");
    }
    else
    {
        printf("Failure to open files.\n");
        return e_failure;
    }

    // call check capacity funcgio
    if (check_capacity(encInfo) == e_success)
    {
        printf("File has enough capacity for encoding.\n");
    }
    else
    {
        printf("File does not have enough capacity for encoding\n");
        return e_failure;
    }

    /*
    if success-> print succes
    {
    }
    else print failure and rteurn e_failure
    */

    // next call copy_bmp_header function
    // just cprint success
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    printf("Header Copy Successfull.\n");

    /*
    next call encode_magic_string make sure  to return e_success

    */
    encode_magic_string(MAGIC_STRING, encInfo);
    printf("Magic String Encoding Successfull.\n");
    /*Encode extension size*/

    encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo); // should not be hardcoded
    printf("Secret File Extn Size Encoding Successfull.\n");

    /* Encode secret file extenstion */

    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
    printf("Secret File Extn Encoding Successfull.\n");

    /* Encode secret file size */
    encode_secret_file_size(encInfo->size_secret_file, encInfo);
    printf("Secret File Size Encoding Successfull.\n");

    /* Encode secret file data*/
    encode_secret_file_data(encInfo);
    printf("Secret File Data Encoding Successfull.\n");

    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    printf("Remaining Image Data Copy Successful.\n");

    return e_success;
}