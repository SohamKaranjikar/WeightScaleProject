#!/bin/env bash
#exit on Error (even within a pipeline) and treat Unset variables as errors
set -euo pipefail
#for tracing
#set -x

cd "$(dirname "$0")"

DIR_OUT="./Out"
mkdir -p "$DIR_OUT"
pdflatex --output-directory="$DIR_OUT" ./Src/report.tex
