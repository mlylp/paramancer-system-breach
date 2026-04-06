#ifndef TIPOS_H
#define TIPOS_H

#define MAX_GUESSES 100

typedef struct Session
{
    char timestamp[32];
    int target;
    int attempts_count;
    int guesses[MAX_GUESSES];
    int low_count;
    int high_count;
} Session;

typedef struct Stats
{
    double average_attempts;
    int best_attempts;
    int worst_attempts;
    double stddev_attempts;
    double avg_low_bias;
    double avg_high_bias;
} Stats;

#endif