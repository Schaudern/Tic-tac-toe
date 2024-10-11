#include <iostream>
#include <vector>
#include <climits>
#include <chrono>

using namespace std;

class Exception : public exception
{
protected:
    string error;
public:
    Exception(string s)
    {
        error = s;
    }
    void print()
    {
        cout << "Exception: " << error;
    }
};

class WrongIndexesException : public Exception
{
protected:
    int index1;
    int index2;
public:
    WrongIndexesException(string s, int i1, int i2) : Exception(s)
    {
        index1 = i1;
        index2 = i2;
    }
    void print()
    {
        cout << "WrongIndexesException: " << error << ", row index = " << index1 << ", column index = " << index2;
    }
};

class WrongSizeException : public Exception
{
private:
    int size;
public:
    WrongSizeException(int s, string str) : Exception(str)
    {
        size = s;
    }
    void print()
    {
        cout << "WrongSizeException: " << error << ", size = " << size;
    }
};

class NegativeParameterException : public Exception
{
private:
    int count;
public:
    NegativeParameterException(int c, string str) : Exception(str)
    {
        count = c;
    }
    void print()
    {
        cout << " NegativeParameterException: " << error << "win_count = " << count;
    }
};

// Класс для работы с игровым полем
class Board
{
private:
    int size; // Размер игрового поля
    char** grid; //Игровое поле

public:
    Board(int size) : size(size)
    {
        // Инициализация пустого игрового поля
        if (size <= 0 or size > 10)
            throw WrongSizeException(size, "error");
        grid = new char* [size];
        for (int i = 0; i < size; i++)
        {
            grid[i] = new char[size];
            for (int j = 0; j < size; j++)
            {
                grid[i][j] = ' ';
            }
        }
    }

    Board(const Board& b)
    {
        size = b.size;
        grid = new char* [size];
        for (int i = 0; i < size; i++)
        {
            grid[i] = new char[size];
            for (int j = 0; j < size; j++)
            {
                grid[i][j] = b.grid[i][j];
            }
        }
    }

    ~Board()
    {
        for (int i = 0; i < size; i++)
        {
            delete[] grid[i];
        }
        delete[] grid;
    }

    // Печать игрового поля
    void printBoard()
    {
        cout << endl;
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
                cout << grid[i][j] << " ";
            cout << endl;
        }
    }

    // Проверка победной комбинации
    bool checkWin(char symbol, int Win_Count, int row, int col)
    {
        if (row >= size or col >= size)
        {
            throw WrongIndexesException("Going beyond array boundaries", row, col);
        }
        //Проверка строк и столбцов
        int row_count = 0;
        int k = col;
        while (k < size and grid[row][k] == symbol)
        {
            row_count++;
            k++;
        }
        k = col;
        while (k >= 0 and grid[row][k] == symbol)
        {
            row_count++;
            k--;
        }
        row_count = row_count - 1;
        if (row_count == Win_Count)
            return true;

        int col_count = 0;
        int w = row;
        while (w < size and grid[w][col] == symbol)
        {
            col_count++;
            w++;
        }
        w = row;
        while (w >= 0 and grid[w][col] == symbol)
        {
            col_count++;
            w--;
        }
        col_count = col_count - 1;
        if (col_count == Win_Count)
            return true;

        //Проверка диагоналей
        k = row;
        w = col;
        int diag_count = 0;
        while (k >= 0 and w < size and grid[k][w] == symbol)
        {
            diag_count++;
            k--;
            w++;
        }
        k = row;
        w = col;
        while (k < size and w >= 0 and grid[k][w] == symbol)
        {
            diag_count++;
            k++;
            w--;
        }
        diag_count--;
        if (diag_count == Win_Count)
            return true;

        diag_count = 0;
        k = row;
        w = col;
        while (k >= 0 and w >= 0 and grid[k][w] == symbol)
        {
            diag_count++;
            k--;
            w--;
        }
        k = row;
        w = col;
        while (k < size and w < size and grid[k][w] == symbol)
        {
            diag_count++;
            k++;
            w++;
        }
        diag_count--;
        if (diag_count == Win_Count)
            return true;

        return false;
    }

    friend class Game;
};

// Класс для реализации игры
class Game
{
private:
    int size; // Размер игрового поля
    int winCount; // Количество символов в ряд для победы
    Board board; // Игровое поле

public:
    Game(int size, int winCount) : size(size), winCount(winCount), board(size)
    {
        if (winCount < 0)
            throw NegativeParameterException(winCount, "error");
    }

    // Ход компьютера
    bool makeComputerMove()
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        int row, col;
        int bestScore = INT_MIN;

        // Поиск лучшего хода с помощью алгоритма альфа-бета отсечения
        for (int i = 0; i < size; i++)
        {
            char* ptr = board.grid[i];
            for (int j = 0; j < size; j++)
            {
                if (*ptr == ' ')
                {
                    *ptr = 'O';
                    int score = minimax(board, 0, false, INT_MIN, INT_MAX, i, j);
                    *ptr = ' ';
                    if (score > bestScore)
                    {
                        bestScore = score;
                        row = i;
                        col = j;
                    }
                }
                ptr++;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        cout << '\n';
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        board.grid[row][col] = 'O';
        cout << "Компьютер сделал ход: (" << row << ", " << col << ")" << std::endl;
        cout << "time = " << duration.count();
        board.printBoard();

        // Проверка победы компьютера
        if (board.checkWin('O', winCount, row, col))
        {
            std::cout << "Компьютер победил!" << endl;
            return true;
        }

        return false;
    }

    // Рекурсивная функция для алгоритма альфа-бета отсечения
    int minimax(Board& board, int depth, bool isMaximizingPlayer, int alpha, int beta, int r, int c) {
        if (isMaximizingPlayer)
        {
            if (board.checkWin('X', winCount, r, c))
            {
                return -10 + depth;
            }
        }
        else
        {
            if (board.checkWin('O', winCount, r, c))
            {
                return 10 - depth;
            }
        }

        bool full = true;
        for (int i = 0; i < size; i++)
            for (int j = i; j < size; j++)
            {
                if (board.grid[i][j] == ' ' or board.grid[j][i] == ' ')
                    full = false;
            }
        if (full)
            return 0;

        if (depth > 5)
            return 0;

        if (isMaximizingPlayer) {
            int bestScore = INT_MIN;
            for (int i = 0; i < size; i++)
            {
                char* ptr = board.grid[i];
                for (int j = 0; j < size; j++)
                {
                    if (*ptr == ' ')
                    {
                        if (beta <= alpha)
                            return bestScore;
                        *ptr = 'O';
                        int score = minimax(board, depth + 1, false, alpha, beta, i, j);
                        *ptr = ' ';
                        bestScore = score > bestScore ? score : bestScore;
                        alpha = bestScore;
                    }
                    ptr++;
                }
            }
            return bestScore;
        }
        else {
            int bestScore = INT_MAX;
            for (int i = 0; i < size; i++)
            {
                char* ptr = board.grid[i];
                for (int j = 0; j < size; j++)
                {
                    if (*ptr == ' ')
                    {
                        if (beta <= alpha)
                            return bestScore;
                        *ptr = 'X';
                        int score = minimax(board, depth + 1, true, alpha, beta, i, j);
                        *ptr = ' ';
                        bestScore = score < bestScore ? score : bestScore;
                        beta = bestScore;
                    }
                    ptr++;
                }
            }
            return bestScore;
        }
    }

    // Ход пользователя
    bool makeUserMove()
    {
        int row, col;
        cout << "Ваш ход: ";
        cin >> row >> col;
        while (row < 0 || row >= size || col < 0 || col >= size || board.grid[row][col] != ' ')
        {
            cout << "Некорректный ход. Введите координаты еще раз: ";
            cin >> row >> col;
        }
        board.grid[row][col] = 'X';
        board.printBoard();
        if (board.checkWin('X', winCount, row, col))
        {
            cout << "Вы победили! Поздравляю!" << endl;
            return true;
        }
        return false;
    }

    // Запуск игры
    void startGame()
    {
        cout << "Игра \"Крестики-нолики\"" << endl << endl;
        cout << "Введите координаты ячейки, чтобы сделать ход. Нумерация строк и столбцов начинается с 0." << std::endl;
        cout << "Вы играете за крестики, компьютер - за нолики." << endl << endl;

        // Печать пустого игрового поля
        board.printBoard();

        // Игровой цикл
        while (true)
        {
            if (makeUserMove())
                break;

            // Проверка ничьей
            bool full = true;
            for (int i = 0; i < size; i++)
                for (int j = i; j < size; j++)
                {
                    if (board.grid[i][j] == ' ' or board.grid[j][i] == ' ')
                    {
                        full = false;
                        break;
                    }
                }
            if (full)
            {
                cout << "Ничья!" << endl;
                break;
            }

            if (makeComputerMove())
                break;

            // Проверка ничьей
            full = true;
            for (int i = 0; i < size; i++)
                for (int j = i; j < size; j++)
                {
                    if (board.grid[i][j] == ' ' or board.grid[j][i] == ' ')
                    {
                        full = false;
                        break;
                    }
                }
            if (full)
            {
                cout << "Ничья!" << endl;
                break;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    int size, winCount;
    std::cout << "Введите размер игрового поля: ";
    std::cin >> size;
    std::cout << "Введите количество символов в ряд для победы: ";
    std::cin >> winCount;

    try
    {
        Game game(size, winCount);
        game.startGame();
    }
    catch (WrongIndexesException e)
    {
        e.print();
    }
    catch (WrongSizeException e)
    {
        e.print();
    }
    catch (NegativeParameterException e)
    {
        e.print();
    }
    catch (Exception e)
    {
        e.print();
    }

    char c; cin >> c;
    return 0;
}