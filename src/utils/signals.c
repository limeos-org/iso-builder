/**
 * This code is responsible for signal handling and graceful shutdown
 * during the ISO build process.
 */

#include "all.h"

// Global state for signal handler cleanup.
static volatile int g_interrupted = 0;
static char g_cleanup_dir[COMMAND_PATH_MAX_LENGTH] = {0};

static void signal_handler(int sig)
{
    (void)sig;
    g_interrupted = 1;
}

void install_signal_handlers(const char *cleanup_dir)
{
    // Store the cleanup directory path.
    if (cleanup_dir != NULL)
    {
        strncpy(g_cleanup_dir, cleanup_dir, sizeof(g_cleanup_dir) - 1);
        g_cleanup_dir[sizeof(g_cleanup_dir) - 1] = '\0';
    }

    // Set up signal handlers.
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

int check_interrupted(void)
{
    if (g_interrupted)
    {
        LOG_WARNING("Build interrupted by signal, cleaning up...");
        if (g_cleanup_dir[0] != '\0')
        {
            rm_rf(g_cleanup_dir);
        }
        return 1;
    }
    return 0;
}

void clear_cleanup_dir(void)
{
    g_cleanup_dir[0] = '\0';
}
