#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[]) // int argc, char *argv[] array of ptrs
{
    // read inputs from user(option, source_file, secret_file)with extn

    // step1 -> check  check_operation_type(option) is returned _encode or e_decode
    OperationType res = check_operation_type(argv[1]); // check this
    // e_encode
    if (res == e_encode)
    {
        // EncodeInfo encInfo;
        EncodeInfo encInfo; // created a struct var
        // step 1 -> call read_and_validate_encode_args(&encInfo inputs)
        if (read_and_validate_encode_args(argv, &encInfo) == e_success) // in first argu need to pass entire args list
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
    // e_decode

    // e_unsupported
    //  print error msg and stop the program
}

OperationType check_operation_type(char *symbol)
{
    // step 1 -> check symbol is -e or -d
    //  -e -> return e_encode
    // - d -> return e_decode
    if (!(strcmp(symbol, "-e")))
    { //*symbol=='-e' dont do this ciz its 2 chars not 1
        return e_encode;
    }
    else if (!(strcmp(symbol, "-d")))
    { // note strcmp returns zzero on succes so must use ! here
        return e_decode;
    }
    return e_unsupported;

    // return e_unsupported
}
