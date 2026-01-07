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

/** The maximum length for constructed shell command strings. */
#define MAX_COMMAND_LENGTH 512

/** The maximum length for file path strings. */
#define MAX_PATH_LENGTH 256

/** The maximum length for URL strings. */
#define MAX_URL_LENGTH 512

/** The list of required LimeOS component binaries. */
static const char *REQUIRED_COMPONENTS[] = {
    "installation-wizard"
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
#include "utils/command.h"
#include "utils/fs.h"
#include "utils/log.h"
#include "utils/validate.h"
