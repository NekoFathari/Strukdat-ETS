#include <iostream>
#include <stack>
#include <string>
using namespace std;

stack<string> undoStack;
stack<string> redoStack;
string currentText = "";

void insertText(const string& text) {
    undoStack.push(currentText);              // Simpan state sebelum perubahan
    currentText += text;
    while (!redoStack.empty()) redoStack.pop(); // Reset redo
}

void deleteLastLine() {
    undoStack.push(currentText);
    size_t pos = currentText.rfind('\n');
    if (pos == string::npos) {
        currentText = "";
    } else {
        currentText = currentText.substr(0, pos + 1);
    }
    while (!redoStack.empty()) redoStack.pop();
}

void undo() {
    if (!undoStack.empty()) {
        redoStack.push(currentText);
        currentText = undoStack.top();
        undoStack.pop();
        cout << currentText;
    } else {
        cout << "[Undo Stack Kosong]\n";
    }
}

void redo() {
    if (!redoStack.empty()) {
        undoStack.push(currentText);
        currentText = redoStack.top();
        redoStack.pop();
        cout << currentText;
    } else {
        cout << "[Redo Stack Kosong]\n";
    }
}

void simulateEditor() {
    string input;
    cout << "=== Simple Text Editor ===\n";
    cout << "Commands:\n";
    cout << "  Ctrl+S : Save\n";
    cout << "  Ctrl+U : Undo\n";
    cout << "  Ctrl+Y : Redo\n";
    cout << "  Ctrl+D : Delete Last Line\n";
    cout << "  Ctrl+X : Exit\n";
    cout << "  \\n     : Newline\n";

    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "Ctrl+X") break;
        else if (input == "Ctrl+S") {
            cout << "[SAVE] Current Text:\n" << currentText;
        }
        else if (input == "Ctrl+U") undo();
        else if (input == "Ctrl+Y") redo();
        else if (input == "Ctrl+D") deleteLastLine(); // boleh tambah cout kalau kamu mau
        else if (input == "\\n") insertText("\n");
        else insertText(input + "\n");
    }
}

int main() {
    simulateEditor();
    return 0;
}