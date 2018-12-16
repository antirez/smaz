#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "smaz.h"

#define MAXSIZE 4096

void usage()
{
	printf("Options: \n");
	printf("	-c compress\n");
	printf("	-d decompress\n");
	printf("	-h show this page\n");
	printf("	--help same as -h\n");
}

int main(int argc, char const *argv[])
{
	if (argc < 4 && argv[1] == NULL)
	{
		fprintf(stderr, "Usage: %s options infile outfile\n", argv[0]);
		usage();
		exit(EXIT_FAILURE);
	}

	else if (strncmp(argv[1], "--help", 6) == 0 || strncmp(argv[1], "-h", 2) == 0)
	{

		printf("Usage: %s options infile outfile\n", argv[0]);
		usage();
		exit(EXIT_SUCCESS);
	}

	else if (argc < 4)
	{
		fprintf(stderr, "Usage: %s options infile outfile\n", argv[0]);
		usage();
		exit(EXIT_FAILURE);
	}

	char in[MAXSIZE];
	char out[MAXSIZE];

	FILE *fp_in = fopen(argv[2], "rb");
	FILE *fp_out = fopen(argv[3], "wb");

	int file_size;
	int output_size;

	fseek(fp_in, 0, SEEK_END);
	file_size = ftell(fp_in);
	fseek(fp_in, 0, SEEK_SET);

	fread(in, file_size, 1, fp_in);

	if (strncmp(argv[1], "-c", 2) == 0)
		output_size = smaz_compress(in, file_size, out, MAXSIZE);

	else if (strncmp(argv[1], "-d", 2) == 0)
		output_size = smaz_decompress(in, file_size, out, MAXSIZE);

	else 
	{
		fprintf(stderr, "[!] Invalid Option: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	fwrite(out, output_size, 1, fp_out);

	fclose(fp_in);
	fclose(fp_out);
	return 0;
}