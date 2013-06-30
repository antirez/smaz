#ifndef _SMAZ_H
#define _SMAZ_H

#define SMAZ_LETTER_COUNT ('z'+1)

struct SmazBranch {
    struct SmazBranch *children[SMAZ_LETTER_COUNT];
    char shortcut[12];
    int use_shortcut;
    int shortcut_length;
    int value;
};

struct SmazBranch *smaz_build_trie();
struct SmazBranch *smaz_build_custom_trie(char *codebook[254]);
void smaz_free_trie(struct SmazBranch *t);

int smaz_compress_ref(char *in, int inlen, char *out, int outlen);
int smaz_compress(struct SmazBranch *trie, char *in, int inlen, char *out, int outlen);
int smaz_decompress(char *in, int inlen, char *out, int outlen);
int smaz_decompress_custom(char *cb[254], char *in, int inlen, char *out, int outlen);

#endif
