#!/bin/bash

GIT_CURR_BRANCH="`git branch -a | grep '*' | awk '{print $2}'`"

git add .
git commit -m "${GIT_CURR_BRANCH}"
git push --set-upstream origin "${GIT_CURR_BRANCH}"
