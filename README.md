**This repository contains a basic shell program native to Linux operating system
that supports various functionalities, all of which are detailed and demonstrated
in the test cases below!**

![img1](./images/LLdiag.drawio.png)

**The sequence diagram above describes how the shell functions; a user command is 
first entered and parsed. If the command is internal (defined and supported
by my shell code - *cd*/*pwd*/etc), it is simply executed by the shell - the parent process. 
If the command is external (native linux command - *grep*/*cat*/etc), the *fork()* system
call is used to duplicate the shell process and create an identical child process. The child
process then makes another system call, *execvp()*, which overwrites the child to execute
the external command.**

**TEST CASES:**

**Execution of external command *cat* using *fork()* and *execvp()*.**

![img1](./images/forkandexec.png)

**Processing internal commands *cd* and *pwd*.**

![img1](./images/cdandpwd.png)

**Input redirection (<) - grep command source of input redirected to my_shell.c
file, resulting in a search for the text *printf* within the specified file.**

![img1](./images/inputredir.png)

**Output redirection**

![img1](./images/outputredir.png)
