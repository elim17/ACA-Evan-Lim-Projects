#include "util.h"

void print_n_lsb_as_binary(int number, int n_bits)
{
    for (int i = 0; i < n_bits; i++) {
        if ((1 << (n_bits - i - 1)) & number)
            printf("1");
        else
            printf("0");
    }
}
