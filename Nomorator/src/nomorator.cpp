#include "nomorator.h"
#include <cstring>
#include <memory>
NomoratorPage::NomoratorPage(int pn, int r, int c, vector<vector<std::string> > st)
    : pageNumber(pn), rc {r, c}, data(st) {}

NomoratorPage::~NomoratorPage()
{}

int Nomorator::totalPage() const
{
    int range = stop() - start();
    return range % pageDimension() ? (range / pageDimension()) + 1 : range / pageDimension();
}

void Nomorator::setPrefix(const string prf)
{
    extra[0] = prf;
}

void Nomorator::setSufix(const string sfx)
{
    extra[1] = sfx;
}

NomoratorPage Nomorator::getPage(int n)
{
    int page_dimension = row() * column();
    int total_page = totalPage();
    if(n >= total_page || n < 0) {
        return NomoratorPage(-1, 0, 0, vector<vector<string>>());
    }
    int target_page = isReversed() ? total_page - (n + 1) : n;
    int full_bind = total_page / bind();
    int un_bind = total_page % bind();
    int current_bind = target_page / bind();
    int use_un_bind = target_page % bind();
    int incr_bind = bind() * page_dimension;
    int incr_row = use_un_bind && ( current_bind == full_bind ) ? un_bind * column() : bind() * column();
    int incr_col = use_un_bind && ( current_bind == full_bind ) ? un_bind : bind();

    char buffer[100];
    char fmt[8];
    memset(fmt, 0, 8);
    snprintf(fmt, 8, "%%0%dd", padCount());
    vector<vector<string>> ddd;
    for(int r=0; r<row(); ++r) {
        vector<string> dd;
        for(int c=0; c<column(); ++c) {
            int cn = start() + use_un_bind + (current_bind * incr_bind) + (r * incr_row) + (c * incr_col);
            memset(buffer, 0, 100);
            snprintf(buffer, 100, fmt, cn);
            if(padChar() != '0') {
                int nth = 0;
                while(buffer[nth] == '0')
                    buffer[nth++] = padChar();
            }
            string cur_col = prefix();
            cur_col += buffer;
            cur_col += sufix();
            dd.push_back(cur_col);
        }
        ddd.push_back(dd);
    }
    return NomoratorPage(target_page, row(), column(), ddd);
}
