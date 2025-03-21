<?php
include 'db_config.php';

$sql = "SELECT id, valeur, date FROM mesures ORDER BY date DESC";
$result = $conn->query($sql);
?>
<!DOCTYPE html>
<html>
<head>
    <title>Données des Capteurs</title>
</head>
<body>
    <h1>Valeurs des Capteurs</h1>
    <table border="1">
        <tr>
            <th>ID</th>
            <th>Valeur</th>
            <th>Date</th>
        </tr>
        <?php while ($row = $result->fetch_assoc()) { ?>
        <tr>
            <td><?php echo $row['id']; ?></td>
            <td><?php echo $row['valeur']; ?></td>
            <td><?php echo $row['date']; ?></td>
        </tr>
        <?php } ?>
    </table>
</body>
</html>

