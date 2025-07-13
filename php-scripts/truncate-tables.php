<?php

$servername = "localhost";
$dbname = "smarthomeproject_v2.0";
$username = "alexey";
$password = "alex281299";
$api_key_value = "tPmAT5Ab3j7F9";
$api_key = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $conn = new mysqli($servername, $username, $password, $dbname);

        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "TRUNCATE `ControlPanel`";
        $conn->query($sql);

        $sql = "TRUNCATE `GasBoilerController`";
        $conn->query($sql);

        $sql = "TRUNCATE `WeatherStation`";
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