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

int g_seed = 0;

int fastrand() { 
    g_seed = (214013 * g_seed + 2531011); 
    return (g_seed >> 16) & 0x7FFF; 
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
        "oTnBmdtIaFEFHFpgqkGlYdCtqIXFTKIPfJsdIotaZ/oUGWaKHmBzzMQyKteDKLXHedxalAfHzAQTgesqyLzo/.rjxQzbWZPzUbqdnuceRejfVz/xpDBfCGUUdlLYkSyt.uRv.dQaJEW.bPsJrQWjNBbKLFbdLmauPiCdEVHgXKIZazGSriVrjQs.H.itMHFJDuajeCqOtKZFJdyUtEEqbbj.s.FQkAyXHdjHoQxDWvnFfgBMLXtFKJZvnRMiUfAgMJbH/TsXzMSKdlOHkxAJPWD//QbmuNyQWAHVIevtohUfRbCktvHfSuopjQSTWl/fpV/tNMCCSWOINMGptyRBZNobtdL.KMzKqvnnu.A.jWgOMtLrrHpcCB.GLIREreLBK.BsYABRttLHo/QhDrZNSzJPZQR.nPJEJvHMX/sO/H.tksygrsDlCIzyJMR.O.scMfNcfKufJrbeJYcALDfxRYHKTPLmmUeTe",
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
        comprlen = smaz_compress(
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

    printf("TEST PASSED :)\n");
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
    comprlen = smaz_compress(
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
    comprlen = smaz_compress(
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
        hexDump("out", &decomp_out, decomprlen);
        exit(1);
    }
    if (decomp_out[4] != 0) {
        printf( "Error: Incorrect final char on string: %c, expected \\0\n",
                decomp_out[4]
            );
        exit(1);
    }

    smaz_free_trie(trie);

    printf("TEST PASSED :)\n");
}

void bench_trie_smaz() {
    FILE    *infile;
    char    *in;
    char    *comp_out;
    char    *de_comp_out;
    long    numbytes;
    int     num_loops = 1000;

    infile = fopen("war_of_the_worlds.txt", "r");
    if (infile == NULL) {
        printf("Missing war of the worlds text, you can download the text here: http://www.gutenberg.org/ebooks/36 and save it as war_of_the_worlds.txt\n");
        exit(1);
    }

    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    printf("Processing %d bytes, %d times\n", (int)numbytes, num_loops);
    fseek(infile, 0L, SEEK_SET);	
    in = (char*)calloc(numbytes, sizeof(char));	
    comp_out = (char*)calloc(numbytes, sizeof(char));	
    de_comp_out = (char*)calloc(numbytes, sizeof(char));	
    numbytes = fread(in, sizeof(char), numbytes, infile);
    fclose(infile);

    {
        struct timeval t1, t2;
        int x;
        struct SmazBranch *trie;

        trie = smaz_build_trie();

        gettimeofday(&t1, NULL);
        for (x = 0; x < num_loops; x++) {
            smaz_compress(
                    trie,
                    in,
                    numbytes,
                    comp_out,
                    numbytes
                );
        }
        gettimeofday(&t2, NULL);
        printf("time = %u.%06u\n", (unsigned int)t1.tv_sec, (unsigned int)t1.tv_usec);
        printf("time = %u.%06u\n", (unsigned int)t2.tv_sec, (unsigned int)t2.tv_usec);

        smaz_free_trie(trie);
    }

    free(in);
    free(comp_out);
    free(de_comp_out);
}

void bench_old_smaz() {
    FILE    *infile;
    char    *in;
    char    *comp_out;
    char    *de_comp_out;
    long    numbytes;
    int     num_loops = 1000;

    infile = fopen("war_of_the_worlds.txt", "r");
    if (infile == NULL) {
        printf("Missing war of the worlds text, you can download the text here: http://www.gutenberg.org/ebooks/36 and save it as war_of_the_worlds.txt\n");
        exit(1);
    }

    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    printf("Processing %d bytes, %d times\n", (int)numbytes, num_loops);
    fseek(infile, 0L, SEEK_SET);	
    in = (char*)calloc(numbytes, sizeof(char));	
    comp_out = (char*)calloc(numbytes, sizeof(char));	
    de_comp_out = (char*)calloc(numbytes, sizeof(char));	
    numbytes = fread(in, sizeof(char), numbytes, infile);
    fclose(infile);

    {
        struct timeval t1, t2;
        int x;

        gettimeofday(&t1, NULL);
        for (x = 0; x < num_loops; x++) {
            smaz_compress_ref(
                    in,
                    numbytes,
                    comp_out,
                    numbytes
                );
        }
        gettimeofday(&t2, NULL);
        printf("time = %u.%06u\n", (unsigned int)t1.tv_sec, (unsigned int)t1.tv_usec);
        printf("time = %u.%06u\n", (unsigned int)t2.tv_sec, (unsigned int)t2.tv_usec);
    }

    free(in);
    free(comp_out);
    free(de_comp_out);
}

void test_strings() {
    char out[4096];
    char out_good[4096];
    char d[4096];
    int comprlen, decomprlen;
    struct SmazBranch *trie;
    int j = 0;

    trie = smaz_build_trie();

    while(strings[j]) {
        int comprlevel, comprlen_good;

        comprlen = smaz_compress(
                trie,
                strings[j],
                strlen(strings[j]),
                out,
                sizeof(out)
            );

        comprlen_good = smaz_compress_ref(
                strings[j],
                strlen(strings[j]),
                out_good,
                sizeof(out_good)
            );

        comprlevel = 100-((100*comprlen)/strlen(strings[j]));
        decomprlen = smaz_decompress(out,comprlen,d,sizeof(d));

        if (comprlen != comprlen_good ||
            strlen(strings[j]) != (unsigned)decomprlen ||
            memcmp(strings[j],d,decomprlen))
        {
            printf("BUG: error compressing '%s'\n", strings[j]);
            hexDump("in", strings[j], strlen(strings[j]));
            hexDump("out bad", &out, comprlen);
            hexDump("out good", &out_good, comprlen_good);
            exit(1);
        }
        if (comprlevel < 0) {
            printf("'%s' enlarged by %d%%\n",strings[j],-comprlevel);
        } else {
            printf("'%s' compressed by %d%%\n",strings[j],comprlevel);
        }
        j++;
    }

    smaz_free_trie(trie);

    printf("TEST PASSED :)\n");
}

void test_random() {
    char in[512];
    char out[4096];
    char d[4096];
    int comprlen, decomprlen;
    int j, ranlen = 0;
    int times = 1000000;
    struct SmazBranch *trie;

    g_seed = 0;
    trie = smaz_build_trie();

    printf("Encrypting and decrypting %d test strings...\n", times);
    while(times--) {
        char charset[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz/. ";
        ranlen = fastrand() % 512;
        /*printf("doing %d\n", times);*/

        for (j = 0; j < ranlen; j++) {
            if (times & 1)
                in[j] = charset[fastrand() % (sizeof(charset)-1)];
            else
                in[j] = (char)(fastrand() & 0xff);
        }

        comprlen = smaz_compress(trie, in,ranlen,out,sizeof(out));
        /*comprlen = smaz_compress_ref(in,ranlen,out,sizeof(out));*/
        decomprlen = smaz_decompress(out,comprlen,d,sizeof(out));
        if (ranlen != decomprlen || memcmp(in,d,ranlen)) {
            printf("Bug! TEST NOT PASSED: %d\n", 1000000-times);
            hexDump("in", &in, ranlen);
            hexDump("out bad", &out, comprlen);
            comprlen = smaz_compress_ref(in,ranlen,out,sizeof(out));
            hexDump("out good", &out, comprlen);
            exit(1);
        }
    }

    smaz_free_trie(trie);

    printf("TEST PASSED :)\n");
}

void bench_random_old_smaz() {
    char in[512];
    char out[4096];
    int j, ranlen = 0;
    int times = 1000000;
    struct timeval t1, t2;

    g_seed = 0;

    printf("Encrypting and decrypting %d test strings...\n", times);
    gettimeofday(&t1, NULL);

    while(times--) {
        char charset[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz/. ";
        ranlen = fastrand() % 512;
        /*printf("doing %d\n", times);*/

        for (j = 0; j < ranlen; j++) {
            if (times & 1)
                in[j] = charset[fastrand() % (sizeof(charset)-1)];
            else
                in[j] = (char)(fastrand() & 0xff);
        }
        smaz_compress_ref(in,ranlen,out,sizeof(out));
    }
    gettimeofday(&t2, NULL);
    
    printf("time = %u.%06u\n", (unsigned int)t1.tv_sec, (unsigned int)t1.tv_usec);
    printf("time = %u.%06u\n", (unsigned int)t2.tv_sec, (unsigned int)t2.tv_usec);
}

void bench_random_trie() {
    char in[512];
    char out[4096];
    int j, ranlen = 0;
    int times = 1000000;
    struct SmazBranch *trie;
    struct timeval t1, t2;

    g_seed = 0;

    trie = smaz_build_trie();

    printf("Encrypting and decrypting %d test strings...\n", times);
    gettimeofday(&t1, NULL);

    while(times--) {
        char charset[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz/. ";
        ranlen = fastrand() % 512;
        /*printf("doing %d\n", times);*/

        for (j = 0; j < ranlen; j++) {
            if (times & 1)
                in[j] = charset[fastrand() % (sizeof(charset)-1)];
            else
                in[j] = (char)(fastrand() & 0xff);
        }
        smaz_compress(trie, in,ranlen,out,sizeof(out));
    }
    gettimeofday(&t2, NULL);
    
    printf("time = %u.%06u\n", (unsigned int)t1.tv_sec, (unsigned int)t1.tv_usec);
    printf("time = %u.%06u\n", (unsigned int)t2.tv_sec, (unsigned int)t2.tv_usec);

    smaz_free_trie(trie);
}

/* Reverse compression codebook, used for decompression */
static char *Smaz_rcb[254] = {
" ", "the", "e", "t", "a", "of", "o", "and", "i", "n", "s", "e ", "r", " th",
" t", "in", "he", "th", "h", "he ", "to", "\r\n", "l", "s ", "d", " a", "an",
"er", "c", " o", "d ", "on", " of", "re", "of ", "t ", ", ", "is", "u", "at",
"   ", "n ", "or", "which", "f", "m", "as", "it", "that", "\n", "was", "en",
"  ", " w", "es", " an", " i", "\r", "f ", "g", "p", "nd", " s", "nd ", "ed ",
"w", "ed", "http://", "for", "te", "ing", "y ", "The", " c", "ti", "r ", "his",
"st", " in", "ar", "nt", ",", " to", "y", "ng", " h", "with", "le", "al", "to ",
"b", "ou", "be", "were", " b", "se", "o ", "ent", "ha", "ng ", "their", "\"",
"hi", "from", " f", "in ", "de", "ion", "me", "v", ".", "ve", "all", "re ",
"ri", "ro", "is ", "co", "f t", "are", "ea", ". ", "her", " m", "er ", " p",
"es ", "by", "they", "di", "ra", "ic", "not", "s, ", "d t", "at ", "ce", "la",
"h ", "ne", "as ", "tio", "on ", "n t", "io", "we", " a ", "om", ", a", "s o",
"ur", "li", "ll", "ch", "had", "this", "e t", "g ", "e\r\n", " wh", "ere",
" co", "e o", "a ", "us", " d", "ss", "\n\r\n", "\r\n\r", "=\"", " be", " e",
"s a", "ma", "one", "t t", "or ", "but", "el", "so", "l ", "e s", "s,", "no",
"ter", " wa", "iv", "ho", "e a", " r", "hat", "s t", "ns", "ch ", "wh", "tr",
"ut", "/", "have", "ly ", "ta", " ha", " on", "tha", "-", " l", "ati", "en ",
"pe", " re", "there", "ass", "si", " fo", "wa", "ec", "our", "who", "its", "z",
"fo", "rs", ">", "ot", "un", "<", "im", "th ", "nc", "ate", "><", "ver", "ad",
" we", "ly", "ee", " n", "id", " cl", "ac", "il", "</", "rt", " wi", "div",
"e, ", " it", "whi", " ma", "ge", "x", "e c", "men", ".com"
};

void bench_random_compressible() {
    char **in;
    char out[2048];
    int x = 0;
    int times = 500000;
    struct SmazBranch *trie;
    struct timeval t1, t2;

    g_seed = 0;
    trie = smaz_build_trie();

    printf("Creating compressible strings\n");
    in = (char **)calloc(times+1, sizeof(char *));
    for (x = 0; x < times; x++) {
        int charlen = 0;
        in[x] = (char *)calloc(1024, sizeof(char));
        /* 7 being the longest possible string */
        while (charlen < (1024 - 7)) {
            char *val = Smaz_rcb[fastrand() % 254];
            memcpy(in[x]+charlen, val, strlen(val));
            charlen += strlen(val);
        }
    }

    printf("Encrypting and decrypting %d test strings...\n", times);
    gettimeofday(&t1, NULL);

    while (times--) {
        smaz_compress(
                trie,
                in[times],
                strlen(in[times]),
                out,
                sizeof(out)
            );
    }
    gettimeofday(&t2, NULL);
    
    printf("time = %u.%06u\n", (unsigned int)t1.tv_sec, (unsigned int)t1.tv_usec);
    printf("time = %u.%06u\n", (unsigned int)t2.tv_sec, (unsigned int)t2.tv_usec);

    for (x = 0; x < times; x++) {
        free(in[x]);
    }
    free(in);
    smaz_free_trie(trie);
}

void bench_random_compressible_old() {
    char **in;
    char out[2048];
    int x = 0;
    int times = 500000;
    struct timeval t1, t2;

    g_seed = 0;

    printf("Creating compressible strings\n");
    in = (char **)calloc(times+1, sizeof(char *));
    for (x = 0; x < times; x++) {
        int charlen = 0;
        in[x] = (char *)calloc(1024, sizeof(char));
        /* 7 being the longest possible string */
        while (charlen < (1024 - 7)) {
            char *val = Smaz_rcb[fastrand() % 254];
            memcpy(in[x]+charlen, val, strlen(val));
            charlen += strlen(val);
        }
    }

    printf("Encrypting and decrypting %d test strings...\n", times);
    gettimeofday(&t1, NULL);

    while (times--) {
        smaz_compress_ref(
                in[times],
                strlen(in[times]),
                out,
                sizeof(out)
            );
    }
    gettimeofday(&t2, NULL);
    
    printf("time = %u.%06u\n", (unsigned int)t1.tv_sec, (unsigned int)t1.tv_usec);
    printf("time = %u.%06u\n", (unsigned int)t2.tv_sec, (unsigned int)t2.tv_usec);

    for (x = 0; x < times; x++) {
        free(in[x]);
    }
    free(in);
}

int main(void) {

    printf("\n\nTesting result when using too smaller buffer:\n-------------\n");
    test_compress_small_out_buff();
    printf("\n\nTesting null terminators stay there:\n-------------\n");
    test_null_term();
    printf("\n\nTesting a bunch of predefined strings:\n-------------\n");
    test_strings();
    printf("\n\nTesting a bunch of randomly generated strings:\n-------------\n");
    test_random();
    printf("\n\nBenchmarking old smaz with very compressible data:\n-------------\n");
    bench_random_compressible_old();
    printf("\n\nBenchmarking new smaz with very compressible data:\n-------------\n");
    bench_random_compressible();
    printf("\n\nBenchmarking old smaz on war of the worlds:\n-------------\n");
    bench_old_smaz();
    printf("\n\nBenchmarking new smaz on war of the worlds:\n-------------\n");
    bench_trie_smaz();
    printf("\n\nBenchmarking old smaz on random data:\n-------------\n");
    bench_random_old_smaz();
    printf("\n\nBenchmarking new smaz on random data:\n-------------\n");
    bench_random_trie();
    printf("\n\nDone.\n");

    return 0;
}
