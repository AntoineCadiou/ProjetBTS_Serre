// config.h
#ifndef CONFIG_H
#define CONFIG_H

// Configuration réseau
const char* ssid = "A56 de Antoine";
const char* password = "Akdiou2004";

// Configuration serveur
const char* serverIP = "192.168.221.67"; // Adresse IP du serveur WAMP
const int serverPort = 80;
const char* apiEndpoint = "/testprojet/receiveData.php"; // Endpoint pour recevoir les données

// Informations base de données
const char* dbUser = "root";
const char* dbPassword = "";
const char* dbName = "capteurs_db";

#endif