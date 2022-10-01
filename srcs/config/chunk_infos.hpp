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

    class Harl{
    std::string level[4];
    void    (Harl::*exec[4])(void);
    void debug(void);
    void info(void);
    void warning(void);
    void error(void);
    public: 
    Harl();
    ~Harl();
    void complain(std::string level);
};

    Chunk_Infos(std::string str){
        _chunk = str;

        _chunk.erase(0, _chunk.find_first_of('{', 0) + 1);
        _chunk.erase(_chunk.find_last_of('}'), 1);
        while (_chunk.find('#', 0) != std::string::npos)
            _chunk.erase(_chunk.find('#', 0), _chunk.find_first_of('\n', _chunk.find('#', 0)) - _chunk.find('#', 0));

        extract_location_blocks();
        extract_directives();

        while (_directives.size() != 0)
        {
            if (_directives.back().find("listen") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("listen")) == (_directives.back().find("listen") + 6))
                    extract_listen(_directives.back());
            }
            else if (_directives.back().find("server_name") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("server_name")) == (_directives.back().find("server_name") + 11))
                    extract_server_name(_directives.back());
            }
            else if (_directives.back().find("error_page") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("error_page")) == (_directives.back().find("error_page") + 10))
                    extract_default_error_pages(_directives.back());
            }
            else if (_directives.back().find("client_body_buffer_size") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("client_body_buffer_size")) == (_directives.back().find("client_body_buffer_size") + 23))
                    extract_client_body_buffer_size(_directives.back());
            }
            else if (_directives.back().find("root") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("root")) == (_directives.back().find("root") + 4))
                   extract_root(_directives.back());
            }
            else if (_directives.back().find("allow_method") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("allow_method")) == (_directives.back().find("allow_method") + 12))
                    extract_allow_method(_directives.back());
            }
            else if (_directives.back().find("cgi") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("cgi")) == (_directives.back().find("cgi") + 3))
                    extract_cgi(_directives.back());
            }
            else if (_directives.back().find("index") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("index")) == (_directives.back().find("index") + 5))
                    extract_index(_directives.back());
            }
            else if (_directives.back().find("autoindex") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("autoindex")) == (_directives.back().find("autoindex") + 9))
                    extract_autoindex(_directives.back());
            }
            else if (_directives.back().find("try_files") == _directives.back().find_first_not_of("\t\v\n\r\f "))
            {
                if (_directives.back().find_first_of("\t\v\n\r\f ", _directives.back().find("try_files")) == (_directives.back().find("try_files") + 9))
                    extract_try_files(_directives.back());
            }
            _directives.pop_back();
        }

        if (_address.size() == 0)
        {
            _address = "*";
            _port = "80";           
        }
        if (_root.size() == 0)
            _root = "html";
        if(_autoindex.size() == 0)
            _autoindex = "off";
        // std::ofstream ofs;
        // ofs.open("config_result.txt");
        // ofs << _chunk;

        // int n = 1;
        // for (std::list<std::string>::iterator it = _directives.begin(); it != _directives.end(); ++it)
        //     ofs << "\nthe line " << n++ << " is:\n" << *it << std::endl;;

        print_result();
    }

    // void extract_location() {
    //     if (_chunk.first_first)
    // }

    void extract_location_blocks();
    void extract_directives();

    void extract_listen(std::string listen_dir);
    void extract_server_name(std::string server_name_dir);
    void extract_default_error_pages(std::string _error_page_dir);
    void extract_client_body_buffer_size(std::string body_max_dir);
    void extract_root(std::string root_dir);
    void extract_allow_method(std::string allow_method_dir);
    void extract_cgi(std::string cgi_dir);
    void extract_index(std::string index_dir);
    void extract_autoindex(std::string autoindex_dir);
    void extract_try_files(std::string try_files_dir);

    void print_result();

    private:

    std::string _chunk;
    std::list<std::string> _location_blocks;
    std::list<std::string> _directives;





    std::list<std::string> _configs;


    std::string _address;
    std::string _port;
    std::list<std::string> _server_names;

    std::list<std::string> _try_files;
    std::list<std::string> _index;
    std::string _client_body_buffer_size;
    std::string _autoindex;
    std::string _root;
    
    std::list<std::string> _allow_method;
    std::list<std::pair<std::string, std::string> > _cgi;
    std::map<std::string, std::string> _default_error_pages;



    // bool is_http;
};

