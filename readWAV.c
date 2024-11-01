#include "readWAV.h"

    int getMetadata(char *metadataID){
        if (!strcmp(metadataID, "INAM"))
        {
            return 0;
        }
        if (!strcmp(metadataID, "IART"))
        {
            return 1;
        }
        if (!strcmp(metadataID, "ICRD"))
        {
            return 2;
        }
        if (!strcmp(metadataID, "ICMT"))
        {
            return 3;
        }
        if (!strcmp(metadataID, "IGNR"))
        {
            return 4;
        }
        if (!strcmp(metadataID, "IKEY"))
        {
            return 5;
        }
        if (!strcmp(metadataID, "INFO"))
        {
            return 6;
        }
        if (!strcmp(metadataID, "ISFT"))
        {
            return 7;
        }
        return -1;
    }

void readMetadata(FILE *file, WAVHeader *header, char *metadataType){
    int metadataSize;
    fread(&metadataSize, 4, 1, file);
    char *metadatasup = (char *)malloc(metadataSize + 1);
    char *metadata = (char *)malloc(metadataSize + 9);
    fread(metadatasup, 1, metadataSize, file);
    metadatasup[metadataSize] = '\0';
    strcat(metadata, metadataType);
    strcat(metadata, metadatasup);
    strcat(header->metadata, metadata);
    strcat(header->metadata, "^");
}

int initHeader(WAVHeader *header, FILE *file){
    // Inicializa o header
    header->RIFFID[4] = '\0';
    header->fmtChunkType[4] = '\0';
    header->formatChunkID[4] = '\0';
    header->dataChunkID[4] = '\0';
    header->dataSize = 0;
    header->LISTSize = 0;
    header->metadata = NULL;
    strcpy(header->LISTType, "NULL");
    strcpy(header->LISTID, "NULL");
    // Leitura inicial para o RIFF e WAVE
    fread(header->RIFFID, 1, 4, file);
    if (strcmp(header->RIFFID, "RIFF") != 0) {
        printf("\033[31mError\033[0m: RIFF ID not found\n");
        return 0;
    }
    header->RIFFID[4] = '\0';
    fread(&header->fileSize, 4, 1, file);
    fread(header->fmtChunkType, 1, 4, file);
    header->fmtChunkType[4] = '\0';
    if (strcmp(header->fmtChunkType, "WAVE") != 0) {
        printf("\033[31mError\033[0m: lib only supports WAVE type.\nfile type: %s\n", header->fmtChunkType);
        return 0;
    }

    fread(header->formatChunkID, 1, 4, file);
    header->formatChunkID[4] = '\0';
    fread(&header->formatChunkSize, 4, 1, file);
    fread(&header->formatType, 2, 1, file);
    fread(&header->numChannels, 2, 1, file);
    fread(&header->sampleFrequence, 4, 1, file);
    fread(&header->byteRate, 4, 1, file);
    fread(&header->bytesPerBlock, 2, 1, file);
    fread(&header->bitsPerSample, 2, 1, file);
    if (header->byteRate != header->sampleFrequence * (header->numChannels * header->bitsPerSample / 8)) {
        printf("\033[31mError\033[0m: byteRate is incorrect\n");
        return 0;
    }
    fread(header->dataChunkID, 1, 4, file);
    fread(&header->dataSize, 4, 1, file);
    header->dataChunkID[4] = '\0';
    if (strcmp(header->dataChunkID, "data") == 0) {
        return 1;
    }else{
        if(strcmp(header->dataChunkID, "LIST") == 0){
            strcpy(header->LISTID, header->dataChunkID);
            header->LISTSize = header->dataSize;
        }
        header->metadata = (char *)malloc(header->dataSize);
        if (header->metadata == NULL){
            printf("\033[31mError\033[0m: metadata not allocated\n");
            return 0;
        }
        while (ftell(file) < header->dataSize+45){
            int id;
            char metadataID[5] = {0};
            int metadataSize;
            fread(metadataID, 1, 1, file);

            if (metadataID[0] == 'd') {
                char temp[4];
                fread(temp, 1, 3, file);
                temp[3] = '\0';
                strcat(metadataID, temp);
                strcpy(header->dataChunkID, metadataID);
                fread(&header->dataSize, 4, 1, file);
                break;
            }
            
            if (metadataID[0] == 'I') {
                char temp[4];
                fread(temp, 1, 3, file);
                temp[3] = '\0';
                strcat(metadataID, temp);
                id = getMetadata(metadataID);
                switch (id){
                    case INAM:
                        readMetadata(file, header,"INAM");
                        break;
                    case IART:
                        readMetadata(file, header,"IART");
                        break;
                    case ICRD:
                        readMetadata(file, header,"ICRD");
                        break;
                    case ICMT:
                        readMetadata(file, header,"ICMT");
                        break;
                    case IGNR:
                        readMetadata(file, header,"IGNR");
                        break;
                    case IKEY:
                        readMetadata(file, header,"IKEY");
                        break;
                    case INFO:
                        strcpy(header->LISTType, "INFO");
                        break;
                    case ISFT:
                        readMetadata(file, header,"ISFT");
                        break;
                    default:
                        printf("\033[31mError\033[0m: metadata ID not found\n");
                        return 0;
                }
            }       
        }
        int metadataSizes[7] = {0};
        short metadataSizesLen = 1;
        char *metadataCopy = strdup(header->metadata);
        char *token = strtok(metadataCopy, "^");
        int i = 0;
        while (token != NULL){
            metadataSizes[i] = strlen(token) - 4;
            token = strtok(NULL, "^");
            i++;
            metadataSizesLen++;
        }
        header->metadataSizesLen = metadataSizesLen;
        header->metadataSizes = (int *)calloc(metadataSizesLen, sizeof(int));
        for (int i = 0; i < metadataSizesLen; i++){
            header->metadataSizes[i] = metadataSizes[i];
        }
    }
    return 1;
}


void printHeader(WAVHeader header){
    printf("\033[33mRIFFID: %s\n", header.RIFFID);
    printf("fileSize: %d\n", header.fileSize);
    printf("fmtChunkType: %s\n", header.fmtChunkType);
    printf("formatChunkID: \"%s\"\n", header.formatChunkID);
    printf("formatChunkSize: %d\n", header.formatChunkSize);
    printf("formatType: %d\n", header.formatType);
    printf("numChannels: %d\n", header.numChannels);
    printf("sampleFrequence: %d\n", header.sampleFrequence);
    printf("byteRate: %d\n", header.byteRate);
    printf("bytesPerBlock: %d\n", header.bytesPerBlock);
    printf("bitsPerSample: %d\n", header.bitsPerSample);
    printf("dataChunkID: \"%s\"\n", header.dataChunkID);
    printf("dataSize: %d\n", header.dataSize);
    if (strcmp(header.LISTID, "NULL")){
        printf("LISTID: %s\n", header.LISTID);
        printf("LISTSize: %d\n", header.LISTSize);
    }
    if (strcmp(header.LISTType, "NULL")){
        printf("INFOID: %s\n", header.LISTType);
    }
    if (header.metadata != NULL){
        char *metadataCopy = strdup(header.metadata);
        char metadateType[5];
        char *token = strtok(metadataCopy, "^");
        while (token != NULL){
            strncpy(metadateType, token, 4);
            printf("metadata: %s\n", metadateType);
            token += 4;
            printf("Metadata [%s]: %s\n", metadateType, token);
            token = strtok(NULL, "^");
            if (token == NULL) break;
        }
        free(metadataCopy);
    }
    printf("\033[0m");
}

int copyHeader(WAVHeader header, WAVHeader *newHeader){
    /*
    do not copy metadata
    */
    
    newHeader->metadata = NULL;
    strcpy(newHeader->LISTType, "NULL");
    strcpy(newHeader->LISTID, "NULL");
    if (strcmp(header.RIFFID, "RIFF")) {
        printf("\033[31mError\033[0m: header does not exist\n");
        return 0;
    }

    newHeader->fileSize = header.fileSize;
    newHeader->formatChunkSize = header.formatChunkSize;
    newHeader->formatType = header.formatType;
    newHeader->numChannels = header.numChannels;
    newHeader->sampleFrequence = header.sampleFrequence;
    newHeader->byteRate = header.byteRate;
    newHeader->bytesPerBlock = header.bytesPerBlock;
    newHeader->bitsPerSample = header.bitsPerSample;
    newHeader->dataSize = header.dataSize;
    strcpy(newHeader->RIFFID, header.RIFFID);
    strcpy(newHeader->fmtChunkType, header.fmtChunkType);
    strcpy(newHeader->formatChunkID, header.formatChunkID);
    strcpy(newHeader->dataChunkID, header.dataChunkID);
    return 1;
}


int readWAV(char *filePath, WAVFile *wavFile){
    FILE *file = fopen(filePath, "rb");
    if(file == NULL){
        printf("\033[31mError\033[0m: file not found\n");
        fclose(file);
        return 0;
    }
    wavFile->filePath = filePath;
    WAVHeader *header = (WAVHeader *)malloc(sizeof(WAVHeader));
    if (header == NULL){
        printf("\033[31mError\033[0m: header not allocated\n");
        fclose(file);
        free(header);
        return 0;
    }
    wavFile->header = header;
    if(!initHeader(header, file)){
        fclose(file);
        return 0;
    }
    wavFile->data = (short *)calloc(wavFile->header->dataSize/sizeof(short), sizeof(short));
    if (wavFile->data == NULL){
        printf("\033[31mError\033[0m: data not allocated\n");
        fclose(file);
        free(header);
        return 0;
    }
    for (int i = 0; i < wavFile->header->dataSize/sizeof(short); i++)
    {
        fread(&wavFile->data[i], sizeof(short), 1, file);
    }
    
    wavFile->duration = header->dataSize / header->byteRate;
    fclose(file);
    return 1;
}

int writeWAV(char *newFilePath, WAVFile *wavFile) {
    FILE *file = fopen(newFilePath, "wb");
    if (file == NULL) {
        printf("\033[31mError\033[0m: cannot open file for writing\n");
        return 0;
    }

    fwrite(wavFile->header->RIFFID, 1, 4, file);
    fwrite(&wavFile->header->fileSize, 4, 1, file);
    fwrite(wavFile->header->fmtChunkType, 1, 4, file);
    fwrite(wavFile->header->formatChunkID, 1, 4, file);
    fwrite(&wavFile->header->formatChunkSize, 4, 1, file);
    fwrite(&wavFile->header->formatType, 2, 1, file);
    fwrite(&wavFile->header->numChannels, 2, 1, file);
    fwrite(&wavFile->header->sampleFrequence, 4, 1, file);
    fwrite(&wavFile->header->byteRate, 4, 1, file);
    fwrite(&wavFile->header->bytesPerBlock, 2, 1, file);
    fwrite(&wavFile->header->bitsPerSample, 2, 1, file);
    if (strcmp(wavFile->header->LISTID, "NULL")){
        fwrite(wavFile->header->LISTID, 1, 4, file);
        fwrite(&wavFile->header->LISTSize, 4, 1, file);
        fwrite(wavFile->header->LISTType, 1, 4, file);

        char *metadataCopy = strdup(wavFile->header->metadata);
        char *token = strtok(metadataCopy, "^");
        for (int i = 0; i < wavFile->header->metadataSizesLen && token != NULL; i++) {
            char metadataID[5] = {0};
            strncpy(metadataID, token, 4);
            fwrite(metadataID, 1, 4, file);
            fwrite(&wavFile->header->metadataSizes[i], 4, 1, file);
            fwrite(token + 4, 1, wavFile->header->metadataSizes[i], file);
            token = strtok(NULL, "^");
        }
        free(metadataCopy);
    }

    fwrite(wavFile->header->dataChunkID, 1, 4, file);
    fwrite(&wavFile->header->dataSize, 4, 1, file);
    for (int i = 0; i < wavFile->header->dataSize/sizeof(short); i++) {
        fwrite(&wavFile->data[i], sizeof(short), 1, file);
    }
    printf("Data written\n");
    fclose(file);
    return 1;
}

void invertSamples(WAVFile *wavFile){
    if(wavFile->header->numChannels != 2){
        printf("\033[31mError\033[0m: WAV file is not stereo\n");
        return;
    }
    printf("Inverting samples\n");      
    short temp;
    for (int i = 0; i < wavFile->header->dataSize/sizeof(short); i+=2){
        temp = wavFile->data[i];
        wavFile->data[i] = wavFile->data[i+1];
        wavFile->data[i+1] = temp;
    }
}

int copyWAVFile(WAVFile wavFile, WAVFile *newWavFile){
    if(!copyHeader(*wavFile.header, newWavFile->header)) return 0;
    printf("Copying WAV file\n");
    newWavFile->duration = wavFile.duration;
    newWavFile->data = (short *)calloc(wavFile.header->dataSize/sizeof(short), sizeof(short));
    if (newWavFile->data == NULL){
        printf("\033[31mError\033[0m: data not allocated\n");
        return 0;
    }
    memcpy(newWavFile->data, wavFile.data, wavFile.header->dataSize);
    return 1;
}

void printWAVFileInfo(WAVFile wavFile){
    printHeader(*wavFile.header);
    printf("Duration: %d seconds\n", wavFile.duration);
    printf("\n");
}

void invertSignal(WAVFile *wavFile){
    printf("Inverting signal\n");
    for (int i = 0; i < wavFile->header->dataSize/sizeof(short); i++){
        wavFile->data[i] = -wavFile->data[i];
    }
}

void addNoise(WAVFile *wavFile, int noiselevel){
    printf("Inverting signal\n");
    int noise;
    for (int i = 0; i < wavFile->header->dataSize/sizeof(short); i++){
        noise = rand() % noiselevel;
        while (noise + wavFile->data[i] > 32767 || noise + wavFile->data[i] < -32768)
        {
            noise = rand() % noiselevel;
        }
        
        wavFile->data[i] = wavFile->data[i] + noise;
    }
}