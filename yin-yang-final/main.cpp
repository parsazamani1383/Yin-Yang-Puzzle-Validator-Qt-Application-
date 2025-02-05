// Project: Yin-Yang Puzzle Validator
// Description: A Qt-based application to validate and display all valid configurations of a Yin-Yang puzzle.
// Author: Parsa zamani
// Institution: University of Kurdistan, Computer Engineering Department
// Degree: Bachelor's in Computer Engineering
//
// This program uses DFS and BFS to explore valid configurations of a 4x4 Yin-Yang puzzle.
// The puzzle ensures all same-colored cells are connected, and no 2x2 block of the same color exists.

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QGridLayout>
#include <QLabel>
#include <vector>
#include <queue>
#include <stack>
#include <string>

#define WHITE 0
#define BLACK 1

// Function to get the color of a cell at position `pos` from the encoded state.
unsigned int getColor(unsigned int state, int pos) {
    return (state >> (pos + 16)) & 1;
}

// Function to set the color of a cell at position `pos` in the encoded state.
unsigned int setColor(unsigned int state, int pos, int color) {
    if (color == BLACK) {
        return state | (1 << (pos + 16));
    } else {
        return state & ~(1 << (pos + 16));
    }
}

// Function to check if a cell at position `pos` exists in the encoded state.
bool exists(unsigned int state, int pos) {
    return (state >> pos) & 1;
}

// Function to set the existence of a cell at position `pos` in the encoded state.
unsigned int setExistence(unsigned int state, int pos, bool existence) {
    if (existence) {
        return state | (1 << pos);
    } else {
        return state & ~(1 << pos);
    }
}

// Recursive function for DFS to check connectivity of same-colored cells.
// `visited` keeps track of already visited cells.
void dfsConnectivity(unsigned int state, int pos, int color, std::vector<bool>& visited) {
    int row = pos / 4;
    int col = pos % 4;
    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    visited[pos] = true;

    for (int i = 0; i < 4; i++) {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        int newPos = newRow * 4 + newCol;
        if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4 &&
            !visited[newPos] && (getColor(state, newPos) == color || !exists(state, newPos))) {
            dfsConnectivity(state, newPos, color, visited);
        }
    }
}

// Function to validate the state based on the Yin-Yang puzzle rules.
bool isValid(unsigned int state, int pos) {
    // Check connectivity for both colors.
    for (int color = WHITE; color <= BLACK; color++) {
        std::vector<bool> visited(16, false);
        bool foundColor = false;
        for (int i = 0; i < 16; i++) {
            if (getColor(state, i) == color) {
                dfsConnectivity(state, i, color, visited);
                foundColor = true;
                break;
            }
        }
        if (foundColor) {
            for (int i = 0; i < 16; i++) {
                if (exists(state, i)) {
                    if ((getColor(state, i) == color) && !visited[i]) {
                        return false;
                    }
                }
            }
        }
    }

    // Check for 2x2 blocks of the same color.
    int row = pos / 4;
    int col = pos % 4;
    for (int i = -1; i <= 0; i++) {
        for (int j = -1; j <= 0; j++) {
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < 3 && newCol >= 0 && newCol < 3) {
                if (exists(state, newRow * 4 + newCol) && exists(state, newRow * 4 + newCol + 1) &&
                    exists(state, (newRow + 1) * 4 + newCol) && exists(state, (newRow + 1) * 4 + newCol + 1) &&
                    getColor(state, newRow * 4 + newCol) == getColor(state, newRow * 4 + newCol + 1) &&
                    getColor(state, newRow * 4 + newCol) == getColor(state, (newRow + 1) * 4 + newCol) &&
                    getColor(state, newRow * 4 + newCol) == getColor(state, (newRow + 1) * 4 + newCol + 1)) {
                    return false;
                }
            }
        }
    }

    return true;
}

// Convert the encoded state to a string representation.
std::string stateToString(unsigned int state) {
    std::string result;
    for (int i = 0; i < 16; i++) {
        if (exists(state, i)) {
            result += (getColor(state, i) == BLACK) ? 'B' : 'W';
        } else {
            result += '-';
        }
    }
    return result;
}

// Generate valid states using Depth-First Search (DFS).
std::vector<unsigned int> generateValidStates() {
    std::vector<unsigned int> validStates;
    unsigned int initialState = 0;
    std::stack<unsigned int> stateStack;
    stateStack.push(initialState);

    while (!stateStack.empty()) {
        unsigned int state = stateStack.top();
        stateStack.pop();

        int pos = -1;
        for (int i = 0; i < 16; i++) {
            if (!exists(state, i)) {
                pos = i;
                break;
            }
        }

        if (pos == -1) {
            if (isValid(state, 16)) {
                validStates.push_back(state);
            }
        } else {
            for (int color = WHITE; color <= BLACK; color++) {
                unsigned int newState = setExistence(setColor(state, pos, color), pos, true);
                if (isValid(newState, pos)) {
                    stateStack.push(newState);
                }
            }
        }
    }
    return validStates;
}

// Generate valid states using Breadth-First Search (BFS).
std::vector<unsigned int> bfsGenerateValidStates() {
    std::vector<unsigned int> validStates;
    unsigned int initialState = 0;
    std::queue<unsigned int> stateQueue;
    stateQueue.push(initialState);

    while (!stateQueue.empty()) {
        unsigned int state = stateQueue.front();
        stateQueue.pop();

        int pos = -1;
        for (int i = 0; i < 16; i++) {
            if (!exists(state, i)) {
                pos = i;
                break;
            }
        }

        if (pos == -1) {
            if (isValid(state, 16)) {
                validStates.push_back(state);
            }
        } else {
            for (int color = WHITE; color <= BLACK; color++) {
                unsigned int newState = setExistence(setColor(state, pos, color), pos, true);
                if (isValid(newState, pos)) {
                    stateQueue.push(newState);
                }
            }
        }
    }
    return validStates;
}

// Class to display a puzzle state in a new window.
class PuzzleDisplay : public QMainWindow {
    Q_OBJECT
public:
    PuzzleDisplay(unsigned int state, QWidget* parent = nullptr) : QMainWindow(parent) {
        auto* centralWidget = new QWidget(this);
        auto* gridLayout = new QGridLayout(centralWidget);

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                int pos = row * 4 + col;
                QLabel* cell = new QLabel(this);
                cell->setFixedSize(50, 50);

                QString style = "border: 1px solid gray; ";
                if (exists(state, pos)) {
                    if (getColor(state, pos) == BLACK) {
                        style += "background-color: black;";
                    } else {
                        style += "background-color: white;";
                    }
                } else {
                    style += "background-color: lightgray;";
                }
                cell->setStyleSheet(style);
                gridLayout->addWidget(cell, row, col);
            }
        }

        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);

        setCentralWidget(centralWidget);
        setWindowTitle("Puzzle Display");
        resize(210, 210);
    }
};

// Main application window with options to generate states using DFS or BFS.
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    std::vector<unsigned int> validStates;

public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        auto* centralWidget = new QWidget(this);
        auto* layout = new QVBoxLayout(centralWidget);

        auto* generateDFSButton = new QPushButton("Generate with DFS", this);
        auto* generateBFSButton = new QPushButton("Generate with BFS", this);
        auto* listWidget = new QListWidget(this);

        layout->addWidget(generateDFSButton);
        layout->addWidget(generateBFSButton);
        layout->addWidget(listWidget);

        // Connect DFS generation button
        connect(generateDFSButton, &QPushButton::clicked, this, [this, listWidget]() {
            listWidget->clear();
            validStates = generateValidStates();
            for (size_t i = 0; i < validStates.size(); ++i) {
                listWidget->addItem(QString::number(i + 1) + ": " + QString::fromStdString(stateToString(validStates[i])));
            }
        });

        // Connect BFS generation button
        connect(generateBFSButton, &QPushButton::clicked, this, [this, listWidget]() {
            listWidget->clear();
            validStates = bfsGenerateValidStates();
            for (size_t i = 0; i < validStates.size(); ++i) {
                listWidget->addItem(QString::number(i + 1) + ": " + QString::fromStdString(stateToString(validStates[i])));
            }
        });

        // Connect list item click to display the puzzle
        connect(listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            int index = item->text().split(":").first().toInt() - 1;
            if (index >= 0 && index < validStates.size()) {
                PuzzleDisplay* display = new PuzzleDisplay(validStates[index]);
                display->show();
            }
        });

        setCentralWidget(centralWidget);
        setWindowTitle("Yin Yang Puzzle Validator");
        resize(400, 300);
    }
};

#include "main.moc"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
