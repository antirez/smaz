#ifndef _SMAZ_H
#define _SMAZ_H

struct SmazBranch {
    int value;
    struct SmazBranch **children;
    char *shortcut;
    int shortcut_length;
};

struct SmazBranch *smaz_build_trie();
void smaz_free_trie(struct SmazBranch *t);

int smaz_compress(char *in, int inlen, char *out, int outlen);
int smaz_compress_trie(struct SmazBranch *trie, char *in, int inlen, char *out, int outlen);
int smaz_decompress(char *in, int inlen, char *out, int outlen);

#endif
