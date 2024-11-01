#include <stdio.h>
#include <stdlib.h>
#include "readWAV.h"

int main(int argc, char *argv[]){
    if (argc < 3) {
        printf("Usage: %s <inputFilePath> <outputFolder>\n", argv[0]);
        return 1;
    }

    char *inputFilePath = argv[1];
    char *outputfolder = argv[2];
    WAVHeader header, invertedHeader, noiseHeader;
    WAVFile wavFile, invertedWavFile, noiseWavFile;
    wavFile.header = &header;
    invertedWavFile.header = &invertedHeader;
    noiseWavFile.header = &noiseHeader;
    if (!readWAV(inputFilePath, &wavFile))
    {
        printf("\033[31mError\033[0m: could not read WAV file\n");
        return 0;
    }
    copyWAVFile(wavFile, &invertedWavFile);
    invertSamples(&invertedWavFile);
    char *outputFile = (char *)malloc(strlen(outputfolder) + strlen("\\audioInvertido.wav") + 1);
    strcpy(outputFile, outputfolder);
    strcat(outputFile, "\\audioInvertido.wav");
    writeWAV(outputFile, &invertedWavFile);
    free(outputFile);
    copyWAVFile(wavFile, &noiseWavFile);
    addNoise(&noiseWavFile, 1000);
    outputFile = (char *)malloc(strlen(outputfolder) + strlen("\\audioSinalInvertido.wav") + 1);
    strcpy(outputFile, outputfolder);
    strcat(outputFile, "\\audioComRuido.wav");
    writeWAV(outputFile, &noiseWavFile);
    free(outputFile);
    free(wavFile.header->metadata);
    free(noiseWavFile.data);
    free(wavFile.data);
    free(invertedWavFile.data);
    return 1;
}