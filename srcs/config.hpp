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

class Serv {

    public:

    private:

    std::string ip;
    int port;

    std::list<std::string> names;

    std::list<std::map<std::string, std::string> > locations;

};

class Config {

    public:

    Config(){}
    ~Config(){}

    Config(std::string str): file(str) {

        while (file.find_first_of("http") > 0)
            file.erase(0, 1);
        std::string::iterator it = file.begin();
        for (unsigned int i = 0; i <= file.find_first_of("{"); ++i)
            ++it;
        int n = 1;




        while (it != file.end())
        {
            std::cout << *it;
            ++it;
        }



        // while (file.size() > 0 && n > 0)


        // std::cout << file << std::endl;



    }

    private:

    std::string file;

    std::list<Serv> servers;

};