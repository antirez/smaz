#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "smaz.h"


void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = addr;

    if (desc != NULL)
        printf ("%s:\n", desc);

    for (i = 0; i < len; i++) {

        if ((i % 16) == 0) {
            if (i != 0)
                printf ("  %s\n", buff);

            printf ("  %04x ", i);
        }

        printf (" %02x", pc[i]);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    printf ("  %s\n", buff);
}

int main(void) {
    char in[512];
    char out[4096];
    char d[4096];
    int comprlen, decomprlen;
    int j, ranlen, x;
    int times = 1000000;
    struct Branch *trie;
    char *strings[] = {
        "nojQfTh",
        "ht",
        "QtZpZuMhlzfgHFEGA.Kja/hsIayllFSAMFDl.fQ/bJdzfzCvxdclaIbzzWyhbOhCj.nydSJSbmPUzhOHYqszMhvIBqqsSluQkxLbcUuRVXmhS.CrCIBPpKXEPbyhLDLJNn.pVGFEdFmKDC VLAk.LWDqLOlmhyvviIzBOBWsWGQpIPJjftiEd updeZIZjBVrOmDPGJmcZZ CziiEeAhtvkUnYdaFuvKGvdmQnmGaZVtWCpaxpVozEWjc/HyGQFMaiMqjzKYmgPGzSxsFPuCjP JcHUinZvLWVPTSarCUUYQmSGGyPYfeXCEunngaxFxPleyZjNtClHCRdYdsxWkiopaZqU.kaINJmZiUmp",
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
    trie = buildTrie();

    /*
    printf("here: %d\n", trie->children['9'-'\n']);
    exit(0);
    trie = newTrie();
    addToBranch(trie, "f", 1);
    addToBranch(trie, "for", 3);
    addToBranch(trie, "fo", 2);
    exit(0);
    for (x = 0; x < 'z' - '\n'; x++) {
        printf("here: %c %d\n", x+'\n', trie->children['o'-'\n']->children[x]);
    }
    printf("here: '%s'\n", trie->children['f'-'\n']->children['o'-'\n']->shortcut);
    */


    while(strings[j]) {
        int comprlevel;
        int comprlen2;

        comprlen = smaz_compress_trie(trie, strings[j],strlen(strings[j]),out,sizeof(out));
        /*hexDump("out bad", &out, 400);*/
        comprlen2 = smaz_compress(strings[j],strlen(strings[j]),out,sizeof(out));
        /*hexDump("out good", &out, 400);*/
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
    {
        struct timeval t1, t2;
        double elapsedTime;

        gettimeofday(&t1, NULL);
        while(times--) {
            char charset[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz/. ";
            ranlen = random() % 512;
            /*printf("doing %d\n", times);*/

            for (j = 0; j < ranlen; j++) {
                if (times & 1)
                    in[j] = charset[random() % (sizeof(charset)-1)];
                else
                    in[j] = (char)(random() & 0xff);
            }

            /*comprlen = smaz_compress_trie(trie, in,ranlen,out,sizeof(out));*/
            comprlen = smaz_compress(in,ranlen,out,sizeof(out));
            decomprlen = smaz_decompress(out,comprlen,d,sizeof(out));

            if (ranlen != decomprlen || memcmp(in,d,ranlen)) {
                printf("Bug! TEST NOT PASSED\n");
                hexDump("in", &in, ranlen);
                hexDump("out bad", &out, comprlen);
                comprlen = smaz_compress(in,ranlen,out,sizeof(out));
                hexDump("out good", &out, comprlen);
                exit(1);
            }
        }
        
        gettimeofday(&t2, NULL);
        
        /*elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; 
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;*/
        printf("time = %u.%06u\n", t1.tv_sec, t1.tv_usec);
        printf("time = %u.%06u\n", t2.tv_sec, t2.tv_usec);

        printf("TEST PASSED :)\n");
    }

    getchar();
    return 0;
}
