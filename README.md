# osFINAL
changed files:
userprog/addrespace+userprog/syscal+userprog/exception+thread/thread+test/halt+test/shell
Added functions:

Addrespace:

  AddrSpace(int numParentPages, int parentStartPhysPage);

Exception:

  ExecHandler();

  ForkHandler();
  
  JoinHandler();

Fork:

  after incrementing pc register fork handler creats a thread*child sets its parent pid to current threads pid and sets childs pid
  then clones parent addresspace using AddrSpace(int numParentPages, int parentStartPhysPage) and sets its registers to parent regs and adds child to ready to run list.(output and input of fork will be written in register 2 and 4)

Join:

  JoinHandler reads input of join function (pid) from register 4 . if that childs status is CHILD_LIVE or PARENT_WAITING  keep its parent sleep until the child finish and change its status.
  at the end writes the child status in register 2.
Exec:

  first Exec handler reads filename (because we're in kernel space we need to copy the name of the file) then opens it using fileSystem->Open allocates Addrespace to it. at the end space->InitRegisters() and space->RestoreState() are used to set the initial reg values and load page table reg.
  
Exit:

  first it sets exitStatus to zero (if exitStatus == CHILD_LIVE OR exitStatus == PARENT_WAITING) then if we have 1 thread then halt else check if parent is waiting for child or not if yes signal the parent and add it to ready to run
