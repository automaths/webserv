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
#include "location_infos.hpp"

class ChunkInfos {

    public:

    ChunkInfos(){}
    ~ChunkInfos(){}
    ChunkInfos(std::string str){
        _chunk = str;
        clean_comments_header();
        extract_location_blocks();
        extract_lines();
        _directive_types[0] = "listen";
        _directive_types[1] = "server_name";
        _directive_types[2] = "error_page";
        _directive_types[3] = "client_body_buffer_size";
        _directive_types[4] = "root";
        _directive_types[5] = "allow_method";
        _directive_types[6] = "cgi";
        _directive_types[7] = "index";
        _directive_types[8] = "autoindex";
        _directive_types[9] = "try_files";
        exec[0] = &ChunkInfos::extract_listen;
        exec[1] = &ChunkInfos::extract_server_name;
        exec[2] = &ChunkInfos::extract_default_error_pages;
        exec[3] = &ChunkInfos::extract_client_body_buffer_size;
        exec[4] = &ChunkInfos::extract_root;
        exec[5] = &ChunkInfos::extract_allow_method;
        exec[6] = &ChunkInfos::extract_cgi;
        exec[7] = &ChunkInfos::extract_index;
        exec[8] = &ChunkInfos::extract_autoindex;
        exec[9] = &ChunkInfos::extract_try_files;
        extract_directives();
        apply_default();
        while (_location_blocks.size() != 0)
        {
            _locations.push_back(LocationInfos(_location_blocks.front()));
            _location_blocks.pop_front();
        }
        // print_result();
    }
    ChunkInfos& operator=(ChunkInfos &other) {
        if (this != &other)
        {
            // _locations = other._locations;
            _address = other._address;
            _port = other._port;
            _server_names = other._server_names;
            _try_files = other._try_files;
            _index = other._index;
            _client_body_buffer_size = other._client_body_buffer_size;
            _autoindex = other._autoindex;
            _root = other._root;
            _allow_method = other._allow_method;
            _cgi = other._cgi;
            _default_error_pages = other._default_error_pages;
        }
        return *this;
    }
    void                                extract_location_blocks();
    void                                extract_lines();
    void                                extract_directives();
    void                                clean_comments_header();
    void                                extract_rules(std::string rule);
    void                                apply_default();
    void                                print_result();
    void                                extract_listen(std::string directive);
    void                                extract_server_name(std::string directive);
    void                                extract_default_error_pages(std::string directive);
    void                                extract_client_body_buffer_size(std::string directive);
    void                                extract_root(std::string directive);
    void                                extract_allow_method(std::string directive);
    void                                extract_cgi(std::string directive);
    void                                extract_index(std::string directive);
    void                                extract_autoindex(std::string directive);
    void                                extract_try_files(std::string directive);
    // void                                setLocations(std::list<LocationInfos> arg) { _locations = arg; }
    void                                setAddress(std::string arg) { _address = arg; }
    void                                setPort(std::string arg) { _port = arg; }
    void                                setServerName(std::list<std::string> arg) { _server_names = arg; }
    void                                setTryFiles(std::list<std::string> arg) { _try_files = arg; }
    void                                setIndex(std::list<std::string> arg) { _index = arg; }
    void                                setClientBodyBufferMax(std::string arg) { _client_body_buffer_size = arg; }
    void                                setAutoIndex(std::string arg) { _autoindex = arg; }
    void                                setRoot(std::string arg) { _root = arg; }
    void                                setAllowMethod(std::list<std::string> arg) { _allow_method = arg; }
    void                                getCgi(std::map<std::string, std::string> arg) { _cgi = arg; }
    void                                getDefaultErrorPage(std::map<std::string, std::string> arg) { _default_error_pages = arg; }
    std::list<LocationInfos>            getLocations() { return _locations; }
    std::string                         getAddress() { return _address; }
    std::string                         getPort() { return _port; }
    std::list<std::string>              getServerName() { return _server_names; }
    std::list<std::string>              getTryFiles() { return _try_files; }
    std::list<std::string>              getIndex() { return _index; }
    std::string                         getClientBodyBufferMax() { return _client_body_buffer_size; }
    std::string                         getAutoIndex() { return _autoindex; }
    std::string                         getRoot() { return _root; }
    std::list<std::string>              getAllowMethod() { return _allow_method; }
    std::map<std::string, std::string>  getCgi() { return _cgi; }
    std::map<std::string, std::string>  getDefaultErrorPage() { return _default_error_pages; }

    private:

    std::list<LocationInfos>                            _locations;
    std::string                                         _address;
    std::string                                         _port;
    std::list<std::string>                              _server_names;
    std::list<std::string>                              _try_files;
    std::list<std::string>                              _index;
    std::string                                         _client_body_buffer_size;
    std::string                                         _autoindex;
    std::string                                         _root;
    std::list<std::string>                              _allow_method;
    std::map<std::string, std::string>                  _cgi;
    std::map<std::string, std::string>                  _default_error_pages;

    std::string                                         _chunk;
    std::list<std::string>                              _location_blocks;
    std::list<std::string>                              _directives;
    std::list<std::string>                              _configs;
    std::string                                         _directive_types[10];
    void (ChunkInfos::*exec[10])(std::string str);
};

