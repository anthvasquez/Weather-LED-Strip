#include <stdio.h>
#include <stdlib.h>
#include "signal_utils.h"

void signal_sethandler(int sig, sa_sigaction_t handler)
{
		sigset_t emptymask;

		sigemptyset(&emptymask);
		struct sigaction sa = {
				.sa_sigaction = handler,
				.sa_mask = emptymask,
				.sa_flags = SA_RESTART | SA_SIGINFO
		};

		if(sigaction(sig, &sa, NULL) != 0)
		{
				printf("sigaction failed for signal %d\nFATAL ERROR\n", sig);
				exit(EXIT_FAILURE);
		}
}

/**
 * Blocks or unblocks a signal
 * Returns whether or not the signal was already blocked
 */
bool mask_signal(int sig, int how)
{
		sigset_t mask, omask;
		sigemptyset(&mask);
		sigaddset(&mask, sig);
		if(sigprocmask(how, &mask, &omask) != 0)
				printf("Failed to mask signal %d\n", sig);
		return sigismember(&omask, sig);
}
