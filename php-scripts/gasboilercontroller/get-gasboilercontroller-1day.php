<?php

$servername = "localhost";
$dbname = "smarthomeproject_v2.0";
$username = "alexey";
$password = "alex281299";

$conn = mysqli_connect($servername, $username, $password, $dbname);

$mysqli_query = "SELECT `Status`, `Set temperature`, `Current temperature`, `Temperature range`, `Setpoint source`, `Time stamp` FROM GasBoilerController WHERE `Time stamp` > DATE_SUB(NOW(), INTERVAL 1 DAY)";

$result = mysqli_query($conn,$mysqli_query);

if(mysqli_num_rows($result)>0){
while ($row = mysqli_fetch_assoc($result))
{
    $Status =$row["Status"]; 
    $SetTemperature =$row["Set temperature"];
    $CurrentTemperature =$row["Current temperature"];
    $TemperatureRange =$row["Temperature range"];
    $SetpointSource =$row["Setpoint source"];
    $TimeStamp =$row["Time stamp"];
    Print($SetTemperature."|".$CurrentTemperature."|".$Status."|".$TemperatureRange."|".$SetpointSource."|".$TimeStamp."|"."<br/>");
} 
}

?>