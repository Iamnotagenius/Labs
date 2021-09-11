#! /bin/sh
gcc $1 -o ${0%/*}/bin/${1%.c}
