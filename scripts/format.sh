#!/bin/sh

function _show_help()
{
    echo "WIP help"
}

files=[]

function _populate_diffs()
{
    # TODO Make this modular. We should populate diff files against any branch we pass or just select all files.
    echo "Warning. Right now we're only testing files against origin/main. This should be done against the pushed branch."
    files=$(git diff --name-only origin/main -- '*.cpp' '*.h' '*.c' '*.hpp')
}

_populate_diffs

clang-format.exe --Werror --dry-run $files
