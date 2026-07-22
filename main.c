#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[]) // int argc, char *argv[] array of ptrs
{

    OperationType res = check_operation_type(argv[1]); // check this
    // e_encode
    if (res == e_encode)
    {
        // EncodeInfo encInfo;
        EncodeInfo encInfo;                                            
        if (read_and_validate_encode_args(argv, &encInfo) == e_success) 
        {
            if (do_encoding(&encInfo) == e_success)
            {
                printf("Encoding Operation Successfull!!!\n");
            }
            else
            {
                printf("Encoding Operation Failed!!!\n");
            }
        }
        else
        {
            printf("read and validate is failure\n");
        }
    }
    else if (res == e_decode)
    {
        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) != e_success)
        {
            printf("Please Enter Valid Decoding Arguments\n");
            return 0;
        }

        if (open_decode_files(&decInfo) == e_success)
        {
            if (decode_magic_string(MAGIC_STRING, &decInfo) == e_success)
            {
                decode_secret_file_extn_size(&decInfo);
                decode_secret_file_extn(&decInfo);
                decode_secret_file_size(&decInfo);
                decode_secret_file_data(&decInfo);

                printf("Decoding completed successfully into %s\n",
                       decInfo.output_fname);
            }
            else
            {
                printf("Magic string mismatch.\n");
            }
        }
        else
        {
            printf("Unable to open files.\n");
        }
    }
    else{

        printf("Invalid arguments have been entered");
        return 0;
    }
    
}

OperationType check_operation_type(char *symbol)
{
   
    if (!(strcmp(symbol, "-e")))
    { //*symbol=='-e' dont do this ciz its 2 chars not 1
        return e_encode;
    }
    else if (!(strcmp(symbol, "-d")))
    { // note strcmp returns zzero on succes so must use ! here
        return e_decode;
    }
    return e_unsupported;

}
