# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.22.1)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(gmsh_sdk-populate NONE)



include(ExternalProject)
ExternalProject_Add(gmsh_sdk-populate
                     "UPDATE_DISCONNECTED" "False" "URL" "https://gmsh.info/bin/Linux/gmsh-4.13.1-Linux64-sdk.tgz"
                    SOURCE_DIR          "/home/koopark/claude/KooSimulationDataReader/build/_deps/gmsh_sdk-src"
                    BINARY_DIR          "/home/koopark/claude/KooSimulationDataReader/build/_deps/gmsh_sdk-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


