/*
 * Read fuzzed input from mmaped file and compress/uncompress it.
 * First byte of the file is input's len.
 *
 * afl-gcc -O2 -g smaz_fuzz.c smaz.c
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "smaz.h"

int main(int argc, char *argv[])
{
	const size_t pagesz = 4096;
	const int prot  = PROT_READ|PROT_WRITE;
	const int flags = MAP_FILE|MAP_PRIVATE;
	char *buf, *in, *out;
	unsigned int inlen, outlen, compressed, decompressed;
	int fd;

	fd = open(argv[1], O_RDONLY);
	buf = mmap(NULL, pagesz, prot, flags, fd, 0);
	close(fd);

	in = buf + 1;
	inlen = *((const uint8_t *)buf);
	out = buf + 1 + inlen;
	outlen = pagesz - inlen - 1;

#if 0 /* Valid inputs. */
	compressed = smaz_compress(in, inlen, out, outlen);
	assert(compressed <= outlen);
	decompressed = smaz_decompress(out, compressed, in, inlen);
	assert(decompressed == inlen);
#else /* Malicious inputs. */
	decompressed = smaz_decompress(in, inlen, out, outlen);
	assert(decompressed <= outlen + 1);
#endif

	return EXIT_SUCCESS;
}
