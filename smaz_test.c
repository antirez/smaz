#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "smaz.h"

void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = addr;

    if (desc != NULL) {
        printf ("%s:\n", desc);
    }

    for (i = 0; i < len; i++) {

        if ((i % 16) == 0) {
            if (i != 0) {
                printf ("  %s\n", buff);
            }
            printf ("  %04x ", i);
        }

        printf (" %02x", pc[i]);

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    printf ("  %s\n", buff);
}

char *strings[] = {
        "ht",
        "foobar",
        "the end",
        "nojQfTh",
        "http://google.com",
        "try it against urls",
        "Mi illumino di immenso",
        "http://programming.reddit.com",
        "This is a small string",
        "not-a-g00d-Exampl333",
        "/media/hdb1/music/Alben/The Bla",
        "and now a few italian sentences:",
        "Smaz is a simple compression library",
        "http://github.com/antirez/smaz/tree/master",
        "L'autore di questa libreria vive in Sicilia",
        "1000 numbers 2000 will 10 20 30 compress very little",
        "this is an example of what works very well with smaz",
        "Nel mezzo del cammin di nostra vita, mi ritrovai in una selva oscura",
        "Nothing is more difficult, and therefore more precious, than to be able to decide",
        "QtZpZuMhlzfgHFEGA.Kja/hsIayllFSAMFDl.fQ/bJdzfzCvxdclaIbzzWyhbOhCj.nydSJSbmPUzhOHYqszMhvIBqqsSluQkxLbcUuRVXmhS.CrCIBPpKXEPbyhLDLJNn.pVGFEdFmKDC VLAk.LWDqLOlmhyvviIzBOBWsWGQpIPJjftiEd updeZIZjBVrOmDPGJmcZZ CziiEeAhtvkUnYdaFuvKGvdmQnmGaZVtWCpaxpVozEWjc/HyGQFMaiMqjzKYmgPGzSxsFPuCjP JcHUinZvLWVPTSarCUUYQmSGGyPYfeXCEunngaxFxPleyZjNtClHCRdYdsxWkiopaZqU.kaINJmZiUmp",
        NULL
    };

void test_compress_small_out_buff() {
    char out[4096];
    struct SmazBranch *trie;
    int comprlen = 0;
    /* skip over the first test string that will give us only 1 byte */
    int j = 1;

    trie = smaz_build_trie();
    while(strings[j]) {
        comprlen = smaz_compress_trie(
                trie,
                strings[j],
                strlen(strings[j]),
                out,
                j
            );
        if (comprlen != j+1) {
            printf("Error: Expected return size: %d, got %d\n", j+1, comprlen);
            exit(1);
        }
        j++;
    }

    smaz_free_trie(trie);
}

void test_null_term() {
    char comp_out[256];
    char decomp_out[256];
    char no_null_str[4] = "test";
    char null_term_str[] = "test"; /* implicit null here */
    int comprlen = 0;
    int decomprlen = 0;
    struct SmazBranch *trie;

    trie = smaz_build_trie();
    comprlen = smaz_compress_trie(
            trie,
            no_null_str,
            4,
            comp_out,
            256
        );
    decomprlen = smaz_decompress(
            comp_out,
            comprlen,
            decomp_out,
            256
        );
    if (decomprlen != 4) {
        printf("Error: Expected return size: %d, got %d\n", 4, decomprlen);
        exit(1);
    }
    if (decomp_out[3] != 't') {
        printf(
                "Error: Incorrect final char on string: %c, expected %c\n",
                decomp_out[3],
                't'
            );
        exit(1);
    }
    comprlen = smaz_compress_trie(
            trie,
            null_term_str,
            strlen(null_term_str)+1, /* include the null terminator this time. */
            comp_out,
            256
        );
    decomprlen = smaz_decompress(
            comp_out,
            comprlen,
            decomp_out,
            256
        );
    if (decomprlen != 5) {
        printf("Error: Expected return size: %d, got %d\n", 5, decomprlen);
        exit(1);
    }
    if (decomp_out[4] != NULL) {
        printf( "Error: Incorrect final char on string: %c, expected NULL",
                decomp_out[4]
            );
        exit(1);
    }

    smaz_free_trie(trie);
}

int main(void) {
    char in[512];
    char out[4096];
    char d[4096];
    int comprlen, decomprlen;
    int j, ranlen, x;
    int times = 1000000;
    struct SmazBranch *trie;

    j=0;

    test_compress_small_out_buff();
    test_null_term();
    trie = smaz_build_trie();

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

            comprlen = smaz_compress_trie(trie, in,ranlen,out,sizeof(out));
            /*comprlen = smaz_compress(in,ranlen,out,sizeof(out));*/
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

    smaz_free_trie(trie);
    getchar();
    return 0;
}
