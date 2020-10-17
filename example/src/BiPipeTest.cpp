#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void)
{
    int pid, n, c, p, k, nbread;
    char buf1[12], buf2[12];
    int fd1[2], fd2[2];
    pipe(fd1);
    pipe(fd2);
    pid = fork();
    if (pid == 0)
    {
        close(fd1[1]);
        close(fd2[0]);
        read(fd1[0], buf2, sizeof(buf2));
        n = atoi(buf2);
        printf("Child read %d\n", n);
        for (int i = 0; i < n; i++)
        {
            printf("child dozes...\n");
            sleep(3);
            printf("child wakes...\n");
            nbread = read(fd1[0], buf2, sizeof(buf2));
            if (nbread == -1)
            {
                fprintf(stderr, "child exits after read failure\n");
                exit(1);
            }
            c = atoi(buf2);
            c = c * 2;
            sprintf(buf2, "%d", c);
            write(fd2[1], buf2, sizeof(buf2));
            printf("Child wrote [%s]\n", buf2);
        }
        close(fd1[0]);
        close(fd2[1]);
        printf("Child done\n");
        exit(0);
    }
    else
    {
        close(fd1[0]);
        close(fd2[1]);
        printf("Enter integer: ");
        scanf("%d", &p);
        sprintf(buf1, "%d", p);
        write(fd1[1], buf1, sizeof(buf1));
        printf("Parent wrote [%s]\n", buf1);
        printf("parent dozes...\n");
        sleep(3);
        printf("parent wakes...\n");
        for (int i = 0; i < p; i++)
        {
            sprintf(buf1, "%d", i);
            write(fd1[1], buf1, sizeof(buf1));
            printf("parent wrote [%s]\n", buf1);
            read(fd2[0], buf2, sizeof(buf2));
            printf("number is: %s\n", buf2);
        }
        close(fd1[1]);
        close(fd2[0]);
        wait(NULL);
    }
    return 0;
}