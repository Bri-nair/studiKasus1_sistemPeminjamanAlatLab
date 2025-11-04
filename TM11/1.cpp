#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

// ===========================
// KELAS ITEM (BASE CLASS)
// ===========================
class Item {
protected:
    string id;
    string judul;
    bool tersedia;

public:
    // konstruktor yang lit
    Item(string id, string judul) : id(id), judul(judul), tersedia(true) {}
    
    virtual ~Item() = default;

    // getter setter biar encapsulation on point
    string getId() const { return id; }
    string getJudul() const { return judul; }
    bool isTersedia() const { return tersedia; }
    void setTersedia(bool status) { tersedia = status; }

    // method virtual pure biar anak2nya wajib override - no cap
    virtual int maxHariPinjam() const = 0;
    virtual double hitungDenda(int hariTerlambat) const = 0;
    virtual unique_ptr<Item> clone() const = 0;
    virtual string getTipe() const = 0;

    // operator overload buat sorting katalog - ez clap
    bool operator<(const Item& lain) const {
        return id < lain.id;
    }

    virtual void tampilInfo() const {
        cout << "ID: " << id << " | Judul: " << judul 
             << " | Status: " << (tersedia ? "Tersedia" : "Dipinjam") << endl;
    }
};

// ===========================
// KELAS BOOK (CHILD CLASS)
// ===========================
class Book : public Item {
private:
    string penulis;
    bool langka;

public:
    Book(string id, string judul, string penulis, bool langka = false)
        : Item(id, judul), penulis(penulis), langka(langka) {}

    // override method - buku langka beda treatment cuy
    int maxHariPinjam() const override {
        return langka ? 3 : 14;  // buku langka cuma 3 hari, biasa 14 hari
    }

    double hitungDenda(int hariTerlambat) const override {
        return 1000.0 * hariTerlambat;  // denda 1000 per hari - sabi
    }

    unique_ptr<Item> clone() const override {
        return make_unique<Book>(*this);
    }

    string getTipe() const override { return "Book"; }

    void tampilInfo() const override {
        Item::tampilInfo();
        cout << "  Penulis: " << penulis << " | Langka: " << (langka ? "Ya" : "Tidak") << endl;
    }
};

// ===========================
// KELAS MAGAZINE (CHILD CLASS)
// ===========================
class Magazine : public Item {
private:
    string edisi;

public:
    Magazine(string id, string judul, string edisi)
        : Item(id, judul), edisi(edisi) {}

    int maxHariPinjam() const override {
        return 7;  // majalah cuma seminggu - standar lah
    }

    double hitungDenda(int hariTerlambat) const override {
        return 500.0 * hariTerlambat;  // lebih murah dari buku
    }

    unique_ptr<Item> clone() const override {
        return make_unique<Magazine>(*this);
    }

    string getTipe() const override { return "Magazine"; }

    void tampilInfo() const override {
        Item::tampilInfo();
        cout << "  Edisi: " << edisi << endl;
    }
};

// ===========================
// KELAS DVD (CHILD CLASS)
// ===========================
class DVD : public Item {
private:
    string sutradara;

public:
    DVD(string id, string judul, string sutradara)
        : Item(id, judul), sutradara(sutradara) {}

    int maxHariPinjam() const override {
        return 3;  // DVD cepet banget - 3 hari doang
    }

    // method biasa
    double hitungDenda(int hariTerlambat) const override {
        return 2000.0 * hariTerlambat;  // paling mahal dendanya bossku
    }

    // OVERLOAD method - ini yang diminta soal, bisa tambah biaya rusak
    double hitungDenda(int hariTerlambat, bool rusak) const {
        double dendaDasar = hitungDenda(hariTerlambat);
        if (rusak) {
            return dendaDasar + 50000.0;  // rusak langsung +50k - sakit
        }
        return dendaDasar;
    }

    unique_ptr<Item> clone() const override {
        return make_unique<DVD>(*this);
    }

    string getTipe() const override { return "DVD"; }

    void tampilInfo() const override {
        Item::tampilInfo();
        cout << "  Sutradara: " << sutradara << endl;
    }
};

// ===========================
// KELAS MEMBER
// ===========================
class Member {
private:
    string id;
    string nama;
    int level;
    int kreditGratis;
    int totalPinjaman;
    int pinjamanSukses;

public:
    Member(string id, string nama) 
        : id(id), nama(nama), level(0), kreditGratis(0), 
          totalPinjaman(0), pinjamanSukses(0) {}

    // getter buat ngecek status member
    string getId() const { return id; }
    string getNama() const { return nama; }
    int getLevel() const { return level; }
    int getKreditGratis() const { return kreditGratis; }
    int getTotalPinjaman() const { return totalPinjaman; }

    // method borrow yang basic - no kredit dipakai
    bool pinjam(Item& item, int hariDiminta) {
        if (!item.isTersedia()) {
            cout << "❌ Waduh, item lagi dipinjam orang lain bro!" << endl;
            return false;
        }

        if (hariDiminta > item.maxHariPinjam()) {
            cout << "❌ Kebanyakan hari cuy! Max cuma " << item.maxHariPinjam() 
                 << " hari untuk item ini" << endl;
            return false;
        }

        if (hariDiminta <= 0) {
            cout << "❌ Hari pinjam harus lebih dari 0 lah!" << endl;
            return false;
        }

        item.setTersedia(false);
        totalPinjaman++;
        cout << "✅ Sip, berhasil pinjam! Jangan lupa balikin ya~" << endl;
        return true;
    }

    // OVERLOAD method borrow - bisa pake kredit gratis nih
    bool pinjam(Item& item, int hariDiminta, bool pakeKredit) {
        if (pakeKredit && kreditGratis > 0) {
            cout << "🎁 Pake kredit gratis nih! Sisa: " << (kreditGratis - 1) << endl;
            kreditGratis--;
            // tetep harus validasi dulu sebelum pake kredit
            if (!item.isTersedia()) {
                cout << "❌ Item lagi dipinjam, kredit balik deh" << endl;
                kreditGratis++;
                return false;
            }
            item.setTersedia(false);
            totalPinjaman++;
            return true;
        }
        return pinjam(item, hariDiminta);
    }

    // method buat balikin item + hitung denda kalo telat
    double balikinItem(Item& item, int hariAktual) {
        if (item.isTersedia()) {
            cout << "❌ Lah, item ini ga dipinjam kok!" << endl;
            return 0.0;
        }

        item.setTersedia(true);
        pinjamanSukses++;
        
        int hariTerlambat = hariAktual - item.maxHariPinjam();
        double denda = 0.0;

        if (hariTerlambat > 0) {
            denda = item.hitungDenda(hariTerlambat);
            cout << "⚠️  Telat " << hariTerlambat << " hari! Denda: Rp" 
                 << fixed << setprecision(0) << denda << endl;
        } else {
            cout << "✅ Tepat waktu! Good job bro 👍" << endl;
        }

        // cek upgrade level - ini logika level upnya
        cekUpgradeLevel();
        
        return denda;
    }

    // method buat balikin DVD dengan opsi rusak - overload hitungDenda dipake disini
    double balikinDVD(DVD& dvd, int hariAktual, bool rusak) {
        if (dvd.isTersedia()) {
            cout << "❌ DVD ini ga dipinjam!" << endl;
            return 0.0;
        }

        dvd.setTersedia(true);
        pinjamanSukses++;

        int hariTerlambat = hariAktual - dvd.maxHariPinjam();
        double denda = 0.0;

        if (hariTerlambat > 0 || rusak) {
            // pake overload method - yang ada parameter rusak
            denda = dvd.hitungDenda(max(0, hariTerlambat), rusak);
            cout << "⚠️  Denda total: Rp" << fixed << setprecision(0) << denda;
            if (rusak) cout << " (termasuk biaya rusak Rp50.000)";
            cout << endl;
        } else {
            cout << "✅ Mantap, DVD balik dalam kondisi aman!" << endl;
        }

        cekUpgradeLevel();
        return denda;
    }

private:
    // method internal buat naik level - system reward gitu
    void cekUpgradeLevel() {
        int levelLama = level;
        
        // Level 0 -> 1: butuh 5 pinjaman sukses
        if (level == 0 && pinjamanSukses >= 5) {
            level = 1;
            kreditGratis += 1;  // bonus 1 kredit
        }
        // Level 1 -> 2: butuh 10 pinjaman sukses total
        else if (level == 1 && pinjamanSukses >= 10) {
            level = 2;
            kreditGratis += 3;  // bonus 3 kredit - mantap
        }

        // kasih notif kalo naik level
        if (level > levelLama) {
            cout << "🎉 LEVEL UP! Sekarang level " << level 
                 << "! Dapet " << (level == 1 ? 1 : 3) 
                 << " kredit gratis!" << endl;
        }
    }

public:
    void tampilInfo() const {
        cout << "\n👤 Member Info:" << endl;
        cout << "ID: " << id << " | Nama: " << nama << endl;
        cout << "Level: " << level << " | Kredit Gratis: " << kreditGratis << endl;
        cout << "Total Pinjaman: " << totalPinjaman 
             << " | Sukses: " << pinjamanSukses << endl;
    }
};

// ===========================
// KELAS PERPUSTAKAAN (MAIN SYSTEM)
// ===========================
class Perpustakaan {
private:
    vector<unique_ptr<Item>> katalog;
    vector<unique_ptr<Member>> daftarMember;
    
    // statistik bulanan - buat laporan
    map<string, int> pinjamanPerTipe;
    double totalDendaBulanIni;
    int totalKreditTerpakai;

public:
    Perpustakaan() : totalDendaBulanIni(0.0), totalKreditTerpakai(0) {}

    // tambah item ke katalog
    void tambahItem(unique_ptr<Item> item) {
        if (cariItemById(item->getId()) != nullptr) {
            cout << "❌ ID udah ada cuy! Pake ID lain dong" << endl;
            return;
        }
        katalog.push_back(move(item));
        cout << "✅ Item berhasil ditambah ke katalog!" << endl;
    }

    // register member baru
    void registerMember(unique_ptr<Member> member) {
        if (cariMemberById(member->getId()) != nullptr) {
            cout << "❌ ID member udah kepake!" << endl;
            return;
        }
        daftarMember.push_back(move(member));
        cout << "✅ Member berhasil terdaftar!" << endl;
    }

    // cari item by ID
    Item* cariItemById(const string& id) {
        for (auto& item : katalog) {
            if (item->getId() == id) {
                return item.get();
            }
        }
        return nullptr;
    }

    // cari member by ID
    Member* cariMemberById(const string& id) {
        for (auto& member : daftarMember) {
            if (member->getId() == id) {
                return member.get();
            }
        }
        return nullptr;
    }

    // sorting katalog by ID - pake operator overload yang udah dibuat
    void sortKatalog() {
        sort(katalog.begin(), katalog.end(), 
            [](const unique_ptr<Item>& a, const unique_ptr<Item>& b) {
                return *a < *b;
            });
        cout << "✅ Katalog udah di-sort!" << endl;
    }

    // tampilkan semua katalog
    void tampilkanKatalog() const {
        cout << "\n📚 === KATALOG PERPUSTAKAAN ===" << endl;
        if (katalog.empty()) {
            cout << "Kosong bro, belum ada item" << endl;
            return;
        }
        for (const auto& item : katalog) {
            item->tampilInfo();
            cout << "---" << endl;
        }
    }

    // tampilkan semua member
    void tampilkanMember() const {
        cout << "\n👥 === DAFTAR MEMBER ===" << endl;
        if (daftarMember.empty()) {
            cout << "Belum ada member yang daftar nih" << endl;
            return;
        }
        for (const auto& member : daftarMember) {
            member->tampilInfo();
            cout << "---" << endl;
        }
    }

    // proses peminjaman
    void prosesPinjam(const string& idMember, const string& idItem, 
                      int hari, bool pakeKredit = false) {
        Member* member = cariMemberById(idMember);
        Item* item = cariItemById(idItem);

        if (!member) {
            cout << "❌ Member ga ketemu!" << endl;
            return;
        }
        if (!item) {
            cout << "❌ Item ga ada di katalog!" << endl;
            return;
        }

        bool sukses = member->pinjam(*item, hari, pakeKredit);
        if (sukses) {
            pinjamanPerTipe[item->getTipe()]++;
            if (pakeKredit) totalKreditTerpakai++;
        }
    }

    // proses pengembalian item biasa
    void prosesKembali(const string& idMember, const string& idItem, int hariAktual) {
        Member* member = cariMemberById(idMember);
        Item* item = cariItemById(idItem);

        if (!member || !item) {
            cout << "❌ Member atau item ga ketemu!" << endl;
            return;
        }

        double denda = member->balikinItem(*item, hariAktual);
        totalDendaBulanIni += denda;
    }

    // proses pengembalian DVD dengan cek rusak
    void prosesKembaliDVD(const string& idMember, const string& idDVD, 
                          int hariAktual, bool rusak) {
        Member* member = cariMemberById(idMember);
        Item* item = cariItemById(idDVD);

        if (!member || !item) {
            cout << "❌ Member atau DVD ga ketemu!" << endl;
            return;
        }

        DVD* dvd = dynamic_cast<DVD*>(item);
        if (!dvd) {
            cout << "❌ Item bukan DVD cuy!" << endl;
            return;
        }

        double denda = member->balikinDVD(*dvd, hariAktual, rusak);
        totalDendaBulanIni += denda;
    }

    // laporan bulanan - statistik lengkap
    void laporanBulanan() const {
        cout << "\n📊 === LAPORAN BULANAN PERPUSTAKAAN ===" << endl;
        cout << "\n📖 Peminjaman per Tipe:" << endl;
        if (pinjamanPerTipe.empty()) {
            cout << "  Belum ada peminjaman bulan ini" << endl;
        } else {
            for (const auto& pair : pinjamanPerTipe) {
                cout << "  " << pair.first << ": " << pair.second << " kali" << endl;
            }
        }
        
        cout << "\n💰 Total Denda Bulan Ini: Rp" 
             << fixed << setprecision(0) << totalDendaBulanIni << endl;
        cout << "🎁 Total Kredit Gratis Terpakai: " << totalKreditTerpakai << endl;
        cout << "\n📚 Total Item di Katalog: " << katalog.size() << endl;
        cout << "👥 Total Member Terdaftar: " << daftarMember.size() << endl;
    }

    // reset statistik bulanan - panggil tiap awal bulan
    void resetStatistikBulanan() {
        pinjamanPerTipe.clear();
        totalDendaBulanIni = 0.0;
        totalKreditTerpakai = 0;
        cout << "✅ Statistik bulanan di-reset!" << endl;
    }
};

// ===========================
// MENU INTERFACE
// ===========================
void tampilkanMenu() {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║   SISTEM PERPUSTAKAAN AMBIS 📚    ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "1. Tambah Item (Book/Magazine/DVD)" << endl;
    cout << "2. Register Member" << endl;
    cout << "3. Tampilkan Katalog" << endl;
    cout << "4. Tampilkan Member" << endl;
    cout << "5. Pinjam Item" << endl;
    cout << "6. Kembalikan Item" << endl;
    cout << "7. Kembalikan DVD (dengan cek rusak)" << endl;
    cout << "8. Sort Katalog" << endl;
    cout << "9. Laporan Bulanan" << endl;
    cout << "10. Reset Statistik Bulanan" << endl;
    cout << "0. Exit" << endl;
    cout << "Pilih menu: ";
}

// ===========================
// FUNGSI TEST CASES
// ===========================
void runTestCases(Perpustakaan& perpus) {
    cout << "\n🧪 === RUNNING TEST CASES ===" << endl;
    cout << "Test cases dari file testcases.txt\n" << endl;

    // Test 1: Tambah 3 item berbeda
    cout << "\n[TEST 1] Tambah 3 item berbeda" << endl;
    perpus.tambahItem(make_unique<Book>("B001", "Laskar Pelangi", "Andrea Hirata", false));
    perpus.tambahItem(make_unique<Magazine>("M001", "National Geographic", "Edisi Januari 2025"));
    perpus.tambahItem(make_unique<DVD>("D001", "Pengabdi Setan 2", "Joko Anwar"));
    perpus.sortKatalog();

    // Test 2: Register member dan pinjam
    cout << "\n[TEST 2] Register member dan pinjam Book" << endl;
    perpus.registerMember(make_unique<Member>("MEM001", "Budi Santoso"));
    perpus.prosesPinjam("MEM001", "B001", 10, false);

    // Test 3: Pinjam DVD dan kembalikan telat dengan rusak
    cout << "\n[TEST 3] Pinjam DVD dan balikin telat + rusak" << endl;
    perpus.prosesPinjam("MEM001", "D001", 3, false);
    perpus.prosesKembaliDVD("MEM001", "D001", 5, true);  // telat 2 hari + rusak

    // Test 4: Tambah member baru, pinjam sampe level up
    cout << "\n[TEST 4] Member baru, pinjam sampai level up" << endl;
    perpus.registerMember(make_unique<Member>("MEM002", "Siti Nurhaliza"));
    perpus.tambahItem(make_unique<Book>("B002", "Harry Potter", "JK Rowling", false));
    perpus.tambahItem(make_unique<Magazine>("M002", "Tempo", "Edisi Februari"));
    perpus.tambahItem(make_unique<Book>("B003", "Ronggeng Dukuh Paruk", "Ahmad Tohari", true));
    
    // Pinjam 5 kali buat naik ke level 1
    for (int i = 0; i < 5; i++) {
        perpus.prosesPinjam("MEM002", "M002", 5, false);
        perpus.prosesKembali("MEM002", "M002", 5);
    }

    // Test 5: Pake kredit gratis
    cout << "\n[TEST 5] Pake kredit gratis hasil level up" << endl;
    perpus.prosesPinjam("MEM002", "B002", 10, true);  // pake kredit
    perpus.prosesKembali("MEM002", "B002", 10);

    // Test 6: Coba pinjam buku langka lebih dari max hari
    cout << "\n[TEST 6] Coba pinjam buku langka melebihi max hari" << endl;
    perpus.prosesPinjam("MEM001", "B003", 10, false);  // buku langka max 3 hari

    // Test 7: Kembalikan tepat waktu
    cout << "\n[TEST 7] Pinjam dan kembalikan tepat waktu" << endl;
    perpus.prosesPinjam("MEM001", "B003", 3, false);
    perpus.prosesKembali("MEM001", "B003", 3);

    // Test 8: Generate laporan bulanan
    cout << "\n[TEST 8] Generate laporan bulanan" << endl;
    perpus.laporanBulanan();

    cout << "\n✅ Semua test cases selesai!\n" << endl;
}

// ===========================
// MAIN FUNCTION
// ===========================
int main() {
    Perpustakaan perpus;
    int pilihan;

    // Auto-run test cases
    cout << "Mau langsung run test cases? (1=Ya, 0=Tidak): ";
    int runTest;
    cin >> runTest;
    cin.ignore();

    if (runTest == 1) {
        runTestCases(perpus);
    }

    // Main menu loop
    while (true) {
        tampilkanMenu();
        cin >> pilihan;
        cin.ignore();

        if (pilihan == 0) {
            cout << "Makasih udah pake sistem perpustakaan! Bye~" << endl;
            break;
        }

        switch (pilihan) {
            case 1: {  // Tambah item
                cout << "Tipe item (1=Book, 2=Magazine, 3=DVD): ";
                int tipe;
                cin >> tipe;
                cin.ignore();

                string id, judul;
                cout << "ID: "; getline(cin, id);
                cout << "Judul: "; getline(cin, judul);

                if (tipe == 1) {
                    string penulis;
                    bool langka;
                    cout << "Penulis: "; getline(cin, penulis);
                    cout << "Langka? (1=Ya, 0=Tidak): "; cin >> langka;
                    perpus.tambahItem(make_unique<Book>(id, judul, penulis, langka));
                } else if (tipe == 2) {
                    string edisi;
                    cout << "Edisi: "; getline(cin, edisi);
                    perpus.tambahItem(make_unique<Magazine>(id, judul, edisi));
                } else if (tipe == 3) {
                    string sutradara;
                    cout << "Sutradara: "; getline(cin, sutradara);
                    perpus.tambahItem(make_unique<DVD>(id, judul, sutradara));
                }
                break;
            }
            case 2: {  // Register member
                string id, nama;
                cout << "ID Member: "; getline(cin, id);
                cout << "Nama: "; getline(cin, nama);
                perpus.registerMember(make_unique<Member>(id, nama));
                break;
            }
            case 3:  // Tampilkan katalog
                perpus.tampilkanKatalog();
                break;
            case 4:  // Tampilkan member
                perpus.tampilkanMember();
                break;
            case 5: {  // Pinjam item
                string idMember, idItem;
                int hari, pakeKredit;
                cout << "ID Member: "; getline(cin, idMember);
                cout << "ID Item: "; getline(cin, idItem);
                cout << "Berapa hari: "; cin >> hari;
                cout << "Pake kredit gratis? (1=Ya, 0=Tidak): "; cin >> pakeKredit;
                perpus.prosesPinjam(idMember, idItem, hari, pakeKredit == 1);
                break;
            }
            case 6: {  // Kembalikan item
                string idMember, idItem;
                int hariAktual;
                cout << "ID Member: "; getline(cin, idMember);
                cout << "ID Item: "; getline(cin, idItem);
                cout << "Berapa hari actual pinjam: "; cin >> hariAktual;
                perpus.prosesKembali(idMember, idItem, hariAktual);
                break;
            }
            case 7: {  // Kembalikan DVD dengan cek rusak
                string idMember, idDVD;
                int hariAktual, rusak;
                cout << "ID Member: "; getline(cin, idMember);
                cout << "ID DVD: "; getline(cin, idDVD);
                cout << "Berapa hari actual: "; cin >> hariAktual;
                cout << "Rusak? (1=Ya, 0=Tidak): "; cin >> rusak;
                perpus.prosesKembaliDVD(idMember, idDVD, hariAktual, rusak == 1);
                break;
            }
            case 8:  // Sort katalog
                perpus.sortKatalog();
                break;
            case 9:  // Laporan bulanan
                perpus.laporanBulanan();
                break;
            case 10:  // Reset statistik
                perpus.resetStatistikBulanan();
                break;
            default:
                cout << "Pilihan ga valid bro!" << endl;
        }
    }

    return 0;
}
