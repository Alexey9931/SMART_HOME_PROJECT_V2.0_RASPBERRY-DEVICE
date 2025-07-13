<?php

$servername = "localhost";
$dbname = "smarthomeproject_v2.0";
$username = "alexey";
$password = "alex281299";
$api_key_value = "tPmAT5Ab3j7F9";
$api_key = $Temperature = $Humidity = $Pressure = $TimeStamp = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $Temperature = test_input($_POST["Temperature"]);
        $Humidity = test_input($_POST["Humidity"]);
        $Pressure = test_input($_POST["Pressure"]);
        $TimeStamp = test_input($_POST["TimeStamp"]);
        
        $conn = new mysqli($servername, $username, $password, $dbname);

        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO `ControlPanel` (`Temperature`, `Humidity`, `Pressure`, `Time stamp`)
        VALUES ('" . $Temperature . "', '" . $Humidity . "', '" . $Pressure . "', '" . $TimeStamp . "')";
        
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