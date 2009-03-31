#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smaz.h"

int main(void) {
    char in[512];
    char out[4096];
    char d[4096];
    int comprlen, decomprlen;
    int j, ranlen;
    int times = 1000000;

    printf("Encrypting and decrypting %d test strings...\n", times);
    while(times--) {
        ranlen = random() % 512;
        for (j = 0; j < ranlen; j++) {
            in[j] = (unsigned char) random() % 256;
        }
        comprlen = smaz_compress(in,ranlen,out,4096);
        decomprlen = smaz_decompress(out,comprlen,d,4096);

        /*
        {
            int i;
            for(i=0;i<comprlen;i++) {
                printf("%d (%c)\n", (unsigned char)out[i],out[i]);
            }
        }
        */

        if (ranlen != decomprlen || memcmp(in,d,ranlen)) {
            printf("Bug! TEST NOT PASSED\n");
            exit(1);
        }
        /* printf("%d -> %d\n", comprlen, decomprlen); */
    }
    printf("TEST PASSED :)\n");
    return 0;
}
