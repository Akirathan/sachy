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

class ChessBoard ;

int size_N ;
int size_M ;
std::unique_ptr<ChessBoard> chessBoard ;

/**
 * Vypise seznam prikazu.
 */
void printHelp() ;

/**
 * Naparsuje radku podle ' '
 * Dulezite, kdyz cteme sachovnici ze souboru.
 * @param line
 * @return 
 */
std::vector<std::string> parseLine(const std::string & line) ;

class Field ;

class Coordinate {
private:
    int coordinate_ ; //souradnice pro board
    int X_, Y_ ;
    bool outOfBounds_ ;
    std::string stringContent_ ;
    
public:
    
    Coordinate() {}
    
    Coordinate(const std::string & s) {
        stringContent_ = s ; 
        coordinate_ = translateWithBoundCheck(s) ;
        std::array<int, 2> arr = reverseTranslate(coordinate_) ;
        X_ = arr[0] ;
        Y_ = arr[1] ;
    }
    
    /**
     * Tenhle konstruktor potrebujeme jen pro volani metod getKingLocation.
     * Bound check tu je pro pripad, kdy chceme hram bez krale/kralu.
     * @param index
     */
    Coordinate(int index) {
        //bound check
        if (index >= size_N + size_M) outOfBounds_ = true ;
        coordinate_ = index ;
        std::array<int, 2> arr = reverseTranslate(coordinate_) ;
        X_ = arr[0] ;
        Y_ = arr[1] ;
    }
    
    Coordinate(std::array<int, 2> arr) {
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
    int translateWithBoundCheck(const std::string & s) {
        std::string a = "abcdefgh" ;
        int charPos = 0, numPos = 0 ;
        if (isdigit(s[0])) { //"1a"
            charPos = 1 ;
        }
        else { //"a1"
            numPos = 1 ;
        }
        int m = a.find_first_of(s[charPos]) ;
        int n = size_N - (int)s[numPos] + 48 ;
        return translateWithBoundCheck(std::array<int, 2> {n,m}) ;
    }
    
    int translateWithBoundCheck(std::array<int, 2> arr) {
        if (arr[0] >= size_N || arr[0] < 0 ||
                arr[1] >= size_M || arr[1] < 0)
            outOfBounds_ = true ;
        else outOfBounds_ = false ;
        return arr[0] * size_M + arr[1];
    }
    
    static int translate(std::array<int, 2> arr) {
        return arr[0] * size_M + arr[1];
    }
    
    static std::array<int, 2> reverseTranslate(int s) {
        std::array<int, 2> res;
        res[0] = s / size_M;
        res[1] = s % size_M;
        return move(res);
    }
};


class ChessBoard {
public:
    enum fraction {WHITE, BLACK} ;
    
private:
    std::vector< std::unique_ptr< Field>> board_ ;
    fraction player_ ;
    bool checkWhite_, checkBlack_ ; 
    bool gameExit_ ;
    
    void nextPlayer() ;
    fraction getOppositeFraction(fraction & frac) const ;
    
    /**
     * Nastavi sach pro danou frakci.
     */
    void setCheck(fraction & frac) ;
    
    /**
     * Odstrani sach obema frakcim.
     */
    void removeCheck() ;
    
    /**
     * Zkontroluje jestli kral dane frakce na pozici kingLocation ma sach.
     * Implementovano tak, ze se prohleda cela sachovnice a 
     * na kazde nepratelske figurce se zavola metoda canMove
     * @param frac frakce krale
     * @return true v pripade, ze nastal sach
     */
    bool isKingEndangered(fraction frac, Coordinate kingLocation) const ;
    
    /**
     * Vytiskne informaci o tom, kdo je na tahu a kdo ma sach.
     */
    void turnInformation() const ;
    
    /**
     * Inicializuje board_ ze souboru
     */
    void loadFromFile(const std::string & filename) ;
    
public:
    
    ChessBoard(const std::string & filename) ;
    
    /**
     * Obsah field je vykraden
     * @param field Temporalni objekt vytvoreny napriklad podle make_unique<Field>
     */
    void setField(int n, int m, std::unique_ptr<Field> field) ;
    
    /**
     * Vraci pointer pouze k nahlizeni.
     */
    Field * viewField(int n, int m) const ;
    
    /**
     * Vykrade board_[n,m] a pak na stejnem miste vytvori Free
     * @return vykradeny unique_ptr
     */
    std::unique_ptr<Field> getField(int n, int m) ;
    void printBorder() ;
    void print() ;
    Coordinate getKingLocation(fraction) const ;
    void gameTurn() ;
    void gameCycle() ;
    
    /**
     * Kontroluje spravnost daneho pohybu z figure --> location.
     * Jestlize tah je platny, ulozi vysledek do arr.
     */
    bool checkMove(std::array< std::unique_ptr< Coordinate>, 2> & arr, const std::string & figure, const std::string & location) ;
    void makeMove(Coordinate & currentLocation, Coordinate & desiredLocation) ;
    void undoMove() ;
    
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
         * Projde cely board_ a tim inicializuje Database
         */
        void scan() ;
        
        void setKingLocation(const fraction & frac, int location) ;
        int getKingLocation(const fraction & frac) ;
        
        /**
         * Vlozi prvek do moves_ a aktualizuje actualTable_
         */
        void recordMove(Coordinate & figure, Coordinate & newLocation) ;
        
        /**
         * vynda jeden prvek z moves_ a vrati zmeny.
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
    virtual ChessBoard::fraction getFraction() const =0 ;
    virtual std::string print() const =0 ; //figurky se vytisknou na zaklade sve frakce
    
    /**
     * Podle aktualnich souradnic dane figurky (tj. location_) rozhodne,
     * jestli se muze pohnout na coordinate
     * @param coordinate souradnice, kam se figurka chce pohnout
     * @return true = muze se tam pohnout
     */
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) =0 ;
};

//=======================================================
//                      KING
//=======================================================
class King : public Field {
private:
    ChessBoard::fraction fraction_ ;
    
public:
    King(ChessBoard::fraction fraction) ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

//=======================================================
//                      KNIGHT
//=======================================================
class Knight : public Field {
private:
    ChessBoard::fraction fraction_ ;
    
public:
    Knight(ChessBoard::fraction) ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

//=======================================================
//                      PAWN
//=======================================================
class Pawn : public Field {
private:
    ChessBoard::fraction fraction_ ;
    
public:
    Pawn(ChessBoard::fraction) ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

//=======================================================
//                      BISHOP
//=======================================================
class Bishop : public Field {
private:
    ChessBoard::fraction fraction_ ;
    
public:
    Bishop(ChessBoard::fraction) ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

//=======================================================
//                      QUEEN
//=======================================================
class Queen : public Field {
private:
    ChessBoard::fraction fraction_ ;
    
public:
    Queen(ChessBoard::fraction) ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

//=======================================================
//                      ROOK
//=======================================================
class Rook : public Field {
private:
    ChessBoard::fraction fraction_ ;
    
public:
    Rook(ChessBoard::fraction) ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};

//=======================================================
//                      FREE
//=======================================================
class Free : public Field {
public:
    Free() ;
    virtual ChessBoard::fraction getFraction() const override ;
    virtual std::string print() const override ;
    virtual bool canMove(Coordinate & thisLocation, Coordinate & coordinate) override ;
};



#endif 

