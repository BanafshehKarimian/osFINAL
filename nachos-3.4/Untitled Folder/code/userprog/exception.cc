// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#include <stdio.h>

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

    int memval, vaddr;
    int type;// = machine->ReadRegister(2);
void incPC(){
	
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
        machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg)+4);

}
int R_W_Reg(bool rw,int reg , int  v){

	if(rw)
		machine->WriteRegister(reg, v);
	else
		return machine->ReadRegister(reg);
	return -1;

}
int status(int childStatus,int pid){

	 while(childStatus == PARENT_WAITING) {
                // Sleep the thread
                DEBUG('J', "Parent %d  was sleeping: child %d\n", currentThread->getPid(), pid);

                IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
                currentThread->Sleep();
                (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts

                // Obtain the new status
                childStatus = currentThread->getChildStatus(pid);
            }
	return childStatus;

}
static void ForkHandler(){
	printf("forking\n");
	type=R_W_Reg(0,2,0);
	incPC();	
	//creat a thread
	Thread *child = new Thread("child");
	//set parent
	child->parent = currentThread;
	//allocate addressspace
	//child->space = new AddrSpace(currentThread->space->getNumPages(), currentThread->space->getStartPhysPage()); /////////////////////////
	//reg4??????????????????
	machine->WriteRegister(2, 0);
	child->SaveUserState();
	printf("created child_pid::%d of parent with pid %d\n",child->getPid(),child->getPpid());
	R_W_Reg(1,2, child->getPid());
	//set satck for child
	VoidFunctionPtr v = (VoidFunctionPtr) R_W_Reg(0,4,0);
	child->StackAllocate( v , child->getPid());

	IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
        scheduler->ReadyToRun(child);
	(void) interrupt->SetLevel(oldLevel); // re-enable interrupts

}
static void JoinHandler(){

	//remove child by id in reg 4 ???
	int pid = R_W_Reg(0,4,0);
	DEBUG('J', "Joining %d with %d\n", currentThread->getPid(), pid);
	//5) removeExitedChild
	int childStatus = currentThread->getChildStatus(pid);
	//find child thread
	if(childStatus!= CHILD_NOT_FOUND) {
            // The very first time, the child is live, we set the status as
            // parent waiting and send the thread to sleep, if it wakes and
            // the status is still PARENT_WAITING, we send it to sleep
            if(childStatus == CHILD_LIVE) {
                DEBUG('J', "Child %d was live: Parent %d\n", pid, currentThread->getPid());
                currentThread->setChildStatus(pid, PARENT_WAITING);

                // Send the thread to sleep
                IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
                currentThread->Sleep();
                (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
            }
            
            // The status of the thread may have changed when we come over here,
            // so we obtain it again
            childStatus = currentThread->getChildStatus(pid);
	    status(childStatus,pid);
	}
	//get exit status
	//6) getExitStatus
	//clean childs resources
	//reg2 == exitstatus
	//pcforward??????
	DEBUG('J', "Parent %d's child %d's state %d\n", currentThread->getPid(), pid, childStatus);
	R_W_Reg(1,2, childStatus);
	incPC();

}
static void ExecHandler(){

	//args = reg4???
	vaddr = R_W_Reg(0,4,0);
	machine->ReadMem(vaddr, 1, &memval);
/*
	char file[50];
        int i=0;

        //vaddr = machine->ReadRegister(4);
	while ((*(char*)&memval) != '\0') {
            file[i]  = (char)memval;
            ++i;
            vaddr++;
            machine->ReadMem(vaddr, 1, &memval);
        }
	file[i] = (char)memval;*/
	char file2[] ="/home/banafshbts/Desktop/os/Nachos_Operating_Systems_Course-master/Necessary_Packages/nachos-3.4/code/test/halt.c";
	//get filename from userspace 
	//openFile
	//creat a new thread and alloc space
	//reg2 == threadID
	//fork ThreadFuncForUserProg??
	OpenFile *executable = fileSystem->Open(file2);
        AddrSpace *space;

        if (executable == NULL) {
            printf("Unable to open file %s\n", file2);
            return;
        }
        space = new AddrSpace(executable);    
        currentThread->space = space;

        delete executable;			// close file

        space->InitRegisters();		// set the initial register values
        space->RestoreState();		// load page table register

        machine->Run();			// jump to the user progam
	ASSERT(FALSE); 

}



void
ExceptionHandler(ExceptionType which)
{
    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
	//printf("heeeeeeeeeeeereeeeeee\n");
	
	//ForkHandler();
	//printf("heeeeeeeeeeeereeeeeee222222222222222222 %d\n",currentThread->getPid());
	//JoinHandler();
	//ExecHandler();
	//if(currentThread->getPid()!=1)
   		interrupt->Halt();
    }
   else if ((which == SyscallException) && (type == SC_Exec)) {
   	ExecHandler();
    }
   else if ((which == SyscallException) && (type == SC_Join)) {
	JoinHandler();
    }
   else if ((which == SyscallException) && (type == SC_Fork)) {
	printf("heeeeeeeeeeeereeeeeeeeeeeeeeeeeee\n");
	ForkHandler();
    }
   else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
   }

}

