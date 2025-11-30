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

$sql = "SELECT `id`, `Status`, `Set temperature`, `Current temperature`, `Temperature range`, `Setpoint source`, `Time stamp` FROM GasBoilerController ORDER BY id DESC";

echo '<p style="font-style:inherit" align=center><font size="5" color="blue" face="Arial">БАЗА ДАННЫХ GasBoilerController проекта SmartHomeProject2.0</p>';
echo '<table cellspacing="5" cellpadding="5" align=center>
      <tr> 
        <td>№</td> 
        <td>Статус котла</td> 
        <td>Уставка темп.</td> 
        <td>Текущая темп.</td> 
        <td>Диапазон темп.</td> 
        <td>Датчик темп.</td> 
        <td>Дата</td>
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_Status = $row["Status"];
        $row_Setpoint = $row["Set temperature"];
        $row_Current = $row["Current temperature"];
        $row_Range = $row["Temperature range"]; 
        $row_SetSource = $row["Setpoint source"]; 
        $row_time = $row["Time stamp"];
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_Status . '</td> 
                <td>' . $row_Setpoint . '</td> 
                <td>' . $row_Current . '</td> 
                <td>' . $row_Range . '</td>
                <td>' . $row_SetSource . '</td> 
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