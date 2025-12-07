
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <algorithm>

class Author {
    std::string name_;
    int birthYear_;
public:
    Author(): name_("Unknown"), birthYear_(1900) {}
    Author(const std::string& n, int y): name_(n), birthYear_(y) {
        if(y < 1850 || y > 2025) throw std::out_of_range("birthYear");
    }
    std::string getName() const { return name_; }
    int getBirthYear() const { return birthYear_; }
    void setBirthYear(int y){ if(y<1850||y>2025) throw std::out_of_range("birthYear"); birthYear_=y; }
    std::string to_string() const { std::ostringstream o; o<<name_<<" ("<<birthYear_<<")"; return o.str(); }
};

class Book {
    std::string title_;
    Author author_;
    int year_;
    double price_;
    std::string isbn_;
public:
    static int totalBooks;
    Book(): title_("Untitled"), author_(), year_(1900), price_(0), isbn_("") { ++totalBooks; }
    Book(const std::string&t,const Author&a,int y,double p,const std::string&i): title_(t),author_(a),year_(y),price_(p),isbn_(i) {
        if(y<1450||y>2025) throw std::out_of_range("year");
        if(p<0) throw std::invalid_argument("price");
        ++totalBooks;
    }
    Book(const Book&o): title_(o.title_),author_(o.author_),year_(o.year_),price_(o.price_),isbn_(o.isbn_){ ++totalBooks; }
    Book& operator=(const Book&o){
        if(this!=&o){ title_=o.title_; author_=o.author_; year_=o.year_; price_=o.price_; isbn_=o.isbn_; }
        return *this;
    }
    Book(Book&&o) noexcept: title_(std::move(o.title_)),author_(std::move(o.author_)),year_(o.year_),price_(o.price_),isbn_(std::move(o.isbn_)){ ++totalBooks; }
    Book& operator=(Book&&o) noexcept {
        if(this!=&o){ title_=std::move(o.title_); author_=std::move(o.author_); year_=o.year_; price_=o.price_; isbn_=std::move(o.isbn_); }
        return *this;
    }
    ~Book(){ --totalBooks; }

    std::string getIsbn() const { return isbn_; }
    Author getAuthor() const { return author_; }
    std::string to_string() const {
        std::ostringstream o; o<<'"'<<title_<<'"'<<" by "<<author_.to_string()<<", "<<year_<<", "<<price_<<" EUR, ISBN: "<<isbn_;
        return o.str();
    }
};
int Book::totalBooks = 0;

class Member {
    std::string name_;
    std::string memberId_;
    int yearJoined_;
public:
    Member(): name_("Unknown"), memberId_(""), yearJoined_(1900) {}
    Member(const std::string&n,const std::string&id,int y): name_(n),memberId_(id),yearJoined_(y){
        if(id.empty()) throw std::invalid_argument("id");
    }
    std::string getMemberId() const { return memberId_; }
};

class Loan {
    std::string isbn_;
    std::string memberId_;
    std::string startDate_;
    std::string dueDate_;
    bool returned_;
public:
    Loan(const std::string&i,const std::string&m,const std::string&s,const std::string&d):
        isbn_(i),memberId_(m),startDate_(s),dueDate_(d),returned_(false){
        if(d<s) throw std::invalid_argument("date");
    }
    std::string getIsbn() const { return isbn_; }
    std::string getMemberId() const { return memberId_; }
    bool isReturned() const { return returned_; }
    void markReturned(){ returned_=true; }
};

class Library {
    std::vector<Book> books_;
    std::vector<Member> members_;
    std::vector<Loan> loans_;
    const Member* findMemberPtr(const std::string&id) const {
        for(const auto&m:members_) if(m.getMemberId()==id) return &m;
        return nullptr;
    }
public:
    void addBook(const Book& b){ books_.push_back(b); }
    void addMember(const Member& m){ members_.push_back(m); }
    bool hasBook(const std::string&i) const {
        return std::any_of(books_.begin(),books_.end(),[&](const Book&b){return b.getIsbn()==i;});
    }
    bool isBookAvailable(const std::string&i) const {
        if(!hasBook(i)) return false;
        for(const auto&l:loans_) if(l.getIsbn()==i && !l.isReturned()) return false;
        return true;
    }
    bool loanBook(const std::string&i,const std::string&m,const std::string&s,const std::string&d){
        if(!hasBook(i)) return false;
        if(!findMemberPtr(m)) return false;
        if(!isBookAvailable(i)) return false;
        loans_.emplace_back(i,m,s,d);
        return true;
    }
};

int main(){
    Library lib;
    Author a("Ivan Vazov",1850);
    Book b1("Pod igoto",a,1894,25.5,"ISBN-001");
    lib.addBook(b1);
    lib.addMember(Member("Petar Petrov","M001",2023));
    lib.loanBook("ISBN-001","M001","2025-11-03","2025-11-17");
}
