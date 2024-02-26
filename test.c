#include "./src/noe.h"
#include "./src/vendors/glad/include/glad/glad.h"

int main(void)
{
    SetWindowConfig(800, 600, "My Window", WINDOW_FLAG_VISIBLE);

    if(!InitApplication()) return -1;

    gladLoadGL();

    while(!WindowShouldClose()) {
        PollInputEvents();
        if(IsKeyDown(KEY_W)) TRACELOG(LOG_INFO, "KEY_W is pressed");
        if(IsKeyDown(KEY_A)) TRACELOG(LOG_INFO, "KEY_A is pressed");
        if(IsKeyDown(KEY_S)) TRACELOG(LOG_INFO, "KEY_S is pressed");
        if(IsKeyDown(KEY_D)) TRACELOG(LOG_INFO, "KEY_D is pressed");

        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SwapGLBuffer();
    }
    
    DeinitApplication();
}
