#!/usr/bin/env bash
# require: bash version >= 4
# usage example: bash remove_license.sh src full
set -eEu


function usage() {
    echo "Usage: bash remove_license.sh <source-directory> <full|free>"
}

if [ $# -ne 2 ]; then
    usage
    exit 1
fi

SOURCE_DIR=${1}
FULL=0
if [ "${2}" = "full" ]; then
    FULL=1
fi

if [ ${FULL} -eq 1 ]; then
    for file in $(find "${SOURCE_DIR}" -name "*.cpp" -or -name "*.h" -or -name "*.cs"); do
        sed -i -z -e "s/\s*\\*\s*This software is released under the MIT License.*\s*\\*\s*https:\\/\\/opensource.org\\/licenses\\/MIT//g" ${file}
        echo "Removed license in ${file}"
    done
fi
