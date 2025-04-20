#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <ctime>

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
ofstream logFile;

// Function to log actions with timestamp
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
        redoStack = stack<string>();
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
    // Clear fullText and rebuild it from lines[]
    fullText.clear();
    for (const string& line : lines) {
        fullText += line + "\n";  // Append each line with newline
    }

    // Save to file
    ofstream file("saved_text.txt");
    file << fullText;  // Write the updated content to the file
    file.close();
    logAction("Save to saved_text.txt");

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
        lines[currentLineIndex] = currentLine;  // Save current line before moving up
        currentLineIndex--;
        currentLine = lines[currentLineIndex];  // Update current line with the one from previous line
        logAction("Moved up to line: " + currentLine);
    }
}

void moveDown() {
    if (currentLineIndex < (int)lines.size() - 1) {
        lines[currentLineIndex] = currentLine;  // Save current line before moving down
        currentLineIndex++;
        currentLine = lines[currentLineIndex];  // Update current line with the one from next line
        logAction("Moved down to line: " + currentLine);
    }
}

void displayText() {
    // This will clear the current output and print it again
    cout << "\r[" << currentLineIndex + 1 << "] > " << currentLine << "\033[K" << flush;  // Overwrite the current line
}

int main() {
    logFile.open(".log.txt", ios::app); // Open log file for appending
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

    cout << "[1] > " << flush;

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
            moveUp();  // Move up in the text
        } else if (ch == 1) {
            moveDown(); // Move down in the text
        } else if (ch == '\n') {
            pushToUndo();
            if (currentLineIndex < lines.size()) {
                lines[currentLineIndex] = currentLine; // Update the line
            } else {
                lines.push_back(currentLine);
            }

            fullText += currentLine + "\n";
            currentLine.clear();
            currentLineIndex = lines.size(); // Move to the new line
            lines.push_back(""); // Create a new empty line
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
                lines[currentLineIndex] = currentLine; // Save changes to the correct line
            if (ch == ' ') {
                isStartOfWord = true;
            }
        }

        // Display the current line and the line number dynamically
        displayText(); // Call the displayText function to keep the interface updated
    }

    cout << "\n[Exiting editor]\n";
    logFile.close();
    return 0;
}
