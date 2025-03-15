#!/bin/bash

CSV_FILE="data/player.csv"

# Cek file ada atau tidak
[ ! -f "$CSV_FILE" ] && { echo "❌ Data tidak ditemukan!"; exit 1; }

echo "=================="
echo " SILAHKAN LOGIN! "
echo "=================="
echo ""
echo "Masukkan email:"
read -r email
echo "Masukkan password:"
read -rs password
echo

password_hash=$(echo -n "$password" | sha256sum | awk '{print $1}')

grep -q ",$email,$password_hash" "$CSV_FILE" && { echo "✅ Login Berhasil!"; exit 0; }

echo "❌ Invalid email or password!"
exit 1
clear