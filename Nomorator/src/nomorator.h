#ifndef NOMORATOR_H
#define NOMORATOR_H
#include <vector>
#include <string>

using std::string;
using std::vector;

struct NomoratorPage {
    const int pageNumber;
    const int rc[2];
    vector<vector<string>> data;
    NomoratorPage(int pn, int r, int c, vector<vector<string>> st);
    ~NomoratorPage();
};

class Nomorator {
    int mparams[6];         // start, stop, row, column, bind, padcount
    char padchar;           // padchar '0'
    string extra[2] {string(), string()};       // prefix & suffix
    bool reverse;           // ordering
public:
    Nomorator(int sta, int sto, int r, int c, int b, int pcnt, char pchar)
        : mparams{sta, sto, r, c, b, pcnt}, padchar(pchar) {}
    Nomorator(int sta, int sto, int r, int c, int b, int pcnt)
        : Nomorator(sta, sto, r, c, b, pcnt, '0') {}
    Nomorator()
        : Nomorator(1, 5000, 9, 2, 100, 5) {}
    inline const int& start() const { return mparams[0]; }
    inline const int& stop() const { return mparams[1]; }
    inline const int& row() const { return mparams[2]; }
    inline const int& column() const { return mparams[3]; }
    inline const int& bind() const { return mparams[4]; }
    inline const int& padCount() const { return mparams[5]; }
    inline const char& padChar() const { return padchar; }
    inline const string prefix() const { return extra[0]; }
    inline const string sufix() const { return extra[1]; }
    inline const bool& isReversed() const { return reverse; }
    inline int pageDimension() const { return mparams[2] * mparams[3]; }
    int totalPage() const;
    int upper() const { return totalPage() * pageDimension() + start(); }

    inline void setStart(int val) { mparams[0] = val; }
    inline void setStop(int val) { mparams[1] = val; }
    inline void setRow(int val) { mparams[2] = val; }
    inline void setColumn(int val) { mparams[3] = val; }
    inline void setBind(int val) { mparams[4] = val; }
    inline void setPadCount(int val) { mparams[5] = val; }
    inline void setPadChar(char val) { padchar = val; }
    inline void setReverse(bool val) { reverse = val; }

    void setPrefix(const string prf);
    void setSufix(const string sfx);

    NomoratorPage getPage(int n);
};

#endif // NOMORATOR_H
