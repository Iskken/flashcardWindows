#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H
#include "Deck.h"
#include <sqlite3.h>

class Deck; //forward declaration to use databaseHandler class as a friend function

class DatabaseHandler{
    sqlite3 *db;
    const char *dbPath;
public:
    DatabaseHandler(const char *path);

    void openDb();
    void closeDb();
    void initializeDb();
    bool insertDeck(const char *deckTitle);
    bool deleteDeck(int deckId);
    bool insertFlashCard(int deckID, const char *question, const char *answer);
    bool deleteFlashCard(int flash_id);
    bool insertChoiceCard(int deckID, const char *question, const char *answer, std::string enumChoices);
    bool deleteChoiceCard(int choice_id);
    void initializeFlashcard(const unsigned char *title,Deck &deck);
    void initializeChoicecards(const unsigned char *title, Deck &deck);
    int getDeckId(const char *deckTitle) const;
    int getFlashId(const char *qu);
    int getChoiceId(const char *qu);
    void initializeDecks(std::vector <Deck> &decks);

    ~DatabaseHandler();
};

#endif