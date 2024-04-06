/*
 * Author: Christian Chuba
 * LinkedIn: https://www.linkedin.com/in/christian-chuba-32a3331/
 * Compile with ... g++ -rdynamic -lbacktrace  -g, to get meaningful symbols
 *
 * Description: bool CallPath::isUnique()
 *              This tracks all unique call paths to a given function.
 *              It is useful for learning new code and error insertion testing.
 * Usage:
 * CallPath::isUnique() - returns true the first time it sees this call stack.
 *              It returns false one subsequent visits.
 *
 * void funcA() {
 *    if (CallPath::isUnique()) {
 *       // isUnique() returns true for every unique caller.  It returns false
 *       // subsequent visits from the same location.
 *
 * License: This code is provided under the MIT License.
 *          You are free to use, modify, and distribute this code for any purpose,
 */
#ifndef _CALLPATH_HXX__
#define _CALLPATH_HXX__  1

#include <execinfo.h>
#include <cstdlib>
#include <cassert>
#include <backtrace.h>
#include <map>
#include <mutex>
#include <string.h>
using namespace std;


// class CallPath
// A singleton to record every unique call path to a given function.
// Usage:
// isUnique() returns 'true' every time it detects a unique call path.
// It stores the hash callers on a list to detect first visit.
//
class CallPath {
public:
    // return true on first visit from this call stack
    static bool isUnique(std::string&  result);

private:
    CallPath() {}                     // to enforce singleton

    // returns the singleton object
    static CallPath&  callPath();

    static CallPath*   mCallPath;     // the singleton created on first access
    static std::mutex  mMutex;

    // mCallList, records previous visits to filter out redundant calls
    // the first value is the hash of the call stack
    // the second value is a counter of visits with the same hash
    std::map<uint64_t, uint64_t>  mCallList;

    // Calculate 64 bit hash from call stack and record it on list if it is unique
    std::size_t  callerHash(const void* data, std::size_t size);
};
CallPath*  CallPath::mCallPath = nullptr;
std::mutex CallPath::mMutex;

CallPath&
CallPath::callPath(void)
{
    if (mCallPath == nullptr) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mCallPath == nullptr) {
            mCallPath = new CallPath;  // Create singleton
        }
    }
    return *mCallPath;
}

// isUnique() returns true for every unique call path to the calling function.
//
// It prints the first occurrence of every call path to cout.  It saves the hash
// of that call path in mCallList to filter out duplicates.
// It excludes itself from the call stack but includes the calling function.
bool
CallPath::isUnique(std::string&  result)
{
    CallPath&  mThis = CallPath::callPath();  // get singleton object

    const int max_frames = 8;
    void* frame_ptrs[max_frames];
    result.reserve(1024); // 1024 is a hint, not a hard limit

    int num_frames = backtrace(frame_ptrs, max_frames);
    size_t  hash = mThis.callerHash(frame_ptrs, num_frames);
    {
        std::lock_guard<std::mutex> lock(mMutex);

        uint64_t  count = mThis.mCallList[hash] + 1;
        mThis.mCallList[hash] = count;
        if (count > 1) {
            return false;   // We have seen this call stack before, ignore it
        }
    }
    // Compile with ...  g++ -rdynamic -lbacktrace  -g, for meaningful symbols

    // New call stack, print out the call stack symbols and return true
    // Copy entire print string into a buffer so that we can list
    // all of the frames in one cout << buf, command.
    char** symbols = backtrace_symbols(frame_ptrs, num_frames);

    std::string  temp;
    if (symbols != nullptr) {
        // Start at '1' to skip this stack frame and star with the immediate caller
        for (int i = 1; i < num_frames; ++i) {

            std::string frame = symbols[i];
            size_t  pos = frame.find('\[');
            if (pos != std::string::npos) {
                frame.resize(pos);  // frame[pos] = '\0', will not change size
            }
            result.append(frame + "\n");
        }
        free(symbols);
    }
    return true;
}

// callerHash(), a utility method that returns the hash from the array of void*
// callers to the calling function.
std::size_t
CallPath::callerHash(const void* data, std::size_t size) {

    // Calculate the # of 64 bit entries in the array of void* (should be the same)
    size *=  sizeof(void*) / sizeof(uint64_t);
    assert(size != 0);

    uint64_t*  data64 = (uint64_t*)data;

    // Using 'std::hash' because it is installed by default on ubuntu Linux
    std::hash<uint64_t> hasher;
    std::size_t hash = 0;
    for (std::size_t i = 0; i < size; ++i) {
        // Simple mixing function (from ChatGPT)
        hash ^= hasher(data64[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}
#endif
