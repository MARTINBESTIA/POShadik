//
// Created by marti on 3. 1. 2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "gameConfiguration.h"

static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n - 1] == '\n') s[n - 1] = '\0';
}

static int read_line(char *buf, size_t buflen) {
    if (!fgets(buf, buflen, stdin)) return 0;  // EOF / error
    return 1;
}

static int read_char_choice(const char *prompt, const char *choices, char *out) {
    char buf[128];

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!read_line(buf, sizeof buf)) return 0; // EOF

        // skip leading whitespace
        size_t i = 0;
        while (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n' || buf[i] == '\r') i++;

        if (buf[i] == '\0') {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        char c = buf[i];

        // ensure there isn't extra non-space junk after the character
        i++;
        while (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n' || buf[i] == '\r') i++;
        if (buf[i] != '\0') {
            printf("Invalid input. Please enter a single character.\n");
            continue;
        }

        if (strchr(choices, c) == NULL) {
            printf("Invalid input. Please try again.\n");
            continue;
        }

        *out = c;
        return 1;
    }
}

static int read_int_range(const char *prompt, int minv, int maxv, int *out) {
    char buf[128];

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!read_line(buf, sizeof buf)) return 0; // EOF
        trim_newline(buf);

        // skip leading whitespace
        char *p = buf;
        while (*p == ' ' || *p == '\t') p++;

        errno = 0;
        char *end = NULL;
        long v = strtol(p, &end, 10);

        if (p == end) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        // allow trailing whitespace only
        while (*end == ' ' || *end == '\t') end++;
        if (*end != '\0') {
            printf("Invalid input. Please enter only a number.\n");
            continue;
        }

        if (errno == ERANGE || v < minv || v > maxv) {
            printf("Invalid input. Please enter a number in range %d..%d.\n", minv, maxv);
            continue;
        }

        *out = (int)v;
        return 1;
    }
}

void fillGameConfiguration(game_conf_t* config) {
    char gameMode = 'S';         // 'S' standard, 'T' time-limit
    int timeLimit = 0;           // seconds (if gameMode == 'T')
    char gameField = 'N';        // 'N' no-obstacles, 'O' obstacles, 'C' custom
    char randomGeneration = 'N'; // 'Y' yes, 'N' no (if gameField == 'O')
    int fieldLengthX = 0;
    int fieldLengthY = 0;
    int gameFieldPath = 0;

    // 1) game mode
    if (!read_char_choice("Select game mode : 'S' standard,  'T' time-limit\n", "ST", &gameMode)) {
        // EOF -> choose defaults or exit; here we just exit.
        fprintf(stderr, "Input ended.\n");
        exit(1);
    }
    config->gameMode = gameMode;

    // 2) time limit
    if (gameMode == 'T') {
        if (!read_int_range("Enter time limit in seconds (1..3600): ", 1, 3600, &timeLimit)) {
            fprintf(stderr, "Input ended.\n");
            exit(1);
        }
        config->timeLimit = timeLimit;
    } else {
        config->timeLimit = 0;
    }

    // 3) field type
    if (!read_char_choice("Select game field type: 'N' no-obstacles, 'O' obstacles, 'C' custom\n", "NOC", &gameField)) {
        fprintf(stderr, "Input ended.\n");
        exit(1);
    }
    config->gameField = gameField;

    // 4) obstacle generation
    if (gameField == 'O') {
        if (!read_char_choice("Random obstacle generation? 'Y' yes, 'N' no\n", "YN", &randomGeneration)) {
            fprintf(stderr, "Input ended.\n");
            exit(1);
        }
        config->randomGeneration = randomGeneration;
    } else {
        // sensible default
        config->randomGeneration = 'N';
    }

    // 5) sizes or custom path
    if (gameField != 'C') {
        if (!read_int_range("Enter field length X (1..100): ", 1, 100, &fieldLengthX)) {
            fprintf(stderr, "Input ended.\n");
            exit(1);
        }
        if (!read_int_range("Enter field length Y (1..100): ", 1, 100, &fieldLengthY)) {
            fprintf(stderr, "Input ended.\n");
            exit(1);
        }

        config->fieldLengthX = fieldLengthX;
        config->fieldLengthY = fieldLengthY;
    } else {
        if (!read_int_range("Enter custom field (1..3): ", 1, 3, &gameFieldPath)) {
            fprintf(stderr, "Input ended.\n");
            exit(1);
        }
        config->customFieldPath = gameFieldPath;
    }
}
