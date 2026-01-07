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

#include "phases/fetch/resolve.h"
#include "phases/fetch/download.h"
#include "phases/rootfs/create.h"
#include "phases/rootfs/strip.h"
#include "phases/rootfs/install.h"
#include "phases/rootfs/init.h"
#include "phases/assemble/grub.h"
#include "phases/assemble/isolinux.h"
#include "phases/assemble/splash.h"
#include "phases/assemble/iso.h"
#include "utils/command.h"
#include "utils/log.h"
#include "utils/version.h"
