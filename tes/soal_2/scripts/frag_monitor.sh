#!/bin/bash

mkdir -p ./logs

LOG_FILE="./logs/fragment.log"
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")

FRAG_USAGE=$(free -m | awk 'NR==2 {printf "%.2f", ($3/$2)*100}')  # Persentase penggunaan memori
FRAG_COUNT=$(df / | awk 'NR==2 {printf "%.2f", $3/1024}')  # Ukuran fragmentasi dalam MB
TOTAL_MEM=$(free -m | awk 'NR==2 {print $2}')  # Total memori dalam MB
AVAILABLE_MEM=$(free -m | awk 'NR==2 {print $7}')  # Memori yang tersedia dalam MB

echo "[${TIMESTAMP}] -- Fragment Usage [${FRAG_USAGE}%] -- Fragment Count [${FRAG_COUNT} MB] -- Details [Total: ${TOTAL_MEM} MB, Available: ${AVAILABLE_MEM} MB]" >> "$LOG_FILE"
