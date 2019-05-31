#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <queue>
#include <string>

using namespace std;

class Automat
{
    static const char lambda = '.';
    int nrStates, nrSimbols, nrFinalStates, nrTranzitions;
    set <int> states;
    set <char> simbols;
    set <int> initialStates;
    set <int> currentStates;
    set <int> finalStates;
    map <pair <int, char>, set <int> > tranzitions;
    void printSetInt (set <int> &myset);
public:
    Automat ();
    Automat (Automat &other);
    void closure (int option = 1);
    void fTransform (char simbol);
    bool inFinalStates (int option = 1);
    friend istream& operator>> (istream &in, Automat &a);
    void testWords (istream &in, ostream &out);
};

int main()
{
    ifstream in ("date.in");
    ofstream out ("date.out");

    Automat a;
    in >> a;

    a.testWords(in, out);

    in.close();
    out.close();
    return 0;
}

void Automat:: testWords (istream &in, ostream &out)
{
    int nrWords, i, j;
    string word;
    in >> nrWords;
    for (i = 0; i < nrWords; ++i) {
        in >> word;
        currentStates = initialStates;
        for (j = 0; word[j] != '\0' && !currentStates.empty(); ++j) {
            fTransform (word[j]);
        }
        out << (inFinalStates () ? "DA\n" : "NU\n");
    }
}

void Automat:: printSetInt (set <int> &myset)
{
    set <int> ::iterator i;
    for (i = myset.begin(); i != myset.end(); ++i) {
        cout << *i << " ";
    }
    cout << "\n";
}

istream& operator>> (istream &in, Automat &a)
{
    int i, x, y;
    char c;
    in >> a.nrStates;
    for (i = 0; i < a.nrStates; ++i) {
        in >> x;
        a.states.insert (x);
    }
    in >> a.nrSimbols;
    for (i = 0; i < a.nrSimbols; ++i) {
        in >> c;
        a.simbols.insert (c);
    }
    in >> x >> a.nrFinalStates;
    a.initialStates.insert (x);
    for (i = 0; i < a.nrFinalStates; ++i) {
        in >> x;
        a.finalStates.insert (x);
    }
    in >> a.nrTranzitions;
    map <pair <int, char>, set <int> > ::iterator iPos;
    for (i = 0; i < a.nrTranzitions; ++i) {
        in >> x >> c >> y;
        if ((iPos = a.tranzitions.find (make_pair(x, c))) != a.tranzitions.end()) {
            iPos->second.insert (y);
        }
        else {
            set <int> solo;
            solo.insert (y);
            a.tranzitions.insert (make_pair (make_pair(x, c), solo));
        }
    }
    a.closure (0); /// <q0>
    return in;
}

bool Automat:: inFinalStates (int option /*= 1*/)
{
    set <int> & myset = ((option) ? currentStates : initialStates);
    set <int> ::iterator i;
    for (i = myset.begin(); i != myset.end(); ++i) {
        if (finalStates.find (*i) != finalStates.end()) {return 1;}
    }
    return 0;
}

void Automat:: fTransform (char simbol)
{
    set <int> initialSet = currentStates;
    if (initialSet.empty() || simbols.find(simbol) == simbols.end()) {currentStates = set <int> ();return;}
    set <int> finalSet;
    set <int> ::iterator iState, jState;
    map <pair <int, char>, set <int> > ::iterator iPos;
    for (iState = initialSet.begin(); iState != initialSet.end(); ++iState) {
        iPos = tranzitions.find (make_pair (*iState, simbol));
        if (iPos != tranzitions.end()) {
            for (jState = iPos->second.begin(); jState != iPos->second.end(); ++jState) {
                finalSet.insert (*jState);
            }
        }
    }
    currentStates = finalSet;
    closure();
}

void Automat:: closure (int option /*= 1*/)
{
    set <int> & mySet = (option) ? currentStates : initialStates;
    queue <int> q;
    set <int> ::iterator iState;
    for (iState = mySet.begin(); iState != mySet.end(); ++iState) {
        q.push (*iState);
    }

    map <pair <int, char>, set <int> > ::iterator iPos;
    while (!q.empty()) {
        iPos = tranzitions.find (make_pair (q.front(), lambda));
        q.pop();
        if (iPos != tranzitions.end()) {
            for (iState = iPos->second.begin(); iState != iPos->second.end(); ++iState) {
                if (mySet.find (*iState) == mySet.end()) {
                    mySet.insert (*iState);
                    q.push (*iState);
                }
            }
        }
    }
}

Automat:: Automat (Automat &other)
{
    this->nrStates = other.nrStates;
    this->nrSimbols = other.nrSimbols;
    this->nrFinalStates = other.nrFinalStates;
    this->nrTranzitions = other.nrTranzitions;
    this->states = other.states;
    this->simbols = other.simbols;
    this->initialStates = other.initialStates;
    this->currentStates = other.currentStates;
    this->finalStates = other.finalStates;
    this->tranzitions = other.tranzitions;
}

Automat:: Automat ()
{
    nrStates = nrSimbols = nrFinalStates = nrTranzitions = 0;
    states = initialStates = currentStates = finalStates = set <int> ();
    simbols = set <char> ();
    tranzitions = map <pair <int, char>, set <int> > ();
}
