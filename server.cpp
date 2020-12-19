#include "server.h"

// g = global
CYsServer g_Server; 

void initServer(void) {
    char* bindaddr = {"192.168.2.3"};
    g_Server.szBindaddr = bindaddr;
    //memcpy(g_Server.szBindaddr,bindaddr,sizeof(bindaddr));
    g_Server.nPort = 4088;

    g_Server.sEc 
}
int main(int argc, const char** argv) {
    return 0;
}