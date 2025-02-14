#include<stdio.h> 
#include<string.h> 
#include<unistd.h> 
#include<fcntl.h> 
  
int main (void) 
{ 
    int fd; 
    int numb_read, numb_write;
    char buf1[12] = "hello world\n"; 
  
    // assume foobar.txt is already created 
    fd = open("hello.txt", O_RDWR | O_CREAT | O_APPEND, 0667);         
    if (-1 == fd) { 
	printf("open() hello.txt failed\n");
    }      

    numb_write = write(fd, buf1, strlen(buf1));
    printf("Write %d bytes to hello.txt\n", numb_write);
  
    lseek(fd, 2, SEEK_SET); //Do sử dụng O_APPEND nên cho dù có set vị trí con trỏ thì nó cũng sẽ tự đông nhảy đến hàng cuối cùng
    write(fd, "linux\n", strlen("linux\n"));
    
    close(fd); 
  
    return 0; 
} 