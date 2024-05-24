#include <windows.h>

int main() {
    // Command to launch Chrome with the URL
    const char* command = "start chrome https://www.youtube.com";

    // Execute the command
    system(command);

    return 0;
}