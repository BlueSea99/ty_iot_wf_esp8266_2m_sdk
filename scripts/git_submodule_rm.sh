#!/bin/sh

SUBMODULE_PATH=$1

if [ -z $SUBMODULE_PATH ]; then
    echo "no submodule path!"
    exit 1
fi

# 转到 GIT 项目顶层路径
while [ ! -d .git ]; do
    cd ..
    if [ $(pwd) = '/' ]; then
        echo "not in a git project!"
        exit 99
    fi
done

git submodule deinit -f -- ${SUBMODULE_PATH} || exit 11
rm -rf .git/modules/${SUBMODULE_PATH} || exit 12
git rm --cached ${SUBMODULE_PATH} || exit 13
git config -f .gitmodules --remove-section submodule.${SUBMODULE_PATH} || exit 14


