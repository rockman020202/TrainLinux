#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int sig) 
{
    if (sig == SIGUSR1) 
    {
        printf("Tiến trình con nhận được tín hiệu SIGUSR1!\n");
    }
}

int main() {
    pid_t pid = fork();
    
    if (pid < 0) 
    {
        perror("Fork error");
        exit(1);
    }
    
    if (pid == 0) 
    {
        signal(SIGUSR1, signal_handler);
        printf("Child process waiting signal\n");
        while (1) 
        {
            pause();
        }
    }
    else 
    { 
        sleep(2); /
        printf("Send signal to chill process (PID: %d)\n", pid);
        kill(pid, SIGUSR1); 
        sleep(1);
    }
    
    return 0;
}