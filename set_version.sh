sed -i '' -E "s/^(AC_INIT\(.*, *\[).*(\]\))$/\1$1\2/g" configure.ac

agvtool bump -all
agvtool new-marketing-version "$1"

