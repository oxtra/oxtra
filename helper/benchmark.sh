#!/bin/bash

if [ $# -eq 0 ]; then
	echo "Usage: ./benchmark.sh <linux command>"
	exit 1
fi

echo "Benchmarking is about to start ... performing 10 runs ..."

#set -e

for i in {1..10}; do
	timestamp=$(date +%s%N)
	"$@" > /dev/null
	time=$((($(date +%s%N) - $timestamp)))
	echo $time
done

echo "Finished simple benchmark"
