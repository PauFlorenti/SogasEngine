#!/bin/sh

function _display_help()
{
    echo "====================================================================="
    echo "Usage format.sh [OPTIONS] ..."
    echo
    echo "Description: The format.sh script will run clang format on .h,"
    echo              " .c, .cpp and .hpp files."
    echo
    echo " -a   This will run the script against all files matching the "
    echo "      pattern."
    echo "      Otherwise, the script will run against the diff files "
    echo "      from origin/main."
    echo "      Optional"
    echo
    echo " -f   This will format your file. By default the script will "
    echo "      perform a dry-run. This will only expose the format errors."
    echo "      Optional"
    echo
    echo " -h   Displays this help dialog."
    echo "      Optional"
    # TODO Verbose option.
    echo "====================================================================="
    exit 1
}

files=[]

function _populate_diffs()
{
    # TODO Make this modular. We should populate diff files against any branch we pass or just select all files.
    echo "Warning. Right now we're only testing files against origin/main. This should be done against the pushed branch."
    files=$(git diff --name-only origin/main -- '*.cpp' '*.h' '*.c' '*.hpp')
}

# --------------------------------
# Start scripts
# --------------------------------

should_format=false

# parse options
while getopts ':fh' option; do
    case ${option} in
    f)
        echo "Forcing clang format on files."
        should_format=true
        ;;
    h)
        _display_help
        ;;
    *)
        echo "Running dry run of clang-format."
        should_format=false
    esac
done

_populate_diffs

if [ -z ${files} ]
then
    echo "No diff files to check."
    # TODO Check if you're on root path or make absolute path instead of relative.
    echo "Make sure you're running the script from root path - ./Sogas/"
    echo
else
    if [ ${should_format} = false ]
    then
        clang-format.exe --dry-run $files
    else
        clang-format.exe -i $files
    fi
fi

exit 0
