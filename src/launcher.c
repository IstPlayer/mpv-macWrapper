/*
 * mpv-macWrapper v1.1.0 — macOS .app bundle launcher for mpv
 *
 * Locates mpv via (in priority order):
 *   1. ~/.config/mpv-macWrapper/path.conf  — saved from previous manual selection
 *   2. $MPV_PATH environment variable
 *   3. /opt/homebrew/bin/mpv  (Apple Silicon Homebrew)
 *   4. /usr/local/bin/mpv     (Intel Homebrew)
 *   5. /opt/local/bin/mpv     (MacPorts)
 *
 * If none of the above work, a native file picker dialog opens so the user
 * can locate mpv manually.  The chosen path is saved to path.conf so
 * subsequent launches skip the dialog.
 *
 * With arguments → passed through to mpv.
 * No arguments   → --idle=yes --force-window=yes (blank window).
 *
 * Build:  clang -O2 -o mpv launcher.c -mmacosx-version-min=12.0
 * Strip:  strip mpv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define CONFIG_DIR  ".config/mpv-macWrapper"
#define CONFIG_FILE "path.conf"

/* ── helpers ──────────────────────────────────────────── */

static char *config_path(void) {
    const char *home = getenv("HOME");
    if (!home) return NULL;

    size_t len = strlen(home) + strlen("/" CONFIG_DIR "/" CONFIG_FILE) + 1;
    char *path = malloc(len);
    if (!path) return NULL;
    snprintf(path, len, "%s/" CONFIG_DIR "/" CONFIG_FILE, home);
    return path;
}

/* Read a single line (mpv path) from the config file, if it exists and is
 * still executable.  Returns a malloc'd string, or NULL. */
static char *read_saved_path(void) {
    char *cp = config_path();
    if (!cp) return NULL;

    FILE *f = fopen(cp, "r");
    free(cp);
    if (!f) return NULL;

    char *line = NULL;
    size_t n = 0;
    ssize_t len = getline(&line, &n, f);
    fclose(f);

    if (len <= 0) { free(line); return NULL; }

    /* strip trailing newline */
    if (line[len - 1] == '\n') line[len - 1] = '\0';

    if (access(line, X_OK) != 0) { free(line); return NULL; }
    return line;
}

/* Persist a path to the config file so future launches remember it. */
static void save_path(const char *mpv_path) {
    const char *home = getenv("HOME");
    if (!home) return;

    char dir[1024];
    snprintf(dir, sizeof(dir), "%s/" CONFIG_DIR, home);
    mkdir(dir, 0755);

    char file[1024];
    snprintf(file, sizeof(file), "%s/" CONFIG_FILE, dir);

    FILE *f = fopen(file, "w");
    if (!f) return;
    fprintf(f, "%s\n", mpv_path);
    fclose(f);
}

/* Open a native file picker via osascript, return the chosen POSIX path
 * (malloc'd), or NULL if the user cancelled. */
static char *file_picker(void) {
    const char *script =
        "set mpvPath to POSIX path of (choose file "
        "  with prompt \"Please locate the mpv binary:\" "
        "  default location (POSIX file \"/opt/homebrew/bin\") "
        "  of type {\"public.unix-executable\"})\n"
        "return mpvPath";

    FILE *fp = popen("/usr/bin/osascript -e 2>/dev/null", "w");
    if (!fp) return NULL;
    fprintf(fp, "%s", script);
    pclose(fp);

    /* popen in write mode doesn't give us output; we need read mode.
     * Re-run with the script as a heredoc via -e. */
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
             "/usr/bin/osascript -e '%s' 2>/dev/null", script);
    fp = popen(cmd, "r");
    if (!fp) return NULL;

    char *line = NULL;
    size_t n = 0;
    ssize_t len = getline(&line, &n, fp);
    pclose(fp);

    if (len <= 0) { free(line); return NULL; }
    if (line[len - 1] == '\n') line[len - 1] = '\0';
    return line;
}

/* ── mpv location ─────────────────────────────────────── */

static const char *CANDIDATES[] = {
    "/opt/homebrew/bin/mpv",
    "/usr/local/bin/mpv",
    "/opt/local/bin/mpv",
    NULL,
};

/*
 * Resolution order:
 *   1. saved config  (~/.config/mpv-macWrapper/path.conf)
 *   2. $MPV_PATH
 *   3. built-in candidate paths
 *   4. file-picker dialog → save → return
 */
static const char *resolve_mpv(void) {
    /* 1. saved config */
    char *saved = read_saved_path();
    if (saved) return saved; /* caller must free */

    /* 2. environment */
    const char *env = getenv("MPV_PATH");
    if (env && access(env, X_OK) == 0)
        return strdup(env);

    /* 3. built-in candidates */
    for (int i = 0; CANDIDATES[i]; i++) {
        if (access(CANDIDATES[i], X_OK) == 0)
            return strdup(CANDIDATES[i]);
    }

    /* 4. file picker */
    char *picked = file_picker();
    if (picked && access(picked, X_OK) == 0) {
        save_path(picked);
        return picked;
    }
    free(picked);
    return NULL;
}

static void alert(const char *title, const char *msg) {
    char script[2048];
    snprintf(script, sizeof(script),
             "display dialog \"%s\" with title \"%s\" "
             "buttons {\"OK\"} default button \"OK\" with icon stop",
             msg, title);
    const char *argv[] = { "/usr/bin/osascript", "-e", script, NULL };
    pid_t pid = fork();
    if (pid == 0) {
        execv("/usr/bin/osascript", (char *const *)argv);
        _exit(1);
    }
}

/* ── main ─────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    const char *mpv = resolve_mpv();
    if (!mpv) {
        write(STDERR_FILENO,
              "mpv not found and file picker was cancelled.\n", 49);
        alert("mpv not found",
              "mpv could not be located.\\n\\n"
              "Install mpv via Homebrew:\\n"
              "  brew install mpv\\n\\n"
              "Or set the MPV_PATH environment variable.");
        return 1;
    }

    int has_args = (argc > 1);
    int extra    = has_args ? 0 : 2;
    int total    = 1 + (argc - 1) + extra + 1;

    char **mpv_args = malloc(total * sizeof(char *));
    if (!mpv_args) { free((void *)mpv); return 1; }

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
    write(STDERR_FILENO, "failed to exec mpv\n", 19);
    free((void *)mpv);
    free(mpv_args);
    return 1;
}
