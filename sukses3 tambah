#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <fstream>
#include <termios.h>
#include <unistd.h>

using namespace std;

stack<string> undoStack;
stack<string> redoStack;
string currentLine = "";
string fullText = "";
bool isBold = false;
bool isItalic = false;
bool isUnderline = false;
bool underlineActive = false;
int currentLineIndex = 0;
vector<string> lines;

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
        redoStack = stack<string>();
    }
}

void handleUndo() {
    if (!undoStack.empty()) {
        redoStack.push(currentLine);
        currentLine = undoStack.top();
        undoStack.pop();
    }
}

void handleRedo() {
    if (!redoStack.empty()) {
        undoStack.push(currentLine);
        currentLine = redoStack.top();
        redoStack.pop();
    }
}

void handleDeleteLastWord() {
    pushToUndo();
    size_t pos = currentLine.find_last_of(' ');
    if (pos != string::npos)
        currentLine = currentLine.substr(0, pos);
    else
        currentLine.clear();
}

void handleSave() {
    string fullContent = fullText + currentLine;
    ofstream file("saved_text.txt");
    file << fullContent << endl;
    file.close();

    currentLine.clear();
    cout << "\n[Saved to saved_text.txt]\n";
    cout << "> " << flush;
}

void promptExit() {
    disableRawMode();
    cout << "\nApakah kamu ingin menyimpan sebelum keluar? (y/n): ";
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
    if (underlineActive) cout << "\033[4m";
    else cout << "\033[24m";
}

void moveUp() {
    if (currentLineIndex > 0) {
        lines[currentLineIndex] = currentLine;
        currentLineIndex--;
        currentLine = lines[currentLineIndex];
    }
}

void moveDown() {
    if (currentLineIndex < (int)lines.size() - 1) {
        lines[currentLineIndex] = currentLine;
        currentLineIndex++;
        currentLine = lines[currentLineIndex];
    }
}

int main() {
    enableRawMode();

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

    cout << "> " << flush;

    char ch;
    bool isStartOfWord = true;

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 24) {
            promptExit();
            break;
        } else if (ch == 21) {
            handleUndo();
        } else if (ch == 25) {
            handleRedo();
        } else if (ch == 4) {
            handleDeleteLastWord();
        } else if (ch == 19) {
            handleSave();
        } else if (ch == 2) {
            toggleBold();
        } else if (ch == 11) {
            toggleItalic();
        } else if (ch == 20) {
            toggleUnderline();
        } else if (ch == 17) {
            moveUp();
        } else if (ch == 1) {
            moveDown();
        } else if (ch == '\n') {
            pushToUndo();
            if (currentLineIndex < lines.size()) {
                lines[currentLineIndex] = currentLine;
            } else {
                lines.push_back(currentLine);
            }
        
            fullText += currentLine + "\n";
        
            currentLine.clear();
            currentLineIndex = lines.size(); // Pindah ke baris baru
            lines.push_back(""); // Buat baris baru kosong
            cout << "\n> " << flush;
            isStartOfWord = true;
        } else if (ch == 127) {
            if (!currentLine.empty()) {
                currentLine.pop_back();
                if (currentLineIndex < lines.size())
                    lines[currentLineIndex] = currentLine;
            }
        }  else {
            if (isStartOfWord) {
                pushToUndo();
                isStartOfWord = false;
            }
            currentLine += ch;
            if (currentLineIndex < lines.size())
        lines[currentLineIndex] = currentLine;
            if (ch == ' ') {
                isStartOfWord = true;
            }
        }

        cout << "\r> " << currentLine << "\033[K" << flush;
    }

    cout << "\n[Exiting editor]\n";
    return 0;
}

