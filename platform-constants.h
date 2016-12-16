#ifndef PLATFORMCONSTANTS_H
#define PLATFORMCONSTANTS_H
#if defined(Q_OS_LINUX)
#define PLATFORM_ICONS "linux"
#define PLATFORM_UPDATE "linux"
#define PLATFORM_AGENT "Linux"
#elif defined(Q_OS_MACOS)
#define PLATFORM_ICONS "mac"
#define PLATFORM_UPDATE "mac"
#define PLATFORM_AGENT "MacOS"
#elif defined(Q_OS_WIN)
#define PLATFORM_ICONS "win"
#define PLATFORM_UPDATE "windows"
#define PLATFORM_AGENT "Windows"
#else
#define PLATFORM_ICONS "linux"
#define PLATFORM_UPDATE "unknown"
#define PLATFORM_AGENT "Unknown"
#endif
#endif // PLATFORMCONSTANTS_H
