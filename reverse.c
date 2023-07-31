// PROGRAM: Reverse reverses a .wav audio file. It accepts input and out WAV files as command line arguments.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 3)
    {
        printf("\nUsage: ./reverse input.wav output.wav\n");
        return 1;
    }

    // Open input file for reading
    FILE *infile = fopen(argv[1], "r");
    if (infile == NULL)
    {
        printf("\nCould not open %s\n", argv[1]);
        return 1;
    }
    // Read header
    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, infile);

    // Use check_format to ensure WAV format
    if (!check_format(header))
    {
        printf("\nNot a valid WAV file\n");
        return 1;
    }

    // Open output file for writing
    FILE *outfile = fopen(argv[2], "w");
    if (outfile == NULL)
    {
        fclose(infile);
        printf("\nCould not open %s\n", argv[2]);
        return 1;
    }

    // Write header to file
    fwrite(&header, sizeof(WAVHEADER), 1, outfile);

    // Use get_block_size to calculate size of block
    int block_size = get_block_size(header);

    // Set pointer to 1 block after the end of infile
    if (fseek(infile, block_size, SEEK_END))
    {
        fclose(infile);
        fclose(outfile);
        return 1;
    }
    // write data in output file in reverse order
    BYTE buffer[block_size];

    // repeat if 1 block before current infile pointer would point to data after header
    while (ftell(infile) - block_size > sizeof(header))
    {
        // set infile pointer to 2 blocks before the last read block
        if (fseek(infile, -2 * block_size, SEEK_CUR))
        {
            fclose(infile);
            fclose(outfile);
            return 1;
        }
        // read next block
        fread(buffer, block_size, 1, infile);
        // write block
        fwrite(buffer, block_size, 1, outfile);
    }

    fclose(infile);
    fclose(outfile);
    return 0;
}

// check format of WAV file
int check_format(WAVHEADER header)
{
    char *marker = "WAVE";
    for (int i = 0; i < 4; i++)
    {
        if (header.format[i] != marker[i])
        {
            return 0;
        }
    }
    return 1;
}

// calculate block size
int get_block_size(WAVHEADER header)
{
    return header.numChannels * header.bitsPerSample / 8;
}