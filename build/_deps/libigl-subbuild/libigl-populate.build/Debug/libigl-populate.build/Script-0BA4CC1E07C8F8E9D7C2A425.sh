#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/tmp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/libigl-populate-mkdir
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/tmp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/libigl-populate-mkdir
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/tmp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/libigl-populate-mkdir
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/tmp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/libigl-subbuild/libigl-populate-prefix/src/libigl-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/libigl-populate-mkdir
fi

