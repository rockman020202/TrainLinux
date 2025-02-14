#include<stdio.h> 
#include<string.h> 
#include<unistd.h> 
#include<fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
  
int main (int argc,char * argv[]) 
{ 
    char *filename = argv[1]; 
    int numb = atoi(argv[2]);
    char mode = argv[3][0];
    char *message = argv[4];
    int fd;
  
    // assume foobar.txt is already created 
    if (mode == 'w') {
        // Mở file để ghi
        fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        write(fd, message, numb);
        close(fd);      
    } else if (mode == 'r')
    {
        fd = open(filename, O_RDONLY);
        if (-1 == fd) { 
            printf("open() %s failed, Please run w first\n", filename);
        } 
        char buffer[1024];
        read(fd, buffer, numb);     
        close(fd);  
    }
    // printf("Wrote %ld bytes to file\n", bytes_written);

    struct stat info;
    printf("File name: %s\n", filename);
    printf("File size: %ld bytes\n", info.st_size);
    printf("Last modified: %s", ctime(&info.st_mtime));

    printf("File type: ");
    if (S_ISREG(info.st_mode)) {
        printf("Regular file\n");
    } else if (S_ISDIR(info.st_mode)) {
        printf("Directory\n");
    } else if (S_ISLNK(info.st_mode)) {
        printf("Symbolic link\n");
    } else {
        printf("Other\n");
    }


  
    return 0; 
} 