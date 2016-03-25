#include <bits/unique_ptr.h>

#include "main.hpp"

using namespace std;


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
            board[Coordinate::translate(array<int, 2>{i,j})]->print() ; //ok
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
void ChessBoard::setBoard() {
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            setField(i, j, make_unique<Free>()) ;
        }
    }
    setField(0, 0, make_unique<King>(ChessBoard::BLACK, Coordinate(array<int, 2>{0,0}))) ;
}

Coordinate & ChessBoard::getKingLocation(ChessBoard::fraction fraction) const  {
    
}

ChessBoard::fraction ChessBoard::getOppositeFraction(ChessBoard::fraction frac) const  {
    if (frac == BLACK) 
        return WHITE ;
    else if (frac == WHITE)
        return BLACK ;
}

void ChessBoard::gameCycle() {
    Coordinate kingLocation = getKingLocation(player) ; 
    //je kingLocation v ohrozeni?
    fraction opponent = getOppositeFraction(player) ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            if (board[Coordinate::translate(array<int, 2>{i,j})]->getFraction() == opponent && 
                board[Coordinate::translate(array<int, 2>{i,j})]->canMove(kingLocation)) {
                check = true ;
            }
        }
    }

    
    nextPlayer() ;
}

/**
* Na souradnice [n,m] dosadi field.
*/
void ChessBoard::setField(int n, int m, unique_ptr<Field> field) {
    board[Coordinate::translate(array<int, 2>{n,m})] = move(field) ;
}

/**
 * Posune na tah dalsiho hrace
 */
void ChessBoard::nextPlayer() {
    if (player == BLACK) {
        player = WHITE ;
    }
    else if (player == WHITE) {
        player = BLACK ;
    }
}

King::King(ChessBoard::fraction fraction, Coordinate coordinate) : type_(ChessBoard::KING), 
    fraction_(fraction), location_(coordinate) {
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

bool King::canMove(Coordinate & coordinate) const {
    int dX = abs(location_.X - coordinate.X) ;
    int dY = abs(location_.Y - coordinate.Y) ;
    if (dX > 1 || dY > 1) return false ;
    else return true ;
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

bool Free::canMove(Coordinate & coordinate) const { }

class A {
public:
    int cislo ;
    A() {}
};

void zkouska(A & a1) {
    A a2(a1) ;
    cout << endl ;
}

int main(int argc, char ** argv) {
    //unique_ptr<ChessBoard> chess = make_unique<ChessBoard>() ;
    /*chess->set() ;
    chess->print() ;
    chess->set() ;
    chess->print() ;*/
    A a1 ;
    zkouska(a1) ;
    
    cout << endl ;
}