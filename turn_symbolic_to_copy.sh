#!/bin/bash

total_files=0
turn_pass_counter=0
turn_fail_counter=0
no_target_counter=0
SEARCH_DIR="$1"

if [ ! -d "$SEARCH_DIR" ]; then
    echo "Error: Directory $SEARCH_DIR not found."
    exit 1
fi

echo "This will remove those symbolic files and copy its targets."
echo "Are you sure to start this? (y/N)"
read ARE_YOU_SURE

if [ "${ARE_YOU_SURE}" == "y" ]; then
    echo "-----------------------------------"
    echo "Start turnning symbolic files to copy under directy ${SEARCH_DIR}"
else
    exit 0
fi

while read -r symlink; do
    total_files=$((total_files + 1))
    target_file=$(readlink -f "$symlink")

    if [ -f "$target_file" ]; then
        echo "Processing symlink: $symlink"
        echo "Target file: $target_file"
        echo "Remove symlink file: $symlink"
        rm -f "$symlink"
        if cp -a "$target_file" "$symlink"; then
            echo "Successfully converted symlink to a copy."
            turn_pass_counter=$((turn_pass_counter + 1))
        else
            echo "Failed to copy target file."
            turn_fail_counter=$((turn_fail_counter + 1))
        fi
    else
        echo "Warning: Symlink target not found for $symlink"
        no_target_counter=$((no_target_counter + 1))
    fi
done < <(find "${SEARCH_DIR}" -type l)

echo "Finish turnning symbolic files to copy under directy ${SEARCH_DIR}"
echo "-----------------------------------"
echo "Summary:"
echo "Turn symbollic to copy under directory: $SEARCH_DIR"
printf "Total files processed:\t %s\n" "$total_files"
printf "Files turnned passed:\t %s\n" "$turn_pass_counter"
printf "Files turnned failed:\t %s\n" "$turn_fail_counter"
printf "Files no target:\t %s\n" "$no_target_counter"
