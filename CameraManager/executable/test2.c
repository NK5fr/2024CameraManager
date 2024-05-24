#include <windows.h>

int main() {
    // Command to launch Chrome with the URL
    const char* command = "start chrome https://www.twitter.com";
    
    // Execute the command
    system(command);

    return 0;
}