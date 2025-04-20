#include <iostream>
#include <stack>
#include <string>
#include <fstream>
#include <termios.h>
#include <unistd.h>

using namespace std;

stack<string> undoStack;
stack<string> redoStack;
string currentLine = "";
string fullText = "";

void enableRawMode() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Non-canonical mode & no echo
    term.c_iflag &= ~(IXON);          // Disable Ctrl+S (XOFF) & Ctrl+Q (XON)
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
        redoStack = stack<string>(); // clear redo
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

    cout << "\n[Saved to saved_text.txt]\n";
    cout << "Isi yang disimpan:\n";

    size_t start = 0;
    size_t end;
    while ((end = fullContent.find('\n', start)) != string::npos) {
        cout << fullContent.substr(start, end - start) << endl;
        start = end + 1;
    }
    if (start < fullContent.size()) {
        cout << fullContent.substr(start) << endl;
    }

    cout << "> " << currentLine << flush;
}

void promptExit() {
    disableRawMode();
    cout << "\nApakah kamu ingin menyimpan sebelum keluar? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        handleSave();
    } else {
        cout << "[Keluar tanpa menyimpan]\n";
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
    cout << "  Enter  : Newline\n\n";

    cout << "> " << flush;

    char ch;
    bool isStartOfWord = true;

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 24) { // Ctrl+X
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
        } else if (ch == '\n') { // Enter = Newline
            pushToUndo();
            fullText += currentLine + "\n";
            currentLine.clear();
            cout << "\n> " << flush;
            isStartOfWord = true;
        } else if (ch == 127) { // Backspace
            if (!currentLine.empty())
                currentLine.pop_back();
        } else {
            if (isStartOfWord) {
                pushToUndo();
                isStartOfWord = false;
            }

            currentLine += ch;

            if (ch == ' ') {
                isStartOfWord = true;
            }
        }

        cout << "\r> " << currentLine << "\033[K" << flush;
    }

    cout << "\n[Exiting editor]\n";
    return 0;
}