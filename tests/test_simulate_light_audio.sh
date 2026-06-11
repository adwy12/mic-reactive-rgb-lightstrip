#!/usr/bin/env bash
set -euo pipefail

c++ -std=c++17 -Wall -Wextra -I. tools/simulate_light_audio.cpp -o /tmp/simulate_light_audio

default_output=$(/tmp/simulate_light_audio)
tuned_output=$(/tmp/simulate_light_audio --sensitivity 80 --noise-reduction 220)

printf '%s\n' "$default_output" | grep -q '^# sensitivity=192,noiseReduction=120$'
printf '%s\n' "$tuned_output" | grep -q '^# sensitivity=80,noiseReduction=220$'
printf '%s\n' "$default_output" | grep -q '^index,avg,peak,level,smoothed,onset,beat$'

default_level=$(printf '%s\n' "$default_output" | awk -F, '$1 == "2" { print $4 }')
tuned_level=$(printf '%s\n' "$tuned_output" | awk -F, '$1 == "2" { print $4 }')

test "$default_level" -gt "$tuned_level"

echo "test_simulate_light_audio: PASS"
