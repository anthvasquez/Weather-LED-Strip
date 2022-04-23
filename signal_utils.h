#ifndef __SIGNAL_UTILS_H
#define __SIGNAL_UTILS_H
#include <signal.h>
#include <stdbool.h>

typedef void (*sa_sigaction_t)(int, siginfo_t *, void *);
void signal_sethandler(int sig, sa_sigaction_t handler);

/**
 * Blocks or unblocks a signal
 * Returns whether or not the signal was already blocked
 */
bool mask_signal(int sig, int how);

#endif //__SIGNAL_UTILS_H
