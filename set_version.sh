sed -i '' -e "s/^VERSION=.*$/VERSION=$1/g" configure.ac

agvtool bump -all
agvtool new-marketing-version "$1"

