/*
 * mpv-mac-app — macOS .app bundle launcher for mpv
 *
 * Locates mpv via the MPV_PATH environment variable, falling back to
 * common Homebrew / system paths.  When invoked with arguments they are
 * passed through to mpv; with no arguments the launcher opens an idle
 * window (--idle=yes --force-window=yes).
 *
 * Build:  clang -O2 -o mpv launcher.c -mmacosx-version-min=12.0
 * Strip:  strip mpv
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Fallback search paths when MPV_PATH is not set */
static const char *CANDIDATES[] = {
    "/opt/homebrew/bin/mpv",         /* Apple Silicon Homebrew */
    "/usr/local/bin/mpv",            /* Intel Homebrew         */
    "/opt/local/bin/mpv",            /* MacPorts               */
    NULL,
};

static const char *find_mpv(void) {
    const char *env = getenv("MPV_PATH");
    if (env && access(env, X_OK) == 0)
        return env;
    for (int i = 0; CANDIDATES[i]; i++) {
        if (access(CANDIDATES[i], X_OK) == 0)
            return CANDIDATES[i];
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    const char *mpv = find_mpv();
    if (!mpv) {
        const char *msg =
            "mpv not found.\n"
            "Set MPV_PATH to your mpv binary, or install:\n"
            "  brew install mpv\n";
        write(STDERR_FILENO, msg, strlen(msg));
        return 1;
    }

    int has_args = (argc > 1);
    int extra    = has_args ? 0 : 2;            /* --idle --force-window */
    int total    = 1 + (argc - 1) + extra + 1;  /* mpv + user_args + extra + NULL */

    char **mpv_args = malloc(total * sizeof(char *));
    if (!mpv_args) return 1;

    int i = 0;
    mpv_args[i++] = (char *)mpv;
    if (!has_args) {
        mpv_args[i++] = "--idle=yes";
        mpv_args[i++] = "--force-window=yes";
    }
    for (int j = 1; j < argc; j++)
        mpv_args[i++] = argv[j];
    mpv_args[i] = NULL;

    execv(mpv, mpv_args);

    /* execv only returns on error */
    const char *err = "failed to exec mpv\n";
    write(STDERR_FILENO, err, strlen(err));
    free(mpv_args);
    return 1;
}
