#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define MAX_PATH 512

/* Fungsi untuk menjalankan perintah eksternal seperti wget atau unzip
menggunakan fork dan exec*/  
void run_command(char *cmd, char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(cmd, args); //menjalankan perintah
        perror("exec gagal");
        exit(1);
    } else {
        wait(NULL); //mengunggu proses anak selesai
    }
}

//download dan ekstrak Clues.zip
void download_and_unzip() {
    // jika folder Clues sudah ada, tidak perlu download ulang
    if (access("Clues", F_OK) == 0) {
        printf("Folder 'Clues' sudah ada.\n");
        return;
    }

    char *wget_args[] = {
        "wget", "-O", "Clues.zip",
        "https://drive.usercontent.google.com/download?id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK", NULL
    };
    run_command("wget", wget_args);

    char *unzip_args[] = {"unzip", "Clues.zip", NULL};
    run_command("unzip", unzip_args);

    remove("Clues.zip");
    printf("Clues.zip berhasil didownload dan diekstrak.\n");
}

//filter file berdasarkan nama (1huruf/angka + ".txt")
void filter_files() {
    mkdir("Filtered", 0777);

    char *folders[] = {"Clues/ClueA", "Clues/ClueB", "Clues/ClueC", "Clues/ClueD"};
    char source[MAX_PATH];
    char target[MAX_PATH];

    for (int i = 0; i < 4; i++) {
        DIR *dir = opendir(folders[i]);
        if (!dir) continue;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            char *name = entry->d_name;

            if (entry->d_type == DT_REG) {
                // Nama file harus: 1 huruf/angka + ".txt" (panjang 5)
                if (strlen(name) == 5 && isalnum(name[0]) &&
                    strcmp(&name[1], ".txt") == 0) {
                    //memindahkan file ke folder filtered
                    snprintf(source, MAX_PATH, "%s/%s", folders[i], name);
                    snprintf(target, MAX_PATH, "Filtered/%s", name);
                    rename(source, target);
                } else {
                    //hapus file yang tidak sesuai
                    snprintf(source, MAX_PATH, "%s/%s", folders[i], name);
                    remove(source);
                }
            }
        }
        closedir(dir);
    }

    printf("File berhasil difilter ke folder 'Filtered'.\n");
}

//menggabungkan isi file menjadi Combined.txt
void combine_files() {
    FILE *out = fopen("Combined.txt", "w");
    if (!out) {
        perror("Gagal membuat Combined.txt");
        return;
    }

    char *nums[] = {"1.txt", "2.txt", "3.txt", "4.txt", "5.txt", "6.txt"};
    char *alphas[] = {"a.txt", "b.txt", "c.txt", "d.txt", "e.txt", "f.txt"};
    char path[MAX_PATH], ch;

    for (int i = 0; i < 6; i++) {
        char *pair[] = {nums[i], alphas[i]};
        for (int j = 0; j < 2; j++) {
            snprintf(path, MAX_PATH, "Filtered/%s", pair[j]);
            FILE *in = fopen(path, "r");
            if (in) {
                //copy isi file ke Combined.txt
                while ((ch = fgetc(in)) != EOF) {
                    fputc(ch, out);
                }
                fclose(in);
                remove(path); 
            }
        }
    }

    fclose(out);
    printf("Isi file berhasil digabung ke 'Combined.txt'.\n");
}

//ROT13 untuk deskripsi huruf
char rot13(char c) {
    if ('a' <= c && c <= 'z') return ((c - 'a' + 13) % 26) + 'a';
    if ('A' <= c && c <= 'Z') return ((c - 'A' + 13) % 26) + 'A';
    return c;
}

//deskripsi isi Combined.txt ke Decoded.txt
void decode_file() {
    FILE *in = fopen("Combined.txt", "r");
    FILE *out = fopen("Decoded.txt", "w");
    if (!in || !out) {
        perror("Gagal membuka file");
        return;
    }

    char ch;
    while ((ch = fgetc(in)) != EOF) {
        fputc(rot13(ch), out);
    }

    fclose(in);
    fclose(out);
    printf("Isi file berhasil didekode ke 'Decoded.txt'.\n");
}

//cara penggunaan program
void print_usage() {
    printf("Cara pakai:\n");
    printf("a) ./action                -> download dan unzip Clues.zip\n");
    printf("b) ./action -m Filter      -> filter file ke folder Filtered\n");
    printf("c) ./action -m Combine     -> gabungkan isi file\n");
    printf("d) ./action -m Decode      -> decode isi file\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        download_and_unzip();
    } else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0) filter_files();
        else if (strcmp(argv[2], "Combine") == 0) combine_files();
        else if (strcmp(argv[2], "Decode") == 0) decode_file();
        else print_usage();
    } else {
        print_usage();
    }

    return 0;
}