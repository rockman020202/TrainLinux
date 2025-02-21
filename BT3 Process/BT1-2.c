#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) 
{    
    pid_t child_pid;
    int status, reVal;
    int numb = atoi(argv[1]);

    child_pid = fork();
    if ( 0 == child_pid ) 
    {   
        /* Process con */
        if ( numb == 1 )
        {
            execlp("ls", "ls", "-l", NULL); /*Nếu tiến trình này được gọi thành công thì các dòng lệnh sau sẽ không được thực hiện*/
        }
        else if ( numb == 2 )
        {
            execlp("date", "date", NULL); /*Nếu tiến trình này được gọi thành công thì các dòng lệnh sau sẽ không được thực hiện*/
        }
        printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
    }
    else 
    {           
        /* Parent process vẫn chạy bình thường */
        reVal = wait(&status);
        if ( reVal == -1 ) {
            printf("wait() unsuccessful\n");
        }
        printf("\nIm the parent process, PID child process: %d\n", reVal);
        printf("My PID is: %d\n", getpid());
    }
    return 0;   
}