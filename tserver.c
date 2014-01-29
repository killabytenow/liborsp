/*****************************************************************************
 * tserver.c
 *
 * A silly little debugger with RSP server.
 *
 * ---------------------------------------------------------------------------
 * stacktrace - Stacktrace printer.
 *   (C) 2008-2012 Gerardo García Peña <killabytenow@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2 of the License, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful, but WITHOUT
 *   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *   more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc., 51
 *   Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#include "log.h"

#include <signal.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

//#define __STACKTRACE_MSG_MACROS__
//#include "stacktrace.h"

sigset_t ss_sigchld;
#define SIGCHLD_SIGPROCMASK(x) { if(sigprocmask(x, &ss_sigchld, NULL) \
                                   FAT("Cannot %s SIGCHLD signal.", ##x); }
#define SIGCHLD_BLOCK()        SIGCHLD_SIGPROCMASK(SIG_BLOCK)
#define SIGCHLD_UNBLOCK()      SIGCHLD_SIGPROCMASK(SIG_UNBLOCK)
struct __traced_pid {
	int pid;
} **traced_pids_list = NULL;
int traced_pids_list_s = 0;
int traced_pids_list_n = 0;
char *finish_reason_str[] = { "dumped/aborted", "killed", "exited" };

static void stacktrace_ptrace_list_add(int pid)
{
	int i;

	i = traced_pids_list_n;
	if(++traced_pids_list_n >= traced_pids_list_s) {
		traced_pids_list_s += sizeof(struct __traced_pid *) * 32;
		if(!(traced_pids_list = realloc(traced_pids_list, traced_pids_list_s)))
			FAT_ERRNO("cannot grow up traced process list.");
	}
	if(!(traced_pids_list[i] = malloc(sizeof(struct __traced_pid))))
		FAT_ERRNO("cannot alloc new __traced_pid struct");
	traced_pids_list[i]->pid = pid;
	traced_pids_list[i+1] = 0;
}

static struct __traced_pid *stacktrace_ptrace_list_get(int pid)
{
	int i;
	for(i = 0; traced_pids_list[i] && traced_pids_list[i]->pid != pid; i++)
		;
	return traced_pids_list[i];
}

static struct __traced_pid *stacktrace_ptrace_list_del(int pid)
{
	int i;
	struct __traced_pid *r;

	/* search pid */
	for(i = 0; traced_pids_list[i] && traced_pids_list[i]->pid != pid; i++)
		;
	r = traced_pids_list[i];

	/* delete pid */
	for(; i < traced_pids_list_n; i++)
		traced_pids_list[i] = traced_pids_list[i+1];
	traced_pids_list_n--;

	return r;
}

static struct __traced_pid *stacktrace_ptrace_list_pop(void)
{
	return traced_pids_list_n
		? traced_pids_list[--traced_pids_list_n]
		: 0;
}

static void stacktrace_ptrace_cleanup(void)
{
	struct __traced_pid *p;

	MSG("Detaching");
	while((p = stacktrace_ptrace_list_pop()) > 0)
		ptrace(PTRACE_DETACH, p->pid, NULL, NULL);
	MSG("Detached from all debugged processes.");
}

static void stacktrace_ptrace_signal_proxy(int signum, siginfo_t *si, void *ucontext)
{
	if(traced_pids_list_n > 0) {
		MSG("Sending %d to %d", signum, traced_pids_list[0]->pid);
		if(ptrace(PTRACE_CONT, traced_pids_list[0]->pid, NULL, signum) < 0)
			ERR_ERRNO("ptrace failed");
	} else {
		FAT("There is not any process being traced. Aborting.");
	}
}

static void stacktrace_ptrace_tracer(void)
{
	int r;
	struct sigaction sa;
	struct __traced_pid *p;

	/* before exit, do tracer cleanup */
	atexit(stacktrace_ptrace_cleanup);

	/* ignore some signals, forfomer others */
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	if(sigaction(SIGTTOU, &sa, NULL) < 0
	|| sigaction(SIGTTIN, &sa, NULL) < 0)
		ERR_ERRNO("Cannot install SIG_IGN signal handler");

	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = stacktrace_ptrace_signal_proxy;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGSTOP, &sa, NULL);
	//sigaddset(&blocked_set, SIGHUP);
	//sigaddset(&blocked_set, SIGINT);
	//sigaddset(&blocked_set, SIGQUIT);
	//sigaddset(&blocked_set, SIGPIPE);
	//sigaddset(&blocked_set, SIGTERM);
	//sa.sa_sigaction = interrupt;
	//sigaction(SIGPIPE, &sa, NULL);

	/* close stdin and stdout file descriptors */
	close(0);
	close(1);

	/* start tracer */
	while(traced_pids_list_n > 0) {
		int event, sdelivered;
		int finish_reason = 0;
		siginfo_t si;

		while(waitid(P_ALL, 0, &si, WEXITED | WSTOPPED)) {
			if(errno == EINTR)
				continue;
			ERR_ERRNO("Error calling waitid");
		}

		if((p = stacktrace_ptrace_list_get(si.si_pid)) == NULL) {
			ERR("Unknown process wait'd (%d with status %d).", si.si_pid, si.si_status);
			exit(1);
			continue;
		}

		//MSG("WIFEXITED(%d)", WIFEXITED(si.si_status));
		//MSG("WEXITSTATUS(%d)", WEXITSTATUS(si.si_status));
		//MSG("WIFSIGNALED(%d)", WIFSIGNALED(si.si_status));
		//MSG("WTERMSIG(%d)", WTERMSIG(si.si_status));
		//MSG("WCOREDUMP(%d)", WCOREDUMP(si.si_status));
		//MSG("WIFSTOPPED(%d)", WIFSTOPPED(si.si_status));
		//MSG("WSTOPSIG(%d)", WSTOPSIG(si.si_status));

		switch(si.si_code) {
		case CLD_EXITED: /* child has exited */
			finish_reason++;
		case CLD_KILLED: /* child was killed */
			finish_reason++;
		case CLD_DUMPED: /* child terminated abnormally */
			MSG("Child %d %s with code %d.",
			      p->pid, finish_reason_str[finish_reason], si.si_status);
			stacktrace_ptrace_list_del(p->pid);
			break;
		case CLD_STOPPED: /* child has stopped */
		case CLD_TRAPPED: /* traced child has trapped */
			///* set STACKTRACE_PTRACE to CHILD */
			//if(setenv("STACKTRACE_PTRACE", "CHILD", 1) < 0)
			//  FAT_ERRNO("Cannot set STACKTRACE_PTRACE env var to CHILD");
			ptrace(PTRACE_GETEVENTMSG, p->pid, NULL, &event);
			sdelivered = WIFSIGNALED(si.si_status) && WTERMSIG(si.si_status) != SIGTRAP
				       ? WTERMSIG(si.si_status)
				       : 0;
			if(si.si_code == CLD_STOPPED || sdelivered)
				MSG("%s (status = %d, signal = %d, event = %d, signal_delivered = %d)",
					si.si_code == CLD_STOPPED ? "cld_stopped" : "cld_trapped",
					si.si_status, si.si_signo, event, sdelivered);
			switch(sdelivered) {
			case SIGILL:
			case SIGSEGV:
				MSG("HIJO TOPUTA %d", sdelivered);
			default:
				r = ptrace(PTRACE_SYSCALL, p->pid, NULL, sdelivered);
				if(r < 0)
					FAT_ERRNO("PTRACE_CONT failed");
			}
			break;
		case CLD_CONTINUED: /* stopped child has continued (since Linux 2.6.9) */
			MSG("cld_continued");
			break;
		default:
			ERR("Unknown si->si_code %d.", si.si_code);
			exit(1);
		}
	}
	MSG("ALL DEBUGGED PROCESSES DIED.");
	exit(1);
}

static void stacktrace_ptrace_child(void)
{
	if(ptrace(PTRACE_TRACEME, 0, (char *) 1, NULL) < 0)
		FAT_ERRNO("ptrace(PTRACE_TRACEME) failed");
	MSG("PTRACE_TRACEME'd and going to STOP.");
	kill(getpid(), SIGSTOP);
}

int main(int argc, char **argv)
{
	int pid;

	if(argc < 2)
		FAT("I need a program and its arguments.");

	/* fork in debugger and debugged processes */
	if((pid = fork()) < 0)
		FAT_ERRNO("Fork failed");

	if(pid) {
		stacktrace_ptrace_list_add(pid);
		stacktrace_ptrace_tracer();
	} else {
		MSG("Launching '%s'", argv[1]);
		stacktrace_ptrace_child();
		execv(argv[1], argv + 1);
	}
	exit(1);
}

