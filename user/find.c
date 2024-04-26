#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
读目录
搜索当前目录，依据文件类型决定操作：
1.T_FILE:printf文件名
2.T_DIR:递归调用

*/

char*
fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p)+1);
    //memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));   this line can cause wrong compare
    return buf;
}

void find(char *path,char *targetfile)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0) //open the path
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) //get file state
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }


    switch(st.type)
    {
    case T_FILE:
    if (strcmp(fmtname(path),targetfile)==0)
    {
        printf("%s\n",path);
    }
    break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
/*
在 read 调用之后，文件描述符 fd 的内部偏移量会自动前进，
这一点由操作系统的文件系统管理。
*/
        while(read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if(de.inum == 0||de.inum==1||strcmp(de.name,".")==0||strcmp(de.name,"..")==0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0) //get file state
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            find(buf,targetfile); //recurse
        }
        
        break;
    }
    close(fd);
}

int main(int argc,char *argv[])
{
    if (argc!=3) //operation path targetfile
    {
        printf("missing argument\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}