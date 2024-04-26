#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
p = get a number from left neighbor
print p
loop:
    n = get a number from left neighbor
    if (p does not divide n)
        send n to right neighbor
*/

void pipeline(int listen)
{
    int this_num=0;
    int pipe_num=0;
    int p[2];
    int forked=0;
    while (1)
    {   
        if (read(listen,&pipe_num,4)==0)//no more number comes,close the listen end
        {
            close(listen);
            if (forked)//if has a child
            {
                close(p[1]);//no more write to the child,so close write end
                int child_pid;
                wait(&child_pid);
            }
            exit(0);
        }
        if (this_num==0)
        {
            this_num=pipe_num;
            printf("prime %d\n",this_num);//the first number one gets is this time's prime
        }
        if(pipe_num%this_num!=0)//cant be divided,continue
        {
            if (!forked)
            {
                pipe(p);
                forked=1;
                if(fork()==0)//child process
                {
                    close(p[1]);
                    close(listen);
                    pipeline(p[0]);//recursive
                }
                else//parent process
                {
                    close(p[0]);
                }
            }
            //else
            //{
            write(p[1],&pipe_num,4);
            //}
        }
    }
}

int main(int argc,char *argv[])
{
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; i++)
    {
        write(p[1],&i,4);    
    }
    close(p[1]);
    pipeline(p[0]);
    exit(0);
}