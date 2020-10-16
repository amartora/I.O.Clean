var mysql = require('mysql');
var connection = mysql.createConnection({
    host: ' ',
    port: '3306',
    user: ' ',
    password: ' ',
    database: 'IoT',
    insecureAuth: true
});
connection.connect();
connection.query('SELECT * from buttonTable', function (err, rows, fields) {
    if (!err)
        console.log('The solution is: ', rows);
    else
        console.log(err);
});
