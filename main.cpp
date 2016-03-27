#include <bits/unique_ptr.h>

#include "main.hpp"

using namespace std;

void printHelp() {
    
}

ChessBoard::ChessBoard() : player(WHITE), gameExit_(false) {
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
    setField(2, 2, make_unique<King>(ChessBoard::WHITE, Coordinate(array<int, 2>{2,2}))) ;
}

Coordinate & ChessBoard::getKingLocation(ChessBoard::fraction fraction) const  {
    
}

ChessBoard::fraction ChessBoard::getOppositeFraction(ChessBoard::fraction & frac) const  {
    if (frac == BLACK) 
        return WHITE ;
    else if (frac == WHITE)
        return BLACK ;
}

void ChessBoard::setCheck(ChessBoard::fraction & fraction) {
    if (fraction == BLACK) {
        checkBlack = true ;
    }
    else if (fraction == WHITE) {
        checkWhite = true ;
    }
}

void ChessBoard::turnInformation() const {
    if (player == BLACK) {
        std::cout << "Black is on turn." ;
        if (checkBlack) {
            std::cout << "Black is in check." ;
        }
    }
    else if (player == WHITE) {
        std::cout << "White is on turn." ;
        if (checkWhite) {
            std::cout << "White is in check." ;
        }
    }
    std::cout << std::endl ;
}

void ChessBoard::gameCycle() {
    while (!gameExit_) {
        gameTurn() ;
    }
}

void ChessBoard::gameTurn() {
    print() ;
    
    //check state
    if (isKingEndangered(player, getKingLocation(player))) {
        setCheck(player) ;
    }
    turnInformation() ; 
    
    //prompt for command
    while (true) {
        std::array< std::unique_ptr< Coordinate>, 2> arr ; //arr[0] = figure, arr[1] = desiredLocation
        string command;
        std::cout << "input: ";
        std::cin >> command;
        if (command == "move") {
            string figure, location;
            std::cin >> figure;
            std::cin >> location;
            if (checkMove(arr, figure, location)) { //valid move from input
                //make the move
                Coordinate & currentLocation = *arr[0];
                Coordinate & desiredLocation = *arr[1];
                Field * field = viewField(arr[0]->getX(), arr[0]->getY());
                field->changeLocation(desiredLocation); //change the location of the figure --> is this necessary?
                setField(desiredLocation.getX(), desiredLocation.getY(), getField(currentLocation.getX(), currentLocation.getY())); //make the move
                nextPlayer() ;
                break ;
            }
            else { //invalid move from input
                continue;
            }
        } 
        else if (command == "surrend") {
            gameExit_ = true ;
            break ;
        }
        else if (command == "help") {
            printHelp() ;
        }
        else {
            std::cout << "Error: unknown command. Type help for help." << endl ;
        }
    }
}

//rovnou vypise chybu
//vrati true = arr je nastaven spravne
bool ChessBoard::checkMove(std::array< std::unique_ptr< Coordinate>, 2> & arr, const string & figure, const string & location) {
    std::unique_ptr<Coordinate> currentLocation = std::make_unique<Coordinate>(figure) ;
    std::unique_ptr<Coordinate> desiredLocation = std::make_unique<Coordinate>(location) ;
    
    //check bounds (that is everything, that inputMove checks)
    if (currentLocation->outOfBounds() || desiredLocation->outOfBounds()) {
        std::cout << "Error: coordination out of board" << std::endl ;
        return false ;
    }
    
    Field * figurePtr = viewField(currentLocation->getX(), currentLocation->getY()); //view figure
    
    //check if there is a figure on currentLocation, e.g. if currentLocation is not Free
    Free * freePtr = dynamic_cast< Free *>( figurePtr) ;
    if (freePtr != nullptr) {
        std::cout << "Error: there is no figure on that location" << std::endl;
        return false ;
    }
    
    //check whether the figure has the right color
    if (figurePtr->getFraction() != player) {
        std::cout << "Error: wrong color of selected figure" << std::endl;
        return false;
    }

    //check for same coordinates
    if (*currentLocation == *desiredLocation) {
        std::cout << "Error: same coordinates in input" << std::endl ;
        return false ;
    }
    
    //check whether the move can be done by this kind of figure
    if (!figurePtr->canMove(*desiredLocation)) {
        std::cout << "Error: desired move cannot be done by this kind of figure" << std::endl;
        return false;
    }

    //check whether king is endangered
    //Coordinate kingLocation = getKingLocation(player);
    if (isKingEndangered(player, getKingLocation(player))) {
        std::cout << "Error: move cannot be done - king is endangered now" << std::endl;
        return false;
    }
    
    //set return value through parameter
    arr[0] = std::move(currentLocation) ;
    arr[1] = std::move(desiredLocation) ;
    
    return true;
}

bool ChessBoard::isKingEndangered(fraction frac, Coordinate & kingLocation) const {
    fraction opponent = getOppositeFraction(frac) ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            if (board[Coordinate::translate(array<int, 2>{i,j})]->getFraction() == opponent &&  //ok
                board[Coordinate::translate(array<int, 2>{i,j})]->canMove(kingLocation)) { //ok
                    return true ;
            }
        }
    }
    return false ;
}

std::array< unique_ptr< Coordinate>, 2> ChessBoard::inputMove() const {
    bool correctInput = false ;
    std::array< std::unique_ptr< Coordinate>, 2> arr ;
    
    while (!correctInput) {
        std::cout << "input move: ";
        //input = "1a 4b" or "a1 b4" or "1a b4"
        string figure, location;
        std::cin >> figure;
        std::cin >> location;

        arr[0] = std::make_unique< Coordinate>(figure); //meni se to vubec tady?
        arr[1] = std::make_unique< Coordinate>(location);
        
        //bound check
        if (arr[0]->outOfBounds() || arr[1]->outOfBounds()) {
            std::cout << "Error: input field out of bounds. Try it again" << std::endl ;
        }
        else 
            correctInput = true ;
    }
    return std::move( arr) ;
}

void ChessBoard::setField(int n, int m, unique_ptr<Field> field) {
    board[Coordinate::translate(array<int, 2>{n,m})] = std::move( field) ;
}

Field * ChessBoard::viewField(int n, int m) const  {
    return board[Coordinate::translate(array<int, 2>{n,m})].get() ;
}

std::unique_ptr< Field> ChessBoard::getField(int n, int m)  {
    std::unique_ptr< Field> ptr = std::move(board[Coordinate::translate(array<int, 2>{n,m})]) ;
    board[Coordinate::translate(array<int, 2>{n,m})] = make_unique< Free>() ;
    return std::move( ptr) ;
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

bool King::canMove(Coordinate & coordinate) {
    int dX = abs(location_.getX() - coordinate.getX()) ;
    int dY = abs(location_.getY() - coordinate.getY()) ;
    if (dX > 1 || dY > 1) return false ;
    else return true ;
}

void King::changeLocation(const Coordinate & coordinate) {
    location_ = coordinate ;
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

bool Free::canMove(Coordinate & coordinate) { }
void Free::changeLocation(const Coordinate & coordinate) { }


void ChessBoard::test1() {
    setBoard() ; //[0,0] = unique_ptr< King>
    
    Field * field = viewField(0, 0) ;
    King * kingPtr = dynamic_cast< King *>( field) ;
    assert (kingPtr != nullptr && "setBoard failed -- [0,0] is not King") ;
    
    //unique_ptr< Field> uniqPtr = getField(0,0) ;
    //setField(1, 1, std::move( uniqPtr)) ;
    setField(1, 1, getField(0,0)) ;
    setField(0, 0, make_unique< Free>()) ; //asi zbytecny
    
    //[0,0] = free, [1,1] = king
    
    field = viewField(1, 1) ;
    kingPtr = dynamic_cast< King *>( field) ;
    assert (kingPtr != nullptr && "board[1,1] is not King") ;
    
    field = viewField(0, 0) ;
    Free * freePtr = dynamic_cast< Free *>( field) ;
    assert (freePtr != nullptr && "board[0,0] is not Free") ; 
} 

void ChessBoard::test2() {
    setBoard() ;
    gameCycle() ;
}

int main(int argc, char ** argv) {
    unique_ptr<ChessBoard> chessBoard = make_unique<ChessBoard>() ;
    chessBoard->test2() ;
    
    cout << endl ;
}