#!/bin/bash


TGT=$1
if [[ $TGT == '' ]]; then
    TGT='all'
fi

shift


if [[ $TGT == 'all' ]]; then
	for tg in src/targets/*; do
		make -r -f $tg/Makefile $@
	done
else
	make -r -f src/targets/$TGT/Makefile $@
fi




