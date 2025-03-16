<?php

$servername = "localhost";
$dbname = "smarthomeproject2.0";
$username = "alexey";
$password = "alex281299";
$api_key_value = "tPmAT5Ab3j7F9";
$api_key = $Status = $SetTemperature = $CurrentTemperature = $TemperatureRange = $SetpointSource = $TimeStamp = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $Status = test_input($_POST["Status"]);
        $SetTemperature = test_input($_POST["SetTemperature"]);
        $CurrentTemperature = test_input($_POST["CurrentTemperature"]);
        $TemperatureRange = test_input($_POST["TemperatureRange"]);
        $SetpointSource = test_input($_POST["SetpointSource"]);
        $TimeStamp = test_input($_POST["TimeStamp"]);
        
        $conn = new mysqli($servername, $username, $password, $dbname);

        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO `GasBoilerController` (`Status`, `Set temperature`, `Current temperature`, `Temperature range`, `Setpoint source`, `Time stamp`)
        VALUES ('" . $Status . "', '" . $SetTemperature . "', '" . $CurrentTemperature . "', '" . $TemperatureRange . "', '" . $SetpointSource . "', '" . $TimeStamp . "')";

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