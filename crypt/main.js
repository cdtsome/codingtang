var cdt = require('./crypt');
var http = require("http");
var querystring = require('querystring');
var util = require('util');
http.createServer(function(req, res) {
	var post = '';     
    req.on('data', function(chunk){    
        post += chunk;
    });
    req.on('end', function(){
		res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end(cdt.descrypt(post));
    });
}).listen(19198);
