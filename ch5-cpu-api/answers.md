# cpi-api thoughts

## fork.py

After reading through `README-fork.md`, I spent some time running `fork.py` with different inputs. The one subtlety that this helped me to appreciate was the fact that, when a parent process exits and it still has living children, those living children become children of the `init` process. This is a simple concept, but it's easy to overlook when you're just reading about it. For example:

```
$ ./fork.py -A a+b,b+c,c+d,d+e,c- -c

                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: c forks d
                               a
                               └── b
                                   └── c
                                       └── d
Action: d forks e
                               a
                               └── b
                                   └── c
                                       └── d
                                           └── e
Action: c EXITS
                               a
                               ├── b
                               ├── d
                               └── e
```

Here, we can see that when `c` exits, `d` and `e` become children of `init`, losing the fact that they were children of `c` **and** that `e` was a child of `d`. Good to know.

### Q2: Playing with `fork_percentage`

If we run the simulator with a large number of actions (`-a 100`) and vary the `fork_percentage`, I make the following predictions:

- With a low fork percentage, many of the actions will be exits. Since we learned above that an exit causes all children to become children of `init`, we should see a very shallow tree. It should also be a very small tree, since the many exits will close any existing processes.

- With a high fork percentage, many of the actions will be forks. Depending on which process is chosen to fork, we could see a very deep tree, or a very wide tree.

Let's check it out!

### Q3: Reverse, Reverse!

If we use the `-t` flag, we are given the process tree and must reverse-engineer the actions that were taken. This should be pretty straightforward, since the processes use increasing alphabetical names as they are created. Let's see how it goes!

```sh
$ ./fork.py -a 8 -s 27 -t

                           Process Tree:
                               a

Action? *A forks B*
                               a
                               └── b
Action? *B forks C*
                               a
                               └── b
                                   └── c
Action? *B forks D*
                               a
                               └── b
                                   ├── c
                                   └── d
Action? *B exists, children become children of init*
                               a
                               ├── c
                               └── d
Action? *D forks E*
                               a
                               ├── c
                               └── d
                                   └── e
Action? *A forks F*
                               a
                               ├── c
                               ├── d
                               │   └── e
                               └── f
Action? *E exits*
                               a
                               ├── c
                               ├── d
                               └── f
Action? *D exits*
                               a
                               ├── c
                               └── f
```

Pretty straightforward, as predicted.

### Other Questions

The other questions were pretty straightforward.

For example, they suggested that I run exactly the sequence I ran to investigate what happens to children when their parent exits.

They also pointed out that, with both the `-t` and `-F` flags set, I could see the final process tree and then compute the actions that led to that tree. But when I played with this, it occurred to me that most of the time, you won't know the exact _sequence_ of actions that occurred, even if you should be able to specify which actions took place.
