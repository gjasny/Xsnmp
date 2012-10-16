#!/bin/bash

echo "Cleaning Xsnmp"

xcodebuild -project Xsnmp.xcodeproj clean

echo "Building Xsnmp"

xcodebuild -project Xsnmp.xcodeproj -scheme Xsnmp build

echo "Packaging Xsnmp"

xcodebuild -project Xsnmp.xcodeproj -scheme Xsnmp archive
