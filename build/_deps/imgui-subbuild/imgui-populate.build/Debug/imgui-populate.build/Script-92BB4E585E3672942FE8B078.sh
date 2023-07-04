#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E echo_append
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-install
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E echo_append
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-install
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E echo_append
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-install
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-build
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E echo_append
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-install
fi

