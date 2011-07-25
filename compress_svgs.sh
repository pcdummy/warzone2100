#!/bin/bash
# vim: set et sts=4 sw=4 encoding=utf-8:

OLDPATH="$(pwd)"
MYPATH="$(cd -P -- "$(dirname -- "$0")" && pwd -P)"

cd ${MYPATH}
for file in $(find . -iname '*.svg'); do
    ./tools/scour/scour.py --remove-metadata --shorten-ids -i "${file}" -o "${file}-scour"
    mv "${file}-scour" "${file}"
done
cd ${OLDPATH}
