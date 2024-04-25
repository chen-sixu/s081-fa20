#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char *argv[])
{
    int pid;
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    char buf[]={'1'};
    if(fork()==0)
    {
        pid=getpid();
        close(0);
        dup(p1[0]);
        close(p1[0]);
        close(p1[1]);
        read(p1[0],buf,1);
        printf("%d: received ping\n",pid);
        close(p2[0]);
        write(p2[1],buf,1);
        exit(0);
    }
    else
    {
        pid=getpid();
        close(p1[0]);
        write(p1[1],buf,1);
        close(p1[1]);
        close(p2[1]);
        read(p2[0],buf,1);
        printf("%d: received pong\n",pid);
        close(p2[0]);
        exit(0);
    }
}