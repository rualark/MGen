git describe --tags --dirty --long > git-version.log
SET /p gitOutput= < git-version.log
DEL git-version.h
ECHO #define GEN_VER_VERSION_STRING   "%gitOutput%\0" >> "git-version.h"
