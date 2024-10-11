# Chapter 7: cpu-sched thoughts

## Question 1

> Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.

As a quick reminder:

- Turnaround time = completion time - arrival time
- Response time = first time job is run - arrival time

### SJF = Shortest Job First

```bash
$ ./scheduler.py -p SJF -j 3 -l 200,200,200
```

Since all three jobs have the same length, the SJF scheduler will run them in the order they arrive. The times are:

1. Job 0: Response time = 0, Turnaround time = 200
2. Job 1: Response time = 200, Turnaround time = 400
3. Job 2: Response time = 400, Turnaround time = 600

Average response time = 200, average turnaround time = 400

### FIFO = First In First Out

Everything should be exactly the same in this case, since the jobs are all the same length and arrive at the same time.

## Question 2

> Now do the same but with jobs of different lengths: 100, 200, and 300.

### SJF = Shortest Job First

```bash
$ ./scheduler.py -p SJF -j 3 -l 100,200,300
```

The SJF scheduler will run the jobs in the order of shortest to longest. The times are:

1. Job 0: Response time = 0, Turnaround time = 100
2. Job 1: Response time = 100, Turnaround time = 300
3. Job 2: Response time = 300, Turnaround time = 600

Average response time = 133.33, average turnaround time = 333.33

Note that with the SJF scheduler, the arrival times are not relevant. The scheduler will always run the shortest job first.

### FIFO = First In First Out

Assuming the jobs arrive in the order 100, 200, 300, the times will be exactly the same as above.

But if the jobs arrive in a different order, then we expect the averages to increase. For example, if the jobs arrive in the order 200, 300, 100, then the times will be:

1. Job 0: Response time = 0, Turnaround time = 200
2. Job 1: Response time = 200, Turnaround time = 500
3. Job 2: Response time = 500, Turnaround time = 600

Average response time = 233.33, average turnaround time = 433.33

## Question 3

> Now do the same, but also with the RR scheduler and a time-slice of 1.

This is more interesting. The jobs are 100, 200, and 300. And the Round-Robin scheduler will alternate between them, giving each one a time-slice of 1, until they're done.

```bash
./scheduler.py -p RR -q 1 -j 3 -l 100,200,300
```

Until the first job completes, each job gets 1 second of time for every 3 seconds that go by. Thus, the shortest job will finish first. And it will happen after 99\*3 + 1 = 298 seconds.

Once the shortest job is done, each job gets 1 second of time for every 2 seconds that go by. Thus, at time 298 + 2 = 300, the first 100 seconds of work will be done on Jobs 1 and 2. Then we do 99 \* 2 = 198 seconds of work back and forth between Jobs 1 and 2. Then the next second of work will complete Job 1, for a turnaround time of 300 + 198 + 1 = 499.

Finally, the longest job will be done when all 600 units of work have been done. And since 1 unit of work always happened per second, that will take 600 seconds.

1. Job 0: Response time = 0, Turnaround time = 298
2. Job 1: Response time = 1, Turnaround time = 499
3. Job 2: Response time = 2, Turnaround time = 600

Average response time = 1, average turnaround time = 465.67

Thus, the Round-Robin scheduler has a much shorter average response time but a longer average turnaround time.

## Question 4

> For what types of workloads does SJF deliver the same turnaround times as FIFO?

It seems like the most obvious way that could happen is if the jobs arrive in non-decreasing order of length. Otherwise, FIFO will run a longer job earlier in the sequence, pushing back the turnaround times of the jobs that come after that longer job.

In the simulator above, it seems like all jobs arrive at the same time, but you could also imagine SJF and FIFO delivering the same turnaround times if the arrivals are staggered such that a job always completes before the next job arrives. In this case, your scheduling policy doesn't matter at all - the jobs will always complete as soon as possible, since there's no overlap.

## Question 5

> For what types of workloads and quantum lengths does SJF deliver the same response times as RR?

This will be tough for SJF to do - we saw above that the response times in RR are a multiple of the quantum length, since we start making progress on each job relatively quickly in RR. This is in stark contrast to SJF, which runs jobs, shorter ones first, to completion before switching to the next job.

If all jobs had the same length and the quantum length was equal to that length, then SJF would deliver the same response times as RR. But that's a pretty contrived scenario.

## Question 6

> What happens to response time with SJF as the job lengths increase? Can you use the simulator to demonstrate the trend?

As job lengths increase, the response time will also increase. This is because the scheduler will always run the shortest job first, and longer jobs will have to wait longer to start running.

## Question 7

> What happens to response time with RR as quantum lengths increase? Can you write an equation that gives the worst-case response time, given $N$ jobs?

Generally speaking, as quantum lengths increase, the response time will also increase. This is because the scheduler will spend more time on each job before switching to the next one.

The worst-case response time will happen for the last job to get a slice of time from the RR scheduler, and all of the jobs will take at least one slice of time to complete, i.e. no early finishing. If there are $N$ jobs and the quantum length is $q$, then the last job will start its first slice of time at time $(N-1)*q$.

We can see that this expression is linear in $q$, so sure enough, as quantum lengths increase, the response time will also increase.
