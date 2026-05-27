#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int has_args = (argc > 1);
    int extra = has_args ? 0 : 2;
    int total = 1 + (argc - 1) + extra + 1;

    char **mpv_args = malloc(total * sizeof(char *));
    if (!mpv_args) return 1;

    int i = 0;
    mpv_args[i++] = "/opt/homebrew/bin/mpv";
    if (!has_args) {
        mpv_args[i++] = "--idle=yes";
        mpv_args[i++] = "--force-window=yes";
    }
    for (int j = 1; j < argc; j++)
        mpv_args[i++] = argv[j];
    mpv_args[i] = NULL;

    execv("/opt/homebrew/bin/mpv", mpv_args);

    const char *msg = "mpv not found.\nInstall: brew install mpv\n";
    write(2, msg, strlen(msg));
    free(mpv_args);
    return 1;
}
