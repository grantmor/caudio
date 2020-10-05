#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>


#define TAU 6.28318530717958647692


typedef struct {
    uint16_t frequency;
    uint64_t samples;
    int16_t* data;
} AudioBuffer16;


typedef struct {
    uint32_t chunkId;
    uint32_t chunkSize;
    uint32_t format;
} WavFileHeader;


typedef struct {
    uint32_t subChunk1Id;
    uint32_t subChunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitDepth;
    // extraParamSize unnecessary for PCM
    // extraParams unnecessary for PCM
} WavFmtChunk;


typedef struct {
    uint32_t subChunk2Id;
    uint32_t subChunk2Size;
} WavDataChunk;


typedef struct {
    WavFileHeader header;
    WavFmtChunk fmtChunk;
    WavDataChunk dataChunk;
    int16_t* data;
} WavFileInt;


WavFileInt* wav_file_create(WavFileHeader header, WavFmtChunk fmtChunk, WavDataChunk dataChunk, uint8_t* audioData) {

}


bool is_system_big_endian () {
    uint32_t test = 0xFF00000000;
    uint8_t leftValue = (uint8_t) (test & 0xFF000000);
    return (leftValue > 0);
}


uint32_t reverse_endianness_uint32_t ( uint32_t value) {
    const uint8_t bitsPerByte = 8;

    const uint32_t rightByte = (value & 0xFF000000);
    const uint32_t midRightByte = (value & 0x00FF0000);
    const uint32_t midLeftByte = (value & 0x0000FF00);
    const uint32_t leftByte = (value & 0x000000FF);    
    
    return ( ( rightByte >> ( 3 * bitsPerByte) ) | (midRightByte >> (1 * bitsPerByte) ) | (midLeftByte << (1 * bitsPerByte) ) | (leftByte << (3 * bitsPerByte) ) );
}

uint32_t reverse_endianness_uint16_t ( uint16_t value) {
    const uint8_t bitsPerByte = 8;

    const uint16_t rightByte = (value & 0xFF00);
    const uint16_t leftByte = (value & 0x00FF);    
    
    return ( (rightByte >> (bitsPerByte) ) | (leftByte << (bitsPerByte)) );
}


WavFileHeader wav_file_header_create(uint32_t dataSize) {

    // add support for big endian {RIFX} (pass enum as parameter)
    
    uint32_t chunkId = 0x52494646; //RIFF in ASCII (big endian)
    chunkId = is_system_big_endian() ? chunkId : reverse_endianness_uint32_t(chunkId);
    
    // uint32_t chunkSize = fileSize - sizeof(chunkId) + sizeof(chunkSize); 
    uint32_t chunkSize = dataSize;
    chunkSize = is_system_big_endian() ? chunkSize : reverse_endianness_uint32_t(chunkSize);

    uint32_t format = 0x57415645; // WAVE in ASCII (big endian)
    format = is_system_big_endian() ? format : reverse_endianness_uint32_t(format);

    WavFileHeader header = {
        .chunkId = chunkId,
        .chunkSize = chunkSize, 
        .format = format
    };

    return header;
}


WavFmtChunk wav_format_chunk_create(uint16_t numChannels, uint32_t sampleRate, uint16_t bitDepth) {
    uint32_t subChunk1Id = 0x666d7420; // "fmt " in ASCII
    uint16_t audioFormat = 1; // endianness
    uint16_t subChunkSize = 16; // 16 for PCM

    uint32_t byteRate = (uint32_t) (sampleRate * numChannels * bitDepth / 8);
    uint16_t blockAlign = (uint16_t) (numChannels * bitDepth / 8);

    subChunk1Id = is_system_big_endian() ? subChunk1Id : reverse_endianness_uint32_t(subChunk1Id); 
    subChunkSize = is_system_big_endian() ? subChunkSize : reverse_endianness_uint32_t(subChunkSize); 
    audioFormat = is_system_big_endian() ? audioFormat : reverse_endianness_uint16_t(audioFormat); 
    numChannels = is_system_big_endian() ? numChannels : reverse_endianness_uint16_t(numChannels); 
    sampleRate = is_system_big_endian() ? sampleRate : reverse_endianness_uint32_t(sampleRate); 
    byteRate = is_system_big_endian() ? byteRate : reverse_endianness_uint32_t(byteRate);
    blockAlign = is_system_big_endian() ? blockAlign : reverse_endianness_uint16_t(blockAlign); 
    bitDepth = is_system_big_endian() ? bitDepth : reverse_endianness_uint16_t(bitDepth); 

    WavFmtChunk fmtChunk = {
        .subChunk1Id = subChunk1Id,
        .subChunk1Size = subChunkSize,
        .audioFormat = audioFormat, // endianness
        .numChannels  = numChannels,
        .sampleRate = sampleRate,
        .byteRate = byteRate,
        .blockAlign = blockAlign,
        .bitDepth =  bitDepth
    };

    return fmtChunk;
}


WavDataChunk wav_data_chunk_create(uint64_t samples, uint16_t bitDepth) {

    uint32_t subChunk2Id = 0x64617461; // "data"
    uint32_t subChunk2Size = (uint32_t) (samples * bitDepth / 8); 

    subChunk2Id = is_system_big_endian() ? subChunk2Id : reverse_endianness_uint32_t(subChunk2Id);
    subChunk2Size = is_system_big_endian() ? subChunk2Size : reverse_endianness_uint32_t(subChunk2Size);

    WavDataChunk dataChunk = {
        .subChunk2Id =  subChunk2Id,
        .subChunk2Size = subChunk2Size
    };

    return dataChunk;
} 


AudioBuffer16* audio_buffer_16_create(uint16_t frequency, uint64_t samples) {
    AudioBuffer16* buffer = calloc(1, sizeof(AudioBuffer16));
    buffer->frequency = frequency;
    buffer->samples = samples;
    buffer->data = calloc(samples, sizeof(int16_t));
    return buffer;
}


AudioBuffer16* audio_buffer_16_init(AudioBuffer16* buffer, uint64_t samples) {
    for (int i = 0; i < samples; i ++) {
        buffer->data[i] = 0; 
    }

    return buffer;
}


int16_t compute_sin_sample_16(uint16_t frequency, uint64_t sample, int16_t amplitude) {
    return (int16_t) amplitude * sin((double)sample / (double) frequency * TAU);
}


AudioBuffer16* audio_buffer_16_sin(uint16_t frequency, AudioBuffer16* buffer) {
    for (uint64_t i = 0; i < buffer->samples; i++) {
        buffer->data[i] = compute_sin_sample_16(frequency, i, INT16_MAX);
    }

    return buffer;
}


void audio_buffer_16_print(AudioBuffer16* buffer) {
    for (int i = 0; i < buffer->samples; i++) {
            printf("Sample %d: %d\n", i, buffer->data[i]);
    }
}


WavFileInt* wav_file_16_create(AudioBuffer16* buffer, uint16_t numChannels) {
    uint64_t sampleRate = buffer->frequency;
    uint16_t bitDepth = 16;
    uint8_t bytesPerBit = 8;

    uint32_t subChunk2Size = (uint32_t) (buffer->samples * numChannels * bitDepth / bytesPerBit);

    WavFmtChunk fmtChunk = wav_format_chunk_create(numChannels, sampleRate, bitDepth);
    WavDataChunk dataChunk = wav_data_chunk_create(sampleRate, bitDepth);
    WavFileHeader headerChunk = wav_file_header_create(subChunk2Size);

    WavFileInt* wavFilePtr = calloc(1, sizeof(WavFileInt));

    WavFileInt wavFile = {
        .header = headerChunk,
        .fmtChunk = fmtChunk,
        .dataChunk = dataChunk,
        .data = buffer->data
    };

    wavFilePtr = &wavFile;
    return wavFilePtr;
}


uint8_t wav_file_write(WavFileHeader header, WavFmtChunk fmtChunk, WavDataChunk dataChunk, AudioBuffer16* buffer) {
   // only variable length is the data 
    FILE* file;
    fopen_s(&file, "sin.wav", "wb");
    if (file == NULL) {
        return 1;
    } else {
        fwrite(&header, sizeof(header), 1, file);
        fwrite(&fmtChunk, sizeof(fmtChunk), 1, file);
        fwrite(&dataChunk, sizeof(dataChunk), 1, file);
        printf("data element size in bytes: %d\n", sizeof(buffer->data[0]));
        printf("buffer->samples: %d\n", buffer->samples);

        size_t bytes_written = fwrite(buffer->data, sizeof(buffer->data[0]), buffer->samples, file);

        printf("bytes written: %d\n", bytes_written);
    }

    fclose(file);
}


int main() {
    uint64_t numSamples = 24000; 
    uint16_t sampleRate = 48000;
    uint16_t noteFrequency = 440.0; 
    uint16_t bitDepth = 16;
    uint16_t numChannels = 1;
   
    AudioBuffer16* testBuffer = calloc(1, sizeof(AudioBuffer16)); // Add fields to AudioBuffer16? 
    
    testBuffer = audio_buffer_16_create(sampleRate, numSamples); 
    testBuffer = audio_buffer_16_sin(noteFrequency, testBuffer); 

    // WavFileInt* wavFile = wav_file_16_create(testBuffer, 1); 
    audio_buffer_16_print(testBuffer);

    //uint32_t dataSize = sizeof(WavFileHeader) + sizeof(WavFmtChunk) + sizeof(WavDataChunk) + ( numSamples * sizeof(uint16_t) );
    uint32_t dataSize = 36 + ( sizeof(uint16_t) * numSamples * numChannels );

    WavFileHeader header = wav_file_header_create(dataSize); 
    WavFmtChunk fmtChunk = wav_format_chunk_create(sampleRate, numChannels, bitDepth);
    WavDataChunk dataChunk = wav_data_chunk_create(numSamples, bitDepth);

    wav_file_write(header, fmtChunk, dataChunk, testBuffer);

    free(testBuffer);

    return 0;
}