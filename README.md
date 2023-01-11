**This repository contains a basic shell program native to Linux operating system
that supports various functionalities, all of which are detailed and demonstrated
in the test cases below!**

**The following sequence diagram describes how the shell functions; a user command is 
first entered and parsed. If the command is internal (defined and supported
by my shell code - cd/pwd/etc), it is simply executed by the shell - the parent process. 
If the command is external (native linux command - grep/cat/etc), the fork() system
call is used to duplicate the shell process and create an identical child process. The child
process then makes another system call, execvp(), which overwrites the child to execute
the external command.**

![img1](./images/LLdiag.drawio.png)

**TEST CASES:**

![img1](./images/LLdiag.drawio.png)
