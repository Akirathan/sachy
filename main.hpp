#ifndef MAIN_HPP_
#define MAIN_HPP

#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <cctype>

using namespace std ;

class Field ;

class ChessBoard {
private:
    array<unique_ptr<Field>, 8*8> board ;
    const int size_N  = 8 ;
    const int size_M = 8 ;
    
    //int translate(array<int, 2> arr) ;
    array<int, 2> reverseTranslate(int s) ; //to ani nepotrebujeme
    void setField(int n, int m, unique_ptr<Field> field) ;
    
public:
    enum fieldType {KING, KNIGHT, FREE};
    enum fraction {WHITE, BLACK} ;
    
    ChessBoard() ;
    void printBorder() ;
    void print() ;
    void set() ;
    int getKingLocation(fraction) ;
};


/**
 * Abstraktni trida reprezentujici jedno policko.
 */
class Field {
public:
    virtual ChessBoard::fieldType getType() const =0 ;
    virtual ChessBoard::fraction getFraction() const =0 ;
    virtual void print() const =0 ; //figurky se vytisknou na zaklade sve frakce
};

class King : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    
public:
    King(ChessBoard::fraction) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual void print() const override ;
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
};



#endif 

