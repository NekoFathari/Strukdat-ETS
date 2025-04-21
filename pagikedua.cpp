#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <ctime>

using namespace std;

// Manual stack class
template<typename T>
class ManualStack {
private:
    vector<T> data;
public:
    void push(const T& value) {
        data.push_back(value);
    }

    void pop() {
        if (!data.empty())
            data.pop_back();
    }

    T& top() {
        return data.back();
    }

    bool empty() const {
        return data.empty();
    }

    void clear() {
        data.clear();
    }
};

ManualStack<string> undoStack;
ManualStack<string> redoStack;
string currentLine = "";
string fullText = "";
bool isBold = false;
bool isItalic = false;
bool isUnderline = false;
bool underlineActive = false;
int currentLineIndex = 0;
vector<string> lines;
ofstream logFile;

void logAction(const string& action) {
    if (logFile.is_open()) {
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%d-%m-%Y[%H:%M:%S]", timeinfo);
        logFile << "[" << timestamp << "] - " << action << endl;
    }
}

void enableRawMode() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_iflag &= ~(IXON);
    tcsetattr(0, TCSANOW, &term);
}

void disableRawMode() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
}

void pushToUndo() {
    if (undoStack.empty() || undoStack.top() != currentLine) {
        undoStack.push(currentLine);
        redoStack.clear();
        logAction("Push to undo: " + currentLine);
    }
}

void handleUndo() {
    if (!undoStack.empty()) {
        redoStack.push(currentLine);
        currentLine = undoStack.top();
        undoStack.pop();
        logAction("Undo: " + currentLine);
    }
}

void handleRedo() {
    if (!redoStack.empty()) {
        undoStack.push(currentLine);
        currentLine = redoStack.top();
        redoStack.pop();
        logAction("Redo: " + currentLine);
    }
}

void handleDeleteLastWord() {
    pushToUndo();
    size_t pos = currentLine.find_last_of(' ');
    if (pos != string::npos)
        currentLine = currentLine.substr(0, pos);
    else
        currentLine.clear();
    logAction("Delete last word: " + currentLine);
}

void handleSave() {
    fullText.clear();
    for (const string& line : lines) {
        fullText += line + "\n";
    }
    ofstream file("saved_text.txt");
    file << fullText;
    file.close();
    logAction("Save to saved_text.txt");
    cout << "\r\n[Saved to saved_text.txt]\n";
    sleep(5);
    cout << "> " << flush;
}

void promptExit() {
    disableRawMode();
    cout << "\r\nApakah kamu ingin menyimpan sebelum keluar? (y/n):";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        lines.push_back(currentLine);
        fullText += currentLine + "\n";
        handleSave();
    } else {
        cout << "[Keluar tanpa menyimpan]\n";
    }
}

void toggleBold() {
    isBold = !isBold;
    cout << (isBold ? "\033[1m" : "\033[22m");
}

void toggleItalic() {
    isItalic = !isItalic;
    cout << (isItalic ? "\033[3m" : "\033[23m");
}

void toggleUnderline() {
    underlineActive = !underlineActive;
    cout << (underlineActive ? "\033[4m" : "\033[24m");
}

void moveUp() {
    if (currentLineIndex > 0) {
        lines[currentLineIndex] = currentLine;
        currentLineIndex--;
        currentLine = lines[currentLineIndex];
        logAction("Moved up to line: " + currentLine);
    }
}

void moveDown() {
    if (currentLineIndex < (int)lines.size() - 1) {
        lines[currentLineIndex] = currentLine;
        currentLineIndex++;
        currentLine = lines[currentLineIndex];
        logAction("Moved down to line: " + currentLine);
    }
}

void displayText() { // menampilkan currentLine di terminal
    cout << "\033[2J\033[H\033[0m"; // Clear screen dan pindah kursor ke posisi awal dan reset format
    cout << "=== Simple Text Editor ===\n";
    cout << "Commands:\n";
    cout << "  Ctrl+S : Save\n";
    cout << "  Ctrl+U : Undo\n";
    cout << "  Ctrl+Y : Redo\n";
    cout << "  Ctrl+D : Delete Last Word\n";
    cout << "  Ctrl+X : Exit (dengan konfirmasi)\n";
    cout << "  Ctrl+B : Toggle Bold\n";
    cout << "  Ctrl+K : Toggle Italic\n";
    cout << "  Ctrl+T : Toggle Underline\n";
    cout << "  Ctrl+Q : Move Up Line\n";
    cout << "  Ctrl+A : Move Down Line\n";
    cout << "  Enter  : Newline\n\n";

    for (int i = 0; i < lines.size(); ++i) {        
        cout << "[" << i + 1 << "] > " << lines[i] << "\033[0m\n";
    }
    cout << "\n";
    if (isBold) cout << "\033[1m";
    if (isItalic) cout << "\033[3m";
    if (underlineActive) cout << "\033[4m";
    cout << "\r[" << currentLineIndex + 1 << "] > " << currentLine << "\033[K" << flush;
    cout << "\033[0m"; // Reset format
}

int main() {
    logFile.open(".log.txt", ios::app);
    enableRawMode();

    // Menampilkan informasi awal
    cout << "\033[2J\033[H"; // Clear screen and move cursor to home position
    cout << "=== Simple Text Editor ===\n";
    cout << "Commands:\n";
    cout << "  Ctrl+S : Save\n";
    cout << "  Ctrl+U : Undo\n";
    cout << "  Ctrl+Y : Redo\n";
    cout << "  Ctrl+D : Delete Last Word\n";
    cout << "  Ctrl+X : Exit (dengan konfirmasi)\n";
    cout << "  Ctrl+B : Toggle Bold\n";
    cout << "  Ctrl+K : Toggle Italic\n";
    cout << "  Ctrl+T : Toggle Underline\n";
    cout << "  Ctrl+Q : Move Up Line\n";
    cout << "  Ctrl+A : Move Down Line\n";
    cout << "  Enter  : Newline\n\n\n";

    cout << "[1] > " << flush;

    char ch;
    bool isStartOfWord = true;

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 24) { // ESC 
            promptExit();
            break;
        } else if (ch == 21) { // Ctrl+U
            handleUndo();
        } else if (ch == 25) { // Ctrl+Y
            handleRedo();
        } else if (ch == 4) { // Ctrl+D
            handleDeleteLastWord();
        } else if (ch == 19) { // Ctrl+S
            handleSave();
        } else if (ch == 2) { // Ctrl+X
            toggleBold();
        } else if (ch == 11) { // Ctrl+K
            toggleItalic();
        } else if (ch == 20) { // Ctrl+T
            toggleUnderline();
        } else if (ch == 17) { // Ctrl+Q
            moveUp();
        } else if (ch == 1) { // Ctrl+A
            moveDown();
        } else if (ch == '\n') { // Enter key
            pushToUndo();
            if (currentLineIndex < lines.size()) { // untuk menyimpan ke dalam baris 
                lines[currentLineIndex] = currentLine; // menyimpan currentLine ke dalam lines
            } else {
                lines.push_back(currentLine); // menambahkan currentLine ke dalam lines
            }
            fullText += currentLine + "\n"; 
            currentLine.clear(); // untuk mengosongkan currentLine setelah menekan Enter
            currentLineIndex = lines.size(); // pindah ke baris baru
            lines.push_back(""); // menambahkan baris kosong untuk input berikutnya
            cout << "\n> " << flush; // menampilkan prompt baru
            isStartOfWord = true;
        } else if (ch == 127) {
            if (!currentLine.empty()) { //ketika currentLine tidak kosong
                currentLine.pop_back(); // menghapus karakter terakhir dari currentLine
                if (currentLineIndex < lines.size()) // jika currentLineIndex lebih kecil dari ukuran lines
                    lines[currentLineIndex] = currentLine; // memperbarui baris yang sesuai di lines
            }
        } else {
            if (isStartOfWord) {
                pushToUndo(); // menyimpan currentLine ke dalam undoStack
                isStartOfWord = false; // menandai bahwa kita sudah tidak di awal kata lagi
            }
            currentLine += ch; // menambahkan karakter yang dimasukkan ke currentLine
            if (currentLineIndex < lines.size()) // memperbarui baris yang sesuai di lines
                lines[currentLineIndex] = currentLine; // memperbarui baris yang sesuai di lines
            if (ch == ' ') {
                isStartOfWord = true;
            }
        }
        displayText(); // menampilkan currentLine di terminal
    }

    cout << "\n[Exiting editor]\n";
    logFile.close();
    return 0;
}
