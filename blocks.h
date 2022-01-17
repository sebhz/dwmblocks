//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
    /*Icon *//*Command *//* Function pointer *//*Update Interval *//*Update Signal */
    {"Vol: ", "echo $(dwm-audio-wrapper gv)", NULL, 0, 1},
    {"Bat: ", "", battery, 60, 0},
    {"Load: ", "", loadavg, 60, 0},
    {"Up: ", "", uptime, 60, 0},
    {"", "", datetime, 60, 0},
    {"", "", sun, 5, 0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = " | ";
static unsigned int delimLen = 5;
