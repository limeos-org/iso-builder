#pragma once

/*
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

#include "config.h"

#include "cache/cache.h"
#include "cache/rootfs.h"
#include "cache/package.h"
#include "phases/preparation/resolve.h"
#include "phases/preparation/download.h"
#include "phases/preparation/preparation.h"
#include "phases/base/create.h"
#include "phases/base/strip.h"
#include "phases/base/base.h"
#include "phases/target/create.h"
#include "phases/target/brand.h"
#include "phases/target/package.h"
#include "phases/target/target.h"
#include "phases/carrier/create.h"
#include "phases/carrier/brand.h"
#include "phases/carrier/install.h"
#include "phases/carrier/init.h"
#include "phases/carrier/embed.h"
#include "phases/carrier/bundle.h"
#include "phases/carrier/carrier.h"
#include "phases/assembly/grub.h"
#include "phases/assembly/isolinux.h"
#include "phases/assembly/iso.h"
#include "phases/assembly/assembly.h"
#include "utils/command.h"
#include "utils/signals.h"
#include "utils/log.h"
#include "utils/version.h"
#include "utils/dependencies.h"
#include "utils/brand/identity.h"
#include "utils/brand/splash.h"
#include "utils/brand/grub.h"
