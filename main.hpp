#ifndef MAIN_HPP_
#define MAIN_HPP

#include <iostream>
#include <array>
#include <memory>

using namespace std ;

class Field ;

class ChessBoard {
private:
    array<unique_ptr<Field>, 3*3> board ;
    const int size_N  = 3 ;
    const int size_M = 3 ;
    
    int translate(array<int, 2> arr) ;
    array<int, 2> reverseTranslate(int s) ;
    
    /**
     * Na souradnice [n,m] dosadi field.
     */
    void setField(int n, int m, unique_ptr<Field> field) ;
    
public:
    enum fieldType {KING, KNIGHT, FREE};
    
    ChessBoard() ;
    
    void print() ;
    
    /**
     * Umisti figurky do hraciho pole.
     */
    void set() ;
};


//reprezentuje jedno policko
class Field {
public:
    virtual ChessBoard::fieldType getType() const =0 ;
    virtual void print() const =0 ;
};

class King : public Field {
private:
    ChessBoard::fieldType type_ ;
    
public:
    King() ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual void print() const override ;
};


class Knight : public Field {
private:
    ChessBoard::fieldType type_ ;
    
public:
    Knight() ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual void print() const override ;
};


class Free : public Field {
private:
    ChessBoard::fieldType type_ ;
    
public:
    Free() ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual void print() const override ;
};



#endif 

