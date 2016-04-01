//#include <bits/unique_ptr.h>

#include "main.hpp"

void printHelp() {
    
}

std::vector<std::string> parseLine(const std::string & line) {
    std::vector<std::string> vec ;
    std::stringstream ss(line) ;
    std::string item ;
    while(getline(ss, item, ' ')) {
        vec.push_back(item) ;
    }
    return std::move(vec) ;
}

std::ostream & operator<<(std::ostream & out, const Coordinate & coordinate) {
    return out << coordinate.stringContent_ ;
}

//=======================================================
//                  CHESSBOARD
//=======================================================

void ChessBoard::loadFromFile(const std::string & filename) {
    std::ifstream input ;
    std::vector<std::string> vecRows ;
    input.open(filename) ;
    std::string line, strN, strM ;
    
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
                board_.push_back(std::make_unique<King>(BLACK));
            } else if (*itItem == "wK") {
                board_.push_back(std::make_unique<King>(WHITE));
            } else if (*itItem == "bN") {
                board_.push_back(std::make_unique<Knight>(BLACK));
            } else if (*itItem == "wN") {
                board_.push_back(std::make_unique<Knight>(WHITE));
            } else if (*itItem == "bP") {
                board_.push_back(std::make_unique<Pawn>(BLACK));
            } else if (*itItem == "wP") {
                board_.push_back(std::make_unique<Pawn>(WHITE));
            } else if (*itItem == "bB") {
                board_.push_back(std::make_unique<Bishop>(BLACK));
            } else if (*itItem == "wB") {
                board_.push_back(std::make_unique<Bishop>(WHITE));
            } else if (*itItem == "bQ") {
                board_.push_back(std::make_unique<Queen>(BLACK));
            } else if (*itItem == "wQ") {
                board_.push_back(std::make_unique<Queen>(WHITE));
            } else if (*itItem == "bR") {
                board_.push_back(std::make_unique<Rook>(BLACK));
            } else if (*itItem == "wR") {
                board_.push_back(std::make_unique<Rook>(WHITE));
            } else if (*itItem == "--") {
                board_.push_back(std::make_unique<Free>());
            }
        }
    }
    std::cout << std::endl ; //DEBUG
}

ChessBoard::ChessBoard(const std::string & filename) : player_(WHITE), gameExit_(false) {
    loadFromFile(filename) ;
    database_ = std::make_unique<Database>(*this) ;
}

void ChessBoard::printBorder() {
    std::array<char, 8> charArray {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'} ;
    for (int i = 0; i < size_M; i++) {
        if (i == 0)
            std::cout << "    "; //4 mezery
        std::cout << charArray[i] << "  " ;
    }
    std::cout << std::endl << std::endl ;
}

void ChessBoard::print() {
    printBorder() ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            if (j == 0)
                std::cout << size_M - i << "  |" ; //cislo radku na zacatek
            std::cout << board_[Coordinate::translate(std::array<int, 2>{i,j})]->print() ;
            std::cout << "|" ;
        }
        std::cout << "  " << size_M - i << std::endl ; //cislo radku na konci
    }
    std::cout << std::endl ;
    printBorder() ;
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
        checkBlack_ = true ;
    }
    else if (fraction == WHITE) {
        checkWhite_ = true ;
    }
}

void ChessBoard::removeCheck() {
    checkBlack_ = false ;
    checkWhite_ = false ;
}

void ChessBoard::turnInformation() const {
    if (player_ == BLACK) {
        std::cout << "Black is on turn." ;
        if (checkBlack_) {
            std::cout << "Black is in check." ;
        }
    }
    else if (player_ == WHITE) {
        std::cout << "White is on turn." ;
        if (checkWhite_) {
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
    if (isKingEndangered(player_, getKingLocation(player_))) {
        setCheck(player_) ;
    }
    turnInformation() ; 
    
    //prompt for command
    while (true) {
        std::array< std::unique_ptr< Coordinate>, 2> arr ; //arr[0] = figure, arr[1] = desiredLocation
        std::string command;
        std::cout << "input: ";
        std::cin >> command;
        if (command == "move") {
            std::string figure, location;
            std::cin >> figure;
            std::cin >> location;
            if (checkMove(arr, figure, location)) { 
                //make the move
                Coordinate & currentLocation = *arr[0];
                Coordinate & desiredLocation = *arr[1];
                makeMove(currentLocation, desiredLocation) ;
                removeCheck() ;
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
            std::cout << "Error: unknown command. Type \"help\" for help." << std::endl ;
        }
    }
}

bool ChessBoard::checkMove(std::array< std::unique_ptr< Coordinate>, 2> & arr, const std::string & figure, const std::string & location) {
    std::unique_ptr<Coordinate> currentLocation = std::make_unique<Coordinate>(figure) ;
    std::unique_ptr<Coordinate> desiredLocation = std::make_unique<Coordinate>(location) ;
    
    //bound check
    if (currentLocation->outOfBounds() || desiredLocation->outOfBounds()) {
        std::cout << "Error: coordination out of board" << std::endl ;
        return false ;
    }
    
    Field * figurePtr = viewField(currentLocation->getX(), currentLocation->getY());
    Field * locationPtr = viewField(desiredLocation->getX(), desiredLocation->getY()) ;
    
    //check if there is a figure on currentLocation, e.g. if currentLocation is not Free
    Free * freePtr = dynamic_cast< Free *>( figurePtr) ;
    if (freePtr != nullptr) {
        std::cout << "Error: there is no figure on that location" << std::endl;
        return false ;
    }
    
    //check whether the figure has the right color
    if (figurePtr->getFraction() != player_) {
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
    
    //zkontroluj, jestli nechceme vyhodit vlastni figurku
    if (locationPtr->getFraction() == player_) {
        std::cout << "Error: cannot throw out own figure" << std::endl ;
    }

    //check whether king is endangered
    //make the move first, and then undo it
    makeMove(*currentLocation, *desiredLocation) ;
    if (isKingEndangered(player_, getKingLocation(player_))) {
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
    
    //hra bez kralu
    if (kingLocation.outOfBounds()) { 
        return false ;
    }
    
    fraction opponent = getOppositeFraction(frac) ;
    for (int i = 0; i < size_N; i++) {
        for (int j = 0; j < size_M; j++) {
            Coordinate thisLocation(std::array<int,2>{i,j}) ;
            if (board_[Coordinate::translate(std::array<int, 2>{i,j})]->getFraction() == opponent && 
                board_[Coordinate::translate(std::array<int, 2>{i,j})]->canMove(thisLocation, kingLocation)) { 
                    return true ;
            }
        }
    }
    return false ;
}

void ChessBoard::setField(int n, int m, std::unique_ptr<Field> field) {
    board_[Coordinate::translate(std::array<int, 2>{n,m})] = std::move(field) ;
}

Field * ChessBoard::viewField(int n, int m) const  {
    return board_[Coordinate::translate(std::array<int, 2>{n,m})].get() ;
}

std::unique_ptr<Field> ChessBoard::getField(int n, int m)  {
    std::unique_ptr<Field> ptr = std::move(board_[Coordinate::translate(std::array<int, 2>{n,m})]) ;
    board_[Coordinate::translate(std::array<int, 2>{n,m})] = std::make_unique<Free>() ;
    return std::move(ptr) ;
}

/**
 * Posune na tah dalsiho hrace
 */
void ChessBoard::nextPlayer() {
    if (player_ == BLACK) {
        player_ = WHITE ;
    }
    else if (player_ == WHITE) {
        player_ = BLACK ;
    }
}

//=======================================================
//                  DATABASE
//=======================================================
ChessBoard::Database::Database(ChessBoard & chessboard) : chessBoard_(chessboard),
        whiteKingLocation_(10000), blackKingLocation_(10000)
{
    scan() ;
}

void ChessBoard::Database::scan() {
    int locationCounter = 0 ;
    for(auto it = chessBoard_.board_.begin(); it != chessBoard_.board_.end(); it++) {
        //*it = unique_ptr<Field>
        Field * fieldPtr = it->get() ;
        
        King * kingPtr = dynamic_cast<King *>(fieldPtr) ;
        Knight * knightPtr = dynamic_cast<Knight *>(fieldPtr) ;
        Pawn * pawnPtr = dynamic_cast<Pawn *>(fieldPtr) ;
        Bishop * bishopPtr = dynamic_cast<Bishop *>(fieldPtr) ;
        Queen * queenPtr = dynamic_cast<Queen *>(fieldPtr) ;
        Rook * rookPtr = dynamic_cast<Rook *>(fieldPtr) ;
        Free * freePtr = dynamic_cast<Free *>(fieldPtr) ;
        
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
        else if (pawnPtr != nullptr) {
            if (pawnPtr->getFraction() == WHITE) initialTable_.push_back(white_pawn) ;
            else if (pawnPtr->getFraction() == BLACK) initialTable_.push_back(black_pawn) ;
        }
        else if (bishopPtr != nullptr) {
            if (bishopPtr->getFraction() == WHITE) initialTable_.push_back(white_bishop) ;
            else if (bishopPtr->getFraction() == BLACK) initialTable_.push_back(black_bishop) ;
        }
        else if (queenPtr != nullptr) {
            if (queenPtr->getFraction() == WHITE) initialTable_.push_back(white_queen) ;
            else if (queenPtr->getFraction() == BLACK) initialTable_.push_back(black_queen) ;
        }
        else if (rookPtr != nullptr) {
            if (rookPtr->getFraction() == WHITE) initialTable_.push_back(white_rook) ;
            else if (rookPtr->getFraction() == BLACK) initialTable_.push_back(black_rook) ;
        }
        else if (freePtr != nullptr) {
            initialTable_.push_back(free) ;
        }
        
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
    move.coord = std::make_pair(figure, newLocation) ; 
    
    //rovnou zmenime actualTable_
    actualTable_[newLocation.getCoordinate()] = actualTable_[figure.getCoordinate()] ;
    actualTable_[figure.getCoordinate()] = free ;
    
    //zkontrolovat, jestli jsme prave netahli kralem
    int king = (chessBoard_.player_ == WHITE) ? white_king : black_king ;
    if (actualTable_[newLocation.getCoordinate()] == king) {
        setKingLocation(chessBoard_.player_, newLocation.getCoordinate()) ;
    }
    
    moves_.push_back(std::move(move)) ;
}

std::pair<Coordinate, Coordinate> ChessBoard::Database::undoMove() {
    //move.coord.first = odkud
    //move.coord.second = kam
    move_t move = moves_.back() ;
    moves_.pop_back() ;
    
    //zkontroluj, jestli to je kral
    if (actualTable_[move.coord.second.getCoordinate()] == white_king ||
            actualTable_[move.coord.second.getCoordinate()] == black_king) {
        setKingLocation(chessBoard_.player_, move.coord.first.getCoordinate()) ;
    }
    
    //revert changes in actualTable_
    actualTable_[move.coord.first.getCoordinate()] = actualTable_[move.coord.second.getCoordinate()] ;
    actualTable_[move.coord.second.getCoordinate()] = free ;
    return std::move(std::make_pair(move.coord.second, move.coord.first)) ;
}


//=======================================================
//                      KING
//=======================================================

King::King(ChessBoard::fraction fraction) : fraction_(fraction) {
}

ChessBoard::fraction King::getFraction() const {
    return fraction_ ;
}
    
std::string King::print() const {
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

Knight::Knight(ChessBoard::fraction fraction) : fraction_(fraction) {
}

ChessBoard::fraction Knight::getFraction() const {
    return fraction_ ;
}

std::string Knight::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wN" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bN" ;
    }
}

bool Knight::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    int dX = abs(thisLocation.getX() - coordinate.getX()) ;
    int dY = abs(thisLocation.getY() - coordinate.getY()) ;
    if ((dX == 2 && dY == 1) || (dX == 1 && dY == 2)) return true ;
    else return false ;
}


//=======================================================
//                    PAWN
//=======================================================

Pawn::Pawn(ChessBoard::fraction fraction) : fraction_(fraction) {
}

ChessBoard::fraction Pawn::getFraction() const {
    return fraction_ ;
}

std::string Pawn::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wP" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bP" ;
    }
}

bool Pawn::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    int dX = abs(thisLocation.getX() - coordinate.getX()) ;
    int dY = abs(thisLocation.getY() - coordinate.getY()) ;
    if (dX > 1 || dY > 1) return false ;
    else return true ;
}


//=======================================================
//                    BISHOP
//=======================================================

Bishop::Bishop(ChessBoard::fraction fraction) : fraction_(fraction) {
}

ChessBoard::fraction Bishop::getFraction() const {
    return fraction_ ;
}

std::string Bishop::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wB" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bB" ;
    }
}

bool Bishop::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    int goalDirectionX = abs(coordinate.getX() - thisLocation.getX()) ;
    int goalDirectionY = abs(coordinate.getY() - thisLocation.getY()) ;
    int dX = 0, dY = 0 ;
    while (dX != goalDirectionX && dX < size_N &&
            dY != goalDirectionY && dY < size_M) {
        dX ++ ; 
        dY ++ ;
    }
    if ((dX != goalDirectionX || dY != goalDirectionY)||(dX >= size_N || dY >= size_M)) return false ; //podminka (dX >= size_N || dY >= size_M) je nejspis zbytecna
    else return true ;
}


//=======================================================
//                    QUEEN
//=======================================================

Queen::Queen(ChessBoard::fraction fraction) : fraction_(fraction) {
}

ChessBoard::fraction Queen::getFraction() const {
    return fraction_ ;
}

std::string Queen::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wQ" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bQ" ;
    }
}

bool Queen::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    //TODO
    return false ;
}


//=======================================================
//                    ROOK
//=======================================================

Rook::Rook(ChessBoard::fraction fraction) : fraction_(fraction) {
}

ChessBoard::fraction Rook::getFraction() const {
    return fraction_ ;
}

std::string Rook::print() const {
    if (fraction_ == ChessBoard::WHITE) {
        return "wR" ;
    }
    else if (fraction_ == ChessBoard::BLACK) {
        return "bR" ;
    }
}

bool Rook::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    //TODO
    return false ;
}


//=======================================================
//                    FREE
//=======================================================

Free::Free() {
}

std::string Free::print() const {
    return "__";
}

bool Free::canMove(Coordinate & thisLocation, Coordinate & coordinate) {
    return false ;
} 

ChessBoard::fraction Free::getFraction() const {
    return ChessBoard::BLACK ;
}

int main(int argc, char ** argv) {
    std::cout << "Input filename or \"standard.txt\" for standard chessboard: " ;
    std::string filename ;
    std::cin >> filename ;
    chessBoard = std::make_unique<ChessBoard>(filename) ;
    chessBoard->gameCycle() ;
}
