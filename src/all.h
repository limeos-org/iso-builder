#pragma once

/**
 * semistatic: expands to 'static' normally, or nothing when -DTESTING is set.
 * This exposes functions for unit testing while keeping them static in
 * production. Declare test-accessible functions in tests/all.h.
 */
#ifdef TESTING
#define semistatic
#else
#define semistatic static
#endif

#include <curl/curl.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <glob.h>
#include <json-c/json.h>
#include <openssl/evp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <limeos-common-lib.h>
#include "config.h"

#include "phases/preparation/resolve.h"
#include "phases/preparation/download.h"
#include "phases/preparation/preparation.h"
#include "phases/base/create.h"
#include "phases/base/strip.h"
#include "phases/base/base.h"
#include "phases/target/create.h"
#include "phases/target/configure.h"
#include "phases/target/package.h"
#include "phases/target/target.h"
#include "phases/live/create.h"
#include "phases/live/configure.h"
#include "phases/live/install.h"
#include "phases/live/autostart.h"
#include "phases/live/embed.h"
#include "phases/live/bundle.h"
#include "phases/live/live.h"
#include "phases/assembly/grub.h"
#include "phases/assembly/iso.h"
#include "phases/assembly/assembly.h"
#include "utils/rootfs.h"
#include "utils/dependencies.h"
#include "utils/branding/identity.h"
#include "utils/branding/plymouth.h"
