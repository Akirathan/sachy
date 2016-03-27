#ifndef MAIN_HPP_
#define MAIN_HPP

#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <cctype>
#include <cassert>

using namespace std ;

const int size_N = 3 ;
const int size_M = 3 ;

/**
 * Outputs keywords and their meaning.
 */
void printHelp() ;

class Field ;

class Coordinate {
private:
    int coordinate_ ; //souradnice pro board
    int X_, Y_ ;
    bool outOfBounds_ ;
    
public:
    
    Coordinate(const string & s) {
        coordinate_ = translateWithBoundCheck(s) ;
        array<int, 2> arr = reverseTranslate(coordinate_) ;
        X_ = arr[0] ;
        Y_ = arr[1] ;
    }
    
    Coordinate(array<int, 2> arr) {
        coordinate_ = translateWithBoundCheck(arr) ;
        X_ = arr[0] ;
        Y_ = arr[1] ;
    }
    
    bool operator== (const Coordinate & other) {
        return this->X_ == other.X_ && this->Y_ == other.Y_ ;
    }
    
    int getCoordinate() { return coordinate_ ;}
    int getX() { return X_ ;}
    int getY() { return Y_ ;}
    bool outOfBounds() { return outOfBounds_ ;}
    /**
     * Preklada string na interni souradnice boardu.
     * @param s "a1" = "1a"
     * @return souradnice do board
     */
    int translateWithBoundCheck(const string & s) {
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
        return translateWithBoundCheck(array<int, 2> {n,m}) ;
    }
    
    int translateWithBoundCheck(array<int, 2> arr) {
        if (arr[0] >= size_N || arr[0] < 0 ||
                arr[1] >= size_M || arr[1] < 0)
            outOfBounds_ = true ;
        else outOfBounds_ = false ;
        return arr[0] * size_M + arr[1];
    }
    
    static int translate(array<int, 2> arr) {
        return arr[0] * size_M + arr[1];
    }
    
    static array<int, 2> reverseTranslate(int s) {
        array<int, 2> res;
        res[0] = s / size_M;
        res[1] = s % size_M;
        return move(res);
    }
};


class ChessBoard {
public:
    enum fieldType {KING, KNIGHT, FREE};
    enum fraction {WHITE, BLACK} ;
    
private:
    array< unique_ptr< Field>, size_N * size_M> board ;
    fraction player ;
    bool checkWhite, checkBlack ; 
    
    void nextPlayer() ;
    fraction getOppositeFraction(fraction & frac) const ;
    
    /**
     * Sets check state for input fraction.
     * TODO can output the information
     * @param frac the fraction on which the check state is set
     */
    void setCheck(fraction & frac) ;
    
    /**
     * Checks whether the king of fraction frac on kingLocation is endangered.
     * Does it by searching all the chessboard for enemy figures
     * and calling method canMove(this.location) on them.
     * @param frac fraction of the king
     * @return true = the king is endangered
     */
    bool isKingEndangered(fraction frac, Coordinate & kingLocation) const ;
    
    /**
     * Prompts the player for input.
     * Does not care about who is actually on turn.
     * @return array[0] = the figure, array[1] = desired coordinates.
     */
    array<unique_ptr<Coordinate>, 2>  inputMove() const ;
    
    /**
     * Prints out the information abou who is on turn
     * and who is in check.
     * Can be extended for additional information.
     */
    void turnInformation() const ;
    
public:
    
    ChessBoard() ;
    
    /**
     * The content of field is robbed.
     * @param field Temporary object made by make_unique< Free>() for example.
     */
    void setField(int n, int m, unique_ptr<Field> field) ;
    
    /**
     * Return pointer with read only content
     * @param n
     * @param m
     * @return Pointer used only for reading
     */
    Field * viewField(int n, int m) const ;
    
    /**
     * Robs [n,m] and then creates Free object at [n,m]
     * @return robbed unique_ptr
     */
    std::unique_ptr< Field> getField(int n, int m) ;
    void printBorder() ;
    void print() ;
    void setBoard() ;
    Coordinate & getKingLocation(fraction) const ; //TODO
    
    /**
     * Zjistovani, jestli je kral v ohrozeni probiha tak,
     * ze se projdeme vsechny nepratelske figurky a vyzkousime,
     * jestli dosahnou na krale.
     */
    void gameTurn() ;
    
    /**
     * Calls inputMove and checks whether the move is correct.
     * When true is returned, the value of the parameter is set to correct move.
     * @param arr
     * @return 
     */
    bool checkMove(std::array< std::unique_ptr< Coordinate>, 2> & arr) ;
    
    void test1() ;
    void test2() ;
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
    virtual bool canMove(Coordinate & coordinate) =0 ;
    
    virtual void changeLocation(const Coordinate & coordinate) =0 ;
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
    virtual bool canMove(Coordinate & coordinate) override ;
    virtual void changeLocation(const Coordinate & coordinate) override ;
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
    virtual bool canMove(Coordinate & coordinate) override ;
    virtual void changeLocation(const Coordinate & coordinate) override ;
};



#endif 

