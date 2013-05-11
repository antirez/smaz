#ifndef _SMAZ_H
#define _SMAZ_H

struct Branch {
    int value;
    struct Branch **children;
    char *shortcut;
    int shortcut_length;
};

struct Branch *smaz_build_trie();
void smaz_free_trie(struct Branch *t);

int smaz_compress(char *in, int inlen, char *out, int outlen);
int smaz_compress_trie(struct Branch *trie, char *in, int inlen, char *out, int outlen);
int smaz_decompress(char *in, int inlen, char *out, int outlen);

#endif
