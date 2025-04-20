#include <iostream>
#include <stack>
#include <fstream>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>

using namespace std;

// Node for Linked List
struct Node {
    string word;
    string format;
    Node* next;

    Node(string w, string f) : word(w), format(f), next(nullptr) {}
};

// Terminal raw input (Linux)
char getChar() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Text Editor Class
class TextEditor {
private:
    Node* head;
    stack<Node*> undoStack;
    stack<Node*> redoStack;
    ofstream logFile;
    string currentFormat;

public:
    TextEditor() {
        head = nullptr;
        currentFormat = "";
        logFile.open(".log.txt", ios::app); // hidden log
    }

    ~TextEditor() {
        clearList();
        logFile.close();
    }

    void clearList() {
        Node* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

    void toggleFormat(char fmt) {
        if (fmt == 'B') currentFormat = "[B]";
        else if (fmt == 'N') currentFormat = "[I]";
        else if (fmt == 'M') currentFormat = "[U]";
        else currentFormat = "";
    }

    void addInput(const string& text) {
        string word = "";
        for (char ch : text + " ") {
            if (ch == ' ') {
                if (!word.empty()) {
                    Node* newNode = new Node(word, currentFormat);
                    appendNode(newNode);
                    undoStack.push(newNode);
                    log("Input: " + word + " " + currentFormat);
                    word.clear();
                }
            } else {
                word += ch;
            }
        }
        // Clear redo stack karena input baru
        while (!redoStack.empty()){
            delete redoStack.top();
            redoStack.pop();
        }
    }

    void appendNode(Node* node) {
        if (!head) head = node;
        else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = node;
        }
    }

    void deleteLastWord() {
        if (!head) return;
        if (!head->next) {
            undoStack.push(head);
            log("Delete: " + head->word);
            head = nullptr;
            return;
        }

        Node* temp = head;
        while (temp->next && temp->next->next) temp = temp->next;
        Node* del = temp->next;
        temp->next = nullptr;
        undoStack.push(del);
        log("Delete: " + del->word);
    }

    void undo() {
        if (undoStack.empty()) return;
    
        Node* last = undoStack.top(); undoStack.pop();
        Node* removed = removeLastNode();
    
        if (removed) {
            redoStack.push(removed);
            log("Undo: " + removed->word);
        }
    }
    

    void redo() {
        if (redoStack.empty()) return;
        Node* redoNode = redoStack.top(); redoStack.pop();
        appendNode(redoNode);
        undoStack.push(redoNode);
        log("Redo: " + redoNode->word);
    }

    Node* removeLastNode() {
        if (!head) return nullptr;
        if (!head->next) {
            Node* temp = head;
            head = nullptr;
            return temp;
        }
    
        Node* temp = head;
        while (temp->next && temp->next->next) temp = temp->next;
        Node* removed = temp->next;
        temp->next = nullptr;
        return removed;
    }
    

    void displayText() {
        Node* temp = head;
        cout << "\nCurrent text: ";
        while (temp) {
            cout << temp->format << temp->word << " ";
            temp = temp->next;
        }
        cout << endl;
    }

    void saveToFile() {
        log("Saved to file.");
        cout << "\n[SAVED TO .log.txt]\n";
    }

    void log(const string& activity) {
        logFile << activity << endl;
    }
};

// === MAIN ===

int main() {
    TextEditor editor;
    string input = "";
    bool running = true;

    cout << "==== Simple Text Editor Terminal ====\n";
    cout << "Shortcut:\n";
    cout << "  Ctrl+B = Bold\n";
    cout << "  Ctrl+N = Italic\n";
    cout << "  Ctrl+L = Underline\n";
    cout << "  Ctrl+D = Delete last word\n";
    cout << "  Ctrl+U = Undo\n";
    cout << "  Ctrl+Y = Redo\n";
    cout << "  Ctrl+S = Save log file\n";
    cout << "  Enter  = Submit input\n";
    cout << "  ESC    = Exit\n";
    cout << "=====================================\n\n";

    while (running) {
        cout << "\n> ";
        input = "";

        while (true) {
            char ch = getChar();

            if (ch == 2) { // Ctrl+B
                editor.toggleFormat('B');
                cout << "\r[BOLD MODE] > " << flush;
                continue;
            } else if (ch == 14) { // Ctrl+N
                editor.toggleFormat('N');
                cout << "\r[ITALIC MODE] > " << flush;
                continue;
            } else if (ch == 12) { // Ctrl+L
                editor.toggleFormat('M');
                cout << "\r[UNDERLINE MODE] > " << flush;
                continue;
            }else if (ch == 127 || ch == 8) {
                if (!input.empty()) {
                    input.pop_back(); // hapus 1 karakter dari input
                    cout << "\b \b" << flush; // hapus dari layar: back, space, back
                }
                continue;
            
            
            } else if (ch == 4) { // Ctrl+D
                editor.deleteLastWord();
                editor.displayText();
                cout << "\n> " << input << flush;
                continue;
            } else if (ch == 21) { // Ctrl+U
                editor.undo();
                editor.displayText();
                cout << "\n> " << input << flush;
                continue;
            } else if (ch == 25) { // Ctrl+Y
                editor.redo();
                editor.displayText();
                cout << "\n> " << input << flush;
                continue;
            } else if (ch == 19) { // Ctrl+S
                editor.saveToFile();
                cout << "\n> " << input << flush;
                continue;
            } else if (ch == 27) { // ESC
                running = false;
                break;
            } else if (ch == '\n') {
                if (!input.empty()) {
                    editor.addInput(input);
                    input = "";
                }
                editor.displayText();
                cout << "\n> " << flush;
                continue;
            } else {
                input += ch;
                cout << ch << flush;
            }
        }

        if (!input.empty()) {
            editor.addInput(input);
            editor.displayText();
        }
    }

    cout << "\nExiting editor... Bye!\n";
    return 0;
}
