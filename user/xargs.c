#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define buf_size  512
/*
    读取命令行参数，以空格为分界线当作按行读取
    每读一行，fork一个子进程将命令搭配一个参数进行执行
    xv6不支持\n识别
*/
int main(int argc,char *argv[])
{
    char buf[buf_size+1]={0};
    char *xargv[MAXARG]={0};

    for (int i = 1; i < argc; i++)
    {
        xargv[i-1]=argv[i];
    } //temp buf that contains argvs
    
    int stdin_end=0;
    int occupy=0;

    while (!stdin_end||occupy!=0)
    {
        if (!stdin_end) //parse command line
        {
            int rest_size=buf_size-occupy;
            int read_size=read(0,buf+occupy,rest_size);
            if (read_size<0)
            {
                printf("read error!\n");
            }
            if (read_size==0) //finish reading
            {
                close(0);
                stdin_end=1;
            }
            occupy+=read_size;
        }

        char *line_end=strchr(buf,'\n'); //find where is first \n 
        while (line_end)
        {
            char temp_buf[buf_size+1]={0};
            memcpy(temp_buf,buf,line_end-buf); //temp_buf:store one argument
            xargv[argc-1]=temp_buf; //the argument follows the command
        
            if (fork()==0) //child
            {
                if (exec(argv[1],xargv)<0)
                {
                    printf("xargs:exec error!\n");
                    exit(1);
                }
            }
            else //parent
            {
                /*arguments in buf totally move forward by a step,also update occupy*/
                memmove(buf,line_end+1,occupy-(line_end-buf)-1);
                occupy=occupy-(line_end-buf)-1;
                memset(buf+occupy,0,buf_size-occupy);

                int pid;
                wait(&pid);
                line_end=strchr(buf,'\n');
            }
        }
    }
    exit(0);
}