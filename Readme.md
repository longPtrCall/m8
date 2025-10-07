# M8

M8 is a tiny build system written in C, compliant with a C99 standard.

## Quick Start

Copy this header (m8.h) to your repository root. Create a C file, m8.c and paste following code:
```C
#include "m8.h"

int main(const int argc, char* const argv[]) {

    const char* const sources[] = { "main.c" };
    output = "hello"_executable;
    return m8_main(argv, argv, enumerate(sources), enumerate(default_build_commands));
}
```

Compile and run:
```Shell
cc -o m8 m8.c && ./m8
```

You're done so far. For more examples see the respective directory.
