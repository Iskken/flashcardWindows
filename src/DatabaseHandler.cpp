#include "../include/DatabaseHandler.h"
#include <sqlite3.h>
#include <iostream>
DatabaseHandler::DatabaseHandler(const char *path) : dbPath(path), db(nullptr){}

void DatabaseHandler::openDb()
{
    int check = sqlite3_open(dbPath, &db);
    if (check != SQLITE_OK)
        std::cerr << "Error loading database\n";
}

void DatabaseHandler::initializeDb()
{
    const char *sqlTables = R"(
        CREATE TABLE IF NOT EXISTS decks(
            deck_id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS flashcards(
            flash_id INTEGER PRIMARY KEY AUTOINCREMENT,
            deck_id INTEGER,
            question TEXT NOT NULL,
            answer TEXT NOT NULL,
            FOREIGN KEY (deck_id) REFERENCES decks(deck_id) ON DELETE CASCADE
        );
        CREATE TABLE IF NOT EXISTS choicecards(
            choice_id INTEGER PRIMARY KEY AUTOINCREMENT,
            deck_id INTEGER,
            question TEXT NOT NULL,
            answer TEXT NOT NULL,
            choices TEXT NOT NULL,
            FOREIGN KEY (deck_id) REFERENCES decks(deck_id) ON DELETE CASCADE
        );
    )";

    char *errMsg = nullptr;
    sqlite3_exec(db, sqlTables, nullptr, nullptr,&errMsg);
}

bool DatabaseHandler::insertDeck(const char *deckTitle)
{
    const char *sql = "INSERT INTO decks (title) VALUES (?);";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, deckTitle, -1, SQLITE_STATIC);

    int check = sqlite3_step(stmt);
    if (check != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement";
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseHandler::deleteDeck(int deckId)
{
    const char *sqlFlashDel = "DELETE FROM flashcards WHERE deck_id = ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sqlFlashDel, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, deckId);
    int check = sqlite3_step(stmt);
    if (check != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    const char *sqlChoiceDel = "DELETE FROM choicecards WHERE deck_id = ?";
    sqlite3_prepare_v2(db, sqlChoiceDel, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, deckId);
    check = sqlite3_step(stmt);
    if (check != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    const char *sql = "DELETE FROM decks WHERE deck_id = ?";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, deckId);
    check = sqlite3_step(stmt);
    if (check != SQLITE_DONE) {
        std::cerr << "Error executing SQL statement";
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseHandler::insertFlashCard(int deckID, const char *question, const char *answer)
{
    const char *sqlCheckId = "SELECT deck_id FROM decks WHERE deck_id = ?;";
    const char *sqlInsertion = "INSERT INTO flashcards (deck_id, question, answer) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(db, sqlCheckId, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, deckID);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Error executing SQL statement (such a deck_id doesn't exist)";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, sqlInsertion, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, deckID);
    sqlite3_bind_text(stmt, 2, question, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, answer, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "Error executing statement (inserting flashcard)";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseHandler::deleteFlashCard(int flash_id)
{
    const char *sql = "DELETE FROM flashcards WHERE flash_id = ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt,  1, flash_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "The deletion of flashcard was not successful";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool DatabaseHandler::deleteChoiceCard(int choice_id)
{
    const char *sql = "DELETE FROM choicecards WHERE choice_id = ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt,  1, choice_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "The deletion of choicecard was not successful";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool DatabaseHandler::insertChoiceCard(int deckID, const char *question, const char *answer, std::string enumChoices)
{
    const char *sql1 = "SELECT deck_id FROM decks WHERE deck_id = ?;";
    const char *sql2 = "INSERT INTO choicecards (deck_id, question, answer, choices) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;

    sqlite3_prepare_v2(db, sql1, -1, &stmt, NULL);
    sqlite3_bind_int(stmt,  1, deckID);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Error executing SQL statement (such a deck_id doesn't exist)";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, deckID);
    sqlite3_bind_text(stmt, 2, question, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, answer, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, enumChoices.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "Error executing statement (inserting choicecard)";
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}
void DatabaseHandler::initializeFlashcard(const unsigned char *title, Deck &deck)
{
    int deck_id = getDeckId(reinterpret_cast<const char*>(title));//converting from const unsigned char* to const char *
    const char *sqlGetData = "SELECT question,answer FROM flashcards WHERE deck_id  = ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sqlGetData,-1,&stmt, NULL);
    sqlite3_bind_int(stmt, 1, deck_id);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char * question = sqlite3_column_text(stmt, 0);
        const unsigned char * answer = sqlite3_column_text(stmt, 1);
        
        deck.flist[deck.number_of_cards_flist].setqu(reinterpret_cast<const char*>(question));
        deck.flist[deck.number_of_cards_flist].setans(reinterpret_cast<const char *>(answer));

        deck.number_of_cards_flist++;
    }
    sqlite3_finalize(stmt);
}

void DatabaseHandler::initializeChoicecards(const unsigned char *title, Deck &deck)
{
    int deck_id = getDeckId(reinterpret_cast<const char*>(title));//converting from const unsigned char* to const char *
    const char *sqlGetData = "SELECT question,answer,choices FROM choicecards WHERE deck_id  = ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sqlGetData,-1,&stmt, NULL);
    sqlite3_bind_int(stmt, 1, deck_id);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char * question = sqlite3_column_text(stmt, 0);
        const unsigned char * answer = sqlite3_column_text(stmt, 1);
        const unsigned char * choices = sqlite3_column_text(stmt, 2);
        
        deck.clist[deck.number_of_cards_clist].setqu(reinterpret_cast<const char*>(question));
        deck.clist[deck.number_of_cards_clist].setans(reinterpret_cast<const char *>(answer));
        deck.clist[deck.number_of_cards_clist].denumChoices(reinterpret_cast<const char *>(choices));
        deck.number_of_cards_clist++;
    }
    sqlite3_finalize(stmt);
}

void DatabaseHandler::initializeDecks(std::vector<Deck> &decks){
    const char *sql = "SELECT title FROM decks";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << std::endl;
        return;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *title = sqlite3_column_text(stmt, 0);
        if (title != nullptr) {
            std::string deckTitle(reinterpret_cast<const char*>(title));//convert from const char * to string
            Deck deck(deckTitle);
            initializeFlashcard(title, deck);
            initializeChoicecards(title, deck);
            decks.push_back(deck);
        }
    }
    sqlite3_finalize(stmt);
}

int DatabaseHandler::getDeckId(const char *deckTitle) const
{
    int deck_id = 0;
    const char *sql = "SELECT deck_id FROM decks WHERE title = ?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, deckTitle, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        deck_id = sqlite3_column_int(stmt, 0);
    } else {
        std::cerr << "Error obtaining deck_id or no matching row found: " << std::endl;
    }
    sqlite3_finalize(stmt);
    return deck_id;
}

int DatabaseHandler::getFlashId(const char *qu) 
{
    const char *sql = "SELECT flash_id FROM flashcards WHERE question = ?";
    sqlite3_stmt *stmt;
    int flash_id = -1;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, qu, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        flash_id = sqlite3_column_int(stmt, 0);
    } else {
        std::cerr << "Error obtaining flash_id or no matching row found: " << std::endl;
    }
    sqlite3_finalize(stmt);
    return flash_id;
}
int DatabaseHandler::getChoiceId(const char *qu)
{
    const char *sql = "SELECT choice_id FROM choicecards WHERE question = ?";
    sqlite3_stmt *stmt;
    int choice_id;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, qu, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        choice_id = sqlite3_column_int(stmt, 0);
    } else {
        std::cerr << "Error obtaining flash_id or no matching row found: " << std::endl;
    }
    sqlite3_finalize(stmt);
    return choice_id;
}

void DatabaseHandler::closeDb()
{
    sqlite3_close(db);
}
DatabaseHandler::~DatabaseHandler()
{
    closeDb();
}