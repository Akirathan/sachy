#include <bits/unique_ptr.h>

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

void ChessBoard::printBorder() {
    array<char, 8> charArray {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'} ;
    for (int i = 0; i < size_M; i++) {
        if (i == 0)
            cout << "    "; //4 mezery
        cout << charArray[i] << "  " ;
    }
    cout << endl << endl ;
}

void ChessBoard::print() {
    printBorder() ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            if (j == 0)
                cout << size_M - i << "  |" ; //cislo radku na zacatek
            board[translate(array<int, 2>{i,j})]->print() ; //ok
            cout << "|" ;
        }
        cout << "  " << size_M - i << endl ; //cislo radku na konci
    }
    cout << endl ;
    printBorder() ;
    //board[translate(array<int, 2>{i,j})]->print() ; //ok
}

/**
* Umisti figurky do hraciho pole.
*/
void ChessBoard::set() {
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            setField(i, j, make_unique<Free>()) ;
        }
    }
    setField(0, 0, make_unique<King>(ChessBoard::BLACK)) ;
}

/**
* Na souradnice [n,m] dosadi field.
*/
void ChessBoard::setField(int n, int m, unique_ptr<Field> field) {
    board[translate(array<int, 2>{n,m})] = move(field) ;
}

King::King(ChessBoard::fraction fraction) : type_(ChessBoard::KING), 
    fraction_(fraction) {
}

ChessBoard::fieldType King::getType() const {
    return type_;
}

void King::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        cout << "wK" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        cout << "bK" ;
    }
}

Knight::Knight(ChessBoard::fraction fraction) : type_(ChessBoard::KNIGHT),
        fraction_(fraction) {
}

ChessBoard::fieldType Knight::getType() const {
    return type_;
}

void Knight::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        cout << "wN" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        cout << "bN" ;
    }
}

Free::Free() : type_(ChessBoard::FREE) {
}

ChessBoard::fieldType Free::getType() const {
    return type_;
}

void Free::print() const {
    cout << "__";
}

class A {
public:
    int cislo ;
};

class B : public A {
    
};

int main(int argc, char ** argv) {
    unique_ptr<ChessBoard> chess = make_unique<ChessBoard>() ;
    /*chess->set() ;
    chess->print() ;*/
    chess->set() ;
    chess->print() ;
}