<?php

$servername = "localhost";
$dbname = "smarthomeproject_v2.0";
$username = "alexey";
$password = "alex281299";

$conn = mysqli_connect($servername, $username, $password, $dbname);

$mysqli_query = "SELECT `Temperature`, `Humidity`, `Wind speed`, `Wind direction`, `Rainfall`, `Time stamp` FROM WeatherStation WHERE `Time stamp` > DATE_SUB(NOW(), INTERVAL 5 DAY)";

$result = mysqli_query($conn,$mysqli_query);

if(mysqli_num_rows($result)>0){
while ($row = mysqli_fetch_assoc($result))
{
    $Temperature =$row["Temperature"]; 
    $Humidity =$row["Humidity"];
    $WindSpeed =$row["Wind speed"];
    $WindDirection =$row["Wind direction"];
    $RainFall =$row["Rainfall"];
    $TimeStamp =$row["Time stamp"];
    Print($Temperature."|".$Humidity."|".$WindSpeed."|".$WindDirection."|".$RainFall."|".$TimeStamp."|"."<br/>");
} 
}

?>