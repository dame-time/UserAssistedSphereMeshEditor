#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/tmp/eigen-populate-gitclone.cmake
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/eigen-populate-download
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/tmp/eigen-populate-gitclone.cmake
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/eigen-populate-download
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/tmp/eigen-populate-gitclone.cmake
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/eigen-populate-download
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/tmp/eigen-populate-gitclone.cmake
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/eigen-populate-download
fi

