#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ======= CLASS ITEM (BASE) =======
class Item {
protected:
    string id, title;
    bool available;

public:
    Item(string id, string title) : id(id), title(title), available(true) {}
    virtual ~Item() {}

    string getId() const { return id; }
    string getTitle() const { return title; }
    bool isAvailable() const { return available; }
    void setAvailable(bool a) { available = a; }

    virtual int maxLoanDays() const = 0;
    virtual double computeFine(int daysLate) const = 0;
    virtual void showInfo() const {
        cout << "ID: " << id << " | Judul: " << title 
             << " | Status: " << (available ? "Tersedia" : "Dipinjam") << endl;
    }
};

// ======= CLASS BOOK =======
class Book : public Item {
    string author;
    bool isRare;
public:
    Book(string id, string title, string author, bool isRare)
        : Item(id, title), author(author), isRare(isRare) {}

    int maxLoanDays() const override { return isRare ? 3 : 14; }
    double computeFine(int daysLate) const override { return 1000 * daysLate; }

    void showInfo() const override {
        cout << "[Book] " << title << " by " << author 
             << " | Rare: " << (isRare ? "Yes" : "No") 
             << " | Status: " << (isAvailable() ? "Tersedia" : "Dipinjam") << endl;
    }
};

// ======= CLASS MAGAZINE =======
class Magazine : public Item {
public:
    Magazine(string id, string title) : Item(id, title) {}
    int maxLoanDays() const override { return 7; }
    double computeFine(int daysLate) const override { return 500 * daysLate; }

    void showInfo() const override {
        cout << "[Magazine] " << title 
             << " | Status: " << (isAvailable() ? "Tersedia" : "Dipinjam") << endl;
    }
};

// ======= CLASS DVD =======
class DVD : public Item {
public:
    DVD(string id, string title) : Item(id, title) {}
    int maxLoanDays() const override { return 3; }

    // Overload computeFine()
    double computeFine(int daysLate) const override { return 2000 * daysLate; }
    double computeFine(int daysLate, bool isDamaged) const { 
        double fine = 2000 * daysLate;
        if (isDamaged) fine += 10000; // biaya tambahan
        return fine;
    }

    void showInfo() const override {
        cout << "[DVD] " << title 
             << " | Status: " << (isAvailable() ? "Tersedia" : "Dipinjam") << endl;
    }
};

// ======= CLASS MEMBER =======
class Member {
    string id, name;
    int level;
    int totalLoans;
    int freeCredits;

public:
    Member(string id, string name)
        : id(id), name(name), level(0), totalLoans(0), freeCredits(0) {}

    void borrow(Item &it, int daysRequested) {
        if (!it.isAvailable()) {
            cout << "Item sedang dipinjam orang lain.\n";
            return;
        }

        if (daysRequested > it.maxLoanDays()) {
            cout << "Durasi melebihi batas maksimal (" << it.maxLoanDays() << " hari).\n";
            return;
        }

        it.setAvailable(false);
        totalLoans++;
        cout << name << " berhasil meminjam " << it.getTitle() << " selama " 
             << daysRequested << " hari.\n";
        updateLevel();
    }

    void returnItem(Item &it, int actualDays, bool isDamaged = false) {
        it.setAvailable(true);
        int daysLate = actualDays - it.maxLoanDays();
        if (daysLate > 0) {
            double fine = 0;
            if (DVD *dvd = dynamic_cast<DVD*>(&it)) {
                fine = dvd->computeFine(daysLate, isDamaged);
            } else {
                fine = it.computeFine(daysLate);
            }
            cout << "Terlambat " << daysLate << " hari. Denda: Rp" << fine << endl;
        } else {
            cout << "Dikembalikan tepat waktu.\n";
        }
    }

    void updateLevel() {
        if (totalLoans >= 10) { level = 2; freeCredits = 3; }
        else if (totalLoans >= 5) { level = 1; freeCredits = 1; }
        else { level = 0; freeCredits = 0; }
    }

    void showInfo() const {
        cout << "Member: " << name << " | Level: " << level 
             << " | Credits: " << freeCredits 
             << " | Total Pinjam: " << totalLoans << endl;
    }
};

// ======= MAIN MENU =======
int main() {
    vector<Item*> catalogue;
    vector<Member*> members;

    int choice;
    do {
        cout << "\n=== SISTEM PERPUSTAKAAN INTERAKTIF ===\n";
        cout << "1. Tambah Item\n";
        cout << "2. Lihat Katalog\n";
        cout << "3. Daftarkan Member\n";
        cout << "4. Pinjam Item\n";
        cout << "5. Kembalikan Item\n";
        cout << "6. Lihat Data Member\n";
        cout << "0. Keluar\n";
        cout << "Pilih menu: ";
        cin >> choice;

        if (choice == 1) {
            int jenis;
            cout << "1. Buku  2. Majalah  3. DVD\nPilih tipe: ";
            cin >> jenis;
            string id, title, author;
            bool rare;
            cout << "Masukkan ID: "; cin >> id;
            cout << "Masukkan Judul: "; cin.ignore(); getline(cin, title);
            if (jenis == 1) {
                cout << "Masukkan Penulis: "; getline(cin, author);
                cout << "Apakah langka (1=ya, 0=tidak): "; cin >> rare;
                catalogue.push_back(new Book(id, title, author, rare));
            } else if (jenis == 2)
                catalogue.push_back(new Magazine(id, title));
            else
                catalogue.push_back(new DVD(id, title));

            cout << "Item berhasil ditambahkan!\n";
        }
        else if (choice == 2) {
            cout << "\n=== KATALOG ===\n";
            for (auto i : catalogue) i->showInfo();
        }
        else if (choice == 3) {
            string id, name;
            cout << "Masukkan ID member: "; cin >> id;
            cout << "Masukkan Nama: "; cin.ignore(); getline(cin, name);
            members.push_back(new Member(id, name));
            cout << "Member berhasil didaftarkan!\n";
        }
        else if (choice == 4) {
            string mid, iid; int hari;
            cout << "ID member: "; cin >> mid;
            cout << "ID item: "; cin >> iid;
            cout << "Berapa hari ingin meminjam: "; cin >> hari;

            Member *m = nullptr; Item *it = nullptr;
            for (auto x : members) if (x->getId() == mid) m = x;
            for (auto x : catalogue) if (x->getId() == iid) it = x;

            if (m && it) m->borrow(*it, hari);
            else cout << "Member atau item tidak ditemukan.\n";
        }
        else if (choice == 5) {
            string iid; int hari; bool rusak;
            cout << "ID item: "; cin >> iid;
            cout << "Berapa hari dipinjam: "; cin >> hari;
            cout << "Apakah rusak (1=ya, 0=tidak): "; cin >> rusak;

            Item *it = nullptr;
            for (auto x : catalogue) if (x->getId() == iid) it = x;
            if (it) {
                // asumsi member bebas
                Member temp("M0", "Guest");
                temp.returnItem(*it, hari, rusak);
            } else cout << "Item tidak ditemukan.\n";
        }
        else if (choice == 6) {
            cout << "\n=== DATA MEMBER ===\n";
            for (auto m : members) m->showInfo();
        }

    } while (choice != 0);

    cout << "Program selesai. Terima kasih!\n";
    return 0;
}
