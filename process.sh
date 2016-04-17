#!/bin/bash

mkdir config-processed

for i in $( ls config*.out ); do
    cat $i | ~/POC/torus-random/config-parse/parser0 | sort -u >> config-processed/processed-0-$i
    cat $i | ~/POC/torus-random/config-parse/parser1 | sort -u >> config-processed/processed-1-$i
done