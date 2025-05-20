#include <iostream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <regex>
#include <vector>
#include <sstream>
#include <termios.h>
#include <unistd.h>

using namespace std;

#pragma region // variabel utama

struct Kendaraan
{
    string plat;
    string jenis;
    time_t waktu;
};

string FILE_PARKIR = "data_parkir.txt";
string FILE_MEMBER = "data_member.txt";

#pragma endregion

#pragma region // fungsi pembantu

void lanjut()
{
    cout << endl
         << "Press Enter untuk melanjutkan...";
    cin.get();
    system("clear");
}

void make_lower(string &input)
{
    transform(input.begin(), input.end(), input.begin(), ::tolower);
}

void make_upper(string &input)
{
    transform(input.begin(), input.end(), input.begin(), ::toupper);
}

string lower(string input)
{
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

string upper(string input)
{
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

void garis(int length = 60, char symbol = '-')
{
    cout << setfill(symbol) << setw(length) << "" << setfill(' ') << endl;
}

string center(const string &s, int width = 58)
{
    int len = s.length();
    if (len >= width)
        return s;
    int diff = width - len;
    int pad1 = diff / 2;
    int pad2 = diff - pad1;
    return string(pad1, ' ') + s + string(pad2, ' ');
}

void header_tabel(const string &title, int width = 60)
{
    system("clear");
    cout << endl;
    garis(width, '=');
    cout << "|" << center(title, width - 2) << "|" << endl;
    garis(width, '=');
}

void baris_tabel(const string &label, const string &value, int width = 60)
{
    cout << "| " << setw(20) << left << label << ": " << setw(width - 25) << left << value << "|" << endl;
}

bool login(bool &admin)
{
    string admin_username = "admin";
    string admin_password = "admin";
    char pil;

    while (true)
    {
        system("clear");
        header_tabel("LOGIN SISTEM PARKIR WOX-MALL");
        cout << "| 1. Admin" << setw(50) << right << "|" << endl;
        cout << "| 2. Staff" << setw(50) << right << "|" << endl;
        garis(60, '=');
        cout << "| Login sebagai (1/2): ";
        cin >> pil;
        cin.ignore();

        if (pil == '1')
        {

            string username, password;

            cout << "| Username: ";
            getline(cin, username);

            cout << "| Password: ";
            getline(cin, password);

            if (username == admin_username && password == admin_password)
            {
                admin = true;
                cout << "|" << center("Berhasil login sebagai admin.") << "|" << endl;
                garis();
                sleep(1);
                return true;
            }
            else
            {
                cout << "|" << center("Username atau password salah.") << "|" << endl;
                garis();
                lanjut();
            }
        }
        else if (pil == '2')
        {
            admin = false;
            cout << "|" << center("Berhasil login sebagai staff.") << "|" << endl;
            garis();
            sleep(1);
            return true;
        }
        else
        {
            cout << "|" << center("Pilihan yang anda masukkan tidak valid.") << "|" << endl;
            garis();
            lanjut();
        }
    }
}

bool validasi_plat(string plat)
{
    regex regex_plat(R"(^\s*[a-zA-Z]{1,2}\s+\d{1,4}\s*[a-zA-Z]{0,3}\s*$)");
    return regex_match(plat, regex_plat);
}

int hitung_waktu_parkir(time_t awal, time_t akhir)
{
    double total_detik = difftime(akhir, awal);
    double total_jam = total_detik / 3600.0;
    return (total_jam <= 0) ? 0 : static_cast<int>(ceil(total_jam));
}

time_t waktu_sekarang()
{
    return time(nullptr);
}

string format_waktu(time_t time)
{
    tm *local_time = localtime(&time);
    ostringstream oss;
    oss << put_time(local_time, "%H:%M:%S, %d-%m-%Y");
    return oss.str();
}

bool cek_kendaraan_parkir(const string &plat)
{
    ifstream file(FILE_PARKIR);
    string line;
    string plat_upper = upper(plat);

    while (getline(file, line))
    {
        istringstream iss(line);
        string stored_plat, jenis, waktu_str;
        if (getline(iss, stored_plat, '|') &&
            getline(iss, jenis, '|') &&
            getline(iss, waktu_str))
        {
            if (stored_plat == plat_upper)
            {
                file.close();
                return true;
            }
        }
    }
    file.close();
    return false;
}

vector<Kendaraan> baca_data_parkir()
{
    vector<Kendaraan> kendaraan_list;
    ifstream file(FILE_PARKIR);
    string line;

    while (getline(file, line))
    {
        istringstream iss(line);
        string plat, jenis, waktu_str;
        if (getline(iss, plat, '|') &&
            getline(iss, jenis, '|') &&
            getline(iss, waktu_str))
        {
            time_t waktu;
            istringstream(waktu_str) >> waktu;
            kendaraan_list.push_back({plat, jenis, waktu});
        }
    }
    file.close();
    return kendaraan_list;
}

void simpan_data_parkir(const Kendaraan &kendaraan)
{
    ofstream file(FILE_PARKIR, ios::app);
    file << kendaraan.plat << "|" << kendaraan.jenis << "|" << kendaraan.waktu << endl;
    file.close();
}

void hapus_data_parkir(const string &plat)
{
    vector<Kendaraan> kendaraan_list = baca_data_parkir();
    ofstream file(FILE_PARKIR);

    for (const auto &kendaraan : kendaraan_list)
    {
        if (kendaraan.plat != upper(plat))
        {
            file << kendaraan.plat << "|" << kendaraan.jenis << "|" << kendaraan.waktu << endl;
        }
    }
    file.close();
}

void hapus_semua_data_parkir()
{
    ofstream file(FILE_PARKIR, ios::trunc);
    file.close();
}

int hitung_harga_parkir(const string &jenis, time_t waktu_parkir, bool member)
{
    int total_jam = hitung_waktu_parkir(waktu_parkir, waktu_sekarang());
    int harga_pertama = (lower(jenis) == "mobil") ? 5000 : 4000;
    int total_harga = harga_pertama + (harga_pertama * (total_jam - 1));
    return total_harga;
}
#pragma endregion

#pragma region // fungsi menu

void menu_catat_kendaraan_parkir()
{
    cin.ignore();
    header_tabel("CATAT KENDARAAN PARKIR");

    string plat, jenis;
    cout << "| Masukkan plat nomor [q untuk quit]: ";
    getline(cin, plat);

    if (lower(plat) == "q")
        return;

    while (!validasi_plat(plat))
    {
        cout << "| " << setw(58) << left << "Plat nomor tidak valid." << "|" << endl;
        cout << "| Masukkan plat nomor: ";
        getline(cin, plat);
        if (lower(plat) == "q")
            return;
    }

    if (cek_kendaraan_parkir(plat))
    {
        cout << "| " << setw(58) << left << ("Kendaraan " + plat + " sudah terparkir.") << "|" << endl;
        garis();
        lanjut();
        return;
    }

    cout << "| Masukkan jenis kendaraan (mobil/motor) [q untuk quit]: ";
    getline(cin, jenis);

    if (lower(jenis) == "q")
        return;

    make_upper(plat);

    while (lower(jenis) != "mobil" && lower(jenis) != "motor")
    {
        cout << "| " << setw(58) << left << "Jenis kendaraan tidak valid." << "|" << endl;
        cout << "| Masukkan jenis kendaraan (mobil/motor): ";
        getline(cin, jenis);
        if (lower(jenis) == "q")
            return;
    }

    time_t waktu = waktu_sekarang();
    simpan_data_parkir({plat, lower(jenis) == "mobil" ? "Mobil" : "Motor", waktu});

    system("clear");
    header_tabel("CATAT KENDARAAN PARKIR");
    string jenis_kapital = lower(jenis) == "mobil" ? "Mobil" : "Motor";
    cout << "| " << setw(57) << left << ("Berhasil mencatat kendaraan (" + jenis_kapital + "): " + plat) << "|" << endl;
    cout << "| " << setw(57) << left << ("Waktu masuk: " + format_waktu(waktu)) << "|" << endl;
    garis();
    lanjut();
}

void menu_bayar_kendaraan_parkir()
{
    cin.ignore();
    header_tabel("BAYAR KENDARAAN PARKIR");

    string plat;
    bool member = false;
    cout << "| Masukkan plat nomor [q untuk quit]: ";
    getline(cin, plat);

    if (lower(plat) == "q")
        return;

    while (!validasi_plat(plat))
    {
        cout << "| " << setw(58) << left << "Plat nomor tidak valid." << "|" << endl;
        cout << "| Masukkan plat nomor [q untuk quit]: ";
        getline(cin, plat);
        if (lower(plat) == "q")
            return;
    }

    vector<Kendaraan> kendaraan_list = baca_data_parkir();
    bool found = false;

    for (const auto &kendaraan : kendaraan_list)
    {
        if (kendaraan.plat == upper(plat))
        {
            found = true;

            ifstream file(FILE_MEMBER);
            string line;
            while (getline(file, line))
            {
                if (upper(line) == upper(plat))
                {
                    member = true;
                    break;
                }
            }
            file.close();

            system("clear");
            header_tabel("STRUK PEMBAYARAN PARKIR");

            int harga_parkir = hitung_harga_parkir(kendaraan.jenis, kendaraan.waktu, member);

            int diskon;
            if (member)
            {
                diskon = harga_parkir * 0.2;
            }

            baris_tabel("Plat Nomor", kendaraan.plat);
            baris_tabel("Jenis Kendaraan", kendaraan.jenis);
            baris_tabel("Waktu Masuk", format_waktu(kendaraan.waktu));
            baris_tabel("Waktu Keluar", format_waktu(waktu_sekarang()));
            baris_tabel("Status Member", member ? "Ya" : "Tidak");
            baris_tabel("Harga Parkir", "Rp " + to_string(harga_parkir));
            if (member)
            {
                baris_tabel("Diskon Member", "Rp " + to_string(diskon));
                harga_parkir -= harga_parkir * 0.2; // member diskon 20%
            }
            garis();
            cout << "| " << setw(20) << left << "Total Harga" << ": Rp " << setw(32) << left << harga_parkir << "|" << endl;
            garis();

            string input;
            cout << "| Masukkan jumlah uang (cash) [q untuk quit]: Rp ";
            cin >> input;

            if (lower(input) == "q")
                return;

            int uang_pelanggan;
            stringstream ss(input);
            ss >> uang_pelanggan;

            if (ss.fail())
            {
                cout << "| Input tidak valid." << endl;
                return;
            }

            while (uang_pelanggan < harga_parkir)
            {
                cout << "| " << setw(58) << left << "Uang tidak cukup." << "|" << endl;
                cout << "| Masukkan jumlah uang (cash): Rp ";
                cin >> input;
                if (lower(input) == "q")
                    return;

                int uang_pelanggan;
                stringstream ss(input);
                ss >> uang_pelanggan;

                if (ss.fail())
                {
                    cout << "| Input tidak valid." << endl;
                    return;
                }

                if (uang_pelanggan > harga_parkir)
                {
                    cout << "| " << setw(20) << left << "Kembalian" << ": Rp " << setw(32) << left << (uang_pelanggan - harga_parkir) << "|" << endl;
                }

                hapus_data_parkir(plat);
                garis();
                cout << "|" << center("TERIMA KASIH") << "|" << endl;
                cout << "|" << center("Semoga pelayanan kami memuaskan.") << "|" << endl;
                garis();
                lanjut();
                return;
            }
        }
    }
    if (!found)
    {
        cout << "| " << setw(57) << left << ("Kendaraan " + upper(plat) + " tidak ditemukan.") << "|" << endl;
        garis();
        lanjut();
    }
}

void menu_cari_kendaraan_parkir()
{
    cin.ignore();
    header_tabel("CARI KENDARAAN PARKIR");

    vector<Kendaraan> kendaraan_list = baca_data_parkir();

    if (kendaraan_list.empty())
    {
        cout << "| " << setw(58) << left << "Tidak ada kendaraan yang terparkir" << "|" << endl;
        garis();
        lanjut();
        return;
    }

    string plat;
    cout << "| Masukkan plat nomor [q untuk quit]: ";
    getline(cin, plat);

    if (lower(plat) == "q")
        return;

    while (!validasi_plat(plat))
    {
        cout << "| " << setw(58) << left << "Plat nomor tidak valid." << "|" << endl;
        cout << "| Masukkan plat nomor [q untuk quit]: ";
        getline(cin, plat);
        if (lower(plat) == "q")
            return;
    }

    for (const auto &kendaraan : kendaraan_list)
    {
        if (kendaraan.plat == upper(plat))
        {
            header_tabel("INFO KENDARAAN");
            baris_tabel("Plat Nomor", kendaraan.plat);
            baris_tabel("Jenis Kendaraan", kendaraan.jenis);
            baris_tabel("Terparkir Sejak", format_waktu(kendaraan.waktu));
            garis();
            lanjut();
            return;
        }
    }

    cout << "| " << setw(58) << left << "Kendaraan tidak ditemukan" << "|" << endl;
    garis();
    lanjut();
}

void menu_tampilkan_daftar_parkir()
{
    cin.ignore();
    header_tabel("DAFTAR KENDARAAN PARKIR");

    vector<Kendaraan> kendaraan_list = baca_data_parkir();

    if (kendaraan_list.empty())
    {
        cout << "| " << setw(58) << left << "Tidak ada kendaraan yang terparkir" << "|" << endl;
        garis();
        lanjut();
        return;
    }

    for (size_t i = 0; i < kendaraan_list.size(); i++)
    {
        const auto &kendaraan = kendaraan_list[i];
        cout << "| " << i + 1 << ". " << setw(12) << left << kendaraan.plat << " | " << setw(10) << left << kendaraan.jenis << " | " << setw(26) << left << format_waktu(kendaraan.waktu) << "|" << endl;
    }
    garis();
    lanjut();
}

void menu_daftar_member_parkir()
{
    cin.ignore();
    header_tabel("DAFTAR MEMBER PARKIR");

    string plat;
    cout << "| Masukkan plat nomor [q untuk quit]: ";
    getline(cin, plat);

    if (lower(plat) == "q")
        return;

    while (!validasi_plat(plat))
    {
        cout << "| " << setw(57) << left << "Plat nomor tidak valid." << "|" << endl;
        cout << "| Masukkan plat nomor [q untuk quit]: ";
        getline(cin, plat);
        if (lower(plat) == "q")
            return;
    }

    string plat_upper = upper(plat);
    ifstream file_in(FILE_MEMBER);
    string line;

    while (getline(file_in, line))
    {
        if (lower(line) == lower(plat))
        {
            cout << "| " << setw(58) << left << ("Kendaraan " + plat_upper + " sudah terdaftar sebagai member") << "|" << endl;
            garis();
            file_in.close();
            lanjut();
            return;
        }
    }
    file_in.close();

    ofstream file_out(FILE_MEMBER, ios::app);
    file_out << plat_upper << endl;
    file_out.close();

    cout << "| " << setw(58) << left << ("Berhasil mendaftarkan kendaraan " + plat_upper + " sebagai member") << "|" << endl;
    garis();
    lanjut();
}

void menu_tampilkan_member_parkir()
{
    cin.ignore();
    header_tabel("DAFTAR MEMBER PARKIR");
    ifstream file_in(FILE_MEMBER);
    string line;
    int no = 1;

    while (getline(file_in, line))
    {
        cout << "| " << no << ". " << left << setw(54) << line << "|" << endl;
        no++;
    }
    file_in.close();
    garis();
    lanjut();
}

void menu_hapus_daftar_parkir()
{
    cin.ignore();
    header_tabel("HAPUS DAFTAR PARKIR");

    vector<Kendaraan> kendaraan_list = baca_data_parkir();

    if (kendaraan_list.empty())
    {
        cout << "| " << setw(58) << left << "Tidak ada kendaraan yang terparkir" << "|" << endl;
        garis();
        lanjut();
        return;
    }

    char pil;
    cout << "| Apakah anda yakin ingin menghapus " << kendaraan_list.size()
         << " daftar parkir? (y/n): ";
    cin >> pil;

    while (true)
    {
        if (tolower(pil) == 'y')
        {
            hapus_semua_data_parkir();
            cout << "| " << setw(58) << left << "Berhasil menghapus semua daftar parkir" << "|" << endl;
            garis();
            break;
        }
        else if (tolower(pil) == 'n')
        {
            cout << "| " << setw(58) << left << "Penghapusan daftar parkir dibatalkan" << "|" << endl;
            garis();
            break;
        }
        else
        {
            cout << "| " << setw(58) << left << "Pilihan tidak valid." << "|" << endl;
            cout << "| Apakah anda yakin ingin menghapus semua daftar parkir? (y/n): ";
            cin >> pil;
        }
    }
    lanjut();
}

#pragma endregion

int main()
{
    bool admin = false;

    while (!login(admin))
    {
    }

    char pil;
    while (true)
    {
        system("clear");
        garis(60, '=');
        cout << "|" << center("SISTEM PARKIR WOX-MALL") << "|" << endl;
        garis(60, '=');
        cout << "| Login sebagai: " << (admin ? "admin" : "staff") << setw(38) << right << "|" << endl;
        garis(60, '=');
        cout << "| 1. Catat Kendaraan Parkir" << setw(33) << right << "|" << endl;
        cout << "| 2. Bayar Kendaraan Parkir" << setw(33) << right << "|" << endl;
        cout << "| 3. Cari Kendaraan Parkir" << setw(34) << right << "|" << endl;
        cout << "| 4. Tampilkan Daftar Parkir" << setw(32) << right << "|" << endl;
        cout << "| 5. Daftar Member Parkir" << setw(35) << right << "|" << endl;
        cout << "| 6. Tampilkan Member Parkir" << setw(32) << right << "|" << endl;

        if (admin)
        {
            cout << "| 7. Hapus Daftar Parkir" << setw(36) << right << "|" << endl;
        }

        cout << "| 0. Keluar" << setw(49) << right << "|" << endl;
        garis(60, '=');
        cout << "| Masukkan pilihan anda: ";
        cin >> pil;

        switch (pil)
        {
        case '1':
            menu_catat_kendaraan_parkir();
            break;
        case '2':
            menu_bayar_kendaraan_parkir();
            break;
        case '3':
            menu_cari_kendaraan_parkir();
            break;
        case '4':
            menu_tampilkan_daftar_parkir();
            break;
        case '5':
            menu_daftar_member_parkir();
            break;
        case '6':
            menu_tampilkan_member_parkir();
            break;
        case '7':
            if (admin)
            {
                menu_hapus_daftar_parkir();
            }
            else
            {
                cout << "| " << setw(58) << left << "Akses ditolak! Hanya untuk admin." << "|" << endl;
                garis(60);
                lanjut();
            }
            break;
        case '0':
            system("clear");
            garis(60, '=');
            cout << "|" << center("TERIMA KASIH") << "|" << endl;
            cout << "|" << center("Telah menggunakan sistem parkir WOX-MALL.") << "|" << endl;
            garis(60, '=');
            exit(0);
        default:
            cout << "| " << setw(58) << left << "Pilihan yang anda masukkan tidak valid." << "|" << endl;
            garis(60);
            lanjut();
        }
    }
    return 0;
}