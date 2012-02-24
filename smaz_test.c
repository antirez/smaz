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
    char *strings[] = {
        "This is a small string",
        "foobar",
        "the end",
        "not-a-g00d-Exampl333",
        "Smaz is a simple compression library",
        "Nothing is more difficult, and therefore more precious, than to be able to decide",
        "this is an example of what works very well with smaz",
        "1000 numbers 2000 will 10 20 30 compress very little",
        "and now a few italian sentences:",
        "Nel mezzo del cammin di nostra vita, mi ritrovai in una selva oscura",
        "Mi illumino di immenso",
        "L'autore di questa libreria vive in Sicilia",
        "try it against urls",
        "http://google.com",
        "http://programming.reddit.com",
        "http://github.com/antirez/smaz/tree/master",
        "/media/hdb1/music/Alben/The Bla",
        NULL
    };

    j=0;
    while(strings[j]) {
        int comprlevel;

        comprlen = smaz_compress(strings[j],strlen(strings[j]),out,sizeof(out));
        comprlevel = 100-((100*comprlen)/strlen(strings[j]));
        decomprlen = smaz_decompress(out,comprlen,d,sizeof(d));
        if (strlen(strings[j]) != (unsigned)decomprlen ||
            memcmp(strings[j],d,decomprlen))
        {
            printf("BUG: error compressing '%s'\n", strings[j]);
            exit(1);
        }
        if (comprlevel < 0) {
            printf("'%s' enlarged by %d%%\n",strings[j],-comprlevel);
        } else {
            printf("'%s' compressed by %d%%\n",strings[j],comprlevel);
        }
        j++;
    }
    printf("Encrypting and decrypting %d test strings...\n", times);
    while(times--) {
        char charset[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz/. ";
        ranlen = random() % 512;

        for (j = 0; j < ranlen; j++) {
            if (times & 1)
                in[j] = charset[random() % (sizeof(charset)-1)];
            else
                in[j] = (char)(random() & 0xff);
        }
        comprlen = smaz_compress(in,ranlen,out,sizeof(out));
        decomprlen = smaz_decompress(out,comprlen,d,sizeof(out));

        if (ranlen != decomprlen || memcmp(in,d,ranlen)) {
            printf("Bug! TEST NOT PASSED\n");
            exit(1);
        }
        /* printf("%d -> %d\n", comprlen, decomprlen); */
    }
    printf("TEST PASSED :)\n");
    return 0;
}
