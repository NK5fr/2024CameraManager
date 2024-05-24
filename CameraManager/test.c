#include <windows.h>

int main() {
    // URL to open
    const char* url = "https://www.google.com";

    // Command to launch Chrome with the URL
    const char* command = "start chrome https://www.google.com";

    // Execute the command
    system(command);

    return 0;
}