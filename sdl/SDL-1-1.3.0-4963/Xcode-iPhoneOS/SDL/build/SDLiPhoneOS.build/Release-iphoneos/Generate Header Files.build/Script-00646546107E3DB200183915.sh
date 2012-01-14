#!/bin/sh
if test -f ~/.bash_profile; then source ~/.bash_profile; fi
if test -f ~/.bashrc; then source ~/.bashrc; fi
sh ../../build-scripts/updaterev.sh
