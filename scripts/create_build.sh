pushd Sogas

test_flag=

# Parse options
while getopts 't' option; do
    case ${option} in
    t)
    test_flag=true
    ;;
    esac
done

if [ -k ${test_flag}]; then
    cmake . -B ./ -DUSE_TESTS=FALSE
else
    echo ${test_flag}
    cmake . -B ./ -DUSE_TESTS=TRUE
fi

rm -rf ./build
