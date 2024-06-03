#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Game {
public:
    class Cell {
    private:
        int row;
        int column;
    public:
        Cell(int row, int column) {
            this->row = row;
            this->column = column;
        }
        int getRow() {
            return row;
        }
        int getColumn() {
            return column;
        }
    };

    class Region {
    private:
        vector<Cell*> matrix;
        int num_cells;
    public:
        Region(int num_cells) {
            this->num_cells = num_cells;
            matrix.resize(num_cells);
        }
        vector<Cell*>& getCells() {
            return matrix;
        }
        void setCell(int pos, Cell* element) {
            matrix[pos] = element;
        }
    };

    class Board {
    private:
        vector<vector<int>> board_values;
        vector<Region*> board_regions;
        int num_rows;
        int num_columns;
        int num_regions;
    public:
        Board(int num_rows, int num_columns, int num_regions) {
            this->num_rows = num_rows;
            this->num_columns = num_columns;
            this->num_regions = num_regions;
            board_values.resize(num_rows, vector<int>(num_columns));
            board_regions.resize(num_regions);
        }
        vector<vector<int>>& getValues() {
            return board_values;
        }
        int getValue(int row, int column) {
            return board_values[row][column];
        }
        Region* getRegion(int index) {
            return board_regions[index];
        }
        vector<Region*>& getRegions() {
            return board_regions;
        }
        void setValue(int row, int column, int value) {
            board_values[row][column] = value;
        }
        void setRegion(int index, Region* initial_region) {
            board_regions[index] = initial_region;
        }
        void setValues(vector<vector<int>>& values) {
            board_values = values;
        }
    };

    Board* sudoku;

    Game() {
        sudoku = nullptr;
    }

    vector<vector<int>> solver() {
        solve();
        return sudoku->getValues();
    }

    bool solve() {
        for (int r = 0; r < sudoku->num_rows; r++) {
            for (int c = 0; c < sudoku->num_columns; c++) {
                if (sudoku->getValue(r, c) == -1) {
                    int num_of_cells = 0;
                    Cell* cell_position = nullptr;
                    Region* cell_region = nullptr;
                    for (Region* region : sudoku->getRegions()) {
                        for (Cell* cell : region->getCells()) {
                            if (cell->getColumn() == c && cell->getRow() == r) {
                                num_of_cells = region->getCells().size();
                                cell_position = cell;
                                cell_region = region;
                            }
                        }
                    }
                    for (int i = 1; i <= num_of_cells; i++) {
                        if (!is_taken(cell_position, i) && is_unique(cell_position, cell_region, i) && !in_region(cell_region, i)) {
                            sudoku->setValue(r, c, i);
                            bool isValid = solve();
                            if (isValid) {
                                return true;
                            } else {
                                sudoku->setValue(r, c, -1);
                            }
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    bool is_unique(Cell* cl, Region* r, int value) {
        for (Cell* cells : r->getCells()) {
            if (cells->getColumn() == cl->getColumn() && sudoku->getValue(cells->getRow(), cells->getColumn()) == value && cells->getRow() != cl->getRow()) {
                return false;
            }
            if (cells->getRow() == cl->getRow() && sudoku->getValue(cells->getRow(), cells->getColumn()) == value && cells->getColumn() != cl->getColumn()) {
                return false;
            }
        }
        return true;
    }

    bool in_region(Region* r, int value) {
        for (Cell* cell : r->getCells()) {
            int cell_value = sudoku->getValue(cell->getRow(), cell->getColumn());
            if (cell_value != -1 && cell_value == value) {
                return true;
            }
        }
        return false;
    }

    bool is_taken(Cell* cl, int value) {
        int column = cl->getColumn();
        int row = cl->getRow();
        int left = column - 1;
        int right = column + 1;
        for (int i = -1; i <= 1; i++) {
            int c = row + i;
            if (c <= (sudoku->num_rows - 1) && c >= 0 && left >= 0 && left <= (sudoku->num_columns - 1)) {
                if (sudoku->getValue(c, left) == value) {
                    return true;
                }
            }
            if (c <= (sudoku->num_rows - 1) && c >= 0 && right >= 0 && right <= (sudoku->num_columns - 1)) {
                if (sudoku->getValue(c, right) == value) {
                    return true;
                }
            }
        }
        int up = row + 1;
        int down = row - 1;
        if (up >= 0 && up < sudoku->num_rows) {
            if (sudoku->getValue(up, column) == value) {
                return true;
            }
        }
        if (down >= 0 && down < sudoku->num_rows) {
            if (sudoku->getValue(down, column) == value) {
                return true;
            }
        }
        return false;
    }

    int main() {
        int rows, columns;
        cin >> rows >> columns;
        vector<vector<int>> board(rows, vector<int>(columns));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                string value;
                cin >> value;
                if (value == "-") {
                    board[i][j] = -1;
                } else {
                    try {
                        board[i][j] = stoi(value);
                    } catch (...) {
                        cout << "Oops, something went wrong" << endl;
                    }
                }
            }
        }
        int regions;
        cin >> regions;
        Game game;
        game.sudoku = new Game::Board(rows, columns, regions);
        game.sudoku->setValues(board);
        for (int i = 0; i < regions; i++) {
            int num_cells;
            cin >> num_cells;
            Game::Region* new_region = new Game::Region(num_cells);
            for (int j = 0; j < num_cells; j++) {
                string cell;
                cin >> cell;
                string value1 = cell.substr(cell.find("(") + 1, cell.find(",") - cell.find("(") - 1);
                string value2 = cell.substr(cell.find(",") + 1, cell.find(")") - cell.find(",") - 1);
                Game::Cell* new_cell = new Game::Cell(stoi(value1) - 1, stoi(value2) - 1);
                new_region->setCell(j, new_cell);
            }
            game.sudoku->setRegion(i, new_region);
        }
        vector<vector<int>> answer = game.solver();
        for (int i = 0; i < answer.size(); i++) {
            for (int j = 0; j < answer[0].size(); j++) {
                cout << answer[i][j];
                if (j < answer[0].size() - 1) {
                    cout << " ";
                }
            }
            cout << endl;
        }
    }
};

int main() {
    Game::main();
    return 0;
}
