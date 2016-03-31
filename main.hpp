#ifndef MAIN_HPP_
#define MAIN_HPP

#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <cctype>
#include <cassert>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std ;

class ChessBoard ;

int size_N ;
int size_M ;
std::unique_ptr<ChessBoard> chessBoard ;

/**
 * Outputs keywords and their meaning.
 */
void printHelp() ;

/**
 * Naparsuje radku podle ' '.
 * Dulezite, kdyz cteme sachovnici ze souboru.
 * @param line
 * @return 
 */
std::vector<std::string> parseLine(const string & line) ;

class Field ;

class Coordinate {
private:
    int coordinate_ ; //souradnice pro board
    int X_, Y_ ;
    bool outOfBounds_ ;
    std::string stringContent_ ;
    
public:
    
    Coordinate() {}
    
    Coordinate(const string & s) {
        stringContent_ = s ; //copy
        coordinate_ = translateWithBoundCheck(s) ;
        array<int, 2> arr = reverseTranslate(coordinate_) ;
        X_ = arr[0] ;
        Y_ = arr[1] ;
    }
    
    /**
     * Tenhle konstruktor potrebujeme jen pro volani metod getKingLocation.
     * Bound check tu ani nemusi byt.
     * @param index
     */
    Coordinate(int index) {
        //bound check
        if (index >= size_N + size_M) outOfBounds_ = true ; //tady by ten bound check ani nemusel byt
        coordinate_ = index ;
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
    
    friend std::ostream & operator<<(std::ostream & out, const Coordinate & coordinate) ;
    
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
    std::vector< std::unique_ptr< Field>> board ;
    fraction player ;
    bool checkWhite, checkBlack ; 
    bool gameExit_ ;
    
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
    bool isKingEndangered(fraction frac, Coordinate kingLocation) const ;
    
    /**
     * Prompts the player for input as long, as the input is invalid.
     * Does not care about who is actually on turn.
     * @return array[0] = the figure, array[1] = desired coordinates.
     */
    std::array<unique_ptr<Coordinate>, 2>  inputMove() const ;
    
    /**
     * Prints out the information abou who is on turn
     * and who is in check.
     * Can be extended for additional information.
     */
    void turnInformation() const ;
    
    /**
     * This method initializes board.
     * @param filename
     */
    void loadFromFile(const string & filename) ;
    
public:
    
    /**
     * Constructor needs file to specify.
     * @param filename
     */
    ChessBoard(const string & filename) ;
    
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
    Coordinate getKingLocation(fraction) const ;
    
    /**
     * Zjistovani, jestli je kral v ohrozeni probiha tak,
     * ze se projdeme vsechny nepratelske figurky a vyzkousime,
     * jestli dosahnou na krale.
     */
    void gameTurn() ;
    
    void gameCycle() ;
    
    /**
     * Calls inputMove and checks whether the move is valid.
     * When true is returned, the value of the parameter is set to correct move.
     * @param arr
     * @return 
     */
    bool checkMove(std::array< std::unique_ptr< Coordinate>, 2> & arr, const string & figure, const string & location) ;
    
    void makeMove(Coordinate & currentLocation, Coordinate & desiredLocation) ;
    
    void undoMove() ;
    
    void test1() ;
    void test2() ;
    
private:
    class Database {
    private:
        ChessBoard & chessBoard_ ;
        
        const int black_king = 1 ;
        const int black_queen = 2 ;
        const int black_bishop = 3 ;
        const int black_rook = 4 ;
        const int black_knight = 5 ;
        const int black_pawn = 6 ;
        
        const int white_king = -1 ;
        const int white_queen = -2 ;
        const int white_bishop = -3 ;
        const int white_rook = -4 ;
        const int white_knight = -5 ;
        const int white_pawn = -6 ;
        
        const int free = 20 ;
        
        std::vector<int> initialTable_ ;
        
        /**
         * Hodi se pro pripady pocet zijicich / vyhozenych figurek.
         * Na vyhozene figurky potrebujeme znat jeste celkovy pocet figurek na zacatku,
         * nebo porovname s initialTable_
         */
        std::vector<int> actualTable_ ;
        int whiteKingLocation_ ;
        int blackKingLocation_ ;
        
        struct move_t {
            std::pair<Coordinate, Coordinate> coord ;
            std::string nameOfFigure ;
            int codeOfFigure ;
        };
        
        std::vector<move_t> moves_ ;
        
    public:
        /**
         * 
         * @param chessboard reference na vnejsi tridu
         */
        Database(ChessBoard & chessboard) ;
        
        /**
         * Scans the board and initiates the initialTable
         */
        void scan() ;
        
        void setKingLocation(const fraction & frac, int location) ;
        int getKingLocation(const fraction & frac) ;
        
        void recordMove(Coordinate & figure, Coordinate & newLocation) ;
        
        /**
         * Pops one element from moves_ and revert changes
         * on actualTable_
         */
        std::pair<Coordinate, Coordinate> undoMove() ;
    };
    
    std::unique_ptr<Database> database_ ;
};


/**
 * Abstraktni trida reprezentujici jedno policko.
 */
class Field {
public:
    virtual ChessBoard::fieldType getType() const =0 ;
    virtual ChessBoard::fraction getFraction() const =0 ;
    virtual string print() const =0 ; //figurky se vytisknou na zaklade sve frakce
    
    /**
     * Podle aktualnich souradnic dane figurky (tj. location_) rozhodne,
     * jestli se muze pohnout na coordinate
     * @param coordinate souradnice, kam se figurka chce pohnout
     * @return true = muze se tam pohnout
     */
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) =0 ;
};

class King : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    
public:
    King(ChessBoard::fraction fraction) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};


class Knight : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    
public:
    Knight(ChessBoard::fraction) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

class Pawn : public Field {
private:
    ChessBoard::fraction fraction_ ;
    ChessBoard::fieldType type_ ;
    
public:
    Pawn(ChessBoard::fraction) ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual string print() const override ;
};


class Free : public Field {
private:
    ChessBoard::fieldType type_ ;
    
public:
    Free() ;
    virtual ChessBoard::fieldType getType() const override ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};



#endif 

