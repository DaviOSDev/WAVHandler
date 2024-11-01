#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WAVHeader{
    char RIFFID[5];
    int fileSize; // int bytes
    char fmtChunkType[5]; // "WAVE"
    char formatChunkID[5]; // "fmt "
    int formatChunkSize; // 16
    short formatType; // 1 (PCM)
    short numChannels;
    int sampleFrequence;
    int byteRate;
    short bytesPerBlock;
    short bitsPerSample;
    char dataChunkID[5];
    int dataSize;
    char LISTType[5];
    char LISTID[5];
    int LISTSize;
    char *metadata;
    int *metadataSizes;
    short metadataSizesLen;
}WAVHeader;

typedef struct WAVFile{
    WAVHeader *header;
    char *filePath;
    short *data;
    int duration; // in seconds
}WAVFile;

typedef enum MetaID{
    INAM = 0,
    IART = 1,
    ICRD = 2,
    ICMT = 3,
    IGNR = 4,
    IKEY = 5,
    INFO = 6,
    ISFT = 7,
}MetaID;

int initHeader(WAVHeader *header, FILE *file);
void printHeader(WAVHeader header);
int copyHeader(WAVHeader header, WAVHeader *newHeader);
int readWAV(char *filePath, WAVFile *wavFile);
int writeWAV(char *newFilePath, WAVFile *wavFile);
void invertSamples(WAVFile *wavFile);
int copyWAVFile(WAVFile wavFile, WAVFile *newWavFile);
void printWAVFileInfo(WAVFile wavFile);
void makeCopy(WAVFile wavFile);
void invertSignal(WAVFile *wavFile);
void addNoise(WAVFile *wavFile, int noiseLevel);