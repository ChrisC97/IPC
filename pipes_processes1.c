// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

char* ChildStringRequest(){
  char* input_str = malloc(100);
  printf("Enter a string to concatenate:");
  scanf("%s", input_str);
  return input_str;
}

int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org";
    char input_str[100]; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate:");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
 
        // Close reading end of pipe1 & writing end of pipe2.
        close(fd1[0]);
        close(fd2[1]);
  
        // Write input string to first pipe.
        write(fd1[1], input_str, strlen(input_str)+1); 
  
        // Read string from pipe2.
        char secondStr[100]; 
        read(fd2[0], secondStr, 100); 
      
        // Concatinate a fixed string to it.
        int k = strlen(secondStr); 
        int i; 
        for (i=0; i<strlen(fixed_str2); i++) 
            secondStr[k++] = fixed_str2[i]; 
      
        secondStr[k] = '\0';   // string ends with '\0' 
      
        printf("Concatenated string %s\n", secondStr);
      
        // Wait for child to terminate. 
        wait(NULL); 
  
        // Close writing end of pipes. 
        close(fd1[1]);
        close(fd2[0]);
    } 
  
    // child process 
    else
    { 
        // Close writing end of pipe1 & reading end of pipe2.
        close(fd1[1]);
        close(fd2[0]);
      
        // Read string from pipe1.
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); 
  
        // Concatinate a fixed string to it.
        int k = strlen(concat_str); 
        int i; 
        for (i=0; i<strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 
  
        concat_str[k] = '\0';   // string ends with '\0' 
  
        printf("Concatenated string %s\n", concat_str);
      
        // PART 2 //
      
        // Request second string.
        char* cStr = ChildStringRequest();
      
				// Write string to pipe2.
        write(fd2[1], cStr, strlen(cStr)+1); 
      
        // Close reading end of pipe1 & writing end of pipe2.
        close(fd1[0]);
        close(fd2[1]);
      
        // Terminate child.
        exit(0); 
    } 
} 
