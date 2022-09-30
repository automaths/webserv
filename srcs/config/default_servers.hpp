#pragma once

#include "library.hpp"

// Choisir le port et lhost de chaque "serveur".
// Setup server_names ou pas.
// Le premier serveur pour un host:port sera le serveur par défaut pour cet host:port (ce qui signifie quil répondra à toutes les requêtes qui nappartiennent pas à un autre serveur).
// Setup des pages derreur par défaut.
// Limiter la taille du body des clients.
// Setup des routes avec une ou plusieurs des règles/configurations suivantes (les routes nutiliseront pas de regexp) :
    // Définir une liste de méthodes HTTP acceptées pour la route.
    // Définir une redirection HTTP.
    // Définir un répertoire ou un fichier à partir duquel le fichier doit être recherché (par exemple si lurl /kapouet est rootée sur /tmp/www, lurl /kapouet/pouic/toto/pouet est /tmp/www/pouic/toto/pouet).
    // Activer ou désactiver le listing des répertoires.
    // Set un fichier par défaut comme réponse si la requête est un répertoire.
    // Exécuter CGI en fonction de certaines extensions de fichier (par exemple .php).
    // Rendre la route capable daccepter les fichiers téléchargés et configurer où cela doit être enregistré.
        // Vous vous demandez ce quest un CGI ?
        // Parce que vous nallez pas appeler le CGI mais utiliser directement le chemin complet comme PATH_INFO.
        // Souvenezvous simplement que pour les requêtes fragmentées, votre serveur doit la défragmenter et le CGI attendra EOF comme fin du body.
        // Même chosespour la sortie du CGI. Si aucun content_length nest renvoyé par le CGI, EOF signifiera la fin des données renvoyées.
        // Votre programme doit appeler le CGI avec le fichier demandé comme premier argument.
        // Le CGI doit être exécuté dans le bon répertoire pour laccès au fichier de chemin relatif.
        // votre serveur devrait fonctionner avec un seul CGI (phpCGI, Python, etc.). Vous devez fournir des fichiers de configuration et des fichiers de base par défaut pour tester et démontrer que chaque fonctionnalité fonctionne pendant lévaluation.

class Default_Location_Infos {

    public:

    Default_Location_Infos() {}
    ~Default_Location_Infos() {}

    Default_Location_Infos(int variation) {
        if (variation == 1)
            _path = "/one";
        if (variation == 2)
            _path = "/two";
        if (variation == 3)
            _path = "/three";
    }
    Default_Location_Infos& operator=(const Default_Location_Infos& other) { 
        if (this != &other)
        {
            _path = other._path;
            _sub_domains = other._sub_domains;
        }
        return *this;
    }
    std::string getPath() { return _path; }
    std::list<Default_Location_Infos> getSubDomains() { return _sub_domains; }

    private:

    std::string _path;
    std::list<Default_Location_Infos> _sub_domains;

};

class Default_Error_Page_Infos {

    public:

    Default_Error_Page_Infos() {};
    ~Default_Error_Page_Infos() {};
    Default_Error_Page_Infos(std::string number, std::string path): _number(number), _path(path) {}
    Default_Error_Page_Infos& operator=(const Default_Error_Page_Infos& other) {
        if (this != &other)
        {
            _number = other._number;
            _path = other._path;
        }
        return *this;
    }
    std::string getNumber() { return _number; }
    std::string getPath() { return _path; }

    private:

    std::string _number;
    std::string _path;
};


class Default_Unit_Infos {

    public:

    Default_Unit_Infos() {}
    ~Default_Unit_Infos() {}
    Default_Unit_Infos(int variation) {
        if (variation == 1)
        {
            // _address = "localhost";
            _port = "6000";
            _server_name = "server_one";
            _locations.push_back(Default_Location_Infos(1));
            _client_body_max_size = "200M";
            _default_error_pages.push_back(Default_Error_Page_Infos("404", "/error_page_one.html"));
        }
        if (variation == 2)
        {
            // _address = "*";
            _port = "7000";
            _server_name = "server_two";
            _locations.push_back(Default_Location_Infos(2));
            _client_body_max_size = "300M";
            _default_error_pages.push_back(Default_Error_Page_Infos("401", "/error_page_two.html"));
        }
        if (variation == 3)
        {
            // _address ="127.0.0.1" ;
            _port = "8000";
            _server_name = "server_three";
            _locations.push_back(Default_Location_Infos(3));
            _client_body_max_size = "400M";
            _default_error_pages.push_back(Default_Error_Page_Infos("402", "/error_page_three.html"));
        }
    }

    // std::string getAddress() { return _address; }
    std::string getPort() { return _port; }
    std::list<Default_Location_Infos> getLocation() { return _locations; }
    std::list<Default_Error_Page_Infos> getErrorPage() { return _default_error_pages; }
    std::string getClientBodyMax() { return _client_body_max_size; }

    private:

    // std::string _address;
    std::string _port;
    std::string _server_name;
    std::list<Default_Location_Infos> _locations;
    std::list<Default_Error_Page_Infos> _default_error_pages;
    std::string _client_body_max_size;
    // std::list<std::string> _cgi;
};

class Default_Servers_Infos {

    public:

    Default_Servers_Infos() {
        Default_Unit_Infos server_one(1);
        Default_Unit_Infos server_two(2);
        Default_Unit_Infos server_three(3);
        _servers.push_back(server_one);
        _servers.push_back(server_two);
        _servers.push_back(server_three);
    }
    ~Default_Servers_Infos() {}
    Default_Servers_Infos& operator=(Default_Servers_Infos& other) {
        if (this != &other)
            _servers = other._servers;
        return (*this);
    }

    void print_servers() {
        int n = 1;
        for (std::list<Default_Unit_Infos>::iterator it = _servers.begin(); it != _servers.end(); ++it)
        {
            std::cout << "------| SERVER NUMERO " << n++ << " |------ \n" << std::endl;
            // std::cout << "Address : " << it->getAddress() << std::endl;
            std::cout << "Port: " << it->getPort() << std::endl;
            std::cout << "Location: " << it->getLocation().front().getPath() << std::endl;
            std::cout << "Body Max Size: " << it->getClientBodyMax() << std::endl;
            std::cout << "Error Page: error number " << it->getErrorPage().front().getNumber() << " and path " << it->getErrorPage().front().getPath() << std::endl;
            std::cout << std::endl;

    // std::string _address;
    // std::string _port;
    // std::list<Default_Location_Infos> _locations;
    // std::list<Default_Error_Page_Infos> _default_error_pages;
    // std::string _client_body_max_size;

        }
    }

    private:

    std::list<Default_Unit_Infos> _servers;
};