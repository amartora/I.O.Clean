var mysql = require('mysql');
var connection = mysql.createConnection({
    host: '100.8.221.42',
    port: '3306',
    user: 'cleanadmin',
    password: 'cleanpassword',
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
