#ifndef VERSION_H
#define VERSION_H

#ifndef GIT_DATE
#define GIT_DATE "unknown"
#endif

#ifndef GIT_HASH
#define GIT_HASH "unknown"
#endif

#ifndef GIT_DIRTY
#define GIT_DIRTY "unknown"
#endif

/**
 * @file version.h
 * @brief Displays the current version of MacaroniOS.
 */


/**
 * @brief Prints help information related to the version command.
 */
void version_help(void);

/**
 * @brief Displays the latest version.
 */
void version_latest(void);

/**
 * @brief Displays the past and present versions.
 */
void version_history(void);

/**
 * @brief Main handler for the version command.
 * @param args The argument string passed after 'version'
 */
void version_command(const char *args);

#endif
