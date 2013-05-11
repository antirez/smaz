#ifndef _SMAZ_H
#define _SMAZ_H

struct Branch {
    int value;
    struct Branch **children;
    char *shortcut;
    int shortcut_length;
};

struct Branch *newTrie();
struct Branch *buildTrie();
void freeBranch(struct Branch *t);
void addToBranch(struct Branch *t, char *remEntry, int value);

int smaz_compress(char *in, int inlen, char *out, int outlen);
int smaz_compress_trie(struct Branch *trie, char *in, int inlen, char *out, int outlen);
int smaz_decompress(char *in, int inlen, char *out, int outlen);

#endif
