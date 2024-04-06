/*
 * Author: Christian Chuba
 * LinkedIn: https://www.linkedin.com/in/christian-chuba-32a3331/
 * Compile with ... g++ -rdynamic -lbacktrace  -g, to get meaningful symbols
 *          (see Makefile)
 *
 * Description: Gives a sample usage of callpath.hxx, by having two code paths
 *          call the same functions 100 times to show that isUnique() will
 *          only trigger twice.
 *
 * License: This code is provided under the MIT License.
 *          You are free to use, modify, and distribute this code for any purpose,
 */

#include "callpath.hxx"
#include <iostream>

void funcC() {
    std::string   result;
    if (CallPath::isUnique(result)) {
        cout << result << endl;
    }
}

void funcB() {
    std::string   result;
    if (CallPath::isUnique(result)) {
        cout << result << endl;
    }
}

void funcA() {
    funcB();
}

int main() {
    for (int i = 0; i < 100; i++) {
        funcA();  // will only print call stack once
        funcC();  // will only print call stack once
    }
    /*
    Sample output:
    isUnique() 7 Frames = ./a.out(_Z5funcBv+0xd)
    ./a.out(_Z5funcAv+0xd)
    ./a.out(main+0x1a)
    /lib/x86_64-linux-gnu/libc.so.6(+0x29d90)
    /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80)
    ./a.out(_start+0x25)

    isUnique() 6 Frames = ./a.out(_Z5funcCv+0xd)
    ./a.out(main+0x1f)
    /lib/x86_64-linux-gnu/libc.so.6(+0x29d90)
    /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80)
    ./a.out(_start+0x25)
    */
    return 0;
}
