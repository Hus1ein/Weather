#include <iostream>
#include <curl.h>
#include <fstream>
#include <string.h>
#include <iomanip>

using namespace std;
//Imenovanje i incijalizacija funkcija

CURLcode download(string url, string out);
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata);
void prognoza();
void unosGrada(string &naziv);

int main()
{
    //Unosenje zeljenog grada (ukljujuci gradove sa vise od 1 rijeci
    string grad;
    unosGrada(grad);

    //Postavljanje zeljenog grada na API;
    grad ="http://api.openweathermap.org/data/2.5/forecast/daily?q="+grad+"&mode=xml&units=metric&cnt=7&appid=0acbef229ccd1d00524aa4fd4cc361f5";

    //provjera da li su zadate operacije prosle bez greske
    if (CURLE_OK == download(grad,"Data.xml")){
        cout << "\n\nDownloaded successfully.\n\n" << endl <<"Content of a file:\n"<<endl;
        prognoza();
    } //u slcaju da se desio error neki
    else
        cout<<"Download failed!"<<endl;

    return 0;
}
//Da ne masamo u main funckij sav code
void unosGrada(string &naziv){
    cout<<"Unesite grad: ";
    getline(cin,naziv);
    while(naziv.find(' ')!=string::npos)
        naziv.erase(naziv.find(' '),1);
}
//Postavljanje zadatih operacija u curl
//I slanje upita serveru te primanje i DL odgovora,manje vise ocigledno u code
CURLcode download(string url, string out) {

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = NULL;
    fstream output(out.c_str(), ios::out | ios::binary);
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        cout<<"\nDownloading file....\n"<<endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());//postavljanje "adrese" servera
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//pozivanje/dodjeljivanje write funkcije
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);//upisivanje podataka
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);//izvjestavanje o output-u
        res = curl_easy_perform(curl);//izvrsi zadate operacije
        curl_easy_cleanup(curl);//prekini konekciju sa serverom
        return res;//vrati "flag" od operacija
    }
    else{
        return CURLE_FAILED_INIT; //u slucaju greske "red flag"
    }
}
//funkcija za pisanje podataka u file
size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata){

    fstream *out = static_cast<fstream *>(userdata);
    size_t nbytes = size * nmemb;
    out->write(ptr, nbytes);
    return nbytes;
}
//izolovanje potrebnih podataka iz file (data mining/search)
void prognoza(){

	string rijec;
	fstream x;
	x.open("Data.xml",ios::in);
	getline(x,rijec);
	if(rijec[1]=='?' && rijec[2]=='x')
        getline(x,rijec);
    x.close();

    cout<<" "<<setw(50)<<setfill('-')<<"\n";

    cout<<"| Sedmodnevna prognoza za grad: "<<
    rijec.substr(rijec.find("<name>")+6,rijec.find("</name>")-rijec.find("<name>")-6)<<
    ","<<rijec.substr(rijec.find("<country>")+9,2);
    cout<<setw(21-(rijec.find("</name>")-rijec.find("<name>"-6)))<<setfill(' ')<<"|"<<endl;

    cout<<" "<<setw(50)<<setfill('-')<<"\n";

    rijec=rijec.substr(rijec.find("<forecast>"+10));

    for(int i=0;i<7;i++){
        cout<<"| "<<setw(22)<<setfill(' ')<<"Date: "<<rijec.substr(rijec.find("<time day")+11,10)<<
        setw(17)<<setfill(' ')<<"|"<<endl;
        rijec=rijec.substr(rijec.find("<time day")+11);

        cout<<" "<<setw(50)<<setfill('-')<<"\n";

        cout<<"| "<<"Forecast: "<<rijec.substr(rijec.find("name=")+6,rijec.find(" var=")-7-rijec.find("name="));
        cout<<setw(39-(rijec.find(" var=")-7-rijec.find("name=")))<<setfill(' ')<<"|"<<endl;

        cout<<"| Temperature day: "<<rijec.substr(rijec.find("<temperature")+18,rijec.find(" min")-19-rijec.find("<temperature"));
        cout<<setw(32-(rijec.find("<temperature")+18,rijec.find(" min")-19-rijec.find("<temperature")))<<setfill(' ')<<"|"<<endl;

        cout<<"| Min: "<<rijec.substr(rijec.find(" min")+6,rijec.find(" max")-7-rijec.find(" min"));
        cout<<setw(44-(rijec.find(" min")+6,rijec.find(" max")-7-rijec.find(" min")))<<setfill(' ')<<"|"<<endl;

        cout<<"| Max: "<<rijec.substr(rijec.find(" max")+6,rijec.find(" night")-7-rijec.find(" max"));
        cout<<setw(44-(rijec.find(" max")+6,rijec.find(" night")-7-rijec.find(" max")))<<setfill(' ')<<"|"<<endl;

        rijec=rijec.substr(rijec.find("</time>")+7);

        cout<<" "<<setw(50)<<setfill('-')<<"\n";
    }
	//remove("Data.xml");
}
