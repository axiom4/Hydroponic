#!/bin/sh
cd HydroponicWeb
git pull && npm install && ng build --output-hashing=none --aot --optimization --output-path=../data --delete-output-path && gzip -9 ../data/*
