#!/bin/bash

mkdir -p ./logs

LOG_FILE="./logs/core.log"
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")

CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | awk '{print 100 - $8}')  # Menghitung CPU yang dipakai (100% - idle)
CPU_MODEL=$(lscpu | grep "Model name" | awk -F ':' '{print $2}' | sed 's/^[ \t]*//g' | tr -s ' ')

echo "[${TIMESTAMP}] - Core Usage [${CPU_USAGE}%] - Terminal Model [${CPU_MODEL}]" >> "$LOG_FILE" 2>/dev/null