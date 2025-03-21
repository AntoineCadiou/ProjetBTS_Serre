<?php
include 'db_config.php';


// Connexion à la base de données
try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8", $user, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Erreur de connexion à la base de données : " . $e->getMessage());
}

// Récupération des données XML envoyées par l'Arduino
$xmlData = file_get_contents("php://input");

if ($xmlData) {
    // Charger les données XML
    $xml = simplexml_load_string($xmlData);

    if ($xml) {
        // Préparer la requête SQL
        $stmt = $pdo->prepare("INSERT INTO mesures (valeur, date) VALUES (:valeur, NOW())");

        // Parcourir chaque capteur dans le XML
        foreach ($xml->capteur as $capteur) {
            $valeur = (float) $capteur;

            // Exécuter l'insertion
            $stmt->execute(['valeur' => $valeur]);
        }

        echo "Données insérées avec succès !";
    } else {
        echo "Erreur lors de la lecture du XML.";
    }
} else {
    echo "Aucune donnée reçue.";
}
?>
