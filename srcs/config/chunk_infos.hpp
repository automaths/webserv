#pragma once


// Votre serveur doit pouvoir ecouter sur plusieurs ports.
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

#include "library.hpp"

class Chunk_Infos {

    public:

    Chunk_Infos(){}
    ~Chunk_Infos(){}

    Chunk_Infos(std::string str){
        _chunk = str;
        extract_location();
        extract_address_port();
        extract_server_name();
        extract_default_error_pages();
        extract_client_body_buffer_size();
        extract_root();
        extract_allow_method();
        extract_cgi();
        extract_index();
        extract_autoindex();
        extract_try_files();

        print_result();
    }

    void extract_location();
    void extract_address_port();
    void extract_server_name();
    void extract_default_error_pages();
    void extract_client_body_buffer_size();
    void extract_root();
    void extract_allow_method();
    void extract_cgi();
    void extract_index();
    void extract_autoindex();

    void extract_try_files() {
        if (_chunk.find("try_files ", 0) != std::string::npos)
        {
            std::string try_files_dir = _chunk.substr(_chunk.find("try_files ", 0), _chunk.find_first_of(';', _chunk.find("try_files ", 0)) - _chunk.find("try_files ", 0));
            try_files_dir.erase(0, try_files_dir.find_first_of("try_files ") + 10);
            while (try_files_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
                try_files_dir.erase(0, 1);
            while (try_files_dir.size() != 0)
            {  
                if (try_files_dir.find_first_of(" \t\v\n\r\f", 0) != std::string::npos)
                {
                    _try_files.push_back(try_files_dir.substr(0, try_files_dir.find_first_of(" \t\v\n\r\f", 0)));
                    try_files_dir.erase(0, try_files_dir.find_first_of(" \t\v\n\r\f", 0));
                }
                else
                {
                    _try_files.push_back(try_files_dir.substr(0, try_files_dir.size()));
                    try_files_dir.erase(0, try_files_dir.size());
                }
                while(try_files_dir.find_first_of(" \t\v\n\r\f", 0) == 0)
                    try_files_dir.erase(0, 1);
            }
        }
        else
            _try_files.push_back("");
    }

    void print_result();

    private:

    std::string _chunk;

    std::string _address;
    std::string _port;
    std::list<std::string> _server_names;
    std::list<std::string> _try_files;
    std::list<std::string> _index;
    std::string _client_body_buffer_size;
    std::string _autoindex;
    std::string _root;
    std::list<std::string> _location_blocks;
    std::list<std::string> _allow_method;
    std::list<std::pair<std::string, std::string> > _cgi;
    std::map<std::string, std::string> _default_error_pages;

    // bool is_http;
};

