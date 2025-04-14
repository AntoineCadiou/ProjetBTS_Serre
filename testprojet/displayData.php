<?php
include 'db_config.php';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8", $user, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Erreur de connexion à la base de données : " . $e->getMessage());
}

// Vérifie si la requête est une requête AJAX pour récupérer les données
if (isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) === 'xmlhttprequest') {
    $sql = "SELECT id, valeur, zone, id_capteur, date FROM mesures ORDER BY date DESC";
    $stmt = $pdo->query($sql);
    $result = $stmt->fetchAll(PDO::FETCH_ASSOC);

    header('Content-Type: application/json');
    echo json_encode($result);
    exit;
}

// Requête initiale pour afficher les données lors du chargement de la page
$sql = "SELECT id, valeur, zone, id_capteur, date FROM mesures ORDER BY date DESC";
$stmt = $pdo->query($sql);
$result = $stmt->fetchAll(PDO::FETCH_ASSOC);
?>
<!DOCTYPE html>
<html>
<head>
    <title>Données des Capteurs</title>
    <script>
        function fetchData() {
            fetch('displayData.php', {
                headers: {
                    'X-Requested-With': 'XMLHttpRequest'
                }
            })
                .then(response => response.json())
                .then(data => {
                    const tableBody = document.querySelector('#dataTable tbody');
                    tableBody.innerHTML = '';
                    data.forEach(row => {
                        const tr = document.createElement('tr');
                        tr.innerHTML = `
                            <td>${row.id}</td>
                            <td>${row.valeur}</td>
                            <td>${row.zone}</td>
                            <td>${row.id_capteur}</td>
                            <td>${row.date}</td>
                        `;
                        tableBody.appendChild(tr);
                    });
                })
                .catch(error => console.error('Erreur:', error));
        }

        // Actualiser les données toutes les 5 secondes
        setInterval(fetchData, 5000);

        // Appel initial pour charger les données au chargement de la page
        fetchData();
    </script>
</head>
<body>
    <h1>Valeurs des Capteurs</h1>
    <table id="dataTable" border="1">
        <thead>
            <tr>
                <th>ID</th>
                <th>Valeur</th>
                <th>Zone</th>
                <th>Id_capteur</th>
                <th>Date</th>
            </tr>
        </thead>
        <tbody>
            <?php foreach ($result as $row) { ?>
            <tr>
                <td><?php echo htmlspecialchars($row['id']); ?></td>
                <td><?php echo htmlspecialchars($row['valeur']); ?></td>
                <td><?php echo htmlspecialchars($row['zone']); ?></td>
                <td><?php echo htmlspecialchars($row['id_capteur']); ?></td>
                <td><?php echo htmlspecialchars($row['date']); ?></td>
            </tr>
            <?php } ?>
        </tbody>
    </table>
</body>
</html>
