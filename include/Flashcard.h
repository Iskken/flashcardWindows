#ifndef FLASHCARD_H
#define FLASHCARD_H
#include <string>
#include <iostream>
#include <vector>
#include <sstream> // to use std::ostringstream
#include <algorithm>//for std::find
class Flashcard {
protected:
    std::string question;
    std::string answer;
public:
    Flashcard(const std::string &qu = "", const std::string &ans = "")//default constructor
    {
        question = qu;
        answer = ans;
    }
    Flashcard(const Flashcard &other) { //copy constructor
        question = other.question;
        answer = other.answer;
    }
    std::string getqu() const
    {
        return question;
    }
    std::string getans() const
    {
        return answer;
    }
    void setqu(const std::string &qu)
    {
        question = qu;
    }
    void setans(const std::string &ans)
    {
        answer = ans;
    }
    virtual void display() const {
        std::cout << "Q: " << question << "\n" << "A: " << answer << "\n";
    }
};

class ChoiceCard:public Flashcard
{
    std::vector <std::string> options;
public:
    ChoiceCard(const std::string &qu = "", const std::string &ans = ""):Flashcard(qu,ans) {}//default constructor
    ChoiceCard(const std::string &qu, const std::string &ans, const std::vector <std::string> &opt):Flashcard(qu, ans),
    options(opt) {}
    
    ChoiceCard(const ChoiceCard &other) : Flashcard(other) {
        options = other.options;
    }
    std::vector <std::string> getChoices() const
    {
        return options;
    }
    void clearOptions()
    {
        options.clear();
    }
    void addChoice(std::string choice)
    {
        options.push_back(choice);
    }

    int getNumOfOptions() const
    {
        return options.size();
    }
    void checkOption(const int &userOption) const
    {
        if (std::stoi(answer) == userOption)
        {
            std::cout << "Correct!\n";
        }
        else
        {
            std::cout << "Try again\n";
        }
    }

    std::string enumChoices() { //needed to pass into the database as one whole text
        std::ostringstream oss;
        for (int i = 0; i < options.size(); ++i) {
            if (i != 0) oss << ",";
            oss << options[i];
        }
        return oss.str();
    }

    void denumChoices(const std::string &str) { //needed to initialize the choices
        std::istringstream iss(str);
        std::string choice;
        while (std::getline(iss, choice, ',')) {
            options.push_back(choice);
        }
    }

    bool checkExistenceClistAnswer(std::string answer) const{
        for (int i = 0; i < options.size(); i++)
        {
            if (std::find(options.begin(), options.end(), answer) != options.end())
            {
                return true;
            } 
        }
        return false;
    }
    virtual void display() const {
        std::cout << "Q: " << question << "\n" << "A: " << answer << "\n";
        std::cout << "Multiple choices:\n";
        int i = 1;
        for (std::string ch : options)
        {
            std::cout << i << ")" << ch << "\n"; 
            i++;
        }
    }
};
#endif