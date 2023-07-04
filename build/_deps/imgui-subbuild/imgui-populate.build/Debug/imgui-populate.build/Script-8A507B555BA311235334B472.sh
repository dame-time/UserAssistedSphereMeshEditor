#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-complete
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-done
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-complete
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-done
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-complete
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-done
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E make_directory /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/CMakeFiles/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-complete
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -E touch /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/imgui-subbuild/imgui-populate-prefix/src/imgui-populate-stamp/$CONFIGURATION$EFFECTIVE_PLATFORM_NAME/imgui-populate-done
fi

