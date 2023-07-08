#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/tmp/yaml_cpp-populate-gitupdate.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/tmp/yaml_cpp-populate-gitupdate.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/tmp/yaml_cpp-populate-gitupdate.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-src
  /usr/local/Cellar/cmake/3.22.3/bin/cmake -P /Users/davidepaollilo/Workspaces/C++/CustomRenderer/build/_deps/yaml_cpp-subbuild/yaml_cpp-populate-prefix/tmp/yaml_cpp-populate-gitupdate.cmake
fi

