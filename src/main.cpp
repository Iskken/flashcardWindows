#include <sqlite3.h>
#include <iostream>
#include "../include/Deck.h"
#include "../include/DatabaseHandler.h"    
#include <vector>
#include <limits> //to use numeric_limits
#include "../include/CustomException.h"
#include <thread> //libraries to implement the 
#include <chrono> // delay time

void listDecks(std::vector<Deck> &Decks)
{
    if (Decks.empty())
    {
        std::system("cls");
        std::cout << "There are no decks to review!\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }
    int option;
    for (int i = 0; i < Decks.size(); i++)
    {
        std::cout << i+1 << "." << Decks[i].getTitle() << "\n";
    }
    try{
        do 
        {
            std::cout << "To choose a deck to review, type in the number of the deck!\n";
            std::cin >> option;
            if (std::cin.fail()) { //in case the user inputs a text instead of numbers
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cerr << "Invalid input!\n";
            } 
            if (option < 1 || option > Decks.size()) //handling cases when user input is not within boundaries
                {throw CustomException("Invalid input!\n");}
            if (Decks[option-1].getNumFlist() == 0 && Decks[option-1].getNumClist() == 0)
                throw CustomException("There are no cards in this deck!\n");
        } while (option < 1 || option > Decks.size());
        std::system("cls");
        Decks[option-1].shuffleCards();
        Decks[option-1].reviewDeck();
        system("cls");
    }
    catch (const CustomException &e)
    {
        std::system("cls");
        std::cerr << e.what();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void addDeck(DatabaseHandler &db, std::vector <Deck> &Decks)
{
    std::string title;
    std::cout << "Type the name of the new deck:";
    try
    {
        std::cin.ignore();
        std::getline(std::cin, title);
        std::cout << std::endl;
        if (db.insertDeck(title.c_str())) //c_str() function converts from string to const char *
        {
            Deck newdeck(title);
            Decks.push_back(newdeck);
            std::cout << "The deck has been added successfully!\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::system("cls");
        }
        else{
            throw CustomException("Deck insertion is invalid");
        }
    }
    catch(const CustomException& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void addCard(DatabaseHandler &db,std::vector <Deck> &Decks)
{
    if (Decks.empty())
    {
        std::cout << "There are no decks to add card into!\n";
        return;
    }
    int option;
    int deck_id;
    bool validInput = false;
    while (!validInput)
    {
        int deckchoice;
        std::cout << "What type of card would you prefer: 1.Flashcard or 2.ChoiceCard?\n";
        do
        {
            std::cin >> option;
            if (std::cin.fail()) { //in case the user inputs a text instead of numbers
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cerr << "Invalid input! The choice should be either 1 or 2!\n";
            } 
            else if (option != 1 && option != 2)
                std::cerr << "The choice should be either 1 or 2!\n";
        }while (option != 1 && option != 2);
        std::system("cls");
        switch(option)
        {
            case 1:
            {
                std::string question;
                std::string answer;
                std::cout << "Enter the question:\n";
                std::cin.ignore(); 
                std::getline(std::cin, question);
                std::cout << "Enter the answer:\n";
                std::getline(std::cin, answer);
                Flashcard f(question, answer);

                std::system("cls");
                std::cout << "Choose which deck you would like to store this card in?\n";
                for (int i = 0; i < Decks.size(); i++)
                {
                    std::cout << i+1 << "." << Decks[i].getTitle() << "\n";
                }
                do
                {
                    std::cin >> deckchoice;
                    if (std::cin.fail()) { //in case the user inputs a text instead of numbers
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                        std::cerr << "Invalid input!Try again!\n";
                    } 
                    else if (deckchoice < 1 || deckchoice > Decks.size())
                        std::cerr << "The chosen deck index is invalid! Try again!\n";
                } while (deckchoice < 1 || deckchoice > Decks.size());
                Decks[deckchoice-1].addFlashcard(f);

                deck_id = db.getDeckId(Decks[deckchoice-1].getTitle().c_str());
                db.insertFlashCard(deck_id, question.c_str(), answer.c_str());
                
                std::system("cls");
                std::cout << "The card has been added succesfully!\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                validInput = true;
                std::system("cls");
                break;
            }
            case 2: 
            {
                ChoiceCard newcard;
                std::string question;
                std::string choice;
                char response;
                std::string correctAnswer;
                bool addingOptions = true;
                std::cout << "Enter the question:\n";
                std::cin.ignore(); 
                std::getline(std::cin, question);
                newcard.setqu(question);
                std::system("cls");
                while (addingOptions)
                {
                    std::cout << "Enter the choice:\n";
                    std::getline(std::cin, choice);
                    newcard.addChoice(choice);
                    std::cout << "Press 'Y' to continue adding choices or 'N' otherwise\n";
                    do
                    {
                        std::cin>>response;
                        if (tolower(response) == 'n')
                            addingOptions = false;
                    } while (tolower(response) != 'n' && tolower(response) != 'y');
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::system("cls");
                }
                do
                {
                    std::system("cls");
                    std::cout << "Enter the correct answer:\n";
                    std::getline(std::cin, correctAnswer);
                } while (!newcard.checkExistenceClistAnswer(correctAnswer));

                newcard.setans(correctAnswer);
                
                std::cout << "Choose which deck you would like to store this card in?\n";
                for (int i = 0; i < Decks.size(); i++)
                {
                    std::cout << i+1 << "." << Decks[i].getTitle() << "\n";
                }
                do
                {
                    std::cin >> deckchoice;
                    if (deckchoice < 1 || deckchoice > Decks.size()) {
                        std::cerr << "Invalid deck choice. The deck index must be between 1 and " + std::to_string(Decks.size()) + ".Try again!\n";
                }
                } while (deckchoice < 1 || deckchoice > Decks.size());
                Decks[deckchoice-1].addChoicecard(newcard);

                deck_id = db.getDeckId(Decks[deckchoice-1].getTitle().c_str());
                db.insertChoiceCard(deck_id, question.c_str(), newcard.getans().c_str(),newcard.enumChoices());

                std::cout << "The card has been added succesfully!\n";
                validInput = true;
                break;
            }
            default:break;
        }
    }
}

void deleteDeck(DatabaseHandler &db, std::vector <Deck> &decks)
{
    if (decks.empty())
    {
        std::cout << "There are no decks to delete!\n";
        return;
    }
    int option;
    int deckId;
    bool validOption = false;
    std::cout << "Choose which deck you would like to delete:\n";
    for (int i = 0; i < decks.size(); i++)
    {
       std::cout << i+1 << "." << decks[i].getTitle() << "\n";
    }
    while (!validOption) {
        std::cin >> option;
        if (std::cin.fail()) { //in case the user inputs a text instead of numbers
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cerr << "Invalid input!\n";
        } 
        else if (option >= 1 && option <= decks.size()) {
            validOption = true;
        } else {
            std::cerr << "Invalid option! Please try again.\n";
        }
    }
    std::string deckTitle = decks[option-1].getTitle();
    deckId = db.getDeckId(deckTitle.c_str()); //checking
    decks.erase(decks.begin() + (option-1)); //the erase() function should call the destructor of Deck
     std::cout << deckId<< " - decktitle debugging\n";
    try{
        if (!db.deleteDeck(deckId))
            throw CustomException("Deck deletion was unsuccessful");
    }
    catch (const CustomException &e)
    {
        std::cerr << e.what() << "\n";
    }
}

bool checkCardsPresence(std::vector <Deck> &decks) //checks whethere there are cards to review in all of the decks
{
    for (const Deck &d: decks)
    {
        if (d.getNumFlist() > 0 || d.getNumClist() > 0)
            return true;
    }
    return false;
}

void deleteCard(DatabaseHandler &db, std::vector <Deck> &decks)
{
    if (decks.empty() || !checkCardsPresence(decks))
    {
        std::system("cls");
        std::cout << "There are no cards to delete!\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }
    int option;
    int id;
    int cardOption;
    bool validOption = false;
    int deckId;
    std::cout << "Choose the deck you would like to delete the card in:\n";
    for (int i = 0; i < decks.size(); i++)
    {
        std::cout << i+1 << "." << decks[i].getTitle() << "\n";
    }
    while (!validOption) {
        std::cin >> option;
            if (std::cin.fail()) { //in case the user inputs a text instead of numbers
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cerr << "Invalid input!\n";
            } 
        if (option >= 1 && option <= decks.size()) {
            validOption = true;
        } else {
            std::cerr << "Invalid option! Please try again.\n";
        }
    }
    validOption = false;
    std::string deckTitle = decks[option-1].getTitle();
    deckId = db.getDeckId(deckTitle.c_str());
    if (decks[option-1].getNumClist() == 0 && decks[option-1].getNumFlist() == 0)
    {
        std::system("cls");
        std::cout << "There are no cards in this deck!\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    std::system("cls");
    std::cout << "The cards in this deck are the following:\n";
    decks[option-1].browseCards();
    std::cout << "Which card would you like to delete?\n";
    while (!validOption) {
        std::cin >> cardOption;
        if (std::cin.fail()) { //in case the user inputs a text instead of numbers
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cerr << "Invalid input!\n";
            } 
        if (cardOption >= 1 && cardOption <= (decks[option-1].getNumClist() + decks[option-1].getNumFlist())) {
            validOption = true;
        } else {
            std::cerr << "Invalid option! Please try again.\n";
        }
    }
    if (cardOption <= decks[option-1].getNumFlist())
    {
        std::string qu = decks[option-1].getQuestionAtFlist(cardOption-1);
        id = db.getFlashId(qu.c_str());
        db.deleteFlashCard(id);
    }
    else if (cardOption > decks[option-1].getNumFlist() && cardOption <= decks[option-1].getNumClist() + decks[option-1].getNumFlist())
    {
        std::string qu = decks[option-1].getQuestionAtClist(cardOption-1-decks[option-1].getNumFlist());
        id = db.getChoiceId(qu.c_str());
        db.deleteChoiceCard(id);
    }
    decks[option-1].removeCard(cardOption-1);
    std::cout << "The card has been deleted successfully!\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main() {
    DatabaseHandler db("test.db");
    db.openDb();
    db.initializeDb();
    std::vector <Deck> Decks;
    db.initializeDecks(Decks);
    bool running = true;
    while (running)
    {
        std::system("cls");
        std::cout << "1.Review Deck\n2.Add Deck\n3.Add new card\n4.Remove Deck\n5.Remove card\n0.Exit application\n";
        int option;
        std::cin >> option;
        switch (option)
        {
            case 1:std::system("cls");listDecks(Decks);break;
            case 2:std::system("cls");addDeck(db, Decks); break;
            case 3:std::system("cls");addCard(db, Decks);break;
            case 4:std::system("cls");deleteDeck(db, Decks);break;
            case 5:std::system("cls");deleteCard(db, Decks);break;
            case 0:running = false;break;
        }
    }
    return 0;
}