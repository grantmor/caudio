#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


#define TAU 6.28318530717958647692
#define FMT 0x666d7420


typedef struct {
    uint16_t frequency;
    uint64_t samples;
    float* data;
} AudioBuffer32;


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
    float* data;
} WavFile;


WavFile* wav_file_create(WavFileHeader header, WavFmtChunk fmtChunk, WavDataChunk dataChunk, uint8_t* audioData) {

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


float compute_sin_sample(uint16_t frequency, uint64_t sample, float amplitude) {
    return sinf((float) sample / (float) frequency * TAU * amplitude); 
}


AudioBuffer32* audio_buffer_32_create(uint16_t frequency, uint64_t samples) {
    AudioBuffer32* buffer = calloc(1, sizeof(AudioBuffer32));
    buffer->frequency = frequency;
    buffer->samples = samples;
    buffer->data = calloc(samples, sizeof(float));
    return buffer;
}


AudioBuffer32* audio_buffer_32_init(AudioBuffer32* buffer, uint64_t samples) {
    for (int i = 0; i < samples; i ++) {
        buffer->data[i] = 0.0; 
    }
    return buffer;
}

// THIS SHOULD RETURN A REFERENCE
AudioBuffer32* audio_buffer_32_sin(uint16_t frequency, AudioBuffer32* buffer) {
    for (uint64_t i = 0; i < buffer->samples; i ++) {
        buffer->data[i] = compute_sin_sample(frequency, i, 1.0);
    }
    return buffer;
}


void audio_buffer_32_print(AudioBuffer32* buffer) {
    for (int i = 0; i < buffer->samples; i++) {
            printf("Sample %d: %f\n", i, buffer->data[i]);
    }
}


int main() {
    AudioBuffer32* testBuffer = calloc(1, sizeof(AudioBuffer32));
    
    testBuffer = audio_buffer_32_create(48000, 32);
    testBuffer = audio_buffer_32_sin( 440.0, testBuffer);
    audio_buffer_32_print(testBuffer);
    return 0;
}