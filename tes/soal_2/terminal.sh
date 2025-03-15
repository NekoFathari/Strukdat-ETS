#!/bin/bash

while true; do
    echo "============================"
    echo " SELAMAT DATANG DI TERMINAL "
    echo "============================"
    echo ""
    echo "1) Register"
    echo "2) Login"
    echo "3) Exit"
    read -p "Pilih opsi: " main_choice
    echo ""

    clear

    case $main_choice in
        1) bash register.sh ;;
        2) 
            bash login.sh 
            if [[ $? -eq 0 ]]; then
                while true; do
                    bash scripts/manager.sh
                    read -p "Kembali ke Crontab Manager? (y/n): " choice
                    if [[ "$choice" != "y" ]]; then
                        break
                    fi
                done
            fi
        ;;
        3) exit
        sleep 1
        ;;
    esac
done
