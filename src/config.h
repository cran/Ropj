#ifndef CONFIG_H
#define CONFIG_H

#define LIBORIGIN_VERSION_MAJOR 3
#define LIBORIGIN_VERSION_MINOR 0
#define LIBORIGIN_VERSION_BUGFIX 0

#define LIBORIGIN_VERSION ((LIBORIGIN_VERSION_MAJOR << 24) | \
                           (LIBORIGIN_VERSION_MINOR << 16) | \
                           (LIBORIGIN_VERSION_BUGFIX << 8) )
#define LIBORIGIN_VERSION_STRING "${LIBORIGIN_VERSION_MAJOR}.${LIBORIGIN_VERSION_MINOR}.${LIBORIGIN_VERSION_BUGFIX}"

#endif