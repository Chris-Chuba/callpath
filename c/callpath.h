/*
 * Author: Christian Chuba
 * LinkedIn: https://www.linkedin.com/in/christian-chuba-32a3331/
 * Compile with ... g++ -rdynamic -lbacktrace  -g, to get meaningful symbols
 *          (see Makefile)
 *
 * Description: This is the standard C implementation of callpath.hxx
 *         This version has NO C++ syntax and is suitable for lower level projects
 *         I hae a C implementation to replace standard templat libraries for
 *         the hash map and std::string
 *
 * License: This code is provided under the MIT License.
 *          You are free to use, modify, and distribute this code for any purpose,
 */
#ifndef _CALLPATH_H__
#define _CALLPATH_H__  1

#include <execinfo.h>
#include <stdlib.h>
#include <backtrace.h>
#include <string.h>
#include <pthread.h>

int CallPath_isUnique(char* result, size_t resultMax);
int CallPath_isUniqueInternal(void*  frame);

// number of unique call paths we can support
#define CP_MAX_SLOTS  100

// CallPath_isUniqueInternal()
// This replaces std::map and std::hasher by maintaining a simple array with
// linear searching, good enough for test code.
//
// Returns:  1, if this is the first visit from the caller's return address
//           0, to indicate subsequent visits
int CallPath_isUniqueInternal(void*  frame)
{
    static int              slot = 0;
    static void*            callers[CP_MAX_SLOTS];
    static pthread_mutex_t  mutex;

    if (slot > 0) {
        // try a fast test to see if 'frame' is already present
        // without acquiring a lock

        for (int i = 0; i < slot; i++) {
            if (frame == callers[i]) {
                return 0;   // false
            }
        }
    }
    if (slot == 0) {
        // This is thread safe, it's a noop after 1 call
        pthread_mutex_init(&mutex, NULL);
    }

    int   result = 1; // default to 'true'
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < slot; i++) {
        if (frame == callers[i]) {
            result = 0;
            break;
        }
    }
    if (slot == CP_MAX_SLOTS) {
        result = 0;   // no more room
    }
    if (result) {
        callers[slot++] = frame;
    }
    pthread_mutex_unlock(&mutex);
    return result;
}

// CallPath_isUnique()
//
// Returns: 1, if this is the first visit from the caller's return address
//          0, to indicate subsequent visits
//          I would rather use 'bool' but that's not part of standard C

int CallPath_isUnique(char* result, size_t resultMax)
{
    size_t   resultLen = 0;
    const int max_frames = 8;
    void* frame_ptrs[max_frames];

    strcpy(result, "*****");

    int num_frames = backtrace(frame_ptrs, max_frames);

    // Compile with ...  g++ -rdynamic -lbacktrace  -g, for meaningful symbols

    // New call stack, print out the call stack symbols and return true
    // Copy entire print string into a buffer so that we can list
    // all of the frames in one cout << buf, command.
    char** symbols = backtrace_symbols(frame_ptrs, num_frames);

    if (!CallPath_isUniqueInternal(frame_ptrs[2])) {
        free(symbols);
        return 0;   // false
    }

    if (symbols != NULL) {
        // Start at '1' to skip this stack frame and star with the immediate caller
        for (int f = 1; f < num_frames; f++) {

            char* frame = symbols[f];
            char* pos = strchr(frame, '[');
            if (pos != NULL) {
                pos[0] = '\0';
            }
            // printf("frame[%d]=%s\n", f, frame);
            size_t frameLen = strlen(frame);
            if ((frameLen + resultLen + 1) > resultMax) {
                printf("***\n");
                break;
            }
            strcpy(result + resultLen, frame);
            resultLen += frameLen + 1;
            result[resultLen - 1] = '\n';
        }
        result[resultLen] = '\0';
        free(symbols);
    }
    return 1;  // true
}
#endif
