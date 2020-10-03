#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define TAU 6.28318530717958647692


typedef struct {
    uint16_t frequency;
    uint32_t samples;
    float data[];
} AudioBuffer32;


float compute_sin_sample(uint16_t frequency, uint64_t sample, float amplitude) {
    return sinf((float) sample / (float) frequency * TAU * amplitude); 
}


AudioBuffer32* audio_buffer_32_create(uint16_t frequency, uint32_t samples) {
    AudioBuffer32* buffer = calloc(1, sizeof(AudioBuffer32));
    buffer->frequency = frequency;
    buffer->samples = samples;
    return buffer;
}


AudioBuffer32* audio_buffer_32_init(AudioBuffer32* buffer) {
    for (int i = 0; i < buffer->samples; i ++) {
        buffer->data[i] = 0.0; 
    }
}


AudioBuffer32 audio_buffer_32_sin(uint16_t frequency, AudioBuffer32* buffer) {
    for (int i = 0; i < buffer->samples; i ++) {
        buffer->data[i] = compute_sin_sample(frequency, i, 1.0);
    }
}


void audio_buffer_32_print(AudioBuffer32* buffer) {
    for (int i = 0; i < buffer->samples; i++) {
            printf("Sample %d: %f\n", i, buffer->data[i]);
    }
}


int main() {
    AudioBuffer32* testBuffer = audio_buffer_32_create(48000, 48000);
    printf("Hello.");
    audio_buffer_32_sin( 440.0, testBuffer);
    audio_buffer_32_print(testBuffer);
}