#include "./src/noe.h"

int main(void)
{
    if(!InitApplication()) return -1;

    while(!WindowShouldClose()) {
        PollInputEvents();
    }
    
    DeinitApplication();
}
