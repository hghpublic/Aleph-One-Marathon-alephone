#!/bin/sh

set -ex

VERSION="20250829"
DOWNLOAD_URL="https://github.com/Aleph-One-Marathon/alephone/releases/download"

# https://alephone.lhowon.org/scenarios.html

if [ -d temp ]; then
    rm -rf temp
fi
mkdir temp
cd temp
# Marathon trilogy data files
curl -LO "${DOWNLOAD_URL}/release-$VERSION/Marathon-$VERSION-Data.zip"
curl -LO "${DOWNLOAD_URL}/release-$VERSION/Marathon2-$VERSION-Data.zip"
curl -LO "${DOWNLOAD_URL}/release-$VERSION/MarathonInfinity-$VERSION-Data.zip"
# Pathways into Darkness data files
curl -L https://simplici7y.com/items/aleph-one-pathways-into-darkness/downloads/new > AOPID_v1.4.zip
curl -L https://simplici7y.com/items/hd-aopid-graphics/downloads/new > HD_AOPID.zip
curl -L https://simplici7y.com/items/aleph-one-pathways-into-darkness-remastered-sounds/downloads/new > AOPID_Remastered_Sounds_v1.0.zip
# test archives
for a in *.zip; do zip -T "$a"; done
cd -
