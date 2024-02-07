#include "callpath.hxx"

void funcC() {
    CallPath::isUnique();
}

void funcB() {
    CallPath::isUnique();
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
    cout << CallPath::callPath();
    return 0;
}
