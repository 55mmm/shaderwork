#include "Main.h"
#include "App.h"

#include <iostream>
#include <stdexcept>
/*
#define NOMINMAX
#include <Windows.h>
*/
int main(int argc, char* argv[])
{
    try
    {
        App* app = new App;
        app->MainLoop();
        delete app;
    } catch (std::exception& e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        // MessageBoxA(NULL, e.what(), "Runtime error", MB_OK | MB_ICONINFORMATION);
        return 1;
    }
    return 0;
}
