#ifndef MAIN_HPP_
#define MAIN_HPP

#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <cctype>

using namespace std ;

class Field ;

class Coordinate {
private:
    static const int size_N = 3 ;
    static const int size_M = 3 ;
    int coordinate_ ; //souradnice pro board
    
    static array<int, 2> reverseTranslate(int s) {
        array<int, 2> res;
        res[0] = s / size_M;
        res[1] = s % size_M;
        return move(res);
    }
    
public:
    int X, Y ;
    
    Coordinate(const string & s) {
        coordinate_ = translate(s) ;
        array<int, 2> arr = reverseTranslate(coordinate_) ;
        X = arr[0] ;
        Y = arr[1] ;
    }
    
    Coordinate(array<int, 2> arr) {
        coordinate_ = translate(arr) ;
        X = arr[0] ;
        Y = arr[1] ;
    }
    /**
     * Preklada string na interni souradnice boardu.
     * @param s "a1" = "1a"
     * @return souradnice do board
     */
    static int translate(const string & s) {
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
        return translate(array<int, 2> {n,m}) ;
    }
    
    //puvodni ChessBoard::translate(array<int, 2>) ;
    static int translate(array<int, 2> arr) {
        return arr[0] * size_M + arr[1];
    }
};

class ChessBoard {
public:
    enum fieldType {KING, KNIGHT, FREE};
    enum fraction {WHITE, BLACK} ;
    
private:
    array<unique_ptr<Field>, 8*8> board ;
    const int size_N  = 8 ;
    const int size_M = 8 ;
    fraction player ;
    bool check ; //inicializuje se na false
    
    void setField(int n, int m, unique_ptr<Field> field) ;
    void nextPlayer() ;
    fraction getOppositeFraction(fraction frac) const ;
    
public:
    
    ChessBoard() ;
    void printBorder() ;
    void print() ;
    void setBoard() ;
    Coordinate & getKingLocation(fraction) const ; //TODO
    
    /**
     * Zjistovani, jestli je kral v ohrozeni probiha tak,
     * ze se projdeme vsechny nepratelske figurky a vyzkousime,
     * jestli dosahnou na krale.
     */
    void gameCycle() ;
};


/**
 * Abstraktni trida reprezentujici jedno policko.
 */
class Field {
public:
    virtual ChessBoard::fieldType getType() const =0 ;
    virtual ChessBoard::fraction getFraction() const =0 ;
    virtual void print() const =0 ; //figurky se vytisknou na zaklade sve frakce
    
    /**
     * Podle aktualnich souradnic dane figurky (tj. location_) rozhodne,
     * jestli se muze pohnout na coordinate
     * @param coordinate souradnice, kam se figurka chce pohnout
     * @return true = muze se tam pohnout
     */
    virtual bool canMove(Coordinate & coordinate) const =0 ;
};

class King : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    Coordinate location_ ;
    
public:
    King(ChessBoard::fraction fraction, Coordinate coordinate) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual void print() const override ;
    virtual bool canMove(Coordinate & coordinate) const override ;
};


class Knight : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    
public:
    Knight(ChessBoard::fraction) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual void print() const override ;
};

class Pawn : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    
public:
    Pawn(ChessBoard::fraction) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual void print() const override ;
};


class Free : public Field {
private:
    ChessBoard::fieldType type_ ;
    
public:
    Free() ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual void print() const override ;
    virtual bool canMove(Coordinate & coordinate) const override ;
};



#endif 

