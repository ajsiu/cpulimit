CPULIMIT
========

Cpulimit is a tool which limits the CPU usage of a process (expressed in percentage, not in CPU time). It is useful to control batch jobs, when you don't want them to eat too many CPU cycles. The goal is prevent a process from running for more than a specified time ratio. It does not change the nice value or other scheduling priority settings, but the real CPU usage. Also, it is able to adapt itself to the overall system load, dynamically and quickly.
The control of the used CPU amount is done sending SIGSTOP and SIGCONT POSIX signals to processes.
All the children processes and threads of the specified process will share the same percentage of CPU.

Forked from:
https://github.com/opsengine/cpulimit


Changes
--------------------------

cpulimit now fully supports M1 Mac.


Install instructions
--------------------

On Linux:

    $ make
    # cp src/cpulimit /usr/bin

On OS X:

    $ make
    # cp src/cpulimit /usr/local/bin

On FreeBSD:

    $ gmake
    # cp src/cpulimit /usr/bin

Run unit tests:

    $ ./tests/process_iterator_test


Contributions
-------------

If you have any changes, bug fixes, or additional features your best bet is to fork it. I'm not really committed to maintaining this repo.

Todo
----

Add the ability to calculate the timeslice (quantum) of a process.
