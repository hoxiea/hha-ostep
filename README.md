# hha's OSTEP Solutions

During the Fall 2024 semester, I started independently working through my copy of Operating Systems: Three Easy Pieces (OSTEP). [Book website](https://pages.cs.wisc.edu/~remzi/OSTEP/).

I did this for a few reasons:

1. **Learn more about operating systems.** This is an aspect of computer science that I don't know much about, and I wanted to change that.

1. **Expand my tutoring offerings.** One of my students will be taking an OS course in the spring, and I wanted to be able to help them if they needed it.

1. **Improve systems programming.** Refresh and improve my knowledge of the C programming language, and begin to better understand the Linux system calls API. I got a brief taste of C (and operating systems) when I completed [Bradfield](https://bradfieldcs.com/)'s "Software Systems: Beyond the Abstractions" course in 2023. I wanted to build on that.

1. **Do something hard.** I wanted to do something challenging and practice good work habits in the process:

   - **Slow down.** I'm a fast reader of casual reading material, but technical reading is different - I really forced myself to slow down, read carefully, and understand what was being said.
   - **Avoid distractions.** I typically worked on this in the mornings. After checking my calendar for the day, I would turn off my phones and work, eat, or take breaks in the same room as my computer/desk. And when I was reading, my computer was away. Even before I started doing this, I don't think I was particularly distracted by my devices, but it was amazing how productive this way of working was!

# Progress

## Piece 1: Virtualization

| Chapter                                                                              | What To Do                                           | Status |
| ------------------------------------------------------------------------------------ | ---------------------------------------------------- | ------ |
| [Abstraction: Processes](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-intro.pdf)          | Run [process-run.py](cpu-intro)                      | ✅     |
| [Process API](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf)                       | Run [fork.py](cpu-api) and write some code           | ⚠️     |
| [Direct Execution](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-mechanisms.pdf)           | Write some code                                      |
| [Scheduling Basics](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-sched.pdf)               | Run [scheduler.py](cpu-sched)                        |
| [MLFQ Scheduling](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-sched-mlfq.pdf)            | Run [mlfq.py](cpu-sched-mlfq)                        |
| [Lottery Scheduling](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-sched-lottery.pdf)      | Run [lottery.py](cpu-sched-lottery)                  |
| [Multiprocessor Scheduling](http://www.cs.wisc.edu/~remzi/OSTEP/cpu-sched-multi.pdf) | Run [multi.py](cpu-sched-multi)                      |
| [Abstraction: Address Spaces](http://www.cs.wisc.edu/~remzi/OSTEP/vm-intro.pdf)      | Write some code                                      |
| [VM API](http://www.cs.wisc.edu/~remzi/OSTEP/vm-api.pdf)                             | Write some code                                      |
| [Relocation](http://www.cs.wisc.edu/~remzi/OSTEP/vm-mechanism.pdf)                   | Run [relocation.py](vm-mechanism)                    |
| [Segmentation](http://www.cs.wisc.edu/~remzi/OSTEP/vm-segmentation.pdf)              | Run [segmentation.py](vm-segmentation)               |
| [Free Space](http://www.cs.wisc.edu/~remzi/OSTEP/vm-freespace.pdf)                   | Run [malloc.py](vm-freespace)                        |
| [Paging](http://www.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf)                          | Run [paging-linear-translate.py](vm-paging)          |
| [TLBs](http://www.cs.wisc.edu/~remzi/OSTEP/vm-tlbs.pdf)                              | Write some code                                      |
| [Multi-level Paging](http://www.cs.wisc.edu/~remzi/OSTEP/vm-smalltables.pdf)         | Run [paging-multilevel-translate.py](vm-smalltables) |
| [Paging Mechanism](http://www.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys.pdf)            | Run [mem.c](vm-beyondphys)                           |
| [Paging Policy](http://www.cs.wisc.edu/~remzi/OSTEP/vm-beyondphys-policy.pdf)        | Run [paging-policy.py](vm-beyondphys-policy)         |
| [Complete VM](http://www.cs.wisc.edu/~remzi/OSTEP/vm-complete.pdf)                   | No homework (yet)                                    |

## Piece 2: Concurrency

| Chapter                                                                           | What To Do                               | Status |
| --------------------------------------------------------------------------------- | ---------------------------------------- | ------ |
| [Threads Intro](http://www.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)            | Run [x86.py](threads-intro)              |
| [Thread API](http://www.cs.wisc.edu/~remzi/OSTEP/threads-api.pdf)                 | Run [some C code](threads-api)           |
| [Locks](http://www.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf)                    | Run [x86.py](threads-locks)              |
| [Lock Usage](http://www.cs.wisc.edu/~remzi/OSTEP/threads-locks-usage.pdf)         | Write some code                          |
| [Condition Variables](http://www.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)         | Run [some C code](threads-cv)            |
| [Semaphores](http://www.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf)                | Read and write [some code](threads-sema) |
| [Concurrency Bugs](http://www.cs.wisc.edu/~remzi/OSTEP/threads-bugs.pdf)          | Run [some C code](threads-bugs)          |
| [Event-based Concurrency](http://www.cs.wisc.edu/~remzi/OSTEP/threads-events.pdf) | Write some code                          |

## Piece 3: Persistence

| Chapter                                                                                     | What To Do                                            | Status |
| ------------------------------------------------------------------------------------------- | ----------------------------------------------------- | ------ |
| [I/O Devices](http://www.cs.wisc.edu/~remzi/OSTEP/file-devices.pdf)                         | No homework (yet)                                     |
| [Hard Disk Drives](http://www.cs.wisc.edu/~remzi/OSTEP/file-disks.pdf)                      | Run [disk.py](file-disks)                             |
| [RAID](http://www.cs.wisc.edu/~remzi/OSTEP/file-raid.pdf)                                   | Run [raid.py](file-raid)                              |
| [FS Intro](http://www.cs.wisc.edu/~remzi/OSTEP/file-intro.pdf)                              | Write some code                                       |
| [FS Implementation](http://www.cs.wisc.edu/~remzi/OSTEP/file-implementation.pdf)            | Run [vsfs.py](file-implementation)                    |
| [Fast File System](http://www.cs.wisc.edu/~remzi/OSTEP/file-ffs.pdf)                        | Run [ffs.py](file-ffs)                                |
| [Crash Consistency and Journaling](http://www.cs.wisc.edu/~remzi/OSTEP/file-journaling.pdf) | Run [fsck.py](file-journaling)                        |
| [Log-Structured File Systems](http://www.cs.wisc.edu/~remzi/OSTEP/file-lfs.pdf)             | Run [lfs.py](file-lfs)                                |
| [Solid-State Disk Drives](http://www.cs.wisc.edu/~remzi/OSTEP/file-ssd.pdf)                 | Run [ssd.py](file-ssd)                                |
| [Data Integrity](http://www.cs.wisc.edu/~remzi/OSTEP/file-integrity.pdf)                    | Run [checksum.py](file-integrity) and Write some code |
| [Distributed Intro](http://www.cs.wisc.edu/~remzi/OSTEP/dist-intro.pdf)                     | Write some code                                       |
| [NFS](http://www.cs.wisc.edu/~remzi/OSTEP/dist-nfs.pdf)                                     | Write some analysis code                              |
| [AFS](http://www.cs.wisc.edu/~remzi/OSTEP/dist-afs.pdf)                                     | Run [afs.py](dist-afs)                                |
