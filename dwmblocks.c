#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>
#ifndef NO_X
#include<X11/Xlib.h>
#endif
#ifdef __OpenBSD__
#define SIGPLUS			SIGUSR1+1
#define SIGMINUS		SIGUSR1-1
#else
#define SIGPLUS			SIGRTMIN
#define SIGMINUS		SIGRTMIN
#endif
#define LENGTH(X)               (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH		50
#define MIN( a, b ) ( ( a < b) ? a : b )
#define STATUSLENGTH (LENGTH(blocks) * CMDLENGTH + 1)

typedef struct
{
    char *icon;
    char *command;
    void (*func) (char *, int);
    unsigned int interval;
    unsigned int signal;
} Block;

#ifndef __OpenBSD__
void dummysighandler (int num);
#endif

void sighandler (int num);
void getcmds (int time);
void getsigcmds (unsigned int signal);
void setupsignals ();
void sighandler (int signum);
int getstatus (char *str, char *last);
void statusloop ();
void termhandler (int);
void pstdout ();
void psomebar ();

#ifndef NO_X
void setroot ();
static void (*writestatus) () = setroot;
static int setupX ();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif
void launch_volume (void);

#include "dwmblocks.h"
#include "blocks.h"

static char statusbar[LENGTH (blocks)][CMDLENGTH] = { 0 };

static char statusstr[2][STATUSLENGTH];
static int statusContinue = 1;

static char somebarPath[128];
static int somebarFd = -1;
static int usesomebar = 0;

//opens process *cmd and stores output in *output
void
getcmd (const Block * block, char *output)
{
    int i = strlen (block->icon);
    strcpy (output, block->icon);
    if (block->func == NULL) {
        FILE *cmdf = popen (block->command, "r");
        if (!cmdf)
            return;
        if (fgets (output + i, CMDLENGTH - i - delimLen, cmdf) == NULL) {
            /* Purposedly empty. If a block is misbehaving check here */
        }
        pclose (cmdf);
    }
    else {
        block->func (output + i, CMDLENGTH - i - delimLen);
    }
    i = strlen (output);
    if (i == 0) {
        //return if block and command output are both empty
        return;
    }
    //only chop off newline if one is present at the end
    i = output[i - 1] == '\n' ? i - 1 : i;
    if (delim[0] != '\0') {
        strncpy (output + i, delim, delimLen);
    }
    else
        output[i++] = '\0';
}

void
getcmds (int time)
{
    const Block *current;
    for (unsigned int i = 0; i < LENGTH (blocks); i++) {
        current = blocks + i;
        if ((current->interval != 0 && time % current->interval == 0)
            || time == -1)
            getcmd (current, statusbar[i]);
    }
}

void
getsigcmds (unsigned int signal)
{
    const Block *current;
    for (unsigned int i = 0; i < LENGTH (blocks); i++) {
        current = blocks + i;
        if (current->signal == signal)
            getcmd (current, statusbar[i]);
    }
}

void
setupsignals ()
{
#ifndef __OpenBSD__
    /* initialize all real time signals with dummy handler */
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
        signal (i, dummysighandler);
#endif

    for (unsigned int i = 0; i < LENGTH (blocks); i++) {
        if (blocks[i].signal > 0)
            signal (SIGMINUS + blocks[i].signal, sighandler);
    }

}

int
getstatus (char *str, char *last)
{
    strcpy (last, str);
    str[0] = '\0';
    for (unsigned int i = 0; i < LENGTH (blocks); i++)
        strcat (str, statusbar[i]);
    str[strlen (str) - strlen (delim)] = '\0';
    return strcmp (str, last);  //0 if they are the same
}

#ifndef NO_X
void
setroot ()
{
    if (!getstatus (statusstr[0], statusstr[1]))        //Only set root if text has changed.
        return;
    XStoreName (dpy, root, statusstr[0]);
    XFlush (dpy);
}

int
setupX ()
{
    dpy = XOpenDisplay (NULL);
    if (!dpy) {
        fprintf (stderr, "dwmblocks: Failed to open display\n");
        return 0;
    }
    screen = DefaultScreen (dpy);
    root = RootWindow (dpy, screen);
    return 1;
}
#endif

void
pstdout ()
{
    if (!getstatus (statusstr[0], statusstr[1]))        //Only write out if text has changed.
        return;
    printf ("%s\n", statusstr[0]);
    fflush (stdout);
}

void
psomebar ()
{
    if (!getstatus (statusstr[0], statusstr[1]))        //Only write out if text has changed.
        return;
    if (somebarFd < 0) {
        somebarFd = open (somebarPath, O_WRONLY | O_CLOEXEC);
        if (somebarFd < 0 && errno == ENOENT) {
            // assume somebar is not ready yet
            sleep (1);
            somebarFd = open (somebarPath, O_WRONLY | O_CLOEXEC);
        }
        if (somebarFd < 0) {
            perror ("open");
            return;
        }
    }
    dprintf (somebarFd, "status %s\n", statusstr[0]);
}

void
statusloop ()
{
    setupsignals ();
    int i = 1;
    getcmds (-1);
    while (1) {
        sleep (1.0);
        getcmds (i++);
        writestatus ();
        if (!statusContinue)
            break;
    }
}

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void
dummysighandler (int signum)
{
    return;
}
#endif

void
sighandler (int signum)
{
    getsigcmds (signum - SIGPLUS);
    writestatus ();
}

void
termhandler (int v)
{
    statusContinue = 0;
}

void
sigpipehandler ()
{
    close (somebarFd);
    somebarFd = -1;
}

int
main (int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {    //Handle command line arguments
        if (!strcmp ("-d", argv[i]))
            strncpy (delim, argv[++i], delimLen);
        else if (!strcmp ("-p", argv[i]))
            writestatus = pstdout;
        else if (!strcmp ("-s", argv[i]))
            strcpy (somebarPath, argv[++i]);
        else if (!strcmp ("-w", argv[i]))
            usesomebar = 1;
    }

    if (usesomebar) {
        if (!strlen (somebarPath)) {
            strcpy (somebarPath, getenv ("XDG_RUNTIME_DIR"));
            strcat (somebarPath, "/somebar-0");
        }
        writestatus = psomebar;
    }
#ifndef NO_X
    else {
        if (!setupX ())
            return 1;
    }
#endif
    delimLen = MIN (delimLen, strlen (delim));
    delim[delimLen++] = '\0';
    signal (SIGTERM, termhandler);
    signal (SIGINT, termhandler);
    signal (SIGPIPE, sigpipehandler);
    statusloop ();
#ifndef NO_X
    if (!usesomebar) {
        XCloseDisplay (dpy);
    }
#endif
    return 0;
}
