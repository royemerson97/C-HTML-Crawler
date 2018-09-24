#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <stack>

using namespace std;
stack<string> PrimaryStack;
stack<string> AuxStack;

void extractWord(string word)
{
    while (!PrimaryStack.empty())
    {
        if (PrimaryStack.top() != word)
        {
            AuxStack.push(PrimaryStack.top());
        }
        PrimaryStack.pop();
    }

    while (!AuxStack.empty())
    {
        PrimaryStack.push(AuxStack.top());
        AuxStack.pop();
    }
}

int main(int argc, char const *argv[])
{
    string linea, termino;
    string file, extension, outputfile, filter;
    //My variables
    int wordsCounter = 1;
    bool tagHasChanged = false;
    bool isAParsedFile = false;
    bool isJavaScriptCode = false;
    int JSLinesCounter = 1;
    string currentTag = "";
    //end of my variables
    ifstream leer;
    ofstream escribir;
    DIR *dir;
    struct dirent *ent;
    dir = opendir(".");
    ent = readdir(dir);
    while ((ent != NULL))
    {
        if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0))
        {
            file = ent->d_name;
            size_t x = file.find(".");
            extension = file.substr(x);
            isAParsedFile = (file.substr(0, 7) == "Parsed_");
            if (extension == ".txt")
            {
                if (!isAParsedFile)
                {
                    leer.open(file.c_str());
                    outputfile = "Parsed_" + file;
                    escribir.open(outputfile.c_str());
                    while (getline(leer, linea))
                    {
                        stringstream renglon(linea);

                        if (!isJavaScriptCode)
                        {
                            while (getline(renglon, termino, ' '))
                            {
                                if (termino == "<html>")
                                {
                                    escribir << "Comienzo de documento" << '\n';
                                    tagHasChanged = true;
                                    PrimaryStack.push("documento");
                                }
                                else if (termino == "<head>")
                                {
                                    escribir << "Comienzo de cabeceras" << '\n';
                                    tagHasChanged = true;
                                    PrimaryStack.push("cabecera");
                                }
                                else if (termino == "<title>")
                                {
                                    escribir << "Comienzo de Título" << '\n';
                                    tagHasChanged = true;
                                    PrimaryStack.push("titulo");
                                }
                                else if (termino == "<body>")
                                {
                                    escribir << "Comienzo de cuerpo" << '\n';
                                    tagHasChanged = true;
                                    PrimaryStack.push("cuerpo");
                                }
                                else if (termino == "</html>")
                                {
                                    escribir << "Fin de documento" << '\n';
                                    tagHasChanged = true;
                                    extractWord("documento");
                                }
                                else if (termino == "</head>")
                                {
                                    escribir << "Fin de cabeceras" << '\n';
                                    tagHasChanged = true;
                                    extractWord("cabecera");
                                }
                                else if (termino == "</title>")
                                {
                                    escribir << "Fin de Título" << '\n';
                                    tagHasChanged = true;
                                    extractWord("titulo");
                                }
                                else if (termino == "</body>")
                                {
                                    escribir << "Fin de cuerpo" << '\n';
                                    tagHasChanged = true;
                                    extractWord("cuerpo");
                                }
                                else if (termino == "<script>")
                                {
                                    escribir << "Comienzo de código en JS\n";
                                    tagHasChanged = isJavaScriptCode = true;
                                    PrimaryStack.push("script");
                                }
                                else
                                {
                                    escribir << "Palabra # " << wordsCounter++ << " de la sección " << PrimaryStack.top() << " : " << termino << '\n';
                                    tagHasChanged = false;
                                }
                                if (tagHasChanged)
                                {
                                    wordsCounter = 1;
                                }
                            }
                        }
                        else
                        {
                            if (linea == "</script>")
                            {
                                isJavaScriptCode = false;
                                JSLinesCounter += 1;
                                extractWord("script");
                                escribir << "Final de código JS\n";
                            }
                            else
                            {
                                escribir << "Linea de código en JS # " << JSLinesCounter++ << " " << linea <<"\n";
                            }
                        }
                    }
                    escribir << "----------------------\n";
                    if (PrimaryStack.empty())
                    {
                        escribir << "El formato del documento estaba correcto";
                    }
                    else{
                        escribir <<"Checar etiquetas :( formato incompleto";
                    }
                }
            }
            leer.close();
            escribir.close();
        }
        ent = readdir(dir);
    }
    printf("\nSe hace finalizado el programa\n");
    return 0;
}