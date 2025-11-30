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

$sql = "SELECT `id`, `Temperature`, `Humidity`, `Pressure`, `Time stamp` FROM ControlPanel ORDER BY id DESC";

echo '<p style="font-style:inherit" align=center><font size="5" color="blue" face="Arial">БАЗА ДАННЫХ ControlPanel проекта SmartHomeProject2.0</p>';
echo '<table cellspacing="5" cellpadding="5" align=center>
      <tr> 
        <td>№</td> 
        <td>Температура</td> 
        <td>Влажность</td> 
        <td>Атм.давление</td> 
        <td>Дата</td>
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_Temp = $row["Temperature"];
        $row_Hum = $row["Humidity"];
        $row_Pressure = $row["Pressure"];
        $row_time = $row["Time stamp"];
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_Temp . '</td> 
                <td>' . $row_Hum . '</td> 
                <td>' . $row_Pressure . '</td> 
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