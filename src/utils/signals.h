#pragma once
#include "../all.h"

/**
 * Installs signal handlers for graceful shutdown.
 *
 * Sets up handlers for SIGINT and SIGTERM that will trigger cleanup
 * of the specified directory when the process is interrupted.
 *
 * @param cleanup_dir The directory path to clean up on interruption.
 */
void install_signal_handlers(const char *cleanup_dir);

/**
 * Checks if the build was interrupted by a signal.
 *
 * If interrupted, performs cleanup of the registered directory and
 * logs a warning message.
 *
 * @return - `1` - Build was interrupted.
 * @return - `0` - Build was not interrupted.
 */
int check_interrupted(void);

/**
 * Clears the cleanup directory to prevent double cleanup.
 *
 * Call this after manual cleanup to prevent the signal handler
 * from attempting to clean up an already-removed directory.
 */
void clear_cleanup_dir(void);
