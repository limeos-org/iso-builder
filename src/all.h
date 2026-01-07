#pragma once

#include <curl/curl.h>
#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

/** Backward-compatible alias for CONFIG_MAX_COMMAND_LENGTH. */
#define MAX_COMMAND_LENGTH CONFIG_MAX_COMMAND_LENGTH

/** Backward-compatible alias for CONFIG_MAX_PATH_LENGTH. */
#define MAX_PATH_LENGTH CONFIG_MAX_PATH_LENGTH

/** Backward-compatible alias for CONFIG_MAX_URL_LENGTH. */
#define MAX_URL_LENGTH CONFIG_MAX_URL_LENGTH

/** The list of required LimeOS component binaries. */
static const char *REQUIRED_COMPONENTS[] = {
    CONFIG_INSTALLER_BINARY_NAME
};

/** The number of required components. */
#define REQUIRED_COMPONENTS_COUNT \
    (int)(sizeof(REQUIRED_COMPONENTS) / sizeof(REQUIRED_COMPONENTS[0]))

/** The list of optional LimeOS component binaries. */
static const char *OPTIONAL_COMPONENTS[] = {
    "window-manager",
    "display-manager"
};

/** The number of optional components. */
#define OPTIONAL_COMPONENTS_COUNT \
    (int)(sizeof(OPTIONAL_COMPONENTS) / sizeof(OPTIONAL_COMPONENTS[0]))

#include "boot.h"
#include "collector.h"
#include "iso.h"
#include "version.h"
#include "rootfs/components.h"
#include "rootfs/create.h"
#include "rootfs/init.h"
#include "utils/shell.h"
#include "utils/command.h"
#include "utils/log.h"
#include "utils/version.h"
