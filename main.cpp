#include "main.hpp"

using namespace std;

/**
 * prelozi arr = [n,m] na interni souradnicovani.
 */
int ChessBoard::translate(array<int, 2> arr) {
    return arr[0] * size_M + arr[1];
}

array<int, 2> ChessBoard::reverseTranslate(int s) {
    array<int, 2> res;
    res[0] = s / size_M;
    res[1] = s % size_M;
    return move(res);
}

ChessBoard::ChessBoard() {
}

void ChessBoard::print() {
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            if (j == 0)
                cout << "|";
            board[translate(array<int, 2>{i,j})]->print() ; //ok
            cout << "|" ;
        }
        cout << endl;
    }
}

void ChessBoard::set() {
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            setField(i, j, make_unique<Free>()) ;
        }
    }
    setField(0, 0, make_unique<King>()) ;
}

void ChessBoard::setField(int n, int m, unique_ptr<Field> field) {
    board[translate(array<int, 2>{n,m})] = move(field) ;
}

King::King() : type_(ChessBoard::KING) {
}

ChessBoard::fieldType King::getType() const {
    return type_;
}

void King::print() const {
    cout << "K";
}

Knight::Knight() : type_(ChessBoard::KNIGHT) {
}

ChessBoard::fieldType Knight::getType() const {
    return type_;
}

void Knight::print() const {
    cout << "N";
}

Free::Free() : type_(ChessBoard::FREE) {
}

ChessBoard::fieldType Free::getType() const {
    return type_;
}

void Free::print() const {
    cout << "_";
}

class A {
public:
    int cislo ;
};

int main(int argc, char ** argv) {
    unique_ptr<ChessBoard> chess = make_unique<ChessBoard>() ;
    chess->set() ;
    chess->print() ;
    cout << endl ;
}