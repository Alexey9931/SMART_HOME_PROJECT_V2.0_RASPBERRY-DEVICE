<?php

$servername = "localhost";
$dbname = "smarthomeproject_v2.0";
$username = "alexey";
$password = "alex281299";

$conn = mysqli_connect($servername, $username, $password, $dbname);

$result = mysqli_query($conn,$mysqli_query);

if(mysqli_num_rows($result)>0){
while ($row = mysqli_fetch_assoc($result))
{
    $Temperature =$row["Temperature"]; 
    $Humidity =$row["Humidity"];
    $Pressure =$row["Pressure"];
    $TimeStamp =$row["Time stamp"];
    Print($Temperature."|".$Humidity."|".$Pressure."|".$TimeStamp."|"."<br/>");
} 
}

?>