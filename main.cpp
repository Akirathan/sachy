#include <bits/unique_ptr.h>

#include "main.hpp"

using namespace std;

void printHelp() {
    
}

std::vector<std::string> parseLine(const string & line) {
    std::vector<std::string> vec ;
    std::stringstream ss(line) ;
    std::string item ;
    while(getline(ss, item, ' ')) {
        vec.push_back(item) ;
    }
    return std::move(vec) ;
}

std::ostream & operator<<(std::ostream & out, const Coordinate & coordinate) {
    out << coordinate.stringContent_ ;
}

//=======================================================
//                  CHESSBOARD
//=======================================================

void ChessBoard::loadFromFile(const string & filename) {
    std::ifstream input ;
    std::vector<std::string> vecRows ;
    input.open(filename) ;
    string line, strN, strM ;
    
    //read dimensions of board
    getline(input, strN) ;
    getline(input, strM) ;
    size_N = stoi(strN) ;
    size_M = stoi(strM) ;
    //---
    while (getline(input, line)) { 
        vecRows.push_back(line) ;
    }
    
    for (auto itRow = vecRows.begin(); itRow != vecRows.end(); itRow++) { //pres kazdou radku
        std::vector<std::string> items = parseLine(*itRow);
        for (auto itItem = items.begin(); itItem != items.end(); itItem++) { //pres kazdou polozku radky
            if (*itItem == "bK") {
                board.push_back(make_unique<King>(BLACK));
            } else if (*itItem == "wK") {
                board.push_back(make_unique<King>(WHITE));
            } else if (*itItem == "bN") {
                board.push_back(make_unique<Knight>(BLACK));
            } else if (*itItem == "wN") {
                board.push_back(make_unique<Knight>(WHITE));
            } else if (*itItem == "--") {
                board.push_back(make_unique<Free>());
            }
        }
    }
}

ChessBoard::ChessBoard(const string & filename) : player(WHITE), gameExit_(false) {
    loadFromFile(filename) ;
    database_ = make_unique<Database>(*this) ;
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
            cout << board[Coordinate::translate(array<int, 2>{i,j})]->print() ; //ok
            cout << "|" ;
        }
        cout << "  " << size_M - i << endl ; //cislo radku na konci
    }
    cout << endl ;
    printBorder() ;
    //board[translate(array<int, 2>{i,j})]->print() ; //ok
}

Coordinate ChessBoard::getKingLocation(ChessBoard::fraction fraction) const  {
    int index = database_->getKingLocation(fraction) ;
    Coordinate coordinate(index) ; //int --> Coordinate
    return std::move(coordinate) ;
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
                makeMove(currentLocation, desiredLocation) ;
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
            std::cout << "Error: unknown command. Type \"help\" for help." << endl ;
        }
    }
}

//rovnou vypise chybu
//kdyz vrati true, tak arr je nastaven spravne
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
    if (!figurePtr->canMove(*currentLocation, *desiredLocation)) {
        std::cout << "Error: desired move cannot be done by this kind of figure" << std::endl;
        return false;
    }

    //check whether king is endangered
    //make the move first, and then undo it
    makeMove(*currentLocation, *desiredLocation) ;
    if (isKingEndangered(player, getKingLocation(player))) {
        std::cout << "Error: move cannot be done - king is endangered now" << std::endl;
        undoMove() ;
        return false;
    }
    undoMove() ;
    
    /*
    If every test is passed, then
    set return value through parameter.
    */
    arr[0] = std::move(currentLocation) ;
    arr[1] = std::move(desiredLocation) ;
    
    return true;
}

void ChessBoard::makeMove(Coordinate & currentLocation, Coordinate & desiredLocation) {
    database_->recordMove(currentLocation, desiredLocation) ;
    setField(desiredLocation.getX(), desiredLocation.getY(), getField(currentLocation.getX(), currentLocation.getY())); //make the move
    
}

void ChessBoard::undoMove() {
    //revertMove.second = desiredLocation (tj. odkud figurka puvodne prisla)
    //reverMove.first = currentLocation (tj. kam figurka puvodne dosla)
    
    std::pair<Coordinate, Coordinate> revertMove = database_->undoMove() ;
    setField(revertMove.second.getX(), revertMove.second.getY(), getField(revertMove.first.getX(), revertMove.first.getY())) ;
}

bool ChessBoard::isKingEndangered(fraction frac, Coordinate kingLocation) const {
    fraction opponent = getOppositeFraction(frac) ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            Coordinate thisLocation(array<int,2>{i,j}) ;
            if (board[Coordinate::translate(array<int, 2>{i,j})]->getFraction() == opponent &&  //ok
                board[Coordinate::translate(array<int, 2>{i,j})]->canMove(thisLocation, kingLocation)) { //ok
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

//=======================================================
//                  DATABASE
//=======================================================
ChessBoard::Database::Database(ChessBoard & chessboard) : chessBoard_(chessboard)
{
    scan() ;
}

void ChessBoard::Database::scan() {
    int locationCounter = 0 ;
    for(auto it = chessBoard_.board.begin(); it != chessBoard_.board.end(); it++) {
        //*it = unique_ptr<Field>
        Field * fieldPtr = it->get() ;
        
        King * kingPtr = dynamic_cast<King *>(fieldPtr) ;
        Knight * knightPtr = dynamic_cast<Knight *>(fieldPtr) ;
        Free * freePtr = dynamic_cast<Free *>(fieldPtr) ; //tady ten dynamic_cast je nejspis navic
        //TODO doplnit ostatni figurky
        
        if (kingPtr != nullptr) {
            if (kingPtr->getFraction() == WHITE) {
                initialTable_.push_back(white_king) ;
                setKingLocation(WHITE, locationCounter) ; 
            }
            else if (kingPtr->getFraction() == BLACK) {
                initialTable_.push_back(black_king) ;
                setKingLocation(BLACK, locationCounter) ;
            }
        }
        else if (knightPtr != nullptr) {
            if (knightPtr->getFraction() == WHITE) initialTable_.push_back(white_knight) ;
            else if (knightPtr->getFraction() == BLACK) initialTable_.push_back(black_knight) ;
        }
        else if (freePtr != nullptr) {
            initialTable_.push_back(free) ;
        }
        //TODO doplnit ostatni podminky
        
        locationCounter ++ ;
    }
    actualTable_ = initialTable_ ;
}

void ChessBoard::Database::setKingLocation(const fraction & frac, int location) {
    if (frac == WHITE) {
        whiteKingLocation_ = location ;
    }
    else if (frac == BLACK) {
        blackKingLocation_ = location ;
    }
}

int ChessBoard::Database::getKingLocation(const fraction & frac) {
    if (frac == WHITE) {
        return whiteKingLocation_ ;
    }
    else if (frac == BLACK) {
        return blackKingLocation_ ;
    }
}

void ChessBoard::Database::recordMove(Coordinate & figure, Coordinate & newLocation) {
    move_t move ;
    Field * fieldPtr = chessBoard_.viewField(figure.getX(), figure.getY()) ;
    move.nameOfFigure = fieldPtr->print() ;
    move.codeOfFigure = actualTable_[figure.getCoordinate()] ;
    move.coord = std::make_pair(figure, newLocation) ; //copy to move.coord ?
    
    //rovnou zmenime actualTable_
    actualTable_[newLocation.getCoordinate()] = actualTable_[figure.getCoordinate()] ;
    actualTable_[figure.getCoordinate()] = free ;
    
    //check if figure that was moved right now is king
    int king = (chessBoard_.player == WHITE) ? white_king : black_king ;
    if (actualTable_[newLocation.getCoordinate()] == king) {
        setKingLocation(chessBoard_.player, newLocation.getCoordinate()) ;
    }
    
    moves_.push_back(std::move(move)) ;
}

std::pair<Coordinate, Coordinate> ChessBoard::Database::undoMove() {
    //move.coord.first = from
    //move.coord.second = to
    move_t move = moves_.back() ;
    moves_.pop_back() ;
    
    //check if it is king
    if (actualTable_[move.coord.second.getCoordinate()] == white_king ||
            actualTable_[move.coord.second.getCoordinate()] == black_king) {
        setKingLocation(chessBoard_.player, move.coord.first.getCoordinate()) ;
    }
    
    //revert changes in actualTable_
    actualTable_[move.coord.first.getCoordinate()] = actualTable_[move.coord.second.getCoordinate()] ;
    actualTable_[move.coord.second.getCoordinate()] = free ;
    return std::move(std::make_pair(move.coord.second, move.coord.first)) ;
}

//=======================================================
//                      KING
//=======================================================

King::King(ChessBoard::fraction fraction) : type_(ChessBoard::KING), 
    fraction_(fraction) {
}

ChessBoard::fieldType King::getType() const {
    return type_;
}

ChessBoard::fraction King::getFraction() const {
    return fraction_ ;
}
    
string King::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wK" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bK" ;
    }
}

bool King::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    int dX = abs(thisLocation.getX() - coordinate.getX()) ;
    int dY = abs(thisLocation.getY() - coordinate.getY()) ;
    if (dX > 1 || dY > 1) return false ;
    else return true ;
}

//=======================================================
//                    KNIGHT
//=======================================================

Knight::Knight(ChessBoard::fraction fraction) : type_(ChessBoard::KNIGHT),
        fraction_(fraction) {
}

ChessBoard::fieldType Knight::getType() const {
    return type_;
}

ChessBoard::fraction Knight::getFraction() const {
    return fraction_ ;
}

string Knight::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wN" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bN" ;
    }
}

bool Knight::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    
}


//=======================================================
//                    FREE
//=======================================================

Free::Free() : type_(ChessBoard::FREE) {
}

ChessBoard::fieldType Free::getType() const {
    return type_;
}

ChessBoard::fraction Free::getFraction() const { }

string Free::print() const {
    return "__";
}

bool Free::canMove(Coordinate & thisLocation, Coordinate & coordinate) { } 


/*void ChessBoard::test1() {
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
} */

void ChessBoard::test2() {
    //kontrola loadFromFile OK
    Field * fieldPtr = viewField(2, 2) ;
    King * kingPtr = dynamic_cast<King *>(fieldPtr) ;
    assert(kingPtr != nullptr) ;
    //----
}

int main(int argc, char ** argv) {
    std::cout << "Input filename or \"standard.txt\" for standard chessboard: " ;
    string filename ;
    std::cin >> filename ;
    chessBoard = make_unique<ChessBoard>(filename) ;
    chessBoard->gameCycle() ;
    
    //chessBoard->test2() ;
}
