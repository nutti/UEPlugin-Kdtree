#!/usr/bin/env bash
# require: bash version >= 4
# usage example: bash remove_code.sh src 4.27 full output
set -eEu


SUPPORTED_VERSIONS=(
    "4.26.0" "4.27.0"
    "5.0.0" "5.1.0" "5.2.0" "5.3.0"
)

function usage() {
    echo "Usage: bash replace_ENGINE_VERSION.sh <source-directory> <engine-version> <full|free> <output-directory>"
}

if [ $# -ne 4 ]; then
    usage
    exit 1
fi

SOURCE_DIR=${1}
ENGINE_VERSION=${2}
FULL=0
if [ "${3}" = "full" ]; then
    FULL=1
fi
OUTPUT_DIR=${4}
TMP_DIR=$(mktemp -d)

supported=0
for v in "${SUPPORTED_VERSIONS[@]}"; do
    if [ "${v}" = "${ENGINE_VERSION}" ]; then
        supported=1
    fi
done
if [ ${supported} -eq 0 ]; then
    echo "${ENGINE_VERSION} is not supported."
    echo "Supported version is ${SUPPORTED_VERSIONS[*]}"
    exit 1
fi

mkdir -p "${OUTPUT_DIR}"

remove_start_version_regex="[^\S]*//[^\S]*@remove-start[^\S]+UE_VERSION=([0-9.,]+)"
remove_start_full_regex="[^\S]*//[^\S]*@remove-start[^\S]+FULL_VERSION=true"
remove_end_regex="[^\S]*//[^\S]*@remove-end"
enable_delete=0
for file in $(find "${SOURCE_DIR}" -name "*.cpp" -or -name "*.h" -or -name "*.cs"); do
    in_dir_path=$(dirname ${file})
    tmp_dir_path="${TMP_DIR}/${in_dir_path}"
    mkdir -p "${tmp_dir_path}"

    tmp_file_path="${TMP_DIR}/${file}"
    while IFS= read -r line || [ -n "${line}" ]; do
        if [[ "$line" =~ $remove_start_version_regex ]]; then
            versions=${BASH_REMATCH[1]}
            versions=${versions//,/ }

            for version in ${versions[@]}; do
                if [[ ${version} = ${ENGINE_VERSION} ]]; then
                    enable_delete=1
                    break
                fi
            done
        elif [ ${FULL} -eq 1 ] && [[ "$line" =~ $remove_start_full_regex ]]; then
            enable_delete=1
        fi

        if [[ $enable_delete -eq 0 ]]; then
            echo "${line}" >> ${tmp_file_path}
        fi

        if [[ "$line" =~ $remove_end_regex ]]; then
            enable_delete=0
        fi

    done < "${file}"

    echo "Remove code in ${file} >> ${tmp_file_path}"
done


for file in $(find "${TMP_DIR}" -name "*.cpp" -or -name "*.h" -or -name "*.cs"); do
    out_file_path=${file/${TMP_DIR}/${OUTPUT_DIR}}
    cp "${file}" "${out_file_path}"
    echo "Copy file ${file} -> ${out_file_path}"
done

