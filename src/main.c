#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>


#define TAU 6.28318530717958647692
#define FMT 0x666d7420


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
    uint16_t audioFormt;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsDepth;
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
} WavFile16;


WavFile16* wav_file_create(WavFileHeader header, WavFmtChunk fmtChunk, WavDataChunk dataChunk, uint8_t* audioData) {

}


bool is_uint32_t_big_endian (const uint32_t value) {
    if (value == 0) {
        // check to see if this is ok - should probably log this later
        exit(1);
    }

    const uint8_t leftValue = (uint8_t) (value & 0xFF000000);
    const uint8_t rightValue = (uint8_t) (value & 0x000000FF);

    return (leftValue >= rightValue);
}


uint32_t reverse_endianness_uint32_t ( uint32_t value) {
    const uint32_t rightByte = value & 0xFF000000;
    const uint32_t midRightByte = value & 0x00FF0000;
    const uint32_t midLeftByte = value & 0x0000FF00;
    const uint32_t leftByte = value & 0x000000FF;    
    
    return rightByte + midRightByte + midLeftByte + leftByte;
}


void wav_file_write() {}


WavFileHeader wav_file_header_create(uint32_t fileSize) {
    // add support for big endian {RIFX} (pass enum as parameter)
    uint8_t chunkId[] = {'R','I','F','F'};
    uint32_t chunkSize = 0;
    uint8_t format[] = {'W','A','V','E'};

    chunkSize = is_uint32_t_big_endian ? chunkSize: reverse_endianness_uint32_t(chunkSize);

    WavFileHeader header = {*chunkId, chunkSize, *format};

    return header;
}


WavFmtChunk wav_format_chunk_create() {
    
}


WavDataChunk wav_data_chunk_create() {

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


int main() {
    AudioBuffer16* testBuffer = calloc(1, sizeof(AudioBuffer16));

    testBuffer = audio_buffer_16_create(48000, 32);
    testBuffer = audio_buffer_16_sin( 440.0, testBuffer);
    audio_buffer_16_print(testBuffer);


    free(testBuffer);
    return 0;
}