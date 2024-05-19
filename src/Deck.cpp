#include "../include/Deck.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>

Deck::Deck(const std::string &title) 
    : deck_title(title), flist(nullptr), clist(nullptr), 
      number_of_cards_flist(0), number_of_cards_clist(0), 
      capacity_flist(10), capacity_clist(10) {
    flist = new Flashcard[capacity_flist];
    clist = new ChoiceCard[capacity_clist];
}

Deck::Deck(const Deck &other)
    : deck_title(other.deck_title), flist(new Flashcard[other.capacity_flist]),
      clist(new ChoiceCard[other.capacity_clist]), number_of_cards_flist(other.number_of_cards_flist),
      number_of_cards_clist(other.number_of_cards_clist), capacity_flist(other.capacity_flist),
      capacity_clist(other.capacity_clist) {
    for (int i = 0; i< number_of_cards_flist; i++)
    {
        flist[i] = other.flist[i];
    }
    for (int i = 0; i < number_of_cards_clist; i++)
    {
        clist[i] = other.clist[i];
    }
}

void Deck::shuffleCards() //function to shuffle cards of both types
{
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(flist, flist + number_of_cards_flist, std::default_random_engine(seed));

        std::shuffle(clist, clist + number_of_cards_clist, std::default_random_engine(seed));
}

void KeyPress(char expected) {
    char key_pressed;
    do
    {
        std::cin>>key_pressed;
        if (tolower(key_pressed) != expected)
            std::cerr << "Wrong key pressed. Try again!\n";
    } while (tolower(key_pressed) != expected);
}

void Deck::reviewDeck() const {
    for (int i = 0; i < number_of_cards_flist; i++) {
        std::cout << "Question:" << flist[i].getqu() << "\n" << "Press 'Y' key to show the answer\n";
        KeyPress('y');
        std::cout << "Answer:" << flist[i].getans() << "\n";
        std::cout << "Press 'Y' to continue reviewing\n";
        KeyPress('y');
        std::system("clear"); // to clear the console after the review is done
    }

    for (int i = 0; i < number_of_cards_clist; i++) { 
        std::vector<std::string> options = clist[i].getChoices();
        std::cout << "Question:" << clist[i].getqu() << "\n";
        for (int j = 0; j < options.size(); j++) {
            std::cout << j+1 << "." << options[j] << "\n";
        }
        std::cout << "Press 'Y' key to show the answer\n";
        KeyPress('y');
        std::cout << "Answer:" << clist[i].getans() << "\n";
        std::cout << "Press 'Y' to continue reviewing\n";
        KeyPress('y');
        std::system("clear"); // to clear the console after the review is done
    }
    std::cout << "Congratulations! You have finished the deck for now!\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int Deck::getNumFlist() const
{
    return number_of_cards_flist;
}
int Deck::getNumClist() const
{
    return number_of_cards_clist;
}

void Deck::increaseCapacityFlist(int newCapacity)
{
    Flashcard *newlist = new Flashcard[newCapacity];
    for (int i = 0; i < number_of_cards_flist; i++)
    {
        newlist[i] = flist[i];
    }
    delete[] flist;
    flist = newlist;
    capacity_flist = newCapacity;
}
void Deck::addFlashcard(const Flashcard &f) {
    if (number_of_cards_flist == 0)
    {
        flist = new Flashcard[capacity_flist];
    }
    if (number_of_cards_flist == capacity_flist) {
        increaseCapacityFlist(capacity_flist * 2);
    }
    flist[number_of_cards_flist++] = f;
}

void Deck::increaseCapacityClist(int newCapacity)
{
    ChoiceCard *newlist = new ChoiceCard[newCapacity];
    for (int i = 0; i < number_of_cards_clist; i++)
    {
        newlist[i] = clist[i];
    }
    delete[] clist;
    clist = newlist;
    capacity_clist = newCapacity;
}
void Deck::addChoicecard(const ChoiceCard &c) {
    if (number_of_cards_clist == 0)
    {
        clist = new ChoiceCard[capacity_clist];
    }
    if (number_of_cards_clist == capacity_clist) {
        increaseCapacityClist(capacity_clist * 2);
    }
    clist[number_of_cards_clist++] = c;
}

std::string Deck::getQuestionAtFlist(int position) const{
    return flist[position].getqu();
}
std::string Deck::getQuestionAtClist(int position) const{
    return clist[position].getqu();
}
bool Deck::removeCard(int position)
{
    if (position < number_of_cards_flist)
    {
        Flashcard *newlist = new Flashcard[capacity_flist];
        for (int i = 0, j = 0; i < number_of_cards_flist; i++)
        {
            if (i != position) // Skip the card at the given position
            {
                newlist[j++] = flist[i];
            }
        }
        delete[] flist;
        flist = newlist;
        number_of_cards_flist--;
        return true;
    } else if (position >= number_of_cards_flist && position <number_of_cards_clist + number_of_cards_flist)
    {
        ChoiceCard *newlist = new ChoiceCard[capacity_clist];
        position -= number_of_cards_flist;
        for (int i = 0, j = 0; i < number_of_cards_clist; i++)
        {
        if (i != position) // Skip the card at the given position
            {
                newlist[j++] = clist[i];
            }
        }
        delete[] clist;
        clist = newlist;
        number_of_cards_clist--;
        return true;
    }
    return false;
}
void Deck::browseCards() const
{
    int i;
    int j;
    for (i = 0; i < number_of_cards_flist; i++)
    {
        std::cout << i + 1 << ".\n";
        flist[i].display();
        std::cout << "\n";
    }
    for (j = i+1; j-i-1 < number_of_cards_clist; j++)
    {
        std::cout << j << ".\n";
        clist[j-i-1].display();
        std::cout << "\n";
    }
}