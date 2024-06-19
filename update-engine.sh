#!/bin/sh

#this script for update RoninEngine to latest version
echo "Adding remote ronin-engine -> engine"
git remote add -f engine https://github.com/badcast/ronin-engine.git

git subtree pull --prefix=RoninEngine/ engine main
git pull -v engine main
