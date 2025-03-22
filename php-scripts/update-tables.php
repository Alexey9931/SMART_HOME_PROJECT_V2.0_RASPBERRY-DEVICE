<?php

$servername = "localhost";
$dbname = "smarthomeproject2.0";
$username = "alexey";
$password = "alex281299";
$api_key_value = "tPmAT5Ab3j7F9";
$api_key = $days = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $days = test_input($_POST["days"]);

        $conn = new mysqli($servername, $username, $password, $dbname);

        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "DELETE FROM `ControlPanel` WHERE `Time stamp` < DATE_SUB(NOW(), INTERVAL '" . $days . "' DAY)";
        $conn->query($sql);

        $sql = "DELETE FROM `GasBoilerController` WHERE `Time stamp` < DATE_SUB(NOW(), INTERVAL '" . $days . "' DAY)";
        $conn->query($sql);

        $sql = "DELETE FROM `WeatherStation` WHERE `Time stamp` < DATE_SUB(NOW(), INTERVAL '" . $days . "' DAY)";
        $conn->query($sql);

        $conn->close();
    }
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}