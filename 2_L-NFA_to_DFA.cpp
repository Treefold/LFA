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
    int nrStates, nrSimbols, nrFinalStates, nrTranzitions, initialState;
    set <int> states;
    set <char> simbols;
    set <int> initialStates;
    set <int> currentStates;
    set <int> finalStates;
    map <pair <int, char>, set <int> > tranzitions;
    void printSetInt (ostream &out, set <int> &myset);
    void printSetChar (ostream &out, set <char> &myset);
public:
    Automat ();
    Automat (Automat &other);
    Automat& operator= (Automat &other);
    void closure (int option = 1);
    void fTransform (char simbol);
    bool inFinalStates (int option = 1);
    friend istream& operator>> (istream &in, Automat &a);
    friend ostream& operator<< (ostream &out, Automat &a);
    void testWords (istream &in, ostream &out);
    void toDFA (Automat &other);
};

int main()
{
    ifstream in ("date.in");
    ofstream out ("date.out");

    Automat a, b;
    in >> a;
    a.toDFA (b);
    out << b;

    //a.testWords(in, out);

    in.close();
    out.close();
    return 0;
}

void Automat:: toDFA (Automat &other)
{
    Automat a, b = *this;
    a.nrStates = 1;
    a.states.insert (1);
    a.nrSimbols = nrSimbols;
    a.simbols = simbols;
    a.initialState = 1;
    a.initialStates.insert(initialState);

    set <set <int> > newStates;
    map <set <int>, int> newStateName;
    queue <set <int> > q;
    newStates.insert(initialStates);
    newStateName.insert (make_pair (initialStates, 1));
    q.push (initialStates);
    set <char> ::iterator ic;
    set <int> solo;

    while (!q.empty()) {
        b.initialStates = q.front();
        q.pop();
        if (b.inFinalStates (0)) {++a.nrFinalStates; a.finalStates.insert(newStateName[b.initialStates]);}
        for (ic = simbols.begin(); ic != simbols.end(); ++ic) {
            b.currentStates = b.initialStates;
            b.fTransform (*ic);
            if (b.currentStates.size() == 0) {continue;}
            if (newStates.find (b.currentStates) == newStates.end()) {
                newStates.insert (b.currentStates);
                newStateName.insert (make_pair (b.currentStates, ++a.nrStates));
                a.states.insert(a.nrStates);
                q.push (b.currentStates);
            }
            ++a.nrTranzitions;
            solo = set <int> ();
            solo.insert (newStateName[b.currentStates]);
            a.tranzitions.insert (make_pair (make_pair(newStateName[b.initialStates], *ic), solo));
        }
    }
    other = a;
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

void Automat:: printSetChar (ostream &out, set <char> &myset)
{
    set <char> ::iterator ic;
    for (ic = simbols.begin(); ic != simbols.end(); ++ic) {
        out << *ic << " ";
    }
}

void Automat:: printSetInt (ostream &out, set <int> &myset)
{
    set <int> ::iterator i;
    for (i = myset.begin(); i != myset.end(); ++i) {
        out << *i << " ";
    }
}

ostream& operator<< (ostream &out, Automat &a)
{
    out << a.nrStates << "\n";
    a.printSetInt (out, a.states);
    out << "\n" << a.nrSimbols << "\n";
    a.printSetChar (out, a.simbols);
    out << "\n" << a.initialState << "\n" << a.nrFinalStates << "\n";
    a.printSetInt (out, a.finalStates);
    out << "\n" << a.nrTranzitions << "\n";
    map <pair <int, char>, set <int> > ::iterator m;
    set <int> ::iterator ii, ij;
    set <char> ::iterator ic;
    for (ii = a.states.begin(); ii != a.states.end(); ++ii) {
        if ((m = a.tranzitions.find (make_pair (*ii, a.lambda))) != a.tranzitions.end()) {
            out << *ii << " . " << *(m->second.begin()) << "\n";
        }
        for (ic = a.simbols.begin(); ic != a.simbols.end(); ++ic) {
            if ((m = a.tranzitions.find (make_pair (*ii, *ic))) != a.tranzitions.end()) {
                for (ij = m->second.begin(); ij != m->second.end(); ++ij) {
                    out << *ii << " " << *ic << " " << *ij << "\n";
                }
            }
        }
    }
    return out;
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
    in >> a.initialState >> a.nrFinalStates;
    a.initialStates.insert (a.initialState);
    a.closure(0); /// <q0>
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

Automat& Automat:: operator= (Automat &other)
{
    if (this == &other) {return *this;}
    this->nrStates = other.nrStates;
    this->nrSimbols = other.nrSimbols;
    this->nrFinalStates = other.nrFinalStates;
    this->nrTranzitions = other.nrTranzitions;
    this->initialState = other.initialState;
    this->states = other.states;
    this->simbols = other.simbols;
    this->initialStates = other.initialStates;
    this->currentStates = other.currentStates;
    this->finalStates = other.finalStates;
    this->tranzitions = other.tranzitions;
    return *this;
}

Automat:: Automat (Automat &other)
{
    this->nrStates = other.nrStates;
    this->nrSimbols = other.nrSimbols;
    this->nrFinalStates = other.nrFinalStates;
    this->nrTranzitions = other.nrTranzitions;
    this->initialState = other.initialState;
    this->states = other.states;
    this->simbols = other.simbols;
    this->initialStates = other.initialStates;
    this->currentStates = other.currentStates;
    this->finalStates = other.finalStates;
    this->tranzitions = other.tranzitions;
}

Automat:: Automat ()
{
    nrStates = nrSimbols = nrFinalStates = nrTranzitions = initialState = 0;
    states = initialStates = currentStates = finalStates = set <int> ();
    simbols = set <char> ();
    tranzitions = map <pair <int, char>, set <int> > ();
}
