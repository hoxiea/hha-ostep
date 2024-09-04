# OSTEP Chapter 4 Answers

- Author: Hamilton Hoxie Ackerman ([hhackerman.com](https://hhackerman.com))
- Date: August 30, 2024
- Question Source: Chapter 4 of [OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/): "The Abstraction: The Process"
- Code Source: [Original GitHub repo](https://github.com/remzi-arpacidusseau/ostep-homework/tree/master/cpu-intro)

## Question 1

> Run `process-run.py` with the following flags: `-l 5:100,5:100`. What should the CPU utilization be? Why do you know this? Use the `-c` and `-p` flags to find out if you were right.

Running `./process-run.py -l 5:100,5:100` should start two processes, each with 5 instructions, with 100% of those instructions being CPU instructions.

Since our processor only switches processes when the current process is finished or issues an I/O, and since neither of these processes issues an I/O, the CPU utilization should be 100%: it will run all 5 instructions of the first process, then all 5 instructions of the second process, without any pause or time spent on I/O.

Sure enough, we can see this as follows:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -l 5:100,5:100 -p -c
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1
  2        RUN:cpu         READY             1
  3        RUN:cpu         READY             1
  4        RUN:cpu         READY             1
  5        RUN:cpu         READY             1
  6           DONE       RUN:cpu             1
  7           DONE       RUN:cpu             1
  8           DONE       RUN:cpu             1
  9           DONE       RUN:cpu             1
 10           DONE       RUN:cpu             1

Stats: Total Time 10
Stats: CPU Busy 10 (100.00%)         <- 100% utilization!
Stats: IO Busy  0 (0.00%)
```

## Question 2

> Now run with these flags: `./process-run.py -l 4:100,1:0`. These flags specify one process with 4 instructions (all to use the CPU), and one that simply issues an I/O and waits for it to be done. How long does it take to complete both processes?

We can see in the following snippet of Python code that the default time for an I/O operation is 5 time units:

```python
parser.add_option(
    "-L",
    "--iolength",
    default=5,                         # <- 5 time units
    help="how long an IO takes",
    action="store",
    type="int",
    dest="io_length",
)
```

(We can also see this in the sample output provided in the README file, but as I tell my students, "There's no black magic in how computers work." I'd like to be better about finding the source of behavior in the code and understanding it myself, so here we are...)

And looking at the I/O traces in the README file, we can see that in addition to these 5 time units of `BLOCKED` time, there are two additional instructions required for an I/O operation: one to issue the I/O and one to complete it.

Thus, I expect this program to require:

- 4 time units for the first process to complete
- 5 + 2 = 7 time units for the second process to complete

For a total of **11 time units**.

Let's check our work:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -l 4:100,1:0 -p -c
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1
  2        RUN:cpu         READY             1
  3        RUN:cpu         READY             1
  4        RUN:cpu         READY             1
  5           DONE        RUN:io             1
  6           DONE       BLOCKED                           1
  7           DONE       BLOCKED                           1
  8           DONE       BLOCKED                           1
  9           DONE       BLOCKED                           1
 10           DONE       BLOCKED                           1
 11*          DONE   RUN:io_done             1

Stats: Total Time 11            <- Bingo!
Stats: CPU Busy 6 (54.55%)
Stats: IO Busy  5 (45.45%)
```

## Question 3

> Switch the order of the processes: `./process-run.py -l 1:0,4:100`. What happens now? Does switching the order matter? Why?

Switching the order will absolutely matter in this case, because while the first process's I/O operation is blocking the CPU, the second process will be able to execute its CPU instructions.

This should require:

- 1 time unit to initialize P1's I/O operation (`RUN:io`)
- 4 time units for P2 to complete its CPU instructions, during which P1 completes 80% of its 5-unit I/O operation
- 1 time unit to get P1's I/O operation to 100% complete
- 1 time unit for P1 to handle its `RUN:io_done` state

For a total of **7 time units**.

Let's check our work:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -l 1:0,4:100 -p -c
Time        PID: 0        PID: 1           CPU           IOs
  1         RUN:io         READY             1
  2        BLOCKED       RUN:cpu             1             1
  3        BLOCKED       RUN:cpu             1             1
  4        BLOCKED       RUN:cpu             1             1
  5        BLOCKED       RUN:cpu             1             1
  6        BLOCKED          DONE                           1
  7*   RUN:io_done          DONE             1

Stats: Total Time 7            <- Bingo!
Stats: CPU Busy 6 (85.71%)
Stats: IO Busy  5 (71.43%)
```

Thus, we can see that **the order of the processes does matter!** In particular, by starting the I/O-bound process first, we can allow the CPU-bound process to execute its CPU instructions while the I/O operation is blocking the CPU. This reduces the total time required to complete both processes from 11 time units to 7 units, a 36% reduction in time (assuming all operations take the same amount of time).

## Question 4

> We'll now explore some of the other flags. One important flag is -S, which determines how the system reacts when a process issues an I/O. With the flag set to `SWITCH_ON_END`, the system will NOT switch to another process while one is doing I/O, instead waiting until the process is completely finished. What happens if you run the following two processes (`-l 1:0,4:100 -c -S SWITCH_ON_END`), one doing I/O and the other doing CPU work?

This is the same setup as in Question 3, but with the `-S SWITCH_ON_END` flag set. If we really wait until the process is completely finished before switching, we should lose the benefits of starting the I/O-bound process first, since the process won't take advantage of the blocked time to make progress on the CPU-bound process. Thus, I think we'll be back to a total time of 11 units.

Let's check our work:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -l 1:0,4:100 -p -c -S SWITCH_ON_END
Time        PID: 0        PID: 1           CPU           IOs
  1         RUN:io         READY             1
  2        BLOCKED         READY                           1
  3        BLOCKED         READY                           1
  4        BLOCKED         READY                           1
  5        BLOCKED         READY                           1
  6        BLOCKED         READY                           1
  7*   RUN:io_done         READY             1
  8           DONE       RUN:cpu             1
  9           DONE       RUN:cpu             1
 10           DONE       RUN:cpu             1
 11           DONE       RUN:cpu             1

Stats: Total Time 11           <- Bingo!
Stats: CPU Busy 6 (54.55%)
Stats: IO Busy  5 (45.45%)
```

## Question 5

> Now, run the same processes, but with the switching behavior set to switch to another process whenever one is WAITING for I/O (`-l 1:0,4:100 -c -S SWITCH_ON_IO`). What happens now?

This is the default behavior, so we should see the same behavior as in Question 3, where the CPU-bound process is able to execute its CPU instructions while the I/O-bound process is blocked. Thus, we should see a total time of 7 units.

Let's check our work:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -l 1:0,4:100 -p -c -S SWITCH_ON_IO
Time        PID: 0        PID: 1           CPU           IOs
  1         RUN:io         READY             1
  2        BLOCKED       RUN:cpu             1             1
  3        BLOCKED       RUN:cpu             1             1
  4        BLOCKED       RUN:cpu             1             1
  5        BLOCKED       RUN:cpu             1             1
  6        BLOCKED          DONE                           1
  7*   RUN:io_done          DONE             1

Stats: Total Time 7            <- Bingo!
Stats: CPU Busy 6 (85.71%)
Stats: IO Busy  5 (71.43%)
```

And why is this the default behavior? Practically, because you might as well use the CPU while you're waiting for I/O to complete. And in terms of the code, we see the default behavior being set here:

```python
parser.add_option(
    "-S",
    "--switch",
    default="SWITCH_ON_IO",  # <- default behavior
    help="when to switch between processes: SWITCH_ON_IO, SWITCH_ON_END",
    action="store",
    type="string",
    dest="process_switch_behavior",
)
```

The `scheduler` class constructor takes an arguments `process_switch_behavior`, which gets stored in `self.process_switch_behavior`. This gets used here:

```python
    # if this is an IO start instruction, switch to waiting state
    # and add an io completion in the future
    if instruction_to_execute == DO_IO:
        self.move_to_wait(STATE_RUNNING)
        self.io_finish_times[self.curr_proc].append(
            clock_tick + self.io_length + 1
        )
        if self.process_switch_behavior == SCHED_SWITCH_ON_IO:
            self.next_proc()

    # ENDCASE: check if currently running thing is out of instructions
    self.check_if_done()
```

## Question 6

> One other important behavior is what to do when an I/O completes. With `-I IO_RUN_LATER`, when an I/O operation completes, the process that issued it is not necessarily run right away; rather, whatever was running at the time keeps running. What happens when you run this combination of processes? `./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p`.

With this command, we are going to start four processes:

- P1 will issue three operations, all of them I/O
- P2, P3, and P4 will each issue five operations, all of them requiring one time unit worth of CPU time to complete.

Since we set `SWITCH_ON_IO`, we should expect to switch from P1 to P2 after P1 has first issued its `RUN:io` instruction. However, since we set `IO_RUN_LATER`, it's less clear when P1 will be given time to complete its I/O operations: will we switch back to P1 after P2 is done, or will we wait until P3 is done, or P4?

(It's less clear because in the README, it just says that we will switch back to a process that's finished being BLOCKED "when it is natural to".)

Let's find out:

```sh
➜  $ ./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p
Time       PID: 0      PID: 1     PID: 2     PID: 3     CPU    IOs
  1        RUN:io       READY      READY      READY       1
  2       BLOCKED     RUN:cpu      READY      READY       1      1
  3       BLOCKED     RUN:cpu      READY      READY       1      1
  4       BLOCKED     RUN:cpu      READY      READY       1      1
  5       BLOCKED     RUN:cpu      READY      READY       1      1
  6       BLOCKED     RUN:cpu      READY      READY       1      1
  7*        READY        DONE    RUN:cpu      READY       1
  8         READY        DONE    RUN:cpu      READY       1
  9         READY        DONE    RUN:cpu      READY       1
 10         READY        DONE    RUN:cpu      READY       1
 11         READY        DONE    RUN:cpu      READY       1
 12         READY        DONE       DONE    RUN:cpu       1
 13         READY        DONE       DONE    RUN:cpu       1
 14         READY        DONE       DONE    RUN:cpu       1
 15         READY        DONE       DONE    RUN:cpu       1
 16         READY        DONE       DONE    RUN:cpu       1
 17   RUN:io_done        DONE       DONE       DONE       1
 18        RUN:io        DONE       DONE       DONE       1
 19       BLOCKED        DONE       DONE       DONE              1
 20       BLOCKED        DONE       DONE       DONE              1
 21       BLOCKED        DONE       DONE       DONE              1
 22       BLOCKED        DONE       DONE       DONE              1
 23       BLOCKED        DONE       DONE       DONE              1
 24*  RUN:io_done        DONE       DONE       DONE       1
 25        RUN:io        DONE       DONE       DONE       1
 26       BLOCKED        DONE       DONE       DONE              1
 27       BLOCKED        DONE       DONE       DONE              1
 28       BLOCKED        DONE       DONE       DONE              1
 29       BLOCKED        DONE       DONE       DONE              1
 30       BLOCKED        DONE       DONE       DONE              1
 31*  RUN:io_done        DONE       DONE       DONE       1

Stats: Total Time 31
Stats: CPU Busy 21 (67.74%)
Stats: IO Busy  15 (48.39%)
```

That's what I was afraid of: rather than nicely interleaving the I/O operations with the CPU operations, we only manage to interleave P1's first I/O operation with P2's CPU operations. After that, we run P3 and P4 to completion before returning to P1 to finish its first I/O operation (17) and then run its remaining two I/O operations (18-31).

This happens because of how `scheduler.next_proc` is implemented. It essentially considers process ids in order, so without the explicit logic to switch back to P1 after P2 is done, we just keep going through the process ids in order:

```python
    def next_proc(self, pid=-1):
        if pid != -1:
            self.curr_proc = pid
            self.move_to_running(STATE_READY)
            return
        # These two loops cover all processes, starting with the one
        # after the current process and wrapping around
        for pid in range(self.curr_proc + 1, len(self.proc_info)):
            if self.proc_info[pid][PROC_STATE] == STATE_READY:
                self.curr_proc = pid
                self.move_to_running(STATE_READY)
                return
        for pid in range(0, self.curr_proc + 1):
            if self.proc_info[pid][PROC_STATE] == STATE_READY:
                self.curr_proc = pid
                self.move_to_running(STATE_READY)
                return
        return
```

System resources are _not_ being utilized effectively if we do it this way. We can see this partly from the stats, but of course, it's hard to know that something better is possible without thinking through it. In this case, though, there's a _much_ more effective way to run these processes: interleave P1's three I/O operations with the three CPU-bound processes. This would require:

- 2 \* 3 = 6 time units for P1 to start and finish its 3 I/O operations
- 5 time units for P2 to finish its CPU operations
- 5 time units for P3 to finish its CPU operations
- 5 time units for P4 to finish its CPU operations

For a total of **21 time units**. This would be a 32% reduction in time compared to the 31 time units we required.

## Question 7

> Now run the same processes, but with the `-I IO_RUN_IMMEDIATE` set, which immediately runs the process that issued the I/O operation. How does this behavior differ? Why might running a process that just completed an I/O again be a good idea?

This should produce the behavior we just described: P1's I/O operations should be interleaved with the CPU operations of P2, P3, and P4. This should require 21 time units, as we just discussed.

Let's check it out:

```sh
➜  $ ./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_IMMEDIATE -c -p

Time       PID: 0      PID: 1     PID: 2     PID: 3    CPU    IOs
  1        RUN:io       READY      READY      READY      1
  2       BLOCKED     RUN:cpu      READY      READY      1      1
  3       BLOCKED     RUN:cpu      READY      READY      1      1
  4       BLOCKED     RUN:cpu      READY      READY      1      1
  5       BLOCKED     RUN:cpu      READY      READY      1      1
  6       BLOCKED     RUN:cpu      READY      READY      1      1
  7*  RUN:io_done        DONE      READY      READY      1
  8        RUN:io        DONE      READY      READY      1
  9       BLOCKED        DONE    RUN:cpu      READY      1      1
 10       BLOCKED        DONE    RUN:cpu      READY      1      1
 11       BLOCKED        DONE    RUN:cpu      READY      1      1
 12       BLOCKED        DONE    RUN:cpu      READY      1      1
 13       BLOCKED        DONE    RUN:cpu      READY      1      1
 14*  RUN:io_done        DONE       DONE      READY      1
 15        RUN:io        DONE       DONE      READY      1
 16       BLOCKED        DONE       DONE    RUN:cpu      1      1
 17       BLOCKED        DONE       DONE    RUN:cpu      1      1
 18       BLOCKED        DONE       DONE    RUN:cpu      1      1
 19       BLOCKED        DONE       DONE    RUN:cpu      1      1
 20       BLOCKED        DONE       DONE    RUN:cpu      1      1
 21*  RUN:io_done        DONE       DONE       DONE      1

Stats: Total Time 21
Stats: CPU Busy 21 (100.00%)
Stats: IO Busy  15 (71.43%)
```

Sure enough, switching back to a process once its I/O operation is done is a good idea here, and probably in general. If a process performed one I/O operation, it might perform more at some point in the future, so it's a good idea to give it a chance to run again. That involves switching back to it once the I/O is done.

## Question 8

> Now run with some randomly generated processes: `-s 1 -l 3:50,3:50` or `-s 2 -l 3:50,3:50`. See if you can predict how the trace will turn out. What happens if you change the `-I` flag to `IO_RUN_IMMEDIATE` or `IO_RUN_LATER`? What happens if you change the `-S` flag to `SWITCH_ON_IO` or `SWITCH_ON_END`?

Consider the following run:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -s 1 -l 3:50,3:50
Produce a trace of what would happen when you run these processes:
Process 0
  cpu
  io
  io_done
  io
  io_done

Process 1
  cpu
  cpu
  cpu
```

This seems reasonable: we're running two processes, each with three instructions. And the instructions are equally likely to be `cpu` or `I/O` instructions. Using the seed provided, Process 0 had one `cpu` and two `I/O` instructions, while Process 1 had three `cpu` instructions. Given the default values for `-I` and `-S`, I would expect:

- 1 unit for P0's `cpu` instruction
- 1 unit for P0's first `RUN:io` instruction
- 3 units for P1's `cpu` instructions
- 2 units of waiting for P0's I/O operations to complete
- 1 unit for P0's `RUN:io_done` instruction

For a total of **8 time units**. Let's check it out:

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -s 1 -l 3:50,3:50 -c -p
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1
  2         RUN:io         READY             1
  3        BLOCKED       RUN:cpu             1             1
  4        BLOCKED       RUN:cpu             1             1
  5        BLOCKED       RUN:cpu             1             1
  6        BLOCKED          DONE                           1
  7        BLOCKED          DONE                           1
  8*   RUN:io_done          DONE             1
  9         RUN:io          DONE             1
 10        BLOCKED          DONE                           1
 11        BLOCKED          DONE                           1
 12        BLOCKED          DONE                           1
 13        BLOCKED          DONE                           1
 14        BLOCKED          DONE                           1
 15*   RUN:io_done          DONE             1

Stats: Total Time 15
Stats: CPU Busy 8 (53.33%)
Stats: IO Busy  10 (66.67%)
```

Duh, we forgot the fact that P0 has a second I/O instruction, which will take 7 additional time units, for a total of 15. But otherwise, we were correct.

If we use `-S SWITCH_ON_END`, then no interleaving will occur. In this case, `cpu` instructions take 1 time unit and `I/O` instructions take 7 time units, so we're in for 1*4 + 7*2 = 18 time units. This is three more than above, which makes sense: we lose those 3 time units of interleaving.

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -s 1 -l 3:50,3:50 -c -p -S SWITCH_ON_END
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1
  2         RUN:io         READY             1
  3        BLOCKED         READY                           1
  4        BLOCKED         READY                           1
  5        BLOCKED         READY                           1
  6        BLOCKED         READY                           1
  7        BLOCKED         READY                           1
  8*   RUN:io_done         READY             1
  9         RUN:io         READY             1
 10        BLOCKED         READY                           1
 11        BLOCKED         READY                           1
 12        BLOCKED         READY                           1
 13        BLOCKED         READY                           1
 14        BLOCKED         READY                           1
 15*   RUN:io_done         READY             1
 16           DONE       RUN:cpu             1
 17           DONE       RUN:cpu             1
 18           DONE       RUN:cpu             1

Stats: Total Time 18
Stats: CPU Busy 8 (44.44%)
Stats: IO Busy  10 (55.56%)
```

If we use `-I IO_RUN_LATER`, then we'll be back to 15 time units, since the only process that won't be done after Process 1 finishes is Process 0, so we effectively switch back to our I/O process as soon as possible anyway.

```sh
➜  cpu-intro git:(master) ✗ ./process-run.py -s 1 -l 3:50,3:50 -c -p -I IO_RUN_LATER
Time        PID: 0        PID: 1           CPU           IOs
  1        RUN:cpu         READY             1
  2         RUN:io         READY             1
  3        BLOCKED       RUN:cpu             1             1
  4        BLOCKED       RUN:cpu             1             1
  5        BLOCKED       RUN:cpu             1             1
  6        BLOCKED          DONE                           1
  7        BLOCKED          DONE                           1
  8*   RUN:io_done          DONE             1
  9         RUN:io          DONE             1
 10        BLOCKED          DONE                           1
 11        BLOCKED          DONE                           1
 12        BLOCKED          DONE                           1
 13        BLOCKED          DONE                           1
 14        BLOCKED          DONE                           1
 15*   RUN:io_done          DONE             1

Stats: Total Time 15
Stats: CPU Busy 8 (53.33%)
Stats: IO Busy  10 (66.67%)
```

What a cool little simulator!
