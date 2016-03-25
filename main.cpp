#include <bits/unique_ptr.h>

#include "main.hpp"

using namespace std;

class Coordinate {
private:
    static const int size_N = 3 ;
    static const int size_M = 3 ;
    
public:
    /**
     * Preklada string na interni souradnice boardu.
     * @param s "a1" = "1a"
     * @return souradnice do board
     */
    static int toInt(const string & s) {
        string a = "abcdefgh" ;
        int charPos = 0, numPos = 0 ;
        if (isdigit(s[0])) { //"1a"
            charPos = 1 ;
        }
        else { //"a1"
            numPos = 1 ;
        }
        int m = a.find_first_of(s[charPos]) ;
        int n = size_N - (int)s[numPos] + 48 ;
        return toInt(array<int, 2> {n,m}) ;
    }
    
    //puvodni ChessBoard::translate(array<int, 2>) ;
    static int toInt(array<int, 2> arr) {
        return arr[0] * size_M + arr[1];
    }
};

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

/**
 * Vytiskne celou sachovnici ve stylu navrh1.txt
 */
void ChessBoard::print() {
    printBorder() ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            if (j == 0)
                cout << size_M - i << "  |" ; //cislo radku na zacatek
            board[Coordinate::toInt(array<int, 2>{i,j})]->print() ; //ok
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
    board[Coordinate::toInt(array<int, 2>{n,m})] = move(field) ;
}

King::King(ChessBoard::fraction fraction) : type_(ChessBoard::KING), 
    fraction_(fraction) {
}

ChessBoard::fieldType King::getType() const {
    return type_;
}

ChessBoard::fraction King::getFraction() const {
    return fraction_ ;
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

ChessBoard::fraction Knight::getFraction() const {
    return fraction_ ;
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

ChessBoard::fraction Free::getFraction() const { }

void Free::print() const {
    cout << "__";
}


int main(int argc, char ** argv) {
    unique_ptr<ChessBoard> chess = make_unique<ChessBoard>() ;
    /*chess->set() ;
    chess->print() ;
    chess->set() ;
    chess->print() ;*/
    
    cout << endl ;
}