#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: run.sh <source directory>"
    exit 1
fi

readonly SOURCE_DIRECTORY=${1}
readonly LINTER_CMD="clang-format"

error=0
for file in $(find "${SOURCE_DIRECTORY}" -iname "*.h" -o -iname "*.cpp" -o -iname "*.cs" | sort); do
    if ! diff -u "${file}" <(${LINTER_CMD} -style=file "${file}"); then
        ((error+=1))
    fi
done

if ((error > 0)); then
    echo "Error: ${error} files have errors."
    exit 1
fi

exit 0
