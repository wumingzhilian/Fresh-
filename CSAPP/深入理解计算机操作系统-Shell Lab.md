# shell lab

http://csapp.cs.cmu.edu/3e/shlab.pdf

如上是一个实验指导书

![image-20220107153637067](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107153637067.png)

【实验内容】
是对tsh.c中没有填写的函数进行填写，使得该shell能处理前后台运行程序、能够处理ctrl+z、ctrl+c等信号。
需要实现的函数主要有一下五个：

```
void eval(char *cmdline)：解析并执行命令。
int builtin_cmd(char **argv)：检测命令是否为内置命令quit、fg、bg、jobs。
void do_bgfg(char **argv)：实现bg、fg命令。
void waitfg(pid_t pid)：等待前台命令执行完成。
void sigchld_handler(int sig)：处理SIGCHLD信号，即子进程停止或终止。
void sigint_handler(int sig)：处理SIGINT信号，即来自键盘的中断ctrl-c。
void sigtstp_handler(int sig)：处理SIGTSTP信号，即终端停止信号ctrl-z。
```

【实验结果的检验】
通过运行`./tshref`这个已经实现的shell将它的输出结果与我们自己实现的shell的结果进行比较

使用`make testn`用来测试你编写的`shell`执行第`n`组测试数据的输出。

使用`make rtestn`用来测试参考`shell`程序第`n`组测试数据的输出（共`16`组测试数据）。

`tshref.out`包含参考`shell`程序的所有测试数据的输出结果，先看完该文件了解命令格式在开始编码。

【注意】
有必要阅读《深入理解计算机系统 第二版》第8章异常控制流的所有内容。对于以下内容有比较好的了解

**可用辅助函数：**

```
int parseline(const char *cmdline,char **argv)：获取参数列表char **argv，返回是否为后台运行命令（true）。
void clearjob(struct job_t *job)：清除job结构。
void initjobs(struct job_t *jobs)：初始化jobs链表。
void maxjid(struct job_t *jobs)：返回jobs链表中最大的jid号。
int addjob(struct job_t *jobs,pid_t pid,int state,char *cmdline)：在jobs链表中添加job
int deletejob(struct job_t *jobs,pid_t pid)：在jobs链表中删除pid的job。
pid_t fgpid(struct job_t *jobs)：返回当前前台运行job的pid号。
struct job_t *getjobpid(struct job_t *jobs,pid_t pid)：返回pid号的job。
struct job_t *getjobjid(struct job_t *jobs,int jid)：返回jid号的job。
int pid2jid(pid_t pid)：将pid号转化为jid。
void listjobs(struct job_t *jobs)：打印jobs。
void sigquit_handler(int sig)：处理SIGQUIT信号。
```

**注意事项**

```
- tsh的提示符为`tsh>`
- 用户的输入分为第一个的`name`和后面的参数，之间以一个或多个空格隔开。如果`name`是一个tsh内置的命令，那么tsh应该马上处理这个命令然后等待下一个输入。否则，tsh应该假设`name`是一个路径上的可执行文件，并在一个子进程中运行这个文件（这也称为一个工作、job）
- tsh不需要支持管道和重定向
- 如果用户输入`ctrl-c` (`ctrl-z`)，那么`SIGINT` (`SIGTSTP`)信号应该被送给每一个在前台进程组中的进程，如果没有进程，那么这两个信号应该不起作用。
- 如果一个命令以“&”结尾，那么tsh应该将它们放在后台运行，否则就放在前台运行（并等待它的结束）
- 每一个工作（job）都有一个正整数PID或者job ID（JID）。JID通过"%"前缀标识符表示，例如，“%5”表示JID为5的工作，而“5”代笔PID为5的进程。
- tsh应该有如下内置命令：
quit: 退出当前shell

jobs: 列出所有后台运行的工作

bg <job>: 这个命令将会向<job>代表的工作发送SIGCONT信号并放在后台运行，<job>可以是一个PID也可以是一个JID。

fg <job>: 这个命令会向<job>代表的工作发送SIGCONT信号并放在前台运行，<job>可以是一个PID也可以是一个JID。



- tsh应该回收（reap）所有僵尸孩子，如果一个工作是因为收到了一个它没有捕获的（没有按照信号处理函数）而终止的，那么tsh应该输出这个工作的PID和这个信号的相关描述。
```



书上有一个简单的shell我们这里来分析一下

![image-20220107154301113](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107154301113.png)

这里定义了一个eval函数，用来接收我们的cmdline

同时定义了parseline 应该是用来接收参数

shell肯定是一个死循环，除非我们结束，他的样子就是

```
root >
```

肯定先打印，然后fgets获取输入的，stdin就是标准输入，fgets的

feof（stdin）

如果标准输入结束的话，然后exit结束

最后执行eval函数

![image-20220107154318830](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107154318830.png)

![image-20220107154324487](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107154324487.png)

![image-20220107154927423](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107154927423.png)



这里说的很清楚



![image-20220107154331579](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107154331579.png)

& 用来判断是不是在后台运行

## linux信号简介

![image-20220107170359944](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107170359944.png)

![image-20220107170345705](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107170345705.png)

![image-20220107170410242](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107170410242.png)

```
信号--->进程---->信号处理程序---执行------结果
```

![image-20220107170500124](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107170500124.png)

## Shell 简介

简单来说，shell 有两种执行模式：

1. 如果用户输入的命令是内置命令，那么 shell 会直接在当前进程执行（例如 `jobs`）
2. 如果用户输入的是一个可执行程序的路径，那么 shell 会 fork 出一个新进程，并且在这个子进程中执行该程序（例如 `/bin/ls -l -d`）

第二种情况中，每个子进程称为一个 job（当然也可以不止一个，通过管道机制，不过我们这里的实现不需要考虑管道）

如果命令以 `&` 结束，那么这个 job 会在后台执行（比如 `/bin/ls -l -d &`），也就是说 shell 本身不会等待 job 执行完成，直接可以继续输入其他命令；而在其他情况下，则是在前台运行，shell 会等待 job 完成，用户才可以继续输入命令。也就是说同一个时间只可能有一个前台任务，但是后台任务可以有任意多个。

程序的入口是 `int main(int argc, char *argv[])`，对于 `/bin/ls -l -d` 来说，我们有：

```
argc == 3
argv[0] == ''/bin/ls''
argv[1] == ''-l''
argv[2] == ''-d''
```

复制

另外两个需要支持功能是：

- job control：允许用户更改进程的前台/后台状态以进程的状态(running, stopped, or terminated)
  - ctrl-c 会触发 SIGINT 信号并发送给每个前台进程，默认的动作是终止该进程
  - ctrl-z 会触发 SIGTSTP 信号并发送给每个前台进程，默认的动作是挂起该进程，直到再收到 SIGCONT 信号才继续
  - `jobs` 命令会列出正在执行和被挂起的后台任务
  - `bg job` 命令可以让一个被挂起的后台任务继续执行
  - `fg job` 命令可以让一个被挂起的前台任务继续执行
- I/O redirection：重定向输入输出
  - `tsh> /bin/ls > foo`
  - `tsh> /bin/cat < foo`

### 提示

- 不要使用 `sleep()` 来同步
- 不要使用忙等待 `while(1);`
- 使用 `sigsuspend` 来同步
- 竞争条件
- 僵尸进程回收（注意竞争条件以及正确处理信号）
- 等待前台任务（仔细思考怎么样才是好的方式）
- 不要假定进程的执行顺序
- 子进程挂掉的时候应该在一个限定时间内被回收
- 不要在多个地方调用 `waitpid`，很容易造成竞争条件，也会造成程序过分复杂
- 不要使用任何系统调用来管理 terminal group
- `waitpid`, `kill`, `fork`, `execve`, `setpgid`, `sigprocmask` 和 `sigsuspend` 都非常有用，`waitpid` 中的 WUNTRACED 和 WNOHANG 选项也是如此。
- 遇到不清晰的用 `man` 来查看细节
- 实现 signal handler 的时候注意给全部的前台进程组发送 SIGINT 和 SIGTSTP 信号
- 在 `kill` 函数中使用 `-pid` 的格式作为参数
- 在 shell 等待前台工作完成时，需要决定在 `eval` 及 `sigchold handler` 具体的分配，这里有一定技巧
- 在函数 `eval` 中，在 fork 出子进程之前，必须使用 `sigprocmask` 来阻塞 SIGCHLD, SIGINT 和 SIGTSTP 信号，完成之后再取消阻塞。调用 `addjob` 的时候也需要如此。注意，因为子进程也继承了之前的各种状态，所以在子进程中调用 `exec` 执行新程序的时候注意需要取消阻塞，同样也需要恢复默认的 handler（shell 本身已经忽略了这些信号），具体可以看书本的 8.5.6 节
- 不要使用 `top`, `less`, `vi`, `emacs` 之类的复杂程序，使用简单的文本程序如：`/bin/cat`, `/bin/ls`, `/bin/ps`, `/bin/echo`
- 因为毕竟不是真正的 shell，所以在 fork 之后，execve 之前，子进程需要调用 `setpgid(0, 0)`，这样就把子进程放到一个新的进程组里。这样就保证我们的 shell 前台进程组中唯一的进程，当按下 ctrl-c 时，应该捕获 SIGINT 信号并发送给对应的前台进程组中。

同样提供一个 `tshref` 参考程序来作为比对输出（除了进程 id 之外其他需要一模一样），具体是通过 `runtrace` 文件来测试，每个 trace 文件会测试一个特性

![image-20220107174112841](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107174112841.png)



makefile中，写了test0x



## 实验开始

我们的目的就是让我们的

make test0x

结果等于

make rtest0x

![image-20220107170929745](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107170929745.png)

我们很明显发现结果不一样

jobs命令，显示当前shell中有几个进程

我们来修改tsh.c的文件

![image-20220107180048730](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107180048730.png)

这里写了有我们要实现的几个功能

![image-20220107180414584](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220107180414584.png)

首先我们先把书上的模板抄一抄

这里是用来阻塞的信号量的表

```
#include <signal.h>
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);
int sigisemptyset(const sigset_t *set);
int sigorset(sigset_t *dest, const sigset_t *left,const sigset_t *right);
int sigandset(sigset_t *dest, const sigset_t *left,const sigset_t *right);
 
参数:
	sigset_t * 是信号集
	signum 是信号值
	
返回值:
	sigemptyset,sigfillset,sigaddset,sigdelset,sigorset,sigandset正确返回0,错误-1
	sigismember 信号存在信号集中返回1，不存在返回0
	sigisemptyset 是空信号集返回1，不是返回0
```

```
多个信号可使用一个称之为信号集的数据结构来表示。(如果了解select,poll 是不是想到有点类似监视fd的东西？)
信号集是为了方便对多个信号进行处理，一个用户进程常常需要对多个信号做出处理，在 Linux系统中引入了信号集(信号的集合)。
这个信号集有点类似于我们的 QQ 群，一个个的信号相当于 QQ 群里的一个个好友。

信号集是用来表示多个信号的数据类型(sigset_t)，其定义路径为：/usr/include/i386-linux-gnu/bits/sigset.h。

sigemptyset()初始化一个未包含任何成员的信号集
sigfillset()初始化一个信号集使其包含所有的信号(包括实时信号,信号值是34-64，暂时了解就行)
注意:有人觉得那也可以用memset去初始化清空为0,这是错误的，因为 sigset_t 数据类型在linux系统上是一个位掩码，用memset有可能使用位掩码之外的数据结构来实现信号集
sigismember()用来测试信号是否在信号集中
sigaddset()向一个信号集添加一个信号
sigdelset()向一个信号集删除一个信号

sigisemptyset()判断信号集中是否包含信号
sigorset()将left集和right集的并集置于dest集
sigandset()将left集和right集的交集置于dest集

eg:
	sigaddset(&set, SIGINT); // 把 SIGINT 添加到信号集 set
	sigaddset(&set, SIGQUIT);// 把 SIGQUIT 添加到信号集 set
```

example:

```
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <malloc.h>
#include <signal.h>
 
 
int main(int argc, char *argv[])
{
	sigset_t set;	// 定义一个信号集变量
	int ret = 0;
	sigset_t left_set;
	sigset_t right_set;
 
	sigemptyset(&set); // 清空信号集的内容
	
	// 判断 SIGINT 是否在信号集 set 里
	// 在返回 1， 不在返回 0
	ret = sigismember(&set, SIGINT);
	if(ret == 0){
		printf("SIGINT is not a member of set \nret = %d\n", ret);
	}
		
	sigaddset(&set, SIGINT); // 把 SIGINT 添加到信号集 set
	sigaddset(&set, SIGQUIT);// 把 SIGQUIT 添加到信号集 set
	
	// 判断 SIGINT 是否在信号集 set 里
	// 在返回 1， 不在返回 0
	ret = sigismember(&set, SIGINT);
	if(ret == 1){
		printf("SIGINT is a member of set \nret = %d\n", ret);
	}
	
	sigdelset(&set, SIGQUIT); // 把 SIGQUIT 从信号集 set 移除
	
	// 判断 SIGQUIT 是否在信号集 set 里
	// 在返回 1， 不在返回 0
	ret = sigismember(&set, SIGQUIT);
	if(ret == 0){
		printf("SIGQUIT is not a member of set \nret = %d\n", ret);
	}
 
 
	sigemptyset(&left_set); 
	sigemptyset(&right_set); 
 
	sigaddset(&left_set, SIGHUP); 
	sigaddset(&left_set, SIGINT); 
 
	sigaddset(&right_set, SIGINT); 
	sigaddset(&right_set, SIGQUIT);
 
	// set = [SIGHUP,SIGINT,SIGQUIT]
	sigorset(&set,&left_set,&right_set);
	// set = [SIGINT]
	sigandset(&set,&left_set,&right_set);
	
	return 0;
}
```

信号见这里 sigprocmask使用

https://jason--liu.github.io/2019/04/15/use-sigprocmask/

**1、 int setpgid(pid_t pid, pid_t pgid);**

功能1：设置进程pid所在的进程组的ID为pgid，如果pid的实参为0，setpgid(0, 5)等价于setpgid(getpid(), 5)，其中getpid()的作用是获取本进程的pid。

功能2：把进程pid移动到别的进程组（这一功能的一个常见例子就是：shell创建管道线时），原进程组和目标进程组必须处于同一个会话（session）中（相关内容可参考setsid、credentials函数），这时pgid接受的实参应该为一个已经存在的目标进程组的pgid。

返回值：成功返回0，失败返回-1并设置errno。

https://blog.csdn.net/qq_31073871/article/details/80927583

```
        sigfillset(&mask_all);//sigfillset()初始化一个信号集使其包含所有的信号(包括实时信号,信号值是34-64，暂时了解就行)
    	sigemptyset(&mask_one);//初始化一个未包含任何成员的信号集
    	sigaddset(&mask_one, SIGCHLD);//向一个信号集添加一个信号
```

## eval

```
void eval(char *cmdline) 
{
    //step 1初始化个变量和阻塞信号
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int state;
    int argc;
    pid_t curr_pid;//存储当前前台的pid
    sigset_t mask_all,mask_one,mask_prev;

    //设置阻塞集合
    sigemptyset(&mask_one);
    sigaddset(&mask_one,SIGCHLD);
    sigfillset(&mask_all);

    //step 2 解析命令行，得到是否是后台命令，置为state
    strcpy(buf,cmdline);//from cmdline copy to buf
    state = parseline(buf,argv,&argc)? BG :FG;

    //step 3 判断是否是内置命令
    if(!builtin_cmd(argv,argc)){
        //不是内置命令，阻塞SIGCHLD，防止子进程在父进程之前结束，也就是在也就是addjob和deletejob之间，要保证这个顺序
        sigprocmask(SIG_BLOCK, &mask_one, &mask_prev);
        if((curr_pid = fork()) == 0){
            //子进程，先解除对SIGCHLD阻塞
            sigprocmask(SIG_SETMASK,&mask_prev,NULL);
            //改进进程的进程组，不要跟shell的进程组在同一个pid下，然后调用其他execve函数执行相关文件
            setpgid(0,0);
            if(execve(argv[0],argv,environ) < 0){
                //execve就相当于是我们正常的可执行文件，然后给他几个参数，执行成功不会返回，执行失败返回-1
                //没有找到相关文件的情况下，打印消息，直接退出
                printf("%s:Command not foundQAQ.\n",argv[0]);
                }
                exit(0);
            }
            //step 4
            // 创建子进程之后，父进程addjob，整个函数执行期间，保证不能被中断，尤其是在for循环中中断的话，后果很严重
            //因此阻塞所有信号，
            sigprocmask(SIG_BLOCK,&mask_all,NULL);
            addjob(jobs,curr_pid,state,cmdline);
            //再次阻塞STGCHLD
            sigprocmask(SIG_SETMASK,&mask_one,NULL);
            //step 5 判断bg，fg 调用waitf函数，等待前台运行完成，bg打印消息
            //还有一个问题是，如果在前台任务，如果我使用默认的waitpid由于该函数是linux定义的原子性函数，无法被信号中断，那么前台
            //函数在执行的过程中，无法相应SIGINT和SIGSTO信号，这里我使用sigsuspend函数加上while判断fg_stop_or_exit标志的方法。具体见waitfg函数
            if(state == FG){
                waitfg(curr_pid);
            }
            else{
                //输出后台进程的信息
                //获取全局变量，阻塞所有信号，防止被打断
                sigprocmask(SIG_BLOCK,&mask_all,NULL);
                struct job_t* curr_bgmask = getjobpid(jobs,curr_pid);
                printf("[%d] (%d) %s",curr_bgmask->jid,curr_bgmask->pid,curr_bgmask->cmdline);
            }
            //解除所有阻塞
            sigprocmask(SIG_SETMASK,&mask_prev,NULL);
    }
    return;
}

```

接着是builtin_cmd函数

他是检测是否存在默认命令，所以，我们检测到之后，返回1，没有则返回0

## builtin_cmd

```
/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv,int argc)
{
    //初始化变量以及阻塞集合
    char* cmd = argv[0];
    sigset_t mask_all,mask_prev;

    sigfillset(&mask_all);

    if(!strcmp(cmd,"quit")){
        //直接退出,当然也可以检测到还没有执行完，直接kill掉
        exit(0);
        //bg <job>: 这个命令将会向<job>代表的工作发送SIGCONT信号并放在后台运行，<job>可以是一个PID也可以是一个JID。
        //
        //fg <job>: 这个命令会向<job>代表的工作发送SIGCONT信号并放在前台运行，<job>可以是一个PID也可以是一个JID。
    }else if(strcmp(cmd,"fg") == 0 || strcmp(argv[0],"bg") == 0){
        do_bgfg(argv,argc);
        return 1;
        }
    else if(!strcmp(cmd,"jobs")){
        //访问全局变量，需要阻塞全部的信号
        sigprocmask(SIG_BLOCK,&mask_all,&mask_prev);
        listjobs(jobs);
        sigprocmask(SIG_SETMASK,&mask_prev,NULL);
        return 1;
    }
    return 0;     /* not a builtin command */
}

```

接着，我们就是要实现do_bgfg函数，也就是实现bg，fg两条内置命令

1-要区分bg,fg, 以及传入的pid或者jid（job中的id）参数队以ing的进程的状态，前者是if，后者用switch

2-注意用户的错误处理，比如参数输错，参数不够

## do_bgfg

```
tsh> fg %1
Job [1] (9723) stopped by signal 20
kill(pid,signal)的规则
if (pid < 0) 则向|pid|的组中全部发送信号
if(pid > 0) 则就向单个进程发送
if(pid=0) 信号将送往所有与调用kill()的那个进程属同一个使用组的进程。
```

```
void do_bgfg(char **argv,int argc)
{
    //也就是说，参数的数量不等于两个,就去报错
    if(argc != 2){
        printf("%s command requires PID or job id argument \n",argv[0]);
        fflush(stdout);
        //这里清空了标准输入缓冲区，同时显示在屏幕前，防止结果冲掉原来的数据
        return;
    }
    //初始化变量
    char* cmd = argv[0];
    char* para = argv[1];
    struct job_t* curr_job;
    sigset_t mask_all,mask_prev;
    int curr_jid;
    //判断传入的pid还是jid，并且获取对应的job的结构体
    sigfillset(&mask_all);
    if(para[0] == '%'){
        //有百分号也就是jid，没有的话，就是他输入错误
        //atoi (表示 ascii to integer)是把字符串转换成整型数的一个函数
        curr_jid = atoi(&(para[1]));
        //错误处理2，如果传入的参数不是规定的格式，报错
        if(curr_jid == 0){
            printf("%s:argument must be a pid or %%jobid\n",cmd);
            fflush(stdout);
            return ;
        }
    }else{
        curr_jid = atoi(para);
        if(curr_jid == 0){
            printf("%s:argument must be a pid or %%jobid\n",cmd);
            fflush(stdout);
            return ;
        }
        sigprocmask(SIG_BLOCK,&mask_all,&mask_prev);
        //int pid2jid(pid_t pid)：将pid号转化为jid。
        curr_jid = pid2jid(curr_jid);

    }
    sigprocmask(SIG_BLOCK,&mask_all,&mask_prev);
    curr_job = getjobjid(jobs,curr_jid);

    if(curr_job == NULL){
        printf("(%s):No such process\n",para);
        fflush(stdout);
        sigprocmask(SIG_SETMASK,&mask_prev,NULL);
        return;
    }
    //区分bg还是fg
    if(!strcmp(cmd,"bg")){
        //区分job状态
        switch(curr_job->state){
            case ST:
                //bg命令，改变该任务的运行状态，ST->BG,同时发送辛哈哦给对应的子进程
                curr_job->state = BG;
                //在 `kill` 函数中使用 `-pid` 的格式作为参数
                kill(-(curr_job->pid),SIGCONT);
                printf("[%d] (%d) %s",curr_job->jid,curr_job->pid,curr_job->cmdline);
                break;
            case BG:
                //如果该任务就是在后台，就什么也不管
                break;
            //如果bg前台，或者unfef，肯定出错了
            case UNDEF:
                unix_error("bg 出现undef的进程\n");
                break;
            case FG:
                unix_error("bg 出现FG的进程\n");
                break;
        }
    }else{
        //fg
        switch(curr_job->state){
            //如果fg挂起的进程，就重启，并且挂起主进程等待他回收终止
            case ST:
                curr_job->state  = FG;
                kill(-(curr_job->pid),SIGCONT);
                waitfg(curr_job->pid);
                break;
            //如果fg后台进程，那么将他的状态转为前台进程，然后等待他终止，
            case BG:
                curr_job->state = FG;
                waitfg(curr_job->pid);
                break;
            //如果本身是前台进程，就出错
            case FG:
                unix_error("fg 出现FG进程\n");
                break;
            case UNDEF:
                unix_error("fg 出现undef进程\n");
                break;
        }
    }
    sigprocmask(SIG_SETMASK,&mask_prev,NULL);

    return;
}
```

## waitfg函数

```
Block until process pid is no longer the foreground process
```

```
void waitfg(pid_t pid)
{
    //进来之前阻塞了SIGCHLD信号
    sigset_t mask;
    sigemptyset(&mask);
    //前台进程的pid和挂起标志，FGPID=0

    while (fgpid(jobs) > 0)
        sigsuspend(&mask);
    return;
}

```

接下来是处理器handler

```

```

waitpid参数的介绍

![image-20220108233721313](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220108233721313.png)

![image-20220108233728596](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220108233728596.png)

![image-20220108233737228](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220108233737228.png)





## sigchld_handler

```
/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
```

```
void sigchld_handler(int sig) 
{
    /*初始化
     * */
    int olderrno = errno;
    sigset_t mask_all,mask_prev;
    pid_t gc_pid;
    struct job_t* gc_job;
    int status;

    sigfillset(&mask_all);
    //尽可能的回收子进程，同时使用WNOHAND选项使得如果当前几次都没有终止时，直接返回，而不是挂起回收。这样可能会阻碍无法两个短时间结束的后台进程
    //trace05.txt
    while((gc_pid = waitpid(-1,&status,WNOHANG | WUNTRACED)) > 0){
        sigprocmask(SIG_BLOCK,&mask_all,&mask_prev);
        gc_job = getjobpid(jobs,gc_pid);
        if(gc_pid == fgpid(jobs)){
            fg_stop_or_exit = 1;
        }
        if(WIFSTOPPED(status)){
            //子进程停止引起的waitpid函数返回，在判断该进程是否是前台进程
            gc_job->state = ST;
            printf("Job [%d] (%d) terminated by signal %d\n", gc_job->jid, gc_job->pid, WSTOPSIG(status));

        }else{
            //子进程终止引起的返回，判断是否为前台进程
            //并且判断该信号是否是未捕获的信号
            if(WIFSIGNALED(status)){
                printf("Job [%d] (%d) terminated by signal %d\n", gc_job->jid, gc_job->pid, WTERMSIG(status));
            }
            //终止的进程直接回收
            deletejob(jobs,gc_pid);
        }
        fflush(stdout);
        sigprocmask(SIG_SETMASK,&mask_prev,NULL);
    }
    errno = olderrno;
    return;
}

```



## sigint_handler

```
/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
```

也就是说，你接受到ctrl c然后返回到前台的job里面

1. 获取前台进程，判断当前是否有前台进程。如果没有直接返回。有则步骤2

2. 使用kill函数，发送SIGINT/SIGTSTP信号给前台进程组。

```
void sigint_handler(int sig) 
{
    int olderrno = errno;
    sigset_t mask_all,mask_prev;
    pid_t curr_fg_pid;

    sigfillset(&mask_all);
    //访问全局结构体数组，阻塞信号，
    sigprocmask(SIG_BLOCK,&mask_all,&mask_prev);
    //pid_t fgpid(struct job_t *jobs)：返回当前前台运行job的pid号。
    curr_fg_pid = fgpid(jobs);
    sigprocmask(SIG_SETMASK,&mask_prev,NULL);

    if(curr_fg_pid != 0){
        kill(-curr_fg_pid,SIGINT);

    }
    errno = olderrno;
    return;
}

```



## sigtsp_handler

```
/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
```

也就是检测ctrl z，然后发送信号

```
void sigtstp_handler(int sig) 
{
    int olderrno = errno;
    sigset_t mask_all,mask_prev;
    pid_t curr_fg_pid;

    sigfillset(&mask_all);

    sigprocmask(SIG_BLOCK,&mask_all,&mask_prev);
    curr_fg_pid = fgpid(jobs);
    sigprocmask(SIG_SETMASK,&mask_prev,NULL);

    if(curr_fg_pid != 0){
        kill(-curr_fg_pid, SIGTSTP);
    }
    errno = olderrno;
    return;
}
```



## 结果

我们还需要修改以下parseline函数，因为没有argc参数。

我们需要make tsh进行编译我们的程序shell，这里我们make其他的，来生成我们的测试文件

![image-20220109004056870](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109004056870.png)

这里是结果的样例，

![image-20220109004012651](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109004012651.png)

附上我们的结果，发现fg还是有问题，

![image-20220109004652876](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109004652876.png)

发现是，c语言paseline的传值出现了问题，形参的影响，

这里附上paseline的修改

![image-20220109010425505](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109010425505.png)

### parseline

- 全局变量声明

```
int parseline(const char *cmdline, char **argv,int* argc);
```

```
int parseline(const char *cmdline, char **argv,int* argc)
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    //int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    (*argc) = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[(*argc)++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[(*argc)] = NULL;

    if ((*argc) == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[(*argc)-1] == '&')) != 0) {
	argv[--(*argc)] = NULL;
    }
    return bg;
}
```





