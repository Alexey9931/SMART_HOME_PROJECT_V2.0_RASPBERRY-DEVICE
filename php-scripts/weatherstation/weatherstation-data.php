<!DOCTYPE html>
<html>
<head>
   <link rel="stylesheet" href="style1" />
</head>    
<body>
<?php

$servername = "localhost";
$dbname = "smarthomeproject_v2.0";
$username = "alexey";
$password = "alex281299";

$conn = mysqli_connect($servername, $username, $password, $dbname);

$sql = "SELECT `id`, `Temperature`, `Humidity`, `Wind speed`, `Wind direction`, `Rainfall`, `Time stamp` FROM WeatherStation ORDER BY id DESC";

echo '<p style="font-style:inherit" align=center><font size="5" color="blue" face="Arial">БАЗА ДАННЫХ WeatherStation проекта SmartHomeProject2.0</p>';
echo '<table cellspacing="5" cellpadding="5" align=center>
      <tr> 
        <td>№</td> 
        <td>Температура</td> 
        <td>Влажность</td> 
        <td>Скорость ветра</td> 
        <td>Направление ветра</td> 
        <td>Осадки</td> 
        <td>Дата</td>
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_Temp = $row["Temperature"];
        $row_Hum = $row["Humidity"];
        $row_WindSpeed = $row["Wind speed"];
        $row_WindDirect = $row["Wind direction"]; 
        $row_Rainfall = $row["Rainfall"]; 
        $row_time = $row["Time stamp"];
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_Temp . '</td> 
                <td>' . $row_Hum . '</td> 
                <td>' . $row_WindSpeed . '</td> 
                <td>' . $row_WindDirect . '</td>
                <td>' . $row_Rainfall . '</td> 
                <td>' . $row_time . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>