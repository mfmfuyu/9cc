#!/bin/sh
docker run --rm -v $(pwd):/9cc -w /9cc compilerbook $@
